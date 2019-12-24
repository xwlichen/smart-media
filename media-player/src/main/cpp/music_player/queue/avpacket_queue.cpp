/**
 * @date : 2019-11-20 11:42
 * @author: lichen
 * @email : 1960003945@qq.com
 * @description : 
 */
//
#include <log_utils.h>
#include "avpacket_queue.h"

using namespace std;


AVPacketQueue::AVPacketQueue(PlayerStatus *p_player_status) {
    this->p_player_status = p_player_status;
    pthread_mutex_init(&packet_mutex, 0);
    pthread_cond_init(&packet_cond, 0);
}

AVPacketQueue::~AVPacketQueue() {
    clear();
    pthread_mutex_destroy(&packet_mutex);
    pthread_cond_destroy(&packet_cond);

}

int AVPacketQueue::push(AVPacket *p_packet) {
    pthread_mutex_lock(&packet_mutex);
    queue_avpacket.push(p_packet);
    if (p_packet) {
        buffer_size += p_packet->size;
//        LOGE(JNI_DEBUG,"current buffer size : %ld",buffer_size);
    }
    //通知可以已经满足条件使用（通知消费者）
    pthread_cond_signal(&packet_cond);
    pthread_mutex_unlock(&packet_mutex);

    return 0;
}

int AVPacketQueue::pop(AVPacket *p_packet) {
    pthread_mutex_lock(&packet_mutex);
    while (p_player_status != NULL && !p_player_status->is_exit) {
        if (queue_avpacket.size() > 0) {
            AVPacket *packet = queue_avpacket.front();
            //数据拷贝  （引用拷贝，packet里的data没有拷贝）
            if (av_packet_ref(p_packet, packet) == 0) {
                queue_avpacket.pop();
            }
            // 并没有释放 data ，只是解引用了 data，把AVPacket里的引用计数AVBufferRef *buf -1;
            //只有计数为0的时候，才把data真正的释放掉
            av_packet_free(&packet);
            av_free(packet);
            packet = NULL;


            // LOGE("从 queue_avpacket 队列移除，个数为 %d", queue_avpacket.size());
            break;

        } else {
            //通知消费者队列空了，请等待，packet_mutex会解锁
            pthread_cond_wait(&packet_cond, &packet_mutex);
        }
    }

    pthread_mutex_unlock(&packet_mutex);

    return 0;
}


int AVPacketQueue::size() {
    int size = 0;
    pthread_mutex_lock(&packet_mutex);
    size = queue_avpacket.size();
    pthread_mutex_unlock(&packet_mutex);
    return size;
}

bool AVPacketQueue::empty() {
    return queue_avpacket.empty();
}


void AVPacketQueue::clear() {

    //清除队列的每一个 AvPacket* 的内存数据
    pthread_cond_signal(&packet_cond);
    pthread_mutex_lock(&packet_mutex);

    while (!queue_avpacket.empty()) {
        AVPacket *av_packet = queue_avpacket.front();
        queue_avpacket.pop();
        av_packet_free(&av_packet);
        av_free(av_packet);
        av_packet = NULL;

    }

    pthread_mutex_unlock(&packet_mutex);

}
