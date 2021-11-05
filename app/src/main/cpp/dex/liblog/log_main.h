/*
 * Copyright (C) 2005-2017 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _LIBS_LOG_LOG_MAIN_H
#define _LIBS_LOG_LOG_MAIN_H

#include <stdio.h>
#include <android/log.h>
#include <errno.h>


/*
 * Normally we strip the effects of ALOGV (VERBOSE messages),
 * LOG_FATAL and LOG_FATAL_IF (FATAL assert messages) from the
 * release builds be defining NDEBUG.  You can modify this (for
 * example with "#define LOG_NDEBUG 0" at the top of your source
 * file) to change that behavior.
 */


#ifndef LOG_NDEBUG
#define LOG_NDEBUG

#define TAG "DTPrint" // 这个是自定义的LOG的标识
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,TAG ,__VA_ARGS__) // 定义LOGD类型
#define ALOGD(...) __android_log_print(ANDROID_LOG_DEBUG,TAG ,__VA_ARGS__) // 定义LOGD类型
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,TAG ,__VA_ARGS__) // 定义LOGI类型
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN,TAG ,__VA_ARGS__) // 定义LOGW类型
#define ALOGW(...) __android_log_print(ANDROID_LOG_WARN,TAG ,__VA_ARGS__) // 定义LOGW类型
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG ,__VA_ARGS__) // 定义LOGE类型
#define ALOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG ,__VA_ARGS__) // 定义LOGE类型
#define ALOGV(...) __android_log_print(ANDROID_LOG_ERROR,TAG ,__VA_ARGS__) // 定义LOGE类型
#define ALOGI(...) __android_log_print(ANDROID_LOG_ERROR,TAG ,__VA_ARGS__) // 定义LOGE类型
#define LOGF(...) __android_log_print(ANDROID_LOG_FATAL,TAG ,__VA_ARGS__) // 定义LOGF类型


#define LOG_TEXT_MAX_LENGTH        (1024)  //  单条日志大小
#define LOG_FILE_MAX_SIZE    (1024*1024*2) //  文件最大为2MB

enum {
    LOG_LEVEL_NONE = 0,
    LOG_LEVEL_ERR = 1,
    LOG_LEVEL_WARNING = 2,
    LOG_LEVEL_INFO = 3,
    LOG_LEVEL_DEBUG = 4
};

#ifdef  __cplusplus
extern "C" {
#endif
/**
 * 初始化日志选项
 * @param pFile
 * @param filename
 * @param logLevel
 * @param printScreen
 * @return
 */
bool _LogInit(const char *pFile, const char *filename, int logLevel, int printScreen);

/**
 * 写日志
 * @param level
 * @param strFormat
 * @param ...
 */
void WriteTextLog(int level, const char *strFormat, ...);

/**
 * 向文件中写入日志
 * @param level
 * @param log
 */
void WriteTextLogBottom(int level, const char *log);

/**
 * 关闭日志库
 */
void _LogClose();

#ifdef __cplusplus
}
#endif




//======================================================

#define WRITE_LOG(s, ...) do { \
FILE *f = fopen("/data/local/tmp/ark.log.txt", "a+"); \
  fprintf(f, s, __VA_ARGS__); \
  fflush(f); \
  fclose(f); \
} while (0)

#endif

#endif /* _LIBS_LOG_LOG_MAIN_H */