package com.smart.media.player;

import android.text.TextUtils;

import com.smart.media.player.interf.INativePlayer;
import com.smart.media.player.interf.IPlayer;
import com.smart.media.player.interf.ISmartPlayer;
import com.smart.media.player.natives.NativePlayerBase;


/**
 * @date : 2019-12-12 15:37
 * @author: lichen
 * @email : 1960003945@qq.com
 * @description :
 */
public abstract class SmartBasePlayer implements ISmartPlayer {

    protected String source;

    protected INativePlayer nativePlayer;
    protected IPlayer.OnPreparedListener onPreparedListener;
    protected IPlayer.OnInfoListener onInfoListener;
    protected IPlayer.OnErrorListener onErrorListener;

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


    @Override
    public void prepare() {
        if (TextUtils.isEmpty(source)) {
            throw new NullPointerException("source is null, please call method setDataSource");
        }
        nativePlayer.nativePrepare(source);

    }


    @Override
    public void start() {
        nativePlayer.nativeStart();
    }


    @Override
    public void seek(int second) {
        nativePlayer.nativeSeek(second);
    }

    @Override
    public void pause() {
        nativePlayer.nativePause();

    }

    @Override
    public void resume() {
        nativePlayer.nativeResume();

    }


    @Override
    public void stop() {
        nativePlayer.nativeStop();
    }

    @Override
    public long getDuration() {
        long duration = 0;
        return duration;
    }


}
