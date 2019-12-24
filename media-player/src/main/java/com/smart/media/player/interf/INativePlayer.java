package com.smart.media.player.interf;

/**
 * @date : 2019-12-24 16:34
 * @author: lichen
 * @email : 1960003945@qq.com
 * @description :
 */
public interface INativePlayer extends IPlayer {

    void onPrepared();

    void onPause();

    void onLoaing(boolean loading);

    void onCurrentProgressUpdate(int current, int duration);

    void onComplete();

    void onError(int code, String msg);
}
