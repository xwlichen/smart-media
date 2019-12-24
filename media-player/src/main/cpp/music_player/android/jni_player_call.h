/**
 * @date : 2019-11-20 13:50
 * @author: lichen
 * @email : 1960003945@qq.com
 * @description : 
 */
//
#ifndef SMART_MEDIA_JNI_PLAYER_CALL_H
#define SMART_MEDIA_JNI_PLAYER_CALL_H

#include <jni.h>


enum ThreadMode {
    THREAD_CHILD, THREAD_MAIN
};

class JNIPlayerCall {
public:
    JavaVM *javaVM;
    JNIEnv *jniEnv;
    jmethodID jPlayerErrorMid;
    jmethodID jPlayerPreparedMid;
    jmethodID jPlayerloadingMid;
    jmethodID jPlayerProgressMid;
    jmethodID jPlayerCompleteMid;
    jmethodID jPlayerNextMid;




    jobject jPlayerObj;
public:
    JNIPlayerCall(JavaVM *javaVM, JNIEnv *jniEnv, jobject jPlayerObj);

    ~JNIPlayerCall();

public:
    void call_error(ThreadMode thread_mode, int code, const char *msg);

    void call_prepared(ThreadMode thread_mode);

    void call_loading(ThreadMode thread_mode, bool loading);

    void call_progress(ThreadMode thread_mode, int current, int total);

    void call_complete(ThreadMode thread_mode);

    void call_next(ThreadMode thread_mode);


};

#endif //SMART_MEDIA_JNI_PLAYER_CALL_H
