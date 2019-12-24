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


add_library(
        soundtouch
        SHARED


        source/AAFilter.cpp
        source/FIFOSampleBuffer.cpp
        source/FIRFilter.cpp
        source/cpu_detect_x86.cpp
        source/sse_optimized.cpp
        source/RateTransposer.cpp
        source/SoundTouch.cpp
        source/InterpolateCubic.cpp
        source/InterpolateLinear.cpp
        source/InterpolateShannon.cpp
        source/TDStretch.cpp
        source/PeakFinder.cpp

)

target_include_directories(
        soundtouch
        PRIVATE

        include
        source)


# 链接静态库
target_link_libraries(


        soundtouch

        android
        log)















