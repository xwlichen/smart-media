package com.smart.demo;

import android.app.Application;
import android.content.Context;
import android.os.Environment;

import com.smart.cache.HttpProxyCacheServer;
import com.smart.utils.LogUtils;

import static com.smart.cache.Constants.LOG_TAG;

/**
 * @date : 2019-11-11 14:33
 * @author: lichen
 * @email : 1960003945@qq.com
 * @description :
 */
public class AppApplication extends Application {
    private HttpProxyCacheServer proxy;

    @Override
    public void onCreate() {
        super.onCreate();

        final String SDCARD = Environment.getExternalStorageDirectory().getAbsolutePath();
        final String logPath = SDCARD + "/" + this.getPackageName() + "/marssample/log";

        // this is necessary, or may crash for SIGBUS
        final String cachePath = SDCARD + "/Android/data/" + this.getPackageName() + "/xlog";
        String pubKey = "d5caa755197ea401323cbaf253269ecc193dc09bb00d2054f1bfaf9758a6f840e35ea83f2d0f5be49c65547dc7ba79e03f67ccdc8e40cda368ad041a3952a576";

        LogUtils.initXLog("testDemo", logPath, cachePath, pubKey, 0, true);

        LogUtils.e(LOG_TAG, "logPath:" + logPath);
        LogUtils.e(LOG_TAG, "cachepath:" + cachePath);
    }


    public static HttpProxyCacheServer getProxy(Context context) {
        AppApplication app = (AppApplication) context.getApplicationContext();
        return app.proxy == null ? (app.proxy = app.newProxy()) : app.proxy;
    }

    private HttpProxyCacheServer newProxy() {
        return new HttpProxyCacheServer.Builder(this)
                .cacheDirectory(Utils.getVideoCacheDir(this))
                .build();
    }
}
