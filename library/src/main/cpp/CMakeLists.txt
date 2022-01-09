cmake_minimum_required(VERSION 3.9)

#project(CGE)
#
#set(CMAKE_CXX_STANDARD 14)
#
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIE")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIE -frtti -std=c++14")
#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fPIE -pie")
#
##(无) = 重要消息；
##STATUS = 非重要消息；
##WARNING = CMake 警告, 会继续执行；
##AUTHOR_WARNING = CMake 警告 (dev), 会继续执行；
##SEND_ERROR = CMake 错误, 继续执行，但是会跳过生成的步骤；
##FATAL_ERROR = CMake 错误, 终止所有处理过程；
#
#if (NOT DEFINED ENV{NDK})
#    execute_process(COMMAND which ndk-build OUTPUT_VARIABLE CGE_NDK_DIR)
#    if ("${CGE_NDK_DIR}" STREQUAL "")
#        message(FATAL_ERROR "You should define the NDK variable!")
#    else ()
#        execute_process(COMMAND export NDK=$ (dirname `which ndk-build`))
#    endif ()
#endif ()
#
#execute_process(COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/setup_android_toolchain)
#
#set(NDK_STANDALONE_TOOLCHAIN $ENV{NDK}/ndk-build-toolchain)
#message(${NDK_STANDALONE_TOOLCHAIN})
#set(CMAKE_SYSTEM_NAME AndroidARM)
#set(CMAKE_C_COMPILER ${NDK_STANDALONE_TOOLCHAIN}/bin/clang)
#set(CMAKE_CXX_COMPILER ${NDK_STANDALONE_TOOLCHAIN}/bin/clang++)
#set(CMAKE_FIND_ROOT_PATH ${NDK_STANDALONE_TOOLCHAIN})
#
#execute_process(COMMAND find ${CMAKE_CURRENT_SOURCE_DIR}/cge -maxdepth 3 -type d OUTPUT_VARIABLE CGE_CORE_HEADER_DIR)
#execute_process(COMMAND bash -c "find ${CMAKE_CURRENT_SOURCE_DIR} -type f -iname '*.cpp' | grep -v cmake" OUTPUT_VARIABLE CGE_CORE_SRC)
#
#string(REGEX REPLACE "\n" ";" CGE_CORE_HEADER_DIR ${CGE_CORE_HEADER_DIR})
#string(REGEX REPLACE "\n" ";" CGE_CORE_SRC ${CGE_CORE_SRC})

include_directories(
        #        ${CGE_CORE_HEADER_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/common
        ${CMAKE_CURRENT_SOURCE_DIR}/filters
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/interface
        ${CMAKE_CURRENT_SOURCE_DIR}/source
        ${CMAKE_CURRENT_SOURCE_DIR}/faceTracker
        ${CMAKE_CURRENT_SOURCE_DIR}/faceTracker/FaceTracker
        ${CMAKE_CURRENT_SOURCE_DIR}/opencv
        ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg
        #        ${NDK_STANDALONE_TOOLCHAIN}/sysroot/usr/include
)


#add_definitions(-DANDROID_NDK -DCGE_LOG_TAG=\"libCGE\" -DCGE_TEXTURE_PREMULTIPLIED=1 -D_CGE_USE_FFMPEG_ -D__STDC_CONSTANT_MACROS -D_CGE_DISABLE_GLOBALCONTEXT_ -O3 -ffast-math -D_CGE_ONLY_FILTERS_ -DANDROID_ARM_NEON -DANDROID_ABI=armeabi-v7a --sysroot=${NDK_STANDALONE_TOOLCHAIN}/sysroot)

# file(GLOB_RECURSE FM_CORE_RESOURCE "${CMAKE_CURRENT_SOURCE_DIR}/facemagic/*.cpp")

