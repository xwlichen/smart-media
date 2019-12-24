/**
 * @date : 2019-11-20 14:27
 * @author: lichen
 * @email : 1960003945@qq.com
 * @description : 
 */
//
#ifndef SMART_MEDIA_MEDIA_CORE_H
#define SMART_MEDIA_MEDIA_CORE_H


#include "../entity/player_status.h"
#include "../queue/avpacket_queue.h"
#include "../android/jni_player_call.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include "libavutil/time.h"
};


/**
 * 媒体播放器公共类
 */
class MediaCore {
public:
    /**
     * 解码器上下文
     */
    AVCodecContext *p_avcodec_ctx_decoder_in = NULL;


    /**
     * 每帧的timebase
     * av_q2d(AVRational a)函数
     * av_q2d(AVRational);该函数负责把AVRational结构转换成double，通过这个函数可以计算出某一帧在视频中的时间位置
     * st=format_context-streams[index]
     * timebase=st->timebase
     * timestamp(秒) = av_frame->pts * av_q2d(time_base);
     * 计算视频长度的方法：
     * time(秒) = st->duration * av_q2d(time_base);
     */
    AVRational time_base;


    /**
     * 当前流的角标（音频/视频/字幕）
     */
    int stream_index;

    /**
     * 播放状态
     */
    PlayerStatus *p_player_status = NULL;

    /**
     * AVPacket 队列
     */
    AVPacketQueue *p_avpacket_queue = NULL;


    /**
     * 总时长 单位：秒
     */
    int duration = 0;


    /**
     * 当前播放时刻 单位：秒
     */
    double current_time = 0;


    /**
     * 上次更新的时间，主要用于控制回调到 Java 层的频率
     * 单位：秒
     */
    double last_update_time = 0;


    /**
     * 是否读帧结束
     */
    bool read_frame_finished = true;

    /**
     * 回调java层
     */
    JNIPlayerCall *p_jni_player_call;


    /**
     * seek时的互斥锁
     */
    pthread_mutex_t seek_mutex;

public:
    MediaCore(int stream_index, PlayerStatus *p_player_status, JNIPlayerCall *p_jni_player_call);

    ~MediaCore();

public:


    /**
     * 解析公共的解码器上下文
     * 虚函数
     * @param thread_mode
     * @param p_avformat_context  输入媒体相关信息上下文
     * @param p_avformat_ctx_out 输出保存的文件信息上下文
     */
    virtual int analysis_stream(ThreadMode thread_mode, AVFormatContext *p_avformat_ctx_in);


    /**
    * 播放方法
    * 通过使虚函数=0来定义纯虚函数
    * 纯虚函数使用场景：
    * 1，当想在基类中抽象出一个方法，且该基类只做能被继承，而不能被实例化；
    * 2，这个方法必须在派生类(derived class)中被实现；
    */
    virtual void start() = 0;

    /**
     * 进度跳转
     * 主要清空队列，在ffmpeg_utils经过av_seek_frame对av_format_contextseek定位
     * 虚函数
     * @param seconds 单位：秒
     */
    virtual void seek_to(uint64_t seconds);


    /**
     * 释放资源
     */
    void release();


    /**
     * 解析数据过程中出错的回调
     * @param thread_mode
     * @param code
     * @param msg
     */
    void call_player_jni_error(ThreadMode thread_mode, int code, const char *msg);
};


#endif //SMART_MEDIA_MEDIA_CORE_H
