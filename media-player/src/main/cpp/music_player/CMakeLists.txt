# Sets the minimum version of CMake required to build the native library.
cmake_minimum_required(VERSION 3.4.1)
# 设置GCC编译器的编译选项
if (CMAKE_COMPILER_IS_GNUCC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wmissing-declarations -Wmissing-prototypes")
endif (CMAKE_COMPILER_IS_GNUCC)

# 设置clang编译器的编译选项
if (CMAKE_COMPILER_IS_CLANG)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wmissing-declarations -Wmissing-prototypes -Wdocumentation -Wunreachable-code")
endif (CMAKE_COMPILER_IS_CLANG)


# 添加 soundtouch 动态库
set(soundtouch_dir ../soundtouch)
add_subdirectory(${soundtouch_dir} soundtouch)

# 添加头文件路径
include_directories(
        ${soundtouch_dir})


add_library(
        smart-player
        SHARED

        entity/player_status.cpp
        queue/avpacket_queue.cpp

        android/native_music_player.cpp
        android/jni_player_call.cpp

        core/media_core.cpp

        handler/ffmpeg_music_handler.cpp
        handler/audio_handler.cpp
)

# 链接静态库
target_link_libraries(
        smart-player


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


        #openssl
        crypto
        ssl

        #soundtouch
        soundtouch


        android
        log
        OpenSLES
        EGL
        ${GLES-lib})