add_library(
        native-lib
        SHARED
        ${CMAKE_SOURCE_DIR}/common/CommonDefine.cpp
        ${CMAKE_SOURCE_DIR}/common/GLFunctions.cpp
        ${CMAKE_SOURCE_DIR}/common/Global.cpp
        ${CMAKE_SOURCE_DIR}/common/ImageFilter.cpp
        ${CMAKE_SOURCE_DIR}/common/ImageHandler.cpp
        ${CMAKE_SOURCE_DIR}/common/ShaderFunctions.cpp
        ${CMAKE_SOURCE_DIR}/common/TextureUtils.cpp
        ${CMAKE_SOURCE_DIR}/common/Thread.cpp
        ${CMAKE_SOURCE_DIR}/filters/AdvancedEffects.cpp
        ${CMAKE_SOURCE_DIR}/filters/AdvancedEffectsCommon.cpp
        ${CMAKE_SOURCE_DIR}/filters/BeautifyFilter.cpp
        ${CMAKE_SOURCE_DIR}/filters/BilateralBlurFilter.cpp
        ${CMAKE_SOURCE_DIR}/filters/BlendFilter.cpp
        ${CMAKE_SOURCE_DIR}/filters/BrightnessAdjust.cpp
        ${CMAKE_SOURCE_DIR}/filters/ColorBalanceAdjust.cpp
        ${CMAKE_SOURCE_DIR}/filters/ColorLevelAdjust.cpp
        ${CMAKE_SOURCE_DIR}/filters/ColorMappingFilter.cpp
        ${CMAKE_SOURCE_DIR}/filters/ContrastAdjust.cpp
        ${CMAKE_SOURCE_DIR}/filters/CrosshatchFilter.cpp
        ${CMAKE_SOURCE_DIR}/filters/CurveAdjust.cpp
        ${CMAKE_SOURCE_DIR}/filters/DataParsingEngine.cpp
        ${CMAKE_SOURCE_DIR}/filters/DynamicFilters.cpp
        ${CMAKE_SOURCE_DIR}/filters/DynamicWaveFilter.cpp
        ${CMAKE_SOURCE_DIR}/filters/EdgeFilter.cpp
        ${CMAKE_SOURCE_DIR}/filters/EmbossFilter.cpp
        ${CMAKE_SOURCE_DIR}/filters/ExposureAdjust.cpp
        ${CMAKE_SOURCE_DIR}/filters/FilterBasic.cpp
        ${CMAKE_SOURCE_DIR}/filters/HalftoneFilter.cpp
        ${CMAKE_SOURCE_DIR}/filters/HazeFilter.cpp
        ${CMAKE_SOURCE_DIR}/filters/HueAdjust.cpp
        ${CMAKE_SOURCE_DIR}/filters/LerpblurFilter.cpp
        ${CMAKE_SOURCE_DIR}/filters/LiquifyFilter.cpp
        ${CMAKE_SOURCE_DIR}/filters/LookupFilter.cpp
        ${CMAKE_SOURCE_DIR}/filters/MaxValueFilter.cpp
        ${CMAKE_SOURCE_DIR}/filters/MidValueFilter.cpp
        ${CMAKE_SOURCE_DIR}/filters/MinValueFilter.cpp
        ${CMAKE_SOURCE_DIR}/filters/MonochromeAdjust.cpp
        ${CMAKE_SOURCE_DIR}/filters/MosaicBlurFilter.cpp
        ${CMAKE_SOURCE_DIR}/filters/MotionFlowFilter.cpp
        ${CMAKE_SOURCE_DIR}/filters/MultipleEffects.cpp
        ${CMAKE_SOURCE_DIR}/filters/MultipleEffectsCommon.cpp
        ${CMAKE_SOURCE_DIR}/filters/PolarPixellateFilter.cpp
        ${CMAKE_SOURCE_DIR}/filters/PolkaDotFilter.cpp
        ${CMAKE_SOURCE_DIR}/filters/RandomBlurFilter.cpp
        ${CMAKE_SOURCE_DIR}/filters/SaturationAdjust.cpp
        ${CMAKE_SOURCE_DIR}/filters/SelectiveColorAdjust.cpp
        ${CMAKE_SOURCE_DIR}/filters/ShadowHighlightAdjust.cpp
        ${CMAKE_SOURCE_DIR}/filters/SharpenBlurAdjust.cpp
        ${CMAKE_SOURCE_DIR}/filters/SketchFilter.cpp
        ${CMAKE_SOURCE_DIR}/filters/TiltshiftAdjust.cpp
        ${CMAKE_SOURCE_DIR}/filters/VignetteAdjust.cpp
        ${CMAKE_SOURCE_DIR}/filters/WhiteBalanceAdjust.cpp
        ${CMAKE_SOURCE_DIR}/faceTracker/FaceTracker.cpp
        ${CMAKE_SOURCE_DIR}/faceTracker/FaceTrackerWrapper.cpp
        ${CMAKE_SOURCE_DIR}/faceTracker/FaceTracker/CLM.cpp
        ${CMAKE_SOURCE_DIR}/faceTracker/FaceTracker/FCheck.cpp
        ${CMAKE_SOURCE_DIR}/faceTracker/FaceTracker/FDet.cpp
        ${CMAKE_SOURCE_DIR}/faceTracker/FaceTracker/IO.cpp
        ${CMAKE_SOURCE_DIR}/faceTracker/FaceTracker/Patch.cpp
        ${CMAKE_SOURCE_DIR}/faceTracker/FaceTracker/PAW.cpp
        ${CMAKE_SOURCE_DIR}/faceTracker/FaceTracker/PDM.cpp
        ${CMAKE_SOURCE_DIR}/faceTracker/FaceTracker/Tracker.cpp
        ${CMAKE_SOURCE_DIR}/interface/DeformFilterWrapper.cpp
        ${CMAKE_SOURCE_DIR}/interface/FFmpegNativeLibrary.cpp
        ${CMAKE_SOURCE_DIR}/interface/FrameRecorder.cpp
        ${CMAKE_SOURCE_DIR}/interface/FrameRecorderWrapper.cpp
        ${CMAKE_SOURCE_DIR}/interface/FrameRenderer.cpp
        ${CMAKE_SOURCE_DIR}/interface/FrameRendererWrapper.cpp
        ${CMAKE_SOURCE_DIR}/interface/ImageHandlerAndroid.cpp
        ${CMAKE_SOURCE_DIR}/interface/ImageHandlerWrapper.cpp
        ${CMAKE_SOURCE_DIR}/interface/NativeLibrary.cpp
        ${CMAKE_SOURCE_DIR}/interface/SharedGLContext.cpp
        ${CMAKE_SOURCE_DIR}/interface/UtilFunctions.cpp
        ${CMAKE_SOURCE_DIR}/interface/VideoDecoder.cpp
        ${CMAKE_SOURCE_DIR}/interface/VideoEncoder.cpp
        ${CMAKE_SOURCE_DIR}/interface/VideoPlayer.cpp
        ${CMAKE_SOURCE_DIR}/source/CustomFilters.cpp
        ${CMAKE_SOURCE_DIR}/source/CustomFilterWrapper.cpp
        ${CMAKE_SOURCE_DIR}/source/MultiInputFilter.cpp
        ${CMAKE_SOURCE_DIR}/source/VideoUtils.cpp
        ${CMAKE_SOURCE_DIR}/source/customFilter_0.cpp
        ${CMAKE_SOURCE_DIR}/source/customFilter_N.cpp
        ${CMAKE_SOURCE_DIR}/source/customHelper.cpp

)

set(abi_dir ${CMAKE_SOURCE_DIR}/../libs/${ANDROID_ABI})
add_library(ffmpeg SHARED IMPORTED)
set_target_properties(ffmpeg PROPERTIES IMPORTED_LOCATION ${abi_dir}/libffmpeg.so)

add_library(facetracker SHARED IMPORTED)
set_target_properties(facetracker PROPERTIES IMPORTED_LOCATION ${abi_dir}/libFaceTracker.so)

find_library( # Sets the name of the path variable.
        log-lib
        log)

target_link_libraries(
        #        CGE
        native-lib
        ffmpeg
        facetracker
        jnigraphics
        android
        GLESv2
        EGL
        z
        ${log-lib}

)