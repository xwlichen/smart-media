/**
 * @date : 2019-11-20 14:27
 * @author: lichen
 * @email : 1960003945@qq.com
 * @description : 
 */
//
#include <log_utils.h>
#include <s_error.h>
#include "media_core.h"


MediaCore::MediaCore(int stream_index, PlayerStatus *p_player_status,
                     JNIPlayerCall *p_jni_player_call) {
    this->stream_index = stream_index;
    this->p_player_status = p_player_status;
    this->p_jni_player_call = p_jni_player_call;
    this->p_avpacket_queue = new AVPacketQueue(p_player_status);

    //参数attr指定了新建互斥锁的属性。如果参数attr为NULL，则使用默认的互斥锁属性，默认属性为快速互斥锁 。
    // 互斥锁的属性在创建锁的时候指定，在LinuxThreads实现中仅有一个锁类型属性，不同的锁类型在试图对一个已经被锁定的互斥锁加锁时表现不同。
    pthread_mutex_init(&seek_mutex, NULL);
}

MediaCore::~MediaCore() {
    pthread_mutex_destroy(&seek_mutex);
}

int MediaCore::analysis_stream(ThreadMode thread_mode, AVFormatContext *p_avformat_ctx_in) {
    int res;
    AVCodecParameters *p_avcodec_params_in = p_avformat_ctx_in->streams[this->stream_index]->codecpar;

    //查找解码器
    AVCodec *p_avcodec_decoder_in = avcodec_find_decoder(p_avcodec_params_in->codec_id);
    if (!p_avcodec_decoder_in) {
        LOGE(JNI_DEBUG, "can't find stream decoder");
        call_player_jni_error(thread_mode, ERROR_CODE_FIND_STREAM_DECODER,
                              "can't find stream decoder");
        return ERROR_CODE_FIND_STREAM_DECODER;
    }
    //分配生成解码器上下文
    p_avcodec_ctx_decoder_in = avcodec_alloc_context3(p_avcodec_decoder_in);
    //给解码器上下文赋值params
    res = avcodec_parameters_to_context(p_avcodec_ctx_decoder_in,
                                        p_avcodec_params_in);

    if (res < 0) {
        LOGE(JNI_DEBUG, "decoder parameters to context error code : %d ,msg : %s", res,
             av_err2str(res));
        call_player_jni_error(thread_mode, res, av_err2str(res));
        return res;
    }

    //打开解码器
    res = avcodec_open2(p_avcodec_ctx_decoder_in, p_avcodec_decoder_in, NULL);
    if (res < 0) {
        LOGE(JNI_DEBUG, "codec open error code : %d ,msg : %s", res, av_err2str(res));
        call_player_jni_error(thread_mode, res, av_err2str(res));
        return res;
    }
    //单位秒
    duration = p_avformat_ctx_in->duration / AV_TIME_BASE;
    time_base = p_avformat_ctx_in->streams[stream_index]->time_base;

    return 0;



//    //查找编码器，为缓存本地准备
//    AVCodec *p_avcodec_encoder_in = avcodec_find_encoder(p_avcodec_params_in->codec_id);
//    //输出相关配置
//    AVStream *avstream_out = avformat_new_stream(p_avformat_ctx_out, p_avcodec_encoder_in);
//    if (!avstream_out) {
//        LOGE(JNI_DEBUG, "create  streams for output failed");
//    }
//    AVCodec *acvcodec_encoder_out = avcodec_find_encoder(avstream_out->codecpar->codec_id);
//    AVCodecContext *avcodec_ctx_encoder_out = avcodec_alloc_context3(acvcodec_encoder_out);
//
//    res = avcodec_parameters_to_context(avcodec_ctx_encoder_out, p_avcodec_params_in);
//    if (res < 0) {
//        LOGE(JNI_DEBUG, "encoder  parameters to context error code : %d ,msg : %s", res,
//             av_err2str(res));
//    }
//    res = avformat_write_header(p_avformat_ctx_out, NULL);
//    if (res < 0) {
//        LOGE(JNI_DEBUG, "encoder write header error code : %d ,msg : %s", res,
//             av_err2str(res));
//    }


}


void MediaCore::seek_to(uint64_t seconds) {
    if (duration <= 0) {
        return;
    }
//    LOGE(JNI_DEBUG, "duration :%d", duration)
//    pthread_mutex_lock(&seek_mutex);
    if (seconds >= 0 && seconds < duration) {
        p_avpacket_queue->clear();
        last_update_time = 0;
        current_time = 0;
        //可以立即丢掉解码器中缓存帧
        avcodec_flush_buffers(p_avcodec_ctx_decoder_in);

    }
//    pthread_mutex_unlock(&seek_mutex);

}

void MediaCore::release() {
    if (p_avcodec_ctx_decoder_in != NULL) {
        avcodec_close(p_avcodec_ctx_decoder_in);
        avcodec_free_context(&p_avcodec_ctx_decoder_in);
        p_avcodec_ctx_decoder_in = NULL;
    }

    if (p_avpacket_queue != NULL) {
        delete (p_avpacket_queue);
        p_avpacket_queue = NULL;
    }
    if (p_player_status) {
        p_player_status = NULL;
    }

    if (p_jni_player_call) {
        p_player_status = NULL;
    }
}


void MediaCore::call_player_jni_error(ThreadMode thread_mode, int code, const char *msg) {
    if (p_jni_player_call != NULL) {
        p_jni_player_call->call_error(thread_mode, code, msg);
    }
}
