package com.smart.media.player.natives;

import android.os.Handler;
import android.os.Looper;
import android.os.Message;
;

import com.smart.media.player.bean.InfoBean;
import com.smart.media.player.enums.InfoCode;
import com.smart.media.player.interf.INativePlayer;
import com.smart.media.player.interf.IPlayer;

import java.lang.annotation.Native;
import java.lang.ref.WeakReference;

import static com.smart.media.player.natives.NativePlayerBase.MainHandler.CODE_CURRENT_PROGRESS;


/**
 * @date : 2019-12-12 17:30
 * @author: lichen
 * @email : 1960003945@qq.com
 * @description :
 */
public abstract class NativePlayerBase implements INativePlayer {

    private NativePlayerBase.MainHandler currentThreadHandler;

    protected IPlayer.OnPreparedListener onPreparedListener;
    protected IPlayer.OnPauseListener onPauseListener;
    protected IPlayer.OnLoadingStatusListener onLoadingStatusListener;
    protected IPlayer.OnInfoListener onInfoListener;
    protected IPlayer.OnCompleteListener onCompleteListener;
    protected IPlayer.OnErrorListener onErrorListener;


    public NativePlayerBase() {
        if (Looper.myLooper() != Looper.getMainLooper()) {
            Looper.prepare();
        }
        this.currentThreadHandler = new NativePlayerBase.MainHandler(this, Looper.getMainLooper());
    }


    @Override
    public void nativeSeek(int second) {
        this.currentThreadHandler.removeMessages(0);
    }


    protected void onPrepared() {
        this.currentThreadHandler.post(new Runnable() {
            @Override
            public void run() {
                if (NativePlayerBase.this.onPreparedListener != null) {
                    NativePlayerBase.this.onPreparedListener.onPrepared();
                }

            }
        });
    }


    /**
     * @param current  单位s
     * @param duration
     */
    protected void onCurrentProgressUpdate(int current, int duration) {
        Message msg = this.currentThreadHandler.obtainMessage(CODE_CURRENT_PROGRESS, current, 0);
        this.currentThreadHandler.sendMessage(msg);
    }


    private void handleMessage(Message msg) {
        if (msg.what == CODE_CURRENT_PROGRESS && this.onInfoListener != null) {
            InfoBean infoBean = new InfoBean();
            infoBean.setCode(InfoCode.CURRENT_PROGRESS);
            infoBean.setExtraValue((long) msg.arg1);
            this.onInfoListener.onInfo(infoBean);
        }

    }


    static class MainHandler extends Handler {
        private WeakReference<Native> playerWeakReference;

        public static final int CODE_CURRENT_PROGRESS = 0;

        public MainHandler(NativePlayerBase nativePlayerBase, Looper mainLooper) {
            super(mainLooper);
            this.playerWeakReference = new WeakReference(nativePlayerBase);
        }

        @Override
        public void handleMessage(Message msg) {
            NativePlayerBase player = (NativePlayerBase) this.playerWeakReference.get();
            if (player != null) {
                player.handleMessage(msg);
            }

            super.handleMessage(msg);
        }
    }
}
