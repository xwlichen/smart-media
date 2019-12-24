package com.smart.demo.seekbar;


public interface SeekChangeListener {

    void onSeeking(SeekParams seekParams);

    void onStartTrackingTouch(MusicSeekBar seekBar);

    void onStopTrackingTouch(MusicSeekBar seekBar);


}