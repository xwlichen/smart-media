# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.
cmake_minimum_required(VERSION 3.4.1)
#set(CMAKE_VERBOSE_MAKEFILE on)


### 设置GCC编译器的编译选项
if (CMAKE_COMPILER_IS_GNUCC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wmissing-declarations -Wmissing-prototypes")
endif (CMAKE_COMPILER_IS_GNUCC)

# 设置clang编译器的编译选项
if (CMAKE_COMPILER_IS_CLANG)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wmissing-declarations -Wmissing-prototypes -Wdocumentation -Wunreachable-code")
endif (CMAKE_COMPILER_IS_CLANG)

# 添加include目录路径
include_directories(
        common
        include/ffmpeg
        include/openssl)

# 添加ffmpeg库
#add_library( avcodec SHARED IMPORTED )
#set_target_properties( avcodec
#        PROPERTIES IMPORTED_LOCATION
#        ${CMAKE_SOURCE_DIR}/../jniLibs/${ANDROID_ABI}/libavcodec.so )


# 添加共享库搜索路径
LINK_DIRECTORIES(${CMAKE_SOURCE_DIR}/../jniLibs/${ANDROID_ABI})


## Metadata库
#add_subdirectory(${CMAKE_SOURCE_DIR}/)
#
# 媒体播放器
add_subdirectory(${CMAKE_SOURCE_DIR}/music_player)












