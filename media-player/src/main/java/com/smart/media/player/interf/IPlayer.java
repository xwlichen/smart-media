package com.smart.media.player.interf;


import com.smart.media.player.bean.InfoBean;

/**
 * @date : 2019-09-04 18:02
 * @author: lichen
 * @email : 1960003945@qq.com
 * @description :
 */
public interface IPlayer {

    void prepare(String source);

    void start();

    void pause();

    void seek(int second);

    void resume();

    void next(String source);

    void stop();


    long getDuration();


    void setOnPreparedListener(IPlayer.OnPreparedListener onPreparedListener);

    void setOnInfoListener(IPlayer.OnInfoListener onInfoListener);

    void setOnErrorListener(IPlayer.OnErrorListener onErrorListener);


    interface OnPreparedListener {
        void onPrepared();
    }

    interface OnLoadingStatusListener {
        void onLoadingStart();

        /**
         * @param percent  百分比
         * @param netSpeed kbs
         */
        void onLoadingProgress(int percent, float netSpeed);

        void onLoadingEnd();

    }

    interface OnPauseListener {
        void onPause();
    }

    interface OnInfoListener {
        void onInfo(InfoBean data);

    }

    interface OnCompleteListener {
        void onComplete();
    }


    interface OnErrorListener {
        void onError(int code, String msg);
    }

}
