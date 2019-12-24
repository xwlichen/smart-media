package com.smart.media.player;


import com.smart.media.player.natives.NativeMusicPlayer;
import com.smart.media.player.natives.NativePlayerBase;

/**
 * @date : 2019-09-04 18:03
 * @author: lichen
 * @email : 1960003945@qq.com
 * @description :
 */
public class SmartMusicPlayer extends SmartBasePlayer {




    @Override
    public void next(String source) {
        this.source = source;
        nativePlayer.next(this.source);


    }


    @Override
    protected NativePlayerBase createNativePlayer() {
        return new NativeMusicPlayer();
    }



}
