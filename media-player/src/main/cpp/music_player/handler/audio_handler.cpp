/**
 * @date : 2019-11-20 14:27
 * @author: lichen
 * @email : 1960003945@qq.com
 * @description : 
 */
//
#include <s_error.h>
#include <constant.h>
#include <log_utils.h>
#include "audio_handler.h"


AudioHandler::AudioHandler(int stream_index, PlayerStatus *player_status,
                           JNIPlayerCall *p_jni_player_call)
        : MediaCore(stream_index, player_status, p_jni_player_call) {

}

AudioHandler::~AudioHandler() {
    release();
}


int AudioHandler::analysis_stream(ThreadMode thread_mode, AVFormatContext *p_avformat_ctx_in) {
    int res;
    res = MediaCore::analysis_stream(thread_mode, p_avformat_ctx_in);
    if (res < 0) {
        return res;
    }

    sample_rate = p_avcodec_ctx_decoder_in->sample_rate;

    // 处理一些异常的问题
    if (p_avcodec_ctx_decoder_in->channels > 0 && p_avcodec_ctx_decoder_in->channel_layout == 0) {
        p_avcodec_ctx_decoder_in->channel_layout = av_get_default_channel_layout(
                p_avcodec_ctx_decoder_in->channels);
    } else if (p_avcodec_ctx_decoder_in->channels == 0 &&
               p_avcodec_ctx_decoder_in->channel_layout > 0) {
        p_avcodec_ctx_decoder_in->channels = av_get_channel_layout_nb_channels(
                p_avcodec_ctx_decoder_in->channel_layout);
    }


//    frame_buffer_size = p_avcodec_ctx_decoder_in->frame_size * 2 * 2;
    frame_buffer_size = sample_rate * 2 * 2;
    p_convert_out_buffer = (uint8_t *) malloc(frame_buffer_size);

    return 0;



    // ---------- 重采样 end ----------



}


/**
 * 在注册后，opesles 在播放状态下会一直回调 playerCallback
 * 会不断的从队列中获packet 进行解码成pcm数据（通过resample_audio这个方法）
 * @param caller
 * @param pContext
 */
void buffer_queue_callback(SLAndroidSimpleBufferQueueItf caller, void *ctx) {
    AudioHandler *p_audio = (AudioHandler *) ctx;
//    LOGE(JNI_DEBUG, "buffer_queue_callback----");

    if (p_audio != NULL) {
//        LOGE(JNI_DEBUG, "resample_audio start----");

        int data_size = p_audio->resample_audio();
//        LOGE(JNI_DEBUG, "resample_audio end----");


        int buffer_size = data_size;
//        LOGE(JNI_DEBUG, "buffer_size:%d", buffer_size);
//        LOGE(JNI_DEBUG, "p_audio->p_avcodec_ctx_decoder_in->sample_rate:%d",
//             p_audio->p_avcodec_ctx_decoder_in->sample_rate)
//        LOGE(JNI_DEBUG, " buffer_size :%d  ,p_audio->sample_rate * 2 * 2 :%d ,value:%lf",  buffer_size ,p_audio->sample_rate * 2 * 2, buffer_size / ((double)(p_audio->sample_rate * 2 * 2)));

        if (buffer_size > 0) {
            p_audio->current_time +=
                    buffer_size / (p_audio->sample_rate * 2 * 2);





//        LOGE("current : %lf, frame_buffer_size %d", context->current_time, context->frame_buffer_size);
            //进度回掉更新
            if (p_audio->current_time - p_audio->last_update_time >= 1) {
                if (p_audio->p_jni_player_call) {
                    p_audio->p_jni_player_call->call_progress(THREAD_CHILD, p_audio->current_time,
                                                              p_audio->duration);
                    p_audio->last_update_time = p_audio->current_time;
                }
            }
            //播放完成
            if (p_audio->duration > 0 && p_audio->duration <= p_audio->current_time) {
                p_audio->p_jni_player_call->call_complete(THREAD_CHILD);
            }
            (*p_audio->p_android_buffer_queue_itf)->Enqueue(p_audio->p_android_buffer_queue_itf,
                                                            (char *) p_audio->p_convert_out_buffer,
                                                            data_size);


        }
    }


}

