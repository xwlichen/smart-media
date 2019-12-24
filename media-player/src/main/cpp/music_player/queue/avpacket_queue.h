/**
 * @date : 2019-11-20 11:42
 * @author: lichen
 * @email : 1960003945@qq.com
 * @description : 
 */
//
#ifndef SMART_MEDIA_AVPACKET_QUEUE_H
#define SMART_MEDIA_AVPACKET_QUEUE_H


#include <queue>
#include <pthread.h>
#include "../entity/player_status.h"

extern "C" {
#include <libavcodec/avcodec.h>
};


class AVPacketQueue {
public:
    //在频繁调用对象的场景下，直接queue_avpacket创建对象
    std::queue<AVPacket *> queue_avpacket;
    pthread_mutex_t packet_mutex;
    pthread_cond_t packet_cond;
    PlayerStatus *p_player_status;

    long buffer_size = 0;

public:
    AVPacketQueue(PlayerStatus *p_player_status);

    ~AVPacketQueue();

public:
    int push(AVPacket *packet);

    int pop(AVPacket *packet);

    int size();

    /**
     * 判空
     * @return
     */
    bool empty();

    void clear();


};

#endif //SMART_MEDIA_AVPACKET_QUEUE_H
