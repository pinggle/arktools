#include <jni.h>
#include <string>

#include "dex/DexDump.h"

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