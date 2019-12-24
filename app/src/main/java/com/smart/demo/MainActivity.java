package com.smart.demo;

import android.os.Bundle;
import android.os.Environment;
import android.os.Looper;
import android.text.TextUtils;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.TextView;

import com.smart.cache.CacheListener;
import com.smart.cache.HttpProxyCacheServer;
import com.smart.demo.seekbar.MusicSeekBar;
import com.smart.media.player.SmartMusicPlayer;
import com.smart.media.player.bean.InfoBean;
import com.smart.media.player.interf.IPlayer;
import com.smart.utils.LogUtils;

import java.io.File;

import androidx.appcompat.app.AppCompatActivity;

import static com.smart.cache.Constants.LOG_TAG;


public class MainActivity extends AppCompatActivity implements CacheListener {
    File mMusicFile1;
    File mMusicFile = new File(Environment.getExternalStorageDirectory(), "test.mp3");

    String url = "https://cdn.changguwen.com/cms/media/2019815/65499b80-09dd-4ded-bc97-5fc3d5e21a94-1565832374013.mp3";
//    String url="http://x128.songtasty.com/attachment/20191111/QabrdVJ0C2EFR3ylxLOf.mp3";

    // Used to load the 'native-lib' library on application startup.
    private SmartMusicPlayer mPlayer;
    MusicSeekBar musicSeekBar;

    TextView tvTime;
    ImageView test;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        url = "https://cdn.changguwen.com/cms/media/2019923/4ef9cdfb-7867-40e8-8d9b-c41242d60cce-1569231281741-mcd.m3u8";
//        url = "http://cnvod.cnr.cn/audio2018/live/lnzs/201912/ysylg_20191211061331lnzs_h.m4a";
//        url = "http://ngcdn001.cnr.cn/live/zgzs/index.m3u8";
//        url = "https://cdn.changguwen.com/cms/media/2019815/65499b80-09dd-4ded-bc97-5fc3d5e21a94-1565832374013.mp3";
        url = "https://cdn.changguwen.com/cms/media/2019923/4ef9cdfb-7867-40e8-8d9b-c41242d60cce-1569231281741-mcd-00001.ts";
        url = "https://cdn.changguwen.com/cms/media/2019923/4ef9cdfb-7867-40e8-8d9b-c41242d60cce-1569231281741-mcd-00002.ts";

//https://cdn.changguwen.com/cms/media/2019923/4ef9cdfb-7867-40e8-8d9b-c41242d60cce-1569231281741-mcd-00001.ts
        url = "http://mpge.5nd.com/2015/2015-11-26/69708/1.mp3";
        ///storage/emulated/0/test.mp3
//        if (mMusicFile.exists()){
//            mMusicFile.w();
//        }

        checkCachedState();

        String outPath = Environment.getExternalStorageDirectory().getAbsolutePath() + File.separator
                + "test01.mp3";
        mMusicFile1 = new File(outPath);
//        url="http://cdn.changguwen.com/cms/media/2019812/e69e3268-6d24-4446-87c1-3569a66ddf71-1565599111905.mp4";
        Log.e("TAG", "file is path: " + mMusicFile1.getAbsolutePath());

//        SaveUtils.save(url, outPath);

        mPlayer = new SmartMusicPlayer();
////        mPlayer.save(url, outPath);
//        mPlayer.setDataSource(mMusicFile.getAbsolutePath());
        mPlayer.setDataSource(url);

//        mPlayer.setDataSource(getUrl());

////        mPlayer.setDataSource(getUrl(), mMusicFile1.getAbsolutePath());
//        mPlayer.setDataSource(url, mMusicFile1.getAbsolutePath());
//        mPlayer.stop();
        mPlayer.prepare();
//
////        mPlayer.setDataSource(mMusicFile.getAbsolutePath(), mMusicFile1.getAbsolutePath());
//
////
////        mPlayer.setOnErrorListener(new IPlayer.OnErrorListener() {
////            @Override
////            public void onError(int code, String msg) {
////                Log.e("TAG", "error code: " + code);
////                Log.e("TAG", "error msg: " + msg);
////                // Java 的逻辑代码
////            }
////        });
////
        mPlayer.setOnPreparedListener(new IPlayer.OnPreparedListener() {
            @Override
            public void onPrepared() {
                Log.e("JNI_TAG_PLAYER", "准备完毕");

//                new Thread(new Runnable() {
//                    @Override
//                    public void run() {
                mPlayer.start();
//                    }
//                }).start();

            }

        });

