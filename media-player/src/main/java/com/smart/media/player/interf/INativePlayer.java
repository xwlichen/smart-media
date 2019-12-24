package com.smart.media.player.interf;

/**
 * @date : 2019-12-12 16:01
 * @author: lichen
 * @email : 1960003945@qq.com
 * @description :
 */
public interface INativePlayer extends IPlayer {


    void nativePrepare(String source);

    void nativeStart();

    void nativePause();

    void nativeSeek(int second);

    void nativeResume();

    void nativeStop();

    void nativeNext(String source);

    long nativeGetDuration();
}