void AudioHandler::init_opensles() {
    /*OpenSLES OpenGLES 都是自带的
    XXXES 与 XXX 之间可以说是基本没有区别，区别就是 XXXES 是 XXX 的精简
    而且他们都有一定规则，命名规则 slXXX() , glXXX3f*/
    // 创建引擎接口对象

    SLresult result;

    SLEnvironmentalReverbSettings reverbSettings = SL_I3DL2_ENVIRONMENT_PRESET_STONECORRIDOR;
    const SLInterfaceID ids[1] = {SL_IID_ENVIRONMENTALREVERB};
    const SLboolean req[1] = {SL_BOOLEAN_FALSE};

    //创建引擎
    result = slCreateEngine(&p_engine_obj, 0, 0, 0, 0, 0);
    if (SL_RESULT_SUCCESS != result)
        LOGE(JNI_DEBUG, "create_engine_res: %d", SL_RESULT_SUCCESS == result);
    // realize the engine
    result = (*p_engine_obj)->Realize(p_engine_obj, SL_BOOLEAN_FALSE);
    if (SL_RESULT_SUCCESS != result)
        LOGE(JNI_DEBUG, "relize_engine_res: %d", SL_RESULT_SUCCESS == result);

    // get the engine interface, which is needed in order to create other objects
    result = (*p_engine_obj)->GetInterface(p_engine_obj, SL_IID_ENGINE, &p_engine);
    if (SL_RESULT_SUCCESS != result)
        LOGE(JNI_DEBUG, "get the engine interface res: %d", SL_RESULT_SUCCESS == result);
    //设置混音器
    //create output mix, with environmental reverb specified as a non-required interface
    result = (*p_engine)->CreateOutputMix(p_engine, &p_mix_obj, 1, ids, req);
    if (SL_RESULT_SUCCESS != result)
        LOGE(JNI_DEBUG, "create output mix res: %d", SL_RESULT_SUCCESS == result);

    result = (*p_mix_obj)->Realize(p_mix_obj, SL_BOOLEAN_FALSE);
    if (SL_RESULT_SUCCESS != result)
        LOGE(JNI_DEBUG, "realize the output mix res: %d", SL_RESULT_SUCCESS == result);


    // get the environmental reverb interface
    // this could fail if the environmental reverb effect is not available,
    // either because the feature is not present, excessive CPU load, or
    // the required MODIFY_AUDIO_SETTINGS permission was not requested and granted
    result = (*p_mix_obj)->GetInterface(p_mix_obj, SL_IID_ENVIRONMENTALREVERB,
                                        &p_output_mix_environmental_reverb);
    if (SL_RESULT_SUCCESS != result)
        LOGE(JNI_DEBUG, "get the environmental reverb interface res: %d",
             SL_RESULT_SUCCESS == result);

    //ignore unsuccessful result codes for environmental reverb, as it is optional for this example
    result = (*p_output_mix_environmental_reverb)->SetEnvironmentalReverbProperties(
            p_output_mix_environmental_reverb,
            &reverbSettings);
    if (SL_RESULT_SUCCESS != result)
        LOGE(JNI_DEBUG, "SetEnvironmentalReverbProperties res: %d", SL_RESULT_SUCCESS == result);

    //configure audio source
    SLDataLocator_AndroidSimpleBufferQueue android_simple_buffer_queue = {
            SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 2};
    SLDataFormat_PCM format_pcm = {
            SL_DATAFORMAT_PCM, //pcm数据
            2,  //2通道
            static_cast<SLuint32>(get_current_sample_rate_for_opensles(sample_rate)), //441000
//            441000,
            SL_PCMSAMPLEFORMAT_FIXED_16,
            SL_PCMSAMPLEFORMAT_FIXED_16, //2字节 16位
            SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT, //立体声
            SL_BYTEORDER_LITTLEENDIAN};
    SLDataSource audio_src = {&android_simple_buffer_queue, &format_pcm};

    //configure audio sink
    SLDataLocator_OutputMix output_mix = {SL_DATALOCATOR_OUTPUTMIX, p_mix_obj};
    SLDataSink audio_snk = {&output_mix, 0};


    //create audio player
    SLInterfaceID interface_ids[4] = {SL_IID_BUFFERQUEUE,
                                      SL_IID_VOLUME,   //媒体的音量（不是手机全局的音量）
                                      SL_IID_PLAYBACKRATE, //变速
                                      SL_IID_MUTESOLO
    };
    SLboolean interface_required[4] = {SL_BOOLEAN_TRUE,
                                       SL_BOOLEAN_TRUE,
                                       SL_BOOLEAN_TRUE,
                                       SL_BOOLEAN_TRUE};
    result = (*p_engine)->CreateAudioPlayer(p_engine, &p_player_obj, &audio_src, &audio_snk, 4,
                                            interface_ids, interface_required);
    if (SL_RESULT_SUCCESS != result)
        LOGE(JNI_DEBUG, "CreateAudioPlayer res: %d", SL_RESULT_SUCCESS == result);

    //realize the player
    result = (*p_player_obj)->Realize(p_player_obj, SL_BOOLEAN_FALSE);
    if (SL_RESULT_SUCCESS != result)
        LOGE(JNI_DEBUG, "realize the player res: %d", SL_RESULT_SUCCESS == result);

    //get the play interface
    result = (*p_player_obj)->GetInterface(p_player_obj, SL_IID_PLAY, &p_sl_play_itf);
    if (SL_RESULT_SUCCESS != result)
        LOGE(JNI_DEBUG, "get the player interface res: %d", SL_RESULT_SUCCESS == result);

    result = (*p_player_obj)->GetInterface(p_player_obj, SL_IID_PLAY, &p_volume_itf);
    if (SL_RESULT_SUCCESS != result)
        LOGE(JNI_DEBUG, "get the voluem interface res: %d", SL_RESULT_SUCCESS == result);

    //设置缓存队列和回调函数
    result = (*p_player_obj)->GetInterface(p_player_obj, SL_IID_BUFFERQUEUE,
                                           &p_android_buffer_queue_itf);
    if (SL_RESULT_SUCCESS != result)
        LOGE(JNI_DEBUG, "get the buffer queue interface res: %d", SL_RESULT_SUCCESS == result);

    // 每次回调 this 会被带给 playerCallback 里面的 context
    result = (*p_android_buffer_queue_itf)->RegisterCallback(p_android_buffer_queue_itf,
                                                             buffer_queue_callback,
                                                             this);
    if (SL_RESULT_SUCCESS != result)
        LOGE(JNI_DEBUG, "register callback on the buffer queue res: %d",
             SL_RESULT_SUCCESS == result);

    // 设置播放状态
    result = (*p_sl_play_itf)->SetPlayState(p_sl_play_itf, SL_PLAYSTATE_PLAYING);
    if (SL_RESULT_SUCCESS != result)
        LOGE(JNI_DEBUG, "set the player's state to playing res: %d", SL_RESULT_SUCCESS == result);

    //调用回调函数
    buffer_queue_callback(p_android_buffer_queue_itf, this);
}