        mPlayer.setOnInfoListener(new IPlayer.OnInfoListener() {
            @Override
            public void onInfo(InfoBean data) {
                if (data != null) {
                    switch (data.getCode()) {
                        case CURRENT_PROGRESS:
//                            Log.e("xw", "data.getExtraValue():" + data.getExtraValue());
//                            runOnUiThread(new Runnable() {
//                                @Override
//                                public void run() {
                            if (Looper.myLooper() != Looper.getMainLooper()) {
                                Looper.prepare();
                                Log.e("xw", " not main");

                            } else {
                                Log.e("xw", " is main");

                            }
                            String time = data.getExtraValue() + "";
                            tvTime.setText(time);
//                            Toast.makeText(MainActivity.this, time, Toast.LENGTH_SHORT).show();
                            test.setImageResource(R.mipmap.ic_launcher);
//                                }
//                            });

                            break;
                        default:
                            break;
                    }

                }
            }
        });
////
//////        mPlayer.setOnPreparedListener();
////        mPlayer.prepareAsync();
//
//
//        musicSeekBar=findViewById(R.id.sb_progress);
        tvTime = findViewById(R.id.tvTime);
        test = findViewById(R.id.test);

        EditText etSecond = findViewById(R.id.etSecond);

        Button btnPlay = findViewById(R.id.btnPlay);
        Button btnPause = findViewById(R.id.btnPause);
        Button btnResume = findViewById(R.id.btnResume);
        Button btnNext = findViewById(R.id.btnNext);
        Button btnStop = findViewById(R.id.btnStop);

        Button btnSeek = findViewById(R.id.btnSeek);


        btnPause.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                mPlayer.pause();

            }
        });

        btnResume.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                mPlayer.resume();

            }
        });

        btnStop.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                mPlayer.stop();

            }
        });

        btnNext.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                String url = "https://cdn.changguwen.com/cms/media/2019815/65499b80-09dd-4ded-bc97-5fc3d5e21a94-1565832374013.mp3";

                mPlayer.next(url);

            }
        });


        btnPlay.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        mPlayer.start();
                    }
                }).start();

            }
        });

        btnSeek.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                String value = etSecond.getText().toString().trim();
                if (TextUtils.isEmpty(value)) {
                    value = "0";
                }
                int second = Integer.valueOf(value);
                mPlayer.seek(second);
            }
        });


        test.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                finish();
            }
        });

////        musicSeekBar.showLoading();
    }


    private void checkCachedState() {
        HttpProxyCacheServer proxy = AppApplication.getProxy(this);
        boolean fullyCached = proxy.isCached(url);
        if (fullyCached) {
            LogUtils.e(LOG_TAG, "progress 100");
        }
    }

    private String getUrl() {
        HttpProxyCacheServer proxy = AppApplication.getProxy(this);
        proxy.registerCacheListener(this, url);
        String proxyUrl = proxy.getProxyUrl(url);
        LogUtils.e(LOG_TAG, "Use proxy url " + proxyUrl + " instead of original url " + url);
        return proxyUrl;
    }

    @Override
    public void onCacheAvailable(File cacheFile, String url, int percentsAvailable) {
        LogUtils.e(LOG_TAG, String.format("onCacheAvailable. percents: %d, file: %s, url: %s", percentsAvailable, cacheFile, url));
    }


    @Override
    protected void onDestroy() {
        super.onDestroy();
        mPlayer.stop();

    }
}
