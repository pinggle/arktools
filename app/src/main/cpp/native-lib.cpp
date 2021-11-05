#include <jni.h>
#include <string>

#include "dex/DexDump.h"
#include "dex/liblog/log_main.h"

static bool logEnable = false;

extern "C" JNIEXPORT jstring JNICALL
Java_com_dt_arktools_utils_ArkUtils_stringFromJNI(
        JNIEnv *env,
        jclass clazz) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

/**
 * parse dex file;
 */
extern "C" JNIEXPORT jstring JNICALL
Java_com_dt_arktools_utils_ArkUtils_dexDump(
        JNIEnv *env,
        jclass clazz,
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
Java_com_dt_arktools_utils_ArkUtils_LogInit(JNIEnv *env, jclass instance, jstring logFilePath,
                                            jstring logName, jint logfileLevel,
                                            jint logScreenLevel) {
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
Java_com_dt_arktools_utils_ArkUtils_log(JNIEnv *env, jclass type, jint _level, jstring _str) {
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
Java_com_dt_arktools_utils_ArkUtils_logClose(JNIEnv *env, jclass type) {
    _LogClose();
    logEnable = false;
}

///==============================================================================

static jint JNICALL cAdd(JNIEnv *env, jobject jobj, jint x, jint y) {
    LOGI("cAdd x is :%d  y is :%d", x, y);
    return x + y;
}

static jstring JNICALL cSayHi(JNIEnv *env, jobject jobj, jint x, jint y) {
    LOGI("cSayHi runs... will return a string in c");
    return env->NewStringUTF("hello from cSayHi");
}

/**
   第一个参数：javaAdd 是java中的方法名称
   第二个参数：(II)I  是java中方法的签名，可以通过javap -s -p 类名.class 查看
   第三个参数： (jstring *)cSayHi  （返回值类型）映射到native的方法名称

*/
static const JNINativeMethod gMethods[] = {
        {"javaAdd",   "(II)I",                (jint *) cAdd},
        {"javaSayHi", "()Ljava/lang/String;", (jstring *) cSayHi}
};

static jclass myClass;
// 这里是java调用C的存在Native方法的类路径
static const char *const className = "com/dt/arktools/utils/ArkUtils";

extern "C" JNIEXPORT jint JNICALL
JNI_OnLoad(JavaVM *vm, void *reserved) {
    LOGI("jni onload called");
    JNIEnv *env = NULL; //注册时在JNIEnv中实现的，所以必须首先获取它
    jint result = -1;
    if (vm->GetEnv((void **) &env, JNI_VERSION_1_4) != JNI_OK) { //从JavaVM获取JNIEnv，一般使用1.4的版本
        return -1;
    }
    // 获取映射的java类
    myClass = env->FindClass(className);
    if (myClass == NULL) {
        LOGI("cannot get class:%s\n", className);
        return -1;
    }
    // 通过RegisterNatives方法动态注册
    if (env->RegisterNatives(myClass, gMethods, sizeof(gMethods) / sizeof(gMethods[0])) < 0) {
        printf("register native method failed!\n");
        return -1;
    }
    LOGI("jni onload called end...");
    return JNI_VERSION_1_6; //这里很重要，必须返回版本，否则加载会失败。
}


