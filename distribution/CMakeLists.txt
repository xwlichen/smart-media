# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.
cmake_minimum_required(VERSION 3.4.1)
set(CMAKE_VERBOSE_MAKEFILE on)

# 需要引入我们头文件,以这个配置的目录为基准
#include_directories(${PROJECT_SOURCE_DIR}/core/src/main/cpp/include)
#include_directories(${CMAKE_SOURCE_DIR}/src/main/jniLibs/include)
include_directories(${CMAKE_SOURCE_DIR}/src/main/jniLibs/include)
# 添加共享库搜索路径
LINK_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/main/jniLibs/armeabi-v7a)


# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

# 添加共享库搜索路径

# 指定源文件目录
AUX_SOURCE_DIRECTORY(${CMAKE_SOURCE_DIR}/src/main/cpp SRC_LIST)
# FILE(GLOB SRC_LIST "src/main/cpp/*.cpp")

add_library(
        # Sets the name of the library.
        save-utils
        # Sets the library as a shared library.
        SHARED
        # Provides a relative path to your source file(s).
        ${SRC_LIST}
)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
        # 链接额外的 ffmpeg 的编译
        save-utils
        # 编解码(最重要的库)
        avcodec
        # 设备信息
        avdevice
        # 滤镜特效处理库如水印、音频变声
        avfilter
        # 封装格式处理库
        avformat
        # 工具库(大部分库都需要这个库的支持)
        avutil
        # 多媒体后期处理
        postproc
        # 音频采样数据格式转换库
        swresample
        # 视频像素数据格式转换
        swscale
        #链接 android ndk 自带的一些库
        android
        # 链接 OpenSLES
        OpenSLES
        # Links the target library to the log library
        # included in the NDK.
        log
        )


#输出so、头文件到指定文件家
set(DISTRIBUTION_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../distribution)
set_target_properties(
        save-utils
        PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY
        "${DISTRIBUTION_DIR}/save/libs/${ANDROID_ABI}")
#add_custom_command(TARGET smart-player POST_BUILD
#        COMMAND "${CMAKE_COMMAND}" -E
#        copy "${CMAKE_CURRENT_SOURCE_DIR}/src/main/cpp/audio_operation.h"
#        "${DISTRIBUTION_DIR}/player/include/audio_operation.h"
#
#        COMMAND "${CMAKE_COMMAND}" -E
#        copy "${CMAKE_CURRENT_SOURCE_DIR}/src/main/cpp/avpacket_queue.h"
#        "${DISTRIBUTION_DIR}/player/include/avpacket_queue.h"
#
#        COMMAND "${CMAKE_COMMAND}" -E
#        copy "${CMAKE_CURRENT_SOURCE_DIR}/src/main/cpp/ffmpeg_music_handler.h"
#        "${DISTRIBUTION_DIR}/player/include/ffmpeg_music_handler.h"
#
#        COMMAND "${CMAKE_COMMAND}" -E
#        copy "${CMAKE_CURRENT_SOURCE_DIR}/src/main/cpp/jni_player_call.h"
#        "${DISTRIBUTION_DIR}/player/include/jni_player_call.h"
#
#        COMMAND "${CMAKE_COMMAND}" -E
#        copy "${CMAKE_CURRENT_SOURCE_DIR}/src/main/cpp/meida_core.h"
#        "${DISTRIBUTION_DIR}/player/include/meida_core.h"
#
#        COMMAND "${CMAKE_COMMAND}" -E
#        copy "${CMAKE_CURRENT_SOURCE_DIR}/src/main/cpp/player_status.h"
#        "${DISTRIBUTION_DIR}/player/include/player_status.h"
#
#        COMMAND "${CMAKE_COMMAND}" -E
#        copy "${CMAKE_CURRENT_SOURCE_DIR}/src/main/cpp/video_operation.h"
#        "${DISTRIBUTION_DIR}/player/include/video_operation.h"
#        COMMENT "Copying smart-player 's include to output directory")