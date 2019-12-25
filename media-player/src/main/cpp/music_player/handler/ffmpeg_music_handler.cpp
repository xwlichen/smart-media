/**
 * @date : 2019-11-19 17:54
 * @author: lichen
 * @email : 1960003945@qq.com
 * @description : 
 */
//
#include <pthread.h>
#include <log_utils.h>
#include <s_error.h>
#include "ffmpeg_music_handler.h"


FFmpegMusicHandler::FFmpegMusicHandler(const char *input_url_, PlayerStatus *p_player_status_,
                                       JNIPlayerCall *p_jni_player_call_) {
    this->p_jni_player_call = p_jni_player_call_;
    this->p_player_status = p_player_status_;
    // 赋值一份 input_url ，因为怕外面方法结束销毁了 input_url
    this->input_url = (char *) malloc(strlen(input_url_) + 1);
    memcpy(this->input_url, input_url_, strlen(input_url_) + 1);

    exit_flag = false;

    pthread_mutex_init(&init_mutex, NULL);
    pthread_mutex_init(&seek_mutex, NULL);
    pthread_mutex_init(&decode_mutex, NULL);


}

FFmpegMusicHandler::~FFmpegMusicHandler() {
    pthread_mutex_destroy(&init_mutex);
    pthread_mutex_destroy(&seek_mutex);
    pthread_mutex_destroy(&decode_mutex);


}


void *thread_prepare(void *context) {
    FFmpegMusicHandler *ffmpeg_music_handler = (FFmpegMusicHandler *) context;
    ffmpeg_music_handler->prepare_config(THREAD_CHILD);
    pthread_exit(&ffmpeg_music_handler->prepare_thread);
}


void FFmpegMusicHandler::prepare_async() {
    pthread_create(&prepare_thread, NULL, thread_prepare, this);

}

int avformat_callback(void *ctx) {
    FFmpegMusicHandler *fFmpeg = (FFmpegMusicHandler *) ctx;
    if (fFmpeg->p_player_status->is_exit) {
        return AVERROR_EOF;
    }
    return 0;
}

