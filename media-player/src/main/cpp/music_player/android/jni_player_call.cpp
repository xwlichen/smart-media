/**
 * @date : 2019-11-20 13:50
 * @author: lichen
 * @email : 1960003945@qq.com
 * @description : 
 */
//
#include <log_utils.h>
#include "jni_player_call.h"


JNIPlayerCall::JNIPlayerCall(JavaVM *javaVM, JNIEnv *jniEnv, jobject jPlayerObj) {
    this->javaVM = javaVM;
    this->jniEnv = jniEnv;
    this->jPlayerObj = jniEnv->NewGlobalRef(jPlayerObj);

    jclass jPlayerClass = jniEnv->GetObjectClass(jPlayerObj);
    jPlayerErrorMid = jniEnv->GetMethodID(jPlayerClass, "onError", "(ILjava/lang/String;)V");
    jPlayerPreparedMid = jniEnv->GetMethodID(jPlayerClass, "onPrepared", "()V");
    jPlayerloadingMid = jniEnv->GetMethodID(jPlayerClass, "onLoaing", "(Z)V");
    jPlayerProgressMid = jniEnv->GetMethodID(jPlayerClass, "onCurrentProgressUpdate", "(II)V");
    jPlayerCompleteMid = jniEnv->GetMethodID(jPlayerClass, "onComplete", "()V");
    jPlayerNextMid = jniEnv->GetMethodID(jPlayerClass, "onCallNext", "()V");


}

JNIPlayerCall::~JNIPlayerCall() {
//    jniEnv->DeleteGlobalRef(jPlayerObj);
}

void JNIPlayerCall::call_error(ThreadMode thread_mode, int code, const char *msg) {
    // 子线程用不了主线程 jniEnv （native 线程）
    // 子线程是不共享 jniEnv ，他们有自己所独有的
    if (thread_mode == THREAD_MAIN) {
        jstring jMsg = jniEnv->NewStringUTF(msg);
        jniEnv->CallVoidMethod(jPlayerObj, jPlayerErrorMid, code, jMsg);
        jniEnv->DeleteLocalRef(jMsg);
    } else if (thread_mode == THREAD_CHILD) {
        // 获取当前线程的 JNIEnv， 通过 JavaVM
        JNIEnv *env;
        if (javaVM->AttachCurrentThread(&env, 0) != JNI_OK) {
            LOGE(JNI_DEBUG, "get child thread jniEnv error!");
            return;
        }

        jstring jMsg = env->NewStringUTF(msg);
        env->CallVoidMethod(jPlayerObj, jPlayerErrorMid, code, jMsg);
        env->DeleteLocalRef(jMsg);

        javaVM->DetachCurrentThread();
    }
}

/**
 * 回调到 java 层告诉准备好了
 * @param thread_mode
 */
void JNIPlayerCall::call_prepared(ThreadMode thread_mode) {
    LOGE(JNI_DEBUG, "jin call_prepared");

    // 子线程用不了主线程 jniEnv （native 线程）
    // 子线程是不共享 jniEnv ，他们有自己所独有的
    if (thread_mode == THREAD_MAIN) {
        jniEnv->CallVoidMethod(jPlayerObj, jPlayerPreparedMid);
    } else if (thread_mode == THREAD_CHILD) {
        // 获取当前线程的 JNIEnv， 通过 JavaVM
        JNIEnv *env;
        if (javaVM->AttachCurrentThread(&env, 0) != JNI_OK) {
            LOGE(JNI_DEBUG, "get child thread jniEnv error!");
            return;
        }
        env->CallVoidMethod(jPlayerObj, jPlayerPreparedMid);
        javaVM->DetachCurrentThread();
    }

}

void JNIPlayerCall::call_loading(ThreadMode thread_mode, bool loading) {
    if (thread_mode == THREAD_MAIN) {
        jniEnv->CallVoidMethod(jPlayerObj, jPlayerloadingMid, loading);
    } else if (thread_mode == THREAD_CHILD) {
        JNIEnv *env;
        if (javaVM->AttachCurrentThread(&env, 0) != JNI_OK) {
            LOGE(JNI_DEBUG, "get child thread jnienv error.");
            return;
        }
        env->CallVoidMethod(jPlayerObj, jPlayerloadingMid, loading);
        javaVM->DetachCurrentThread();
    }
}


void JNIPlayerCall::call_progress(ThreadMode thread_mode, int current, int total) {
    if (thread_mode == THREAD_MAIN) {
        jniEnv->CallVoidMethod(jPlayerObj, jPlayerProgressMid, current, total);
    } else if (thread_mode == THREAD_CHILD) {
        JNIEnv *env;
        if (javaVM->AttachCurrentThread(&env, 0) != JNI_OK) {
            LOGE(JNI_DEBUG, "get child thread jnienv error.");
            return;
        }

        env->CallVoidMethod(jPlayerObj, jPlayerProgressMid, current, total);

        javaVM->DetachCurrentThread();
    }
}

void JNIPlayerCall::call_complete(ThreadMode thread_mode) {
    if (thread_mode == THREAD_MAIN) {
        jniEnv->CallVoidMethod(jPlayerObj, jPlayerCompleteMid);
    } else if (thread_mode == THREAD_CHILD) {
        JNIEnv *env;
//        if(javaVM){
//            LOGE(JNI_DEBUG,"-------javaVM is not NULL");
//        }else{
//            LOGE(JNI_DEBUG,"-------javaVM is NULL");
//
//        }
        if (javaVM->AttachCurrentThread(&env, 0) != JNI_OK) {
            LOGE(JNI_DEBUG, "get child thread jnienv error.");
            return;
        }
        env->CallVoidMethod(jPlayerObj, jPlayerCompleteMid);
        javaVM->DetachCurrentThread();
    }
}


void JNIPlayerCall::call_next(ThreadMode thread_mode) {
    if (thread_mode == THREAD_MAIN) {
        jniEnv->CallVoidMethod(jPlayerObj, jPlayerNextMid);
    } else if (thread_mode == THREAD_CHILD) {
        JNIEnv *env;
//        if(javaVM){
//            LOGE(JNI_DEBUG,"-------javaVM is not NULL");
//        }else{
//            LOGE(JNI_DEBUG,"-------javaVM is NULL");
//
//        }
        if (javaVM->AttachCurrentThread(&env, 0) != JNI_OK) {
            LOGE(JNI_DEBUG, "get child thread jnienv error.");
            return;
        }
        env->CallVoidMethod(jPlayerObj, jPlayerNextMid);
        javaVM->DetachCurrentThread();
    }
}

