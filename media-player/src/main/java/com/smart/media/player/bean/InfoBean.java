package com.smart.media.player.bean;


import com.smart.media.player.enums.InfoCode;

/**
 * @date : 2019-09-28 10:47
 * @author: lichen
 * @email : 1960003945@qq.com
 * @description :
 */
public class InfoBean {

    private InfoCode code;
    private long extraValue = -1L;
    private String extraMsg = null;

    public InfoCode getCode() {
        return code;
    }

    public void setCode(InfoCode code) {
        this.code = code;
    }

    public long getExtraValue() {
        return extraValue;
    }

    public void setExtraValue(long extraValue) {
        this.extraValue = extraValue;
    }

    public String getExtraMsg() {
        return extraMsg;
    }

    public void setExtraMsg(String extraMsg) {
        this.extraMsg = extraMsg;
    }
}