int AudioHandler::resample_audio() {

    int res;
    data_size = 0;
    while (p_player_status != NULL && !p_player_status->is_exit) {
        // 是不是暂停或者 seek 中
        if (p_player_status != NULL) {
            if (p_player_status->is_seeking) {
                av_usleep(1000 * 100);
                continue;
            }
        }

        // 根据队列中是否有数据来判断是否加载中
        if (p_avpacket_queue != NULL && p_avpacket_queue->empty()) {
            if (p_player_status != NULL &&
                p_player_status->current_state != PLAYER_STATUS_LOADING) {
                p_player_status->current_state = PLAYER_STATUS_LOADING;
                if (p_jni_player_call != NULL) {
                    p_jni_player_call->call_loading(THREAD_CHILD, true);
                }
            }
            av_usleep(1000 * 100);
            continue;
        } else {
            if (p_player_status != NULL &&
                p_player_status->current_state == PLAYER_STATUS_LOADING) {
                p_player_status->current_state = PLAYER_STATUS_PLAYING;
                if (p_jni_player_call != NULL) {
                    p_jni_player_call->call_loading(THREAD_CHILD, false);
                }
            }
        }
        AVPacket *p_avpacket;
        if (read_frame_finished) {
            p_avpacket = av_packet_alloc();
            if (p_avpacket_queue != NULL && p_avpacket_queue->pop(p_avpacket) != 0) {
                av_packet_free(&p_avpacket);
                av_free(p_avpacket);
                p_avpacket = NULL;
                continue;

            }

            // Packet 包，压缩的数据，转码成 pcm 数据
            res = avcodec_send_packet(p_avcodec_ctx_decoder_in, p_avpacket);
            if (res != 0) {
                av_packet_free(&p_avpacket);
                av_free(p_avpacket);
                p_avpacket = NULL;
                continue;

            }
        }

        //解码
        AVFrame *p_avframe = av_frame_alloc();
        res = avcodec_receive_frame(p_avcodec_ctx_decoder_in, p_avframe);
        if (res == 0) {
            read_frame_finished = false;
            if (p_avframe->channels && p_avframe->channel_layout == 0) {
                p_avframe->channel_layout = av_get_default_channel_layout(p_avframe->channels);
            } else if (p_avframe->channels == 0 && p_avframe->channel_layout > 0) {
                p_avframe->channels = av_get_channel_layout_nb_channels(p_avframe->channel_layout);
            }


            // ---------- ffmpeg 重采样  swr_convert----------
            int64_t out_ch_layout = AV_CH_LAYOUT_STEREO; //输出的通道 立体声
            enum AVSampleFormat out_sample_fmt = AVSampleFormat::AV_SAMPLE_FMT_S16; //输出的格式 16位 2字节
            int out_sample_rate = p_avframe->sample_rate; //输出的采样率
            int64_t in_ch_layout = p_avframe->channel_layout; //输入的通道
            enum AVSampleFormat in_sample_fmt = (AVSampleFormat) (p_avframe->format); //输入的格式
            int in_sample_rate = p_avframe->sample_rate; //输入的采样率
            SwrContext *p_swr_ctx = NULL;


            //设置重采样的参数
            p_swr_ctx = swr_alloc_set_opts(NULL,
                                           out_ch_layout,
                                           out_sample_fmt,
                                           out_sample_rate,
                                           in_ch_layout,
                                           in_sample_fmt,
                                           in_sample_rate,
                                           0, NULL);
            if (!p_swr_ctx || swr_init(p_swr_ctx) < 0) {

                av_packet_free(&p_avpacket);
                av_free(p_avpacket);
                p_avpacket = NULL;
                av_frame_free(&p_avframe);
                av_free(p_avframe);
                p_avframe = NULL;
                swr_free(&p_swr_ctx);
                read_frame_finished = true;
                continue;
            }


            // AVPacket -> AVFrame  pFrame->data就是pcm数据
            // 调用重采样的方法，返回值是返回重采样的个数，也就是 pFrame->nb_samples
            // swr_convert的重采样不能改变采样率，输入是多少，输出就要是多少，一般通过ffmpeg的avFilter改变采样率
            nb_out_sample_rate = swr_convert(p_swr_ctx,
                                             &p_convert_out_buffer,
                                             p_avframe->nb_samples,
                                             (const uint8_t **) p_avframe->data,//原始解压过后的数据
                                             p_avframe->nb_samples);

            int out_channels = av_get_channel_layout_nb_channels(AV_CH_LAYOUT_STEREO);
            //重采样的pcm数据大小（不同时间周期的数据大小，可能是一秒）
            //一般标准的数据 44100*2*2 代表1s的数据，通常计算时间间隔
            data_size = nb_out_sample_rate * out_channels *
                        av_get_bytes_per_sample(AV_SAMPLE_FMT_S16);


            int time = p_avframe->pts * av_q2d(time_base); //时间戳 pts单位：微秒
//                LOGE("avFrame->pts: %ld,time:%d", p_avframe->pts, time);
            if (time > current_time) {
                current_time = time;
            }
//            LOGE(JNI_DEBUG, "current_time:%lf", current_time);


            av_packet_free(&p_avpacket);
            av_free(p_avpacket);
            p_avpacket = NULL;
            av_frame_free(&p_avframe);
            av_free(p_avframe);
            p_avframe = NULL;
            swr_free(&p_swr_ctx);
            read_frame_finished = true;




//                dataSize = dataSize * 2 * 2;
            // write 写到缓冲区 pFrame.data -> javabyte
            // size 是多大，装 pcm 的数据
            // 1s 44100 点  2通道 ，2字节    44100*2*2
            // 1帧不是一秒，pFrame->nb_samples点
            break;
        } else {
            av_packet_free(&p_avpacket);
            av_free(p_avpacket);
            p_avpacket = NULL;
            av_frame_free(&p_avframe);
            av_free(p_avframe);
            p_avframe = NULL;
            read_frame_finished = true;
            continue;
            LOGE(JNI_DEBUG, "receive_frame_res %s", av_err2str(res));

        }

        //1. 解引用数据 data ， 2. 销毁 pPacket 结构体内存  3. pPacket = NULL
        //解引用 把指向的引用断开 不会释放pPacket 的内存 解引用数据 data
        av_packet_unref(p_avpacket);
        av_frame_unref(p_avframe);
    }

    return data_size;

}

