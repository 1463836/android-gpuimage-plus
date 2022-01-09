/*
* cgeFrameRendererWrapper.h
*
*  Created on: 2015-11-26
*      Author: Wang Yang
*        Mail: admin@wysaid.org
*/

#include "FrameRendererWrapper.h"
#include "UtilFunctions.h"
#include "FrameRenderer.h"

using namespace CGE;


extern "C" {

/*
 * Class:     org_wysaid_nativePort_CGEFrameRenderer
 * Method:    nativeCreate
 * Signature: ()Ljava/nio/ByteBuffer;
 */
JNIEXPORT jlong JNICALL Java_org_wysaid_nativePort_FrameRenderer_nativeCreateRenderer
        (JNIEnv *env, jobject) {
    printGLInfo();
    FrameRenderer *renderer = new FrameRenderer();
    return (jlong) renderer;
}

/*
 * Class:     org_wysaid_nativePort_CGEFrameRenderer
 * Method:    nativeInit
 * Signature: (Ljava/nio/ByteBuffer;IIII)Z
 */
JNIEXPORT jboolean JNICALL Java_org_wysaid_nativePort_FrameRenderer_nativeInit
        (JNIEnv *env, jobject, jlong addr, jint srcW, jint srcH, jint dstW, jint dstH) {
    FrameRenderer *renderer = (FrameRenderer *) addr;
    bool ret = renderer->init(srcW, srcH, dstW, dstH);
//    logi("native instance address: %p init result %d", renderer, ret);
    return ret;
}

/*
 * Class:     org_wysaid_nativePort_CGEFrameRenderer
 * Method:    nativeUpdate
 * Signature: (Ljava/nio/ByteBuffer;I)V
 */
JNIEXPORT void JNICALL Java_org_wysaid_nativePort_FrameRenderer_nativeUpdate
        (JNIEnv *env, jobject, jlong addr, jint textureOES, jfloatArray matrix) {
    jfloat *mat = env->GetFloatArrayElements(matrix, nullptr);
    FrameRenderer *renderer = (FrameRenderer *) addr;
    renderer->update(textureOES, mat);
    env->ReleaseFloatArrayElements(matrix, mat, 0);
}

/*
 * Class:     org_wysaid_nativePort_CGEFrameRenderer
 * Method:    nativeRender
 * Signature: (Ljava/nio/ByteBuffer;IIII)V
 */
JNIEXPORT void JNICALL Java_org_wysaid_nativePort_FrameRenderer_nativeRender
        (JNIEnv *env, jobject, jlong addr, jint x, jint y, jint width, jint height) {
    FrameRenderer *renderer = (FrameRenderer *) addr;
    renderer->render(x, y, width, height);
}

/*
 * Class:     org_wysaid_nativePort_CGEFrameRenderer
 * Method:    nativeSetSrcRotation
 * Signature: (Ljava/nio/ByteBuffer;F)V
 */
JNIEXPORT void JNICALL Java_org_wysaid_nativePort_FrameRenderer_nativeSetSrcRotation
        (JNIEnv *env, jobject, jlong addr, jfloat rad) {
    FrameRenderer *renderer = (FrameRenderer *) addr;
    renderer->setSrcRotation(rad);
}

/*
 * Class:     org_wysaid_nativePort_CGEFrameRenderer
 * Method:    nativeSetSrcFlipScale
 * Signature: (Ljava/nio/ByteBuffer;FF)V
 */
JNIEXPORT void JNICALL Java_org_wysaid_nativePort_FrameRenderer_nativeSetSrcFlipScale
        (JNIEnv *env, jobject, jlong addr, jfloat x, jfloat y) {
    FrameRenderer *renderer = (FrameRenderer *) addr;
    renderer->setSrcFlipScale(x, y);
}

/*
 * Class:     org_wysaid_nativePort_CGEFrameRenderer
 * Method:    nativeSetRenderRotation
 * Signature: (Ljava/nio/ByteBuffer;F)V
 */
JNIEXPORT void JNICALL Java_org_wysaid_nativePort_FrameRenderer_nativeSetRenderRotation
        (JNIEnv *env, jobject, jlong addr, jfloat rad) {
    FrameRenderer *renderer = (FrameRenderer *) addr;
    renderer->setRenderRotation(rad);
}

/*
 * Class:     org_wysaid_nativePort_CGEFrameRenderer
 * Method:    nativeSetRenderFlipScale
 * Signature: (Ljava/nio/ByteBuffer;FF)V
 */
JNIEXPORT void JNICALL Java_org_wysaid_nativePort_FrameRenderer_nativeSetRenderFlipScale
        (JNIEnv *env, jobject, jlong addr, jfloat x, jfloat y) {
    FrameRenderer *renderer = (FrameRenderer *) addr;
    renderer->setRenderFlipScale(x, y);
}

/*
 * Class:     org_wysaid_nativePort_CGEFrameRenderer
 * Method:    nativeSetFilterWithConfig
 * Signature: (Ljava/nio/ByteBuffer;Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_org_wysaid_nativePort_FrameRenderer_nativeSetFilterWithConfig
        (JNIEnv *env, jobject obj, jlong addr, jstring config) {
    static CGETexLoadArg cgeTexLoadArg;
    cgeTexLoadArg.env = env;
    cgeTexLoadArg.cls = env->FindClass("org/wysaid/nativePort/CGENativeLibrary");

    FrameRenderer *renderer = (FrameRenderer *) addr;
    const char *configStr = env->GetStringUTFChars(config, 0);
    renderer->setFilterWithConfig(configStr, cgeGlobalTextureLoadFunc, &cgeTexLoadArg);
    env->ReleaseStringUTFChars(config, configStr);
}

/*
 * Class:     org_wysaid_nativePort_CGEFrameRenderer
 * Method:    nativeSetFilterIntensity
 * Signature: (Ljava/nio/ByteBuffer;F)V
 */
JNIEXPORT void JNICALL Java_org_wysaid_nativePort_FrameRenderer_nativeSetFilterIntensity
        (JNIEnv *env, jobject, jlong addr, jfloat intensity) {
    FrameRenderer *renderer = (FrameRenderer *) addr;
    renderer->setFilterIntensity(intensity);
}

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_FrameRenderer_nativeSrcResize
        (JNIEnv *env, jobject, jlong addr, jint width, jint height) {
    FrameRenderer *renderer = (FrameRenderer *) addr;
    renderer->srcResize(width, height);
}

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_FrameRenderer_nativeSetMaskTexture
        (JNIEnv *env, jobject, jlong addr, jint texID, jfloat aspectRatio) {
    FrameRenderer *renderer = (FrameRenderer *) addr;
    renderer->setMaskTexture(texID, aspectRatio);
}

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_FrameRenderer_nativeSetMaskTextureRatio
        (JNIEnv *env, jobject, jlong addr, jfloat aspectRatio) {
    FrameRenderer *renderer = (FrameRenderer *) addr;
    renderer->setMaskTextureRatio(aspectRatio);
}

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_FrameRenderer_nativeRelease
        (JNIEnv *env, jobject, jlong addr) {
    FrameRenderer *renderer = (FrameRenderer *) addr;
    delete renderer;
}

JNIEXPORT jint JNICALL Java_org_wysaid_nativePort_FrameRenderer_nativeQueryBufferTexture
        (JNIEnv *env, jobject, jlong addr) {
    FrameRenderer *renderer = (FrameRenderer *) addr;
    return (jint) renderer->getBufferTexture();
}

JNIEXPORT jint JNICALL Java_org_wysaid_nativePort_CGEFrameRenderer_nativeQueryTargetTexture
        (JNIEnv *env, jobject, jlong addr) {
    FrameRenderer *renderer = (FrameRenderer *) addr;
    return (jint) renderer->getTargetTexture();
}

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_FrameRenderer_nativeSetMaskRotation
        (JNIEnv *env, jobject, jlong addr, jfloat rad) {
    FrameRenderer *renderer = (FrameRenderer *) addr;
    renderer->setMaskRotation(rad);
}

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_FrameRenderer_nativeSetMaskFlipScale
        (JNIEnv *env, jobject, jlong addr, jfloat x, jfloat y) {
    FrameRenderer *renderer = (FrameRenderer *) addr;
    renderer->setMaskFlipScale(x, y);
}

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_FrameRenderer_nativeRunProc
        (JNIEnv *env, jobject, jlong addr) {
    FrameRenderer *renderer = (FrameRenderer *) addr;
    renderer->runProc();
}

/////////////////  Render Utils  /////////////////////////////

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_FrameRenderer_nativeDrawCache
        (JNIEnv *env, jobject, jlong addr) {
    FrameRenderer *renderer = (FrameRenderer *) addr;
    renderer->drawCache();
}

JNIEXPORT jlong JNICALL Java_org_wysaid_nativePort_FrameRenderer_nativeGetImageHandler
        (JNIEnv *env, jobject, jlong addr) {
    FrameRenderer *renderer = (FrameRenderer *) addr;
    return (jlong) renderer->getImageHandler();
}

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_FrameRenderer_nativeBindImageFBO
        (JNIEnv *env, jobject, jlong addr) {
    FrameRenderer *renderer = (FrameRenderer *) addr;
    renderer->bindImageFBO();
}

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_FrameRenderer_nativeSwapBufferFBO
        (JNIEnv *, jobject, jlong addr) {
    FrameRenderer *renderer = (FrameRenderer *) addr;
    renderer->swapBufferFBO();
}

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_FrameRenderer_nativeProcessWithFilter
        (JNIEnv *, jobject, jlong addr, jlong filter) {
    FrameRenderer *renderer = (FrameRenderer *) addr;

    //较危险操作， 慎用.
    renderer->getImageHandler()->processingWithFilter((ImageFilterInterfaceAbstract *) filter);
}

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_FrameRenderer_nativeSetFilterWithAddr
        (JNIEnv *, jobject, jlong addr, jlong filter) {
    FrameRenderer *renderer = (FrameRenderer *) addr;
    renderer->setFilter((ImageFilterInterfaceAbstract *) filter);
}


}







