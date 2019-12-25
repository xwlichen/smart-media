/**
 * @date : 2019-12-25 10:54
 * @author: lichen
 * @email : 1960003945@qq.com
 * @description : 
 */
//
#include "soundtouch_wrapper.h"

SoundTouchWrapper::SoundTouchWrapper() {
    create();
}

SoundTouchWrapper::~SoundTouchWrapper() {
    destroy();
}

void SoundTouchWrapper::create() {
    soundtouch = new SoundTouch();
}

void SoundTouchWrapper::destroy() {
    if (soundtouch) {
        soundtouch->clear();
        delete soundtouch;
        soundtouch = NULL;
    }
}

/**
 * 转换
 * @param data              待采样PCM数据
 * @param speed             速度
 * @param pitch             音调
 * @param len               长度
 * @param bytes_per_sample  采样字节数
 * @param n_channel         声道数
 * @param n_sampleRate      采样率
 * @return
 */
int SoundTouchWrapper::translate(short *data, float speed, float pitch, int len,
                                 int bytes_per_sample, int n_channel, int n_sampleRate) {
    // 每个声道采样数量
    int put_n_sample = len / n_channel;
    int nb = 0;
    int pcm_data_size = 0;
    if (soundtouch == NULL) {
        return 0;
    }
    // 设置音调
    soundtouch->setPitch(pitch);
    // 设置速度
    soundtouch->setRate(speed);
    // 设置采样率
    soundtouch->setSampleRate(n_sampleRate);
    // 设置声道数
    soundtouch->setChannels(n_channel);
    // 压入采样数据
    soundtouch->putSamples((SAMPLETYPE *) data, put_n_sample);

    do {
        // 获取转换后的数据
        nb = soundtouch->receiveSamples((SAMPLETYPE *) data, n_sampleRate / n_channel);
        // 计算转换后的数量大小
        pcm_data_size += nb * n_channel * bytes_per_sample;
    } while (nb != 0);

    // 返回转换后的数量大小
    return pcm_data_size;
}

/**
 * 获取SoundTouch对象
 * @return
 */
SoundTouch *SoundTouchWrapper::get_soundtouch() {
    return soundtouch;
}