int AudioHandler::get_current_sample_rate_for_opensles(int sample_rate) {
    int rate = 0;
    switch (sample_rate) {
        case 8000:
            rate = SL_SAMPLINGRATE_8;
            break;
        case 11025:
            rate = SL_SAMPLINGRATE_11_025;
            break;
        case 12000:
            rate = SL_SAMPLINGRATE_12;
            break;
        case 16000:
            rate = SL_SAMPLINGRATE_16;
            break;
        case 22050:
            rate = SL_SAMPLINGRATE_22_05;
            break;
        case 24000:
            rate = SL_SAMPLINGRATE_24;
            break;
        case 32000:
            rate = SL_SAMPLINGRATE_32;
            break;
        case 44100:
            rate = SL_SAMPLINGRATE_44_1;
            break;
        case 48000:
            rate = SL_SAMPLINGRATE_48;
            break;
        case 64000:
            rate = SL_SAMPLINGRATE_64;
            break;
        case 88200:
            rate = SL_SAMPLINGRATE_88_2;
            break;
        case 96000:
            rate = SL_SAMPLINGRATE_96;
            break;
        case 192000:
            rate = SL_SAMPLINGRATE_192;
            break;
        default:
            rate = SL_SAMPLINGRATE_44_1;
    }
    return rate;
}


