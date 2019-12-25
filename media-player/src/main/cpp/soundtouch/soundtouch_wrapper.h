/**
 * @date : 2019-12-25 10:54
 * @author: lichen
 * @email : 1960003945@qq.com
 * @description : 
 */
//
#ifndef SMART_MEDIA_SOUNDTOUCH_WRAPPER_H
#define SMART_MEDIA_SOUNDTOUCH_WRAPPER_H

#include <stdint.h>
#include "include/SoundTouch.h"

using namespace std;


using namespace soundtouch;

class SoundTouchWrapper {

public:
    SoundTouchWrapper();

    virtual ~SoundTouchWrapper();

    // 初始化
    void create();

    // 转换
    int
    translate(short *data, float speed, float pitch, int len, int bytes_per_sample, int n_channel,
              int n_sampleRate);

    // 销毁
    void destroy();

    // 获取SoundTouch对象
    SoundTouch *get_soundtouch();

private:
    SoundTouch *soundtouch;
};


#endif //SMART_MEDIA_SOUNDTOUCH_WRAPPER_H
