/*
* cgeFrameRecorderWrapper.cpp
*
*  Created on: 2015-7-9
*      Author: Wang Yang
*        Mail: admin@wysaid.org
*/

#define _CGE_USE_FFMPEG_ 1

#ifdef _CGE_USE_FFMPEG_

#include "FrameRecorderWrapper.h"
#include "FrameRecorder.h"

#include "UtilFunctions.h"

using namespace CGE;

extern "C" {

/*
 * Class:     org_wysaid_nativePort_CGEFrameRecorder
 * Method:    nativeCreate
 * Signature: ()Ljava/nio/ByteBuffer;
 */
JNIEXPORT jlong JNICALL Java_org_wysaid_nativePort_FrameRecorder_nativeCreateRecorder
        (JNIEnv *env, jobject) {
    cgePrintGLInfo();
    FrameRecorder *recorder = new FrameRecorder();
    return (jlong) recorder;
}

//视频录制相关 API

JNIEXPORT jboolean JNICALL Java_org_wysaid_nativePort_FrameRecorder_nativeStartRecording
        (JNIEnv *env, jobject, jlong addr, jint fps, jstring filename, jint bitRate) {
    const char *path = env->GetStringUTFChars(filename, 0);
    FrameRecorder *recorder = (FrameRecorder *) addr;
    bool ret = recorder->startRecording(fps, path, bitRate);
    env->ReleaseStringUTFChars(filename, path);
    return ret;
}

JNIEXPORT jboolean JNICALL Java_org_wysaid_nativePort_FrameRecorder_nativeIsRecordingStarted
        (JNIEnv *env, jobject, jlong addr) {
    FrameRecorder *recorder = (FrameRecorder *) addr;
    return recorder->isRecordingStarted();
}

JNIEXPORT jboolean JNICALL Java_org_wysaid_nativePort_FrameRecorder_nativeEndRecording
        (JNIEnv *env, jobject, jlong addr, jboolean shouldSave) {
    FrameRecorder *recorder = (FrameRecorder *) addr;
    return recorder->endRecording(shouldSave);
}

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_FrameRecorder_nativePauseRecording
        (JNIEnv *env, jobject, jlong addr) {
    FrameRecorder *recorder = (FrameRecorder *) addr;
    recorder->pauseRecording();
}

JNIEXPORT jboolean JNICALL Java_org_wysaid_nativePort_FrameRecorder_nativeIsRecordingPaused
        (JNIEnv *env, jobject, jlong addr) {
    FrameRecorder *recorder = (FrameRecorder *) addr;
    return recorder->isRecordingPaused();
}

JNIEXPORT jboolean JNICALL Java_org_wysaid_nativePort_FrameRecorder_nativeResumeRecording
        (JNIEnv *env, jobject, jlong addr) {
    FrameRecorder *recorder = (FrameRecorder *) addr;
    return recorder->resumeRecording();
}

JNIEXPORT jdouble JNICALL Java_org_wysaid_nativePort_FrameRecorder_nativeGetTimestamp
        (JNIEnv *env, jobject, jlong addr) {
    FrameRecorder *recorder = (FrameRecorder *) addr;
    return recorder->getRecordingTimestamp();
}

JNIEXPORT jdouble JNICALL Java_org_wysaid_nativePort_FrameRecorder_nativeGetVideoStreamtime
        (JNIEnv *env, jobject, jlong addr) {
    FrameRecorder *recorder = (FrameRecorder *) addr;
    return recorder->getVideoStreamTime();
}

JNIEXPORT jdouble JNICALL Java_org_wysaid_nativePort_FrameRecorder_nativeGetAudioStreamtime
        (JNIEnv *env, jobject, jlong addr) {
    FrameRecorder *recorder = (FrameRecorder *) addr;
    return recorder->getAudioStreamTime();
}

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_FrameRecorder_nativeRecordImageFrame
        (JNIEnv *env, jobject, jlong addr) {
    FrameRecorder *recorder = (FrameRecorder *) addr;
    recorder->recordImageFrame();
}

// static inline bool isBigEndian()
// {
// 	unsigned short n = 0xabcd;
// 	unsigned char* c = (unsigned char*)&n;
// 	CGE_LOG_INFO("N: %x, c[0]: %x, c[1]: %x", n , c[0], c[1]);
// 	return c[0] == 0xab;
// }

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_FrameRecorder_nativeRecordAudioFrame
        (JNIEnv *env, jobject, jlong addr, jobject audioBuffer, jint bufferLen) {
    unsigned short *buffer = (unsigned short *) env->GetDirectBufferAddress(audioBuffer);

    if (buffer != nullptr) {
        //Java一定是大端， 所以需要判断当前cpu到底是大端还是小端。
        // static bool sIsBigEndian = isBigEndian();

        // if(!sIsBigEndian)
        // {
        // 	unsigned char* buffer2 = (unsigned char*)buffer;
        // 	int total = bufferLen * 2;
        // 	for(int i = 0; i < total; i += 2)
        // 	{
        // 		unsigned char c = buffer2[i];
        // 		buffer2[i] = buffer2[i + 1];
        // 		buffer2[i + 1] = c;
        // 	}
        // }

        FrameRecorder *recorder = (FrameRecorder *) addr;
        CGEVideoEncoderMP4::AudioSampleData audioData;
        audioData.channels = 1;
        audioData.data[0] = buffer;
        audioData.nbSamples[0] = bufferLen;
        recorder->recordAudioFrame(audioData);
    } else {
        loge("Record Audio Frame failed!\n");
    }
}

/////////////////  Face Detection  /////////////////////////////

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_FrameRecorder_nativeSetGlobalFilter
        (JNIEnv *env, jobject, jlong addr, jstring config) {
    static CGETexLoadArg texLoadArg;
    texLoadArg.env = env;
    texLoadArg.cls = env->FindClass("org/wysaid/nativePort/CGENativeLibrary");

    FrameRecorder *recorder = (FrameRecorder *) addr;
    const char *configStr = env->GetStringUTFChars(config, 0);
    recorder->setGlobalFilter(configStr, cgeGlobalTextureLoadFunc, &texLoadArg);
    env->ReleaseStringUTFChars(config, configStr);
}

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_FrameRecorder_nativeSetBeautifyFilter
        (JNIEnv *, jobject, jlong addr) {
    FrameRecorder *recorder = (FrameRecorder *) addr;
    auto *handler = recorder->getImageHandler();
    int w = -1, h = -1;

    char buffer[512];
    const char *config = "#unpack @beautify face 1.0";

    if (handler != nullptr) {
        const auto &sz = handler->getOutputFBOSize();
        w = sz.width;
        h = sz.height;
    }

    if (w > 0 && h > 0) {
        sprintf(buffer, "%s %d %d", config, w, h);
        config = buffer;
    }

    recorder->setGlobalFilter(config, nullptr, nullptr);
    logi("启用美化效果!");
}

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_FrameRecorder_nativeSetGlobalFilterIntensity
        (JNIEnv *env, jobject, jlong addr, jfloat intensity) {
    FrameRecorder *recorder = (FrameRecorder *) addr;
    recorder->setGlobalFilterIntensity(intensity);
}

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_FrameRecorder_nativeIsGlobalFilterEnabled
        (JNIEnv *, jobject, jlong addr) {
    FrameRecorder *recorder = (FrameRecorder *) addr;
     recorder->getGlobalFilter() != nullptr;
}

}

#endif