void *thread_decode_start(void *ctx) {
    AudioHandler *context = (AudioHandler *) ctx;\
    context->init_opensles();
    pthread_exit(&context->start_thread);
}


void AudioHandler::start() {
    // 一个线程去解码播放
    pthread_create(&start_thread, NULL, thread_decode_start, this);
}

void AudioHandler::pause() {
    if (p_sl_play_itf != NULL) {
        p_player_status->current_state = PLAYER_STATUS_PAUSED;
        SLresult result = (*p_sl_play_itf)->SetPlayState(p_sl_play_itf, SL_PLAYSTATE_PAUSED);
//        LOGE(JNI_DEBUG, "create_engine_res: %d", SL_RESULT_SUCCESS == result);

    }
}

void AudioHandler::resume() {
    if (p_sl_play_itf != NULL) {
        p_player_status->current_state = PLAYER_STATUS_PLAYING;
        (*p_sl_play_itf)->SetPlayState(p_sl_play_itf, SL_PLAYSTATE_PLAYING);
    }
}

void AudioHandler::stop() {
    if (p_sl_play_itf != NULL) {
        p_player_status->current_state = PLAYER_STATUS_STOP;
        SLresult result = (*p_sl_play_itf)->SetPlayState(p_sl_play_itf, SL_PLAYSTATE_STOPPED);
//        LOGE(JNI_DEBUG, "create_engine_res: %d", SL_RESULT_SUCCESS == result);
    }
}


void AudioHandler::seek_to(uint64_t seconds) {

    MediaCore::seek_to(seconds);

}

void AudioHandler::set_volume(int percent) {

    if (p_volume_itf != NULL) {
        if (percent > 30) {
            (*p_volume_itf)->SetVolumeLevel(p_volume_itf, (100 - percent) * -20);
        } else if (percent > 25) {
            (*p_volume_itf)->SetVolumeLevel(p_volume_itf, (100 - percent) * -22);
        } else if (percent > 20) {
            (*p_volume_itf)->SetVolumeLevel(p_volume_itf, (100 - percent) * -25);
        } else if (percent > 15) {
            (*p_volume_itf)->SetVolumeLevel(p_volume_itf, (100 - percent) * -28);
        } else if (percent > 10) {
            (*p_volume_itf)->SetVolumeLevel(p_volume_itf, (100 - percent) * -30);
        } else if (percent > 5) {
            (*p_volume_itf)->SetVolumeLevel(p_volume_itf, (100 - percent) * -34);
        } else if (percent > 3) {
            (*p_volume_itf)->SetVolumeLevel(p_volume_itf, (100 - percent) * -37);
        } else if (percent > 0) {
            (*p_volume_itf)->SetVolumeLevel(p_volume_itf, (100 - percent) * -40);
        } else {
            (*p_volume_itf)->SetVolumeLevel(p_volume_itf, (100 - percent) * -100);
        }
    }

}

void AudioHandler::set_mute(int mute) {

}

void AudioHandler::set_pitch(float pitch) {

}

void AudioHandler::set_speed(float speed) {}


void AudioHandler::release() {
    if (p_player_obj != NULL) {
        (*p_player_obj)->Destroy(p_player_obj);
        p_player_obj = NULL;
        p_sl_play_itf = NULL;
        p_volume_itf = NULL;
        p_android_buffer_queue_itf = NULL;


    }


    if (p_mix_obj != NULL) {
        (*p_mix_obj)->Destroy(p_mix_obj);
        p_mix_obj = NULL;
        p_output_mix_environmental_reverb = NULL;
    }


    if (p_engine_obj != NULL) {
        (*p_engine_obj)->Destroy(p_engine_obj);
        p_engine_obj = NULL;
        p_engine = NULL;

    }


    if (p_convert_out_buffer) {
        free(p_convert_out_buffer);
        p_convert_out_buffer = NULL;
    }

    MediaCore::release();

//    if (seek_mutex != NULL) {
//        pthread_mutex_destroy(&seek_mutex);
//    }


}
