package com.smart.media.player.natives;

import com.smart.media.player.bean.InfoBean;
import com.smart.media.player.interf.IPlayer;


/**
 * @date : 2019-09-04 15:35
 * @author: lichen
 * @email : 1960003945@qq.com
 * @description :
 */
public class NativeMusicPlayer extends NativePlayerBase {


    private InfoBean infoBean;
    private boolean playNext;
    protected String source;


    static {
        System.loadLibrary("smart-player");

//        System.loadLibrary("avcodec");
//        System.loadLibrary("avdevice");
//        System.loadLibrary("avfilter");
//        System.loadLibrary("avformat");
//        System.loadLibrary("avutil");
//        System.loadLibrary("postproc");
//        System.loadLibrary("swresample");
//        System.loadLibrary("swscale");
    }

    @Override
    public void setOnPreparedListener(IPlayer.OnPreparedListener onPreparedListener) {
        this.onPreparedListener = onPreparedListener;
    }

    @Override
    public void setOnErrorListener(IPlayer.OnErrorListener onErrorListener) {
        this.onErrorListener = onErrorListener;
    }

    public void setOnPauseListener(IPlayer.OnPauseListener onPauseListener) {
        this.onPauseListener = onPauseListener;
    }

    public void setOnLoadingStatusListener(IPlayer.OnLoadingStatusListener onLoadingStatusListener) {
        this.onLoadingStatusListener = onLoadingStatusListener;
    }

    @Override
    public void setOnInfoListener(IPlayer.OnInfoListener onInfoListener) {
        this.onInfoListener = onInfoListener;
        if (infoBean == null) {
            infoBean = new InfoBean();
        }
    }

    public void setOnCompleteListener(IPlayer.OnCompleteListener onCompleteListener) {
        this.onCompleteListener = onCompleteListener;
    }

    //-----------------called from jni -----------------


    private void onCallNext() {
        if (playNext) {
//            String url = "https://cdn.changguwen.com/cms/media/2019815/65499b80-09dd-4ded-bc97-5fc3d5e21a94-1565832374013.mp3";
//        url = "https://cdn.changguwen.com/cms/media/2019923/4ef9cdfb-7867-40e8-8d9b-c41242d60cce-1569231281741-mcd.m3u8";
//        url="https://cdn.changguwen.com/cms/media/2019923/4ef9cdfb-7867-40e8-8d9b-c41242d60cce-1569231281741-mcd-00001.ts";
//        File mMusicFile = new File(Environment.getExternalStorageDirectory(), "test.mp3");
//        url = mMusicFile.getAbsolutePath();
            nPrepare(this.source);
        }
    }




    @Override
    public void prepare(String source) {
        this.source = source;
        nPrepare(source);
    }

    @Override
    public void start() {
        nStart();

    }

    @Override
    public void pause() {
        nPause();

    }


    @Override
    public void seek(int second) {
        super.seek(second);
        nSeekTo(second);
    }

    @Override
    public void resume() {
        nResume();

    }

    @Override
    public void stop() {
        nStop();

    }

    @Override
    public long getDuration() {
        return nGetDuration();
    }

    @Override
    public void next(String source) {
        this.source = source;
        playNext = true;
        stop();

    }


    //-----------------called from jni -----------------


    protected native void nPrepare(String inputUrl);

    protected native void nStart();

    protected native void nPause();

    protected native void nSeekTo(int second);

    protected native void nResume();

    protected native void nStop();

    protected native int nGetDuration();

    protected native void nSetVolume(int percent);

    protected native void nSetMute(int mute);

    protected native void nSetPitch(float pitch);

    protected native void nSetSpeed(float speed);


//
//
//    public native void nSpeed();
//
//
//    public native void nSaveStream(String inputUrl,String outPutUrl);
}
