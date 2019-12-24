/**
 * @date : 2019-11-19 17:54
 * @author: lichen
 * @email : 1960003945@qq.com
 * @description : 
 */
//
#ifndef SMART_MEDIA_FFMPEG_MUSIC_HANDLER_H
#define SMART_MEDIA_FFMPEG_MUSIC_HANDLER_H


#include "../android/jni_player_call.h"
#include "../entity/player_status.h"
#include "audio_handler.h"

extern "C" {
#include "libavformat/avformat.h"
#include "libswresample/swresample.h"
};

class FFmpegMusicHandler {
public:

    AVFormatContext *p_avformat_ctx_in = NULL;
    /**
    * 输出信息的上下文
    * 用于解码播放的时候缓存到手机本地
    */
//    AVFormatContext *p_avformat_ctx_out = NULL;


    char *input_url = NULL;

    pthread_t prepare_thread;
    PlayerStatus *p_player_status;
    JNIPlayerCall *p_jni_player_call = NULL;
    AudioHandler *p_audio = NULL;

    bool exit_flag = false;

    /**
     * 单位：s
     */
    int duration;


    pthread_mutex_t init_mutex;

    pthread_mutex_t seek_mutex;

    pthread_mutex_t decode_mutex;


public:
    FFmpegMusicHandler(const char *input_url, PlayerStatus *p_player_status,
                       JNIPlayerCall *p_jni_player_call);

    ~FFmpegMusicHandler();

public:


    void prepare_async();

    void prepare_config(ThreadMode threadMode);

    void decode_frame_to_avpacket();

    void start();

    void pause();

    void resume();

    void seek_to(int64_t seconds);


    void set_volume(int percent);

    void set_mute(int mute);

    void set_pitch(float pitch);

    void set_speed(float speed);


    void release();

    void call_player_jni_error(ThreadMode threadMode, bool can_release, int code, char *msg);


};

#endif //SMART_MEDIA_FFMPEG_MUSIC_HANDLER_H