void FFmpegMusicHandler::prepare_config(ThreadMode thread_mode) {
    pthread_mutex_lock(&init_mutex);
    int res;


    av_register_all();
    //初始化网路   与avformat_network_deinit成对出现
    avformat_network_init();
    p_avformat_ctx_in = avformat_alloc_context();
    AVDictionary *av_dict = NULL;
    av_dict_set(&av_dict, "timeout", "10000000", 0);//设置超时10秒
    //打开文件流，结束时必须关闭stream avformat_close_input
    //LOGE(JNI_DEBUG, "input_url: %s", input_url);

    p_avformat_ctx_in->interrupt_callback.callback = avformat_callback;
    p_avformat_ctx_in->interrupt_callback.opaque = this;


    res = avformat_open_input(&p_avformat_ctx_in, input_url, NULL, &av_dict);
    if (res < 0) {
        LOGE(JNI_DEBUG, "can't open input_url : %s, %s", input_url, av_err2str(res));
        call_player_jni_error(thread_mode, true, res, av_err2str(res));
        return;
    }


    LOGE(JNI_DEBUG, "open input_url : %s", input_url);


    //查找流的信息
    res = avformat_find_stream_info(p_avformat_ctx_in, NULL);
    if (res < 0) {
        LOGE(JNI_DEBUG, "Can't find stream info input_url : %s, %s", input_url,
             av_err2str(res));
        call_player_jni_error(thread_mode, true, res, av_err2str(res));
        return;
    }
    duration = p_avformat_ctx_in->duration / AV_TIME_BASE;


//    LOGE(JNI_DEBUG, "output_url: %s", output_url)
//
//    res = avformat_alloc_output_context2(&p_avformat_ctx_out, NULL, "mpegts", output_url);
//
//    if (res < 0) {
//        LOGE(JNI_DEBUG, "avformat_alloc_output_context2 error code : %d ,msg : %s", res,
//             av_err2str(res));
//        close_output();
//        call_player_jni_error(thread_mode, false, res, av_err2str(res));
//    }
//    // 创建并初始化一个AVIOContext,用于访问url资源
//    // app需要给存储权限，否则ret=-13
//    res = avio_open2(&p_avformat_ctx_out->pb, output_url, AVIO_FLAG_WRITE, NULL, NULL);
//    if (res < 0) {
//        LOGE(JNI_DEBUG, " open output ctx io error code : %d ,msg : %s", res, av_err2str(res));
//        close_output();
//        call_player_jni_error(thread_mode, false, res, av_err2str(res));
//    }



    // 查找音频流的 index   从文件流中查找音频流index（视频流、字幕流）
    int audio_stream_index = av_find_best_stream(p_avformat_ctx_in,
                                                 AVMediaType::AVMEDIA_TYPE_AUDIO,
                                                 -1,
                                                 -1,
                                                 NULL, 0);
    if (audio_stream_index < 0) {
        LOGE(JNI_DEBUG, "format audio stream error.");
        call_player_jni_error(thread_mode, true, ERROR_CODE_FIND_STREAM,
                              "format audio stream error");
        return;
    }


    double size = (p_avformat_ctx_in->duration * 1.0 / AV_TIME_BASE *
                   p_avformat_ctx_in->bit_rate / 8.0);
//    LOGE(JNI_DEBUG, "file size: %lf", size)


    // 初始化一些解码器之类的准备工作
    p_audio = new AudioHandler(audio_stream_index, p_player_status, p_jni_player_call);
    res = p_audio->analysis_stream(thread_mode, p_avformat_ctx_in);

    if (res < 0) {
        LOGE(JNI_DEBUG, "analysis_stream error.");
        call_player_jni_error(thread_mode, true, ERROR_CODE_FIND_STREAM,
                              "analysis_stream");
    }


    // 回调到 Java 告诉他准备好了
    LOGE(JNI_DEBUG, "prepared");
    if (p_player_status != NULL && !p_player_status->is_exit) {
        p_jni_player_call->call_prepared(thread_mode);
    } else {
        exit_flag = true;
        LOGE(JNI_DEBUG, "exit true start");

    }
    pthread_mutex_unlock(&init_mutex);

}


void FFmpegMusicHandler::start() {
    if (p_audio == NULL) {
        LOGE(JNI_DEBUG, "p_audio == NULL");
        return;
    }
    p_audio->start();

    decode_frame_to_avpacket();

}

void FFmpegMusicHandler::pause() {
    if (p_audio != NULL) {
        p_audio->pause();
    }

}

void FFmpegMusicHandler::seek_to(int64_t seconds) {
    if (p_player_status != NULL) {
        p_player_status->is_seeking = true;
    }


    if (seconds >= 0) {
        if (p_audio != NULL) {
            p_audio->seek_to(seconds);
        }
        pthread_mutex_lock(&seek_mutex);

        int64_t rel = seconds * AV_TIME_BASE;
        int res = avformat_seek_file(p_avformat_ctx_in, -1, INT64_MIN, rel, INT64_MAX, 0);
//        LOGE(JNI_DEBUG, "avformat_seek_file res :%d ,msg : %s", res, av_err2str(res));

        AVDictionary *av_dict = NULL;
        av_dict = p_avformat_ctx_in->metadata;
        if (av_dict != NULL) {
            AVDictionaryEntry *t = NULL;
            while ((t = av_dict_get(av_dict, "", t, AV_DICT_IGNORE_SUFFIX))) {
                LOGE(JNI_DEBUG, "%s: %s", t->key, t->value);
            }

        } else {
            LOGE(JNI_DEBUG, "av_dict is NUll")
        }

        pthread_mutex_unlock(&seek_mutex);

        if (p_player_status != NULL) {
            p_player_status->is_seeking = false;
        }

    }


}

