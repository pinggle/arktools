#include <jni.h>
#include <string>

#include "dex/DexDump.h"
#include "liblog/log_main.h"

static bool logEnable =false;

extern "C" JNIEXPORT jstring JNICALL
Java_com_dt_arktools_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}


extern "C" JNIEXPORT jstring JNICALL
Java_com_dt_arktools_MainActivity_dexDump(
        JNIEnv *env,
        jobject /* this */,
        jstring inputDexPath,
        jstring inputParams) {
    const char *dexPath = env->GetStringUTFChars(inputDexPath, NULL);
    const char *dexParams = env->GetStringUTFChars(inputParams, NULL);
    printf("dexPath:[%s], dexParams:[%s]\n", dexPath, dexParams);

    initOptions(dexParams);
    process(dexPath);

    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}


extern "C"
JNIEXPORT void JNICALL
Java_com_dt_arktools_utils_LogInit(JNIEnv *env, jobject instance, jstring logFilePath,
                                    jstring logName, jint logfileLevel, jint logScreenLevel) {
    if (!logEnable) {
        const char *path = env->GetStringUTFChars(logFilePath, JNI_FALSE);
        const char *name = env->GetStringUTFChars(logName, JNI_FALSE);
        int fileLevel = logfileLevel;
        int screenLevel = logScreenLevel;
        logEnable = _LogInit(path, name, fileLevel, screenLevel);
        env->ReleaseStringUTFChars(logFilePath, path);
        env->ReleaseStringUTFChars(logName, name);
    }

}

extern "C"
JNIEXPORT void JNICALL
Java_com_dt_arktools_utils_log(JNIEnv *env, jclass type, jint _level, jstring _str) {
    if (!logEnable) {
        LOGE("log error! LogInit need");
        return;
    }
    const char *str = env->GetStringUTFChars(_str, JNI_FALSE);
    WriteTextLog(_level, str);
    env->ReleaseStringUTFChars(_str, str);
}


extern "C"
JNIEXPORT void JNICALL
Java_com_dt_arktools_utils_logClose(JNIEnv *env, jclass type) {
    _LogClose();
    logEnable=false;
}