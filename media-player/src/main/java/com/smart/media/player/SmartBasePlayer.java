package com.smart.media.player;

import android.text.TextUtils;

import com.smart.media.player.interf.IPlayer;
import com.smart.media.player.interf.ISmartPlayer;
import com.smart.media.player.natives.NativePlayerBase;
import com.smart.utils.LogUtils;


/**
 * @date : 2019-12-12 15:37
 * @author: lichen
 * @email : 1960003945@qq.com
 * @description :
 */
public abstract class SmartBasePlayer implements ISmartPlayer {

    protected String source;

    protected NativePlayerBase nativePlayer;
    protected IPlayer.OnPreparedListener onPreparedListener = null;
    protected IPlayer.OnInfoListener onInfoListener = null;
    protected IPlayer.OnErrorListener onErrorListener = null;


    protected abstract NativePlayerBase createNativePlayer();


    public SmartBasePlayer() {
        nativePlayer = createNativePlayer();
    }


    @Override
    public void setOnPreparedListener(IPlayer.OnPreparedListener onPreparedListener) {
        this.onPreparedListener = onPreparedListener;
        nativePlayer.setOnPreparedListener(onPreparedListener);
    }


    @Override
    public void setOnInfoListener(IPlayer.OnInfoListener onInfoListener) {
        this.onInfoListener = onInfoListener;
        nativePlayer.setOnInfoListener(onInfoListener);
    }


    @Override
    public void setOnErrorListener(IPlayer.OnErrorListener onErrorListener) {
        this.onErrorListener = onErrorListener;
        nativePlayer.setOnErrorListener(onErrorListener);
    }


    @Override
    public void setDataSource(String source) {
        this.source = source;
    }

    public void prepare() {
        if (TextUtils.isEmpty(source)) {
            throw new NullPointerException("source is null, please call method setDataSource");
        }
        prepare(this.source);
    }

    @Override
    public void prepare(String source) {
        nativePlayer.prepare(source);

    }


    @Override
    public void start() {
        nativePlayer.start();
    }



    @Override
    public void seek(int second) {
        nativePlayer.seek(second);
    }

    @Override
    public void pause() {
        nativePlayer.pause();

    }

    @Override
    public void resume() {
        nativePlayer.resume();

    }

    @Override
    public void setVolume(int percent) {
        nativePlayer.setVolume(percent);
    }

    @Override
    public void setMute(int mute) {
        nativePlayer.setMute(mute);
    }

    @Override
    public void setPitch(float pitch) {
        nativePlayer.setPitch(pitch);

    }

    @Override
    public void setSpeed(float speed) {
        nativePlayer.setSpeed(speed);

    }

    @Override
    public void stop() {
        nativePlayer.stop();
    }

    @Override
    public long getDuration() {
        long duration = nativePlayer.getDuration();
        LogUtils.e("xw", "duration:" + duration);
        return duration;
    }


}
