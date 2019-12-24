/**
 * @date : 2019-09-04 17:30
 * @author: lichen
 * @email : 1960003945@qq.com
 * @description : 
 */
//
#ifndef SMART_MEDIA_PLAYER_STATUS_H
#define SMART_MEDIA_PLAYER_STATUS_H


#define PLAYER_STATUS_PREPARED 1 //准备完成
#define PLAYER_STATUS_PLAYING 2  //正在播放
#define PLAYER_STATUS_PAUSED 3 //暂停
#define PLAYER_STATUS_LOADING 4 //正在加载
#define PLAYER_STATUS_STOP 5 //停止


class PlayerStatus {
public:
    /**
     * 是否退出，打算用这个变量来做退出(销毁)
     */
    bool is_exit = false;
    bool is_seeking = false;


    /**
     * 1-prepared 2-playing(resume) 3-paused 4-isloading  5-stoped
     */
    int current_state = -1;

//    bool is_loading = false;
//    bool is_pause = false;

public:
    PlayerStatus();

};

#endif //SMART_MEDIA_PLAYER_STATUS_H
