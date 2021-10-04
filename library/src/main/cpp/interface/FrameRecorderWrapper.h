/*
* cgeFrameRecorderWrapper.h
*
*  Created on: 2015-7-9
*      Author: Wang Yang
*        Mail: admin@wysaid.org
*/

#include <jni.h>
/* Header for class org_wysaid_nativePort_CGEFrameRecorder */

#ifndef _Included_org_wysaid_nativePort_CGEFrameRecorder
#define _Included_org_wysaid_nativePort_CGEFrameRecorder

//#define __cplusplus 1

#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     org_wysaid_nativePort_CGEFrameRecorder
 * Method:    nativeCreate
 * Signature: ()Ljava/nio/ByteBuffer;
 */
JNIEXPORT jlong

JNICALL Java_org_wysaid_nativePort_FrameRecorder_nativeCreateRecorder
        (JNIEnv *, jobject);

//视频录制相关API

JNIEXPORT jboolean

JNICALL Java_org_wysaid_nativePort_FrameRecorder_nativeStartRecording
        (JNIEnv *, jobject, jlong, jint, jstring, jint);

JNIEXPORT jboolean

JNICALL Java_org_wysaid_nativePort_FrameRecorder_nativeIsRecordingStarted
        (JNIEnv *, jobject, jlong);

JNIEXPORT jboolean

JNICALL Java_org_wysaid_nativePort_FrameRecorder_nativeEndRecording
        (JNIEnv *, jobject, jlong, jboolean);

JNIEXPORT void JNICALL
Java_org_wysaid_nativePort_FrameRecorder_nativePauseRecording
(JNIEnv
*, jobject, jlong);

JNIEXPORT jboolean
JNICALL Java_org_wysaid_nativePort_FrameRecorder_nativeIsRecordingPaused
        (JNIEnv * , jobject, jlong);

JNIEXPORT jboolean
JNICALL Java_org_wysaid_nativePort_FrameRecorder_nativeResumeRecording
        (JNIEnv * , jobject, jlong);

JNIEXPORT jdouble
JNICALL Java_org_wysaid_nativePort_FrameRecorder_nativeGetTimestamp
        (JNIEnv * , jobject, jlong);

JNIEXPORT jdouble
JNICALL Java_org_wysaid_nativePort_FrameRecorder_nativeGetVideoStreamtime
        (JNIEnv * , jobject, jlong);

JNIEXPORT jdouble
JNICALL Java_org_wysaid_nativePort_FrameRecorder_nativeGetAudioStreamtime
        (JNIEnv * , jobject, jlong);

JNIEXPORT void JNICALL
Java_org_wysaid_nativePort_FrameRecorder_nativeSetTempDir
(JNIEnv
*, jobject, jlong, jstring);

JNIEXPORT void JNICALL
Java_org_wysaid_nativePort_FrameRecorder_nativeRecordImageFrame
(JNIEnv
*, jobject, jlong);

JNIEXPORT void JNICALL
Java_org_wysaid_nativePort_FrameRecorder_nativeRecordAudioFrame
(JNIEnv
*, jobject, jlong, jobject, jint);

/////////////////  Face Detection  /////////////////////////////

JNIEXPORT void JNICALL
Java_org_wysaid_nativePort_FrameRecorder_nativeSetGlobalFilter
(JNIEnv
*, jobject, jlong, jstring);

JNIEXPORT void JNICALL
Java_org_wysaid_nativePort_FrameRecorder_nativeSetBeautifyFilter
(JNIEnv
*, jobject, jlong);

JNIEXPORT void JNICALL
Java_org_wysaid_nativePort_FrameRecorder_nativeSetGlobalFilterIntensity
(JNIEnv
*, jobject, jlong, jfloat);

JNIEXPORT void
JNICALL Java_org_wysaid_nativePort_FrameRecorder_nativeIsGlobalFilterEnabled
        (JNIEnv * , jobject, jlong);

#ifdef __cplusplus
}
#endif
#endif

extern "C"
JNIEXPORT void JNICALL
Java_org_wysaid_nativePort_FrameRecorder_nativeSetTempDir(JNIEnv *env, jobject thiz, jlong holder,
                                                          jstring dir) {
    // TODO: implement nativeSetTempDir()
}