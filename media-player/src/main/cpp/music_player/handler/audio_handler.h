/**
 * @date : 2019-11-20 14:27
 * @author: lichen
 * @email : 1960003945@qq.com
 * @description : 
 */
//
#ifndef SMART_MEDIA_AUDIO_HANDLER_H
#define SMART_MEDIA_AUDIO_HANDLER_H


#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include "../core/media_core.h"

extern "C" {
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
};


class AudioHandler : public MediaCore {
public:


    pthread_t start_thread;
    uint8_t *p_convert_out_buffer = NULL;


    //引擎
    SLObjectItf p_engine_obj = NULL;
    SLEngineItf p_engine = NULL;
    SLVolumeItf p_volume_itf = NULL;//opensles 音量控制



    //混音器
    SLObjectItf p_mix_obj = NULL;
    SLEnvironmentalReverbItf p_output_mix_environmental_reverb = NULL;

    //pcm
    SLPlayItf p_sl_play_itf = NULL;//
    SLObjectItf p_player_obj = NULL;//



    //缓冲器队列接口
    SLAndroidSimpleBufferQueueItf p_android_buffer_queue_itf;


    int frame_buffer_size = 0;
    int sample_rate;
    int data_size;
    int nb_out_sample_rate;//重采样后得出输出的采样率


public:
    AudioHandler(int stream_index, PlayerStatus *player_status, JNIPlayerCall *p_jni_player_call);

    ~AudioHandler();

    int analysis_stream(ThreadMode thread_mode, AVFormatContext *p_avformat_ctx_in);


    void init_opensles();

    int resample_audio();

    int get_current_sample_rate_for_opensles(int sample_rate);

    void start();

    void pause();

    /**
     * 继续播放
     */
    void resume();

    /**
     * 退出
     */
    void stop();

    void seek_to(uint64_t seconds);

    void set_volume(int percent);

    void set_mute(int mute);

    void set_pitch(float pitch);

    void set_speed(float speed);


    void release();


};

#endif //SMART_MEDIA_AUDIO_HANDLER_H
