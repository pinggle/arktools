package com.dt.arktools.utils;

import android.util.Log;

/**
 * ark utils;
 */
public class ArkUtils {

    public static final String TAG = "DTPrint";

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    public enum LogLevel {
        LOG_LEVEL_NONE,
        LOG_LEVEL_ERR,
        LOG_LEVEL_WARNING,
        LOG_LEVEL_INFO,
        LOG_LEVEL_DEBUG
    }

    public static void init() {
        try {
            LogInit("/sdcard/ark/", "ark.log",
                    LogLevel.LOG_LEVEL_DEBUG.ordinal(),
                    LogLevel.LOG_LEVEL_DEBUG.ordinal());
            log(LogLevel.LOG_LEVEL_DEBUG.ordinal(), "<<==== ark tool init ====>>");
        } catch (Exception e) {
            Log.e(TAG, "ark utils init failed :" + e.getMessage());
        }
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public static native String stringFromJNI();

    public static native String dexDump(String dexPath, String params);


    // 日志类初始化
    public static native void LogInit(String logFilePath, String logName, int logfileLevel, int logScreenLevel);

    public static native void log(int logLevel, String content);

    public static native void logClose();


}