/**
 * @date : 2019-10-23 10:04
 * @author: lichen
 * @email : 1960003945@qq.com
 * @description : 
 */
//
#ifndef SMART_MEDIA_LOG_UTILS_H
#define SMART_MEDIA_LOG_UTILS_H

#include <android/log.h>

static int JNI_DEBUG = 1;
#define TAG "JNI_TAG_PLAYER"
#define LOGE(debug, format, ...)  if(debug){__android_log_print(ANDROID_LOG_ERROR,TAG,format,##__VA_ARGS__);}

//void set_debug(int debug) {
//    JNI_DEBUG = debug;
//}

#endif //SMART_MEDIA_LOG_UTILS_H
