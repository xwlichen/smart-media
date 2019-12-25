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
#include <soundtouch_wrapper.h>
#include "../core/media_core.h"
#include "../../common/constant.h"

extern "C" {
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
};


class AudioHandler : public MediaCore {
public:


    //引擎
    SLObjectItf p_engine_obj = NULL;
    SLEngineItf p_engine = NULL;


    //混音器
    SLObjectItf p_mix_obj = NULL;
    SLEnvironmentalReverbItf p_output_mix_environmental_reverb = NULL;

    //pcm
    SLPlayItf p_sl_play_itf = NULL;//
    SLObjectItf p_player_obj = NULL;//
    SLMuteSoloItf p_mute_itf = NULL;//声道控制
    SLVolumeItf p_volume_itf = NULL;//opensles 音量控制


    //缓冲器队列接口
    SLAndroidSimpleBufferQueueItf p_android_buffer_queue_itf;


    //SoundTouch
    SoundTouchWrapper *p_soundtouchwrapper = NULL;
    SAMPLETYPE *sample_buffer = NULL; //soundtouch 处理输出的数据
    int nb_soundtouch;//soundtouch 处理输出的数据的采样率
    uint8_t *temp_soundtouch_buffer;


    //音频相关参数
    int volume = 100;//百分比
    int mute = MEDIA_MUTE_CENTER; //声道
    float pitch = 1.0f;//音调
    float speed = 1.0f;//播放音频速度


    pthread_t start_thread;
    uint8_t *convert_out_buffer = NULL; //重采样输出的数据
    int sample_rate;
    int data_size; //每帧的数据大小
    int nb_out_sample_rate;//重采样后得出输出的采样率

    bool read_frame_finished = true;//是否读帧结束
    bool soundtouch_finished = true;//是否soundtouch 处理结束


public:
    AudioHandler(int stream_index, PlayerStatus *player_status, JNIPlayerCall *p_jni_player_call);

    ~AudioHandler();

    int analysis_stream(ThreadMode thread_mode, AVFormatContext *p_avformat_ctx_in);


    void init_opensles();

    /**
     * 获取soundtouch 处理过的数据
     * @return
     */
    int get_soundtouch_data();

    int resample_audio(void **pcmbuf);

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
