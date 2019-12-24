package com.smart.media.player.enums;

/**
 * @date : 2019-09-28 10:45
 * @author: lichen
 * @email : 1960003945@qq.com
 * @description :
 */
public enum InfoCode {
    /**
     * 当前进度
     */
    CURRENT_PROGRESS(2);

    private int value;

    InfoCode(int value) {
        this.value = value;
    }

    public int getValue() {
        return this.value;
    }
}
