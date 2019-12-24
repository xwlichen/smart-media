package com.smart.media.player.interf;

/**
 * @date : 2019-12-12 15:33
 * @author: lichen
 * @email : 1960003945@qq.com
 * @description :
 */
public interface ISmartPlayer extends IPlayer {


    void setDataSource(String url);


    void prepare();

    void start();

    void pause();

    void seek(int second);

    void resume();

    void next(String source);

    void stop();


    long getDuration();


}