void FFmpegMusicHandler::resume() {
    if (p_audio != NULL) {
        p_audio->resume();
    }
}


void FFmpegMusicHandler::set_volume(int percent) {
    if (p_audio != NULL) {
        p_audio->set_volume(percent);
    }

}

void FFmpegMusicHandler::set_mute(int mute) {
    if (p_audio != NULL) {
        p_audio->set_mute(mute);
    }

}

void FFmpegMusicHandler::set_pitch(float pitch) {
    if (p_audio != NULL) {
        p_audio->set_pitch(pitch);
    }
}

void FFmpegMusicHandler::set_speed(float speed) {
    if (p_audio != NULL) {
        p_audio->set_speed(speed);
    }
}


void FFmpegMusicHandler::decode_frame_to_avpacket() {
    int res = 0;
    while (p_player_status != NULL && !p_player_status->is_exit) {

        if (p_player_status->is_seeking) {
            av_usleep(1000 * 100); //暂停100毫秒

            continue;
        }
        if (p_audio->p_avpacket_queue->size() > 40) {
            av_usleep(1000 * 100);
            continue;
        }

        AVPacket *av_packet = av_packet_alloc();
        av_init_packet(av_packet);

        if (p_avformat_ctx_in != NULL && av_packet != NULL) {
            pthread_mutex_lock(&seek_mutex);
            res = av_read_frame(p_avformat_ctx_in, av_packet);
            pthread_mutex_unlock(&seek_mutex);
        } else {
            res = -1;
        }


        if (res >= 0) {
            if (p_audio->stream_index == av_packet->stream_index) {
                p_audio->p_avpacket_queue->push(av_packet);

            } else {
                av_packet_free(&av_packet);
                av_free(av_packet);
            }
        } else {
            av_packet_free(&av_packet);
            av_free(av_packet);
            while (p_player_status != NULL && !p_player_status->is_exit) {
                if (p_audio->p_avpacket_queue->size() > 0) {
                    av_usleep(1000 * 100);
                    continue;
                } else {
                    p_player_status->is_exit = true;
                    break;
                }
            }
            break;
//            if (res == -541478725) {
//                break;
//            }

        }
    }

    exit_flag = true;

}


void FFmpegMusicHandler::call_player_jni_error(ThreadMode thread_mode, bool can_release, int code,
                                               char *msg) {
    exit_flag = true;
    LOGE(JNI_DEBUG, "exit true call_player_jni_error");

    if (can_release) {
//        release();
    }
    if (p_jni_player_call != NULL) {
        // 回调给 java 层调用
        p_jni_player_call->call_error(thread_mode, code, msg);
    }

    pthread_mutex_unlock(&init_mutex);

}

void FFmpegMusicHandler::release() {

    LOGE(JNI_DEBUG, "release ");

    p_player_status->is_exit = true;
    pthread_mutex_lock(&init_mutex);
    int sleepCount = 0;
    while (!exit_flag) {
        if (sleepCount > 1000) {
            exit_flag = true;
        }
        if (JNI_DEBUG) {
            LOGE(JNI_DEBUG, "wait ffmpeg  exit %d", sleepCount);
        }
        sleepCount++;
        av_usleep(1000 * 10);//暂停10毫秒
    }


    if (p_audio != NULL) {
        p_audio->release();
        delete p_audio;
        p_audio = NULL;
    }

    if (p_avformat_ctx_in != NULL) {
        avformat_close_input(&p_avformat_ctx_in);
        avformat_free_context(p_avformat_ctx_in);
        p_avformat_ctx_in = NULL;
    }

    if (p_player_status) {
        p_player_status = NULL;
    }

    if (p_jni_player_call) {
        p_jni_player_call = NULL;
    }


    if (input_url != NULL) {
        free(input_url);
        input_url = NULL;
    }

    pthread_mutex_unlock(&init_mutex);

}
