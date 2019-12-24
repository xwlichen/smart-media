/**
 * @date : 2019-11-19 16:43
 * @author: lichen
 * @email : 1960003945@qq.com
 * @description : 
 */
//

#include <jni.h>
#include <log_utils.h>
#include "jni_player_call.h"
#include "../handler/ffmpeg_music_handler.h"


JNIPlayerCall *p_jni_call;
FFmpegMusicHandler *ffmpeg_music_handler;
PlayerStatus *p_player_status;
JavaVM *pJavaVM = NULL;
pthread_t thread_start;
bool nexit = true;



// 重写 so 被加载时会调用的一个方法
// 去了解动态注册
extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *javaVM, void *reserved) {
    pJavaVM = javaVM;
    JNIEnv *env;
    if (javaVM->GetEnv((void **) &env, JNI_VERSION_1_4) != JNI_OK) {
        return -1;
    }
    return JNI_VERSION_1_4;
}


extern "C"
JNIEXPORT void JNICALL
Java_com_smart_media_player_natives_NativeMusicPlayer_nPrepare(JNIEnv *env, jobject instance,
                                                               jstring input_url_) {
    const char *input_url = env->GetStringUTFChars(input_url_, 0);


    if (p_jni_call == NULL) {
        p_jni_call = new JNIPlayerCall(pJavaVM, env, instance);
    }

    if (ffmpeg_music_handler == NULL) {
        p_player_status = new PlayerStatus();
        ffmpeg_music_handler = new FFmpegMusicHandler(input_url, p_player_status, p_jni_call);
        ffmpeg_music_handler->prepare_async();

    }

    env->ReleaseStringUTFChars(input_url_, input_url);
}

void *startCallBack(void *data) {
    FFmpegMusicHandler *fFmpeg = (FFmpegMusicHandler *) data;
    fFmpeg->start();
    pthread_exit(&thread_start);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_smart_media_player_natives_NativeMusicPlayer_nStart(JNIEnv *env, jobject thiz) {
    if (ffmpeg_music_handler != NULL) {
        pthread_create(&thread_start, NULL, startCallBack, ffmpeg_music_handler);
    }
}


extern "C"
JNIEXPORT void JNICALL
Java_com_smart_media_player_natives_NativeMusicPlayer_nPause(JNIEnv *env, jobject thiz) {
    if (ffmpeg_music_handler != NULL) {
        ffmpeg_music_handler->pause();
    }
}



extern "C"
JNIEXPORT void JNICALL
Java_com_smart_media_player_natives_NativeMusicPlayer_nSeekTo(JNIEnv *env, jobject thiz,
                                                              jint time) {
    if (ffmpeg_music_handler != NULL) {
        ffmpeg_music_handler->seek_to(time);
    }
}


extern "C"
JNIEXPORT void JNICALL
Java_com_smart_media_player_natives_NativeMusicPlayer_nResume(JNIEnv *env, jobject thiz) {
    if (ffmpeg_music_handler != NULL) {
        ffmpeg_music_handler->resume();
    }
}






extern "C"
JNIEXPORT void JNICALL
Java_com_smart_media_player_natives_NativeMusicPlayer_nStop(JNIEnv *env, jobject instance) {

    if (!nexit) {
        return;
    }

    jclass clz = env->GetObjectClass(instance);
    jmethodID jmid_next = env->GetMethodID(clz, "onCallNext", "()V");
    nexit = false;
    if (ffmpeg_music_handler != NULL) {
        ffmpeg_music_handler->release();
        delete (ffmpeg_music_handler);
        ffmpeg_music_handler = NULL;
        if (p_jni_call) {
            delete (p_jni_call);
            p_jni_call = NULL;
        }

        if (p_player_status) {
            delete (p_player_status);
            p_player_status = NULL;
        }

    }
    nexit = true;
    env->CallVoidMethod(instance, jmid_next);

}


extern "C"
JNIEXPORT jint JNICALL
Java_com_smart_media_player_natives_NativeMusicPlayer_nGetDuration(JNIEnv *env, jobject thiz) {
    if (ffmpeg_music_handler != NULL) {
        return ffmpeg_music_handler->duration;
    }

    return 0;


}

extern "C"
JNIEXPORT void JNICALL
Java_com_smart_media_player_natives_NativeMusicPlayer_nSetVolume(JNIEnv *env, jobject thiz,
                                                                 jint percent) {
    if (ffmpeg_music_handler != NULL) {
        ffmpeg_music_handler->set_volume(percent);
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_smart_media_player_natives_NativeMusicPlayer_nSetMute(JNIEnv *env, jobject thiz,
                                                               jint mute) {
    // TODO: implement nSetMute()
}

extern "C"
JNIEXPORT void JNICALL
Java_com_smart_media_player_natives_NativeMusicPlayer_nSetPitch(JNIEnv *env, jobject thiz,
                                                                jfloat pitch) {
    // TODO: implement nSetPitch()
}

extern "C"
JNIEXPORT void JNICALL
Java_com_smart_media_player_natives_NativeMusicPlayer_nSetSpeed(JNIEnv *env, jobject thiz,
                                                                jfloat speed) {
    // TODO: implement nSetSpeed()
}