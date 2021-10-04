/*
* cgeNativeLibrary.cpp
*
*  Created on: 2015-7-9
*      Author: Wang Yang
*        Mail: admin@wysaid.org
*/

#include <jni.h>
#include <android/bitmap.h>
#include <ctime>

#include "NativeLibrary.h"
#include "Global.h"
#include "GLFunctions.h"
#include "SharedGLContext.h"

#include "Filters.h"

#include "UtilFunctions.h"

using namespace CGE;

extern "C" {

/*
 * Class:     org_wysaid_nativePort_CGENativeLibrary
 * Method:    filterImage_MultipleEffects
 * Signature: (Landroid/graphics/Bitmap;Ljava/lang/String;)Landroid/graphics/Bitmap;
 */
JNIEXPORT jobject JNICALL
Java_org_wysaid_nativePort_CGENativeLibrary_cgeFilterImage_1MultipleEffects
        (JNIEnv *env, jclass cls, jobject bmp, jstring config, jfloat intensity) {
    CGETexLoadArg texLoadArg;
    texLoadArg.env = env;
    texLoadArg.cls = cls;

    AndroidBitmapInfo info;
    int w, h, ret;
    char *row;

    CGE_LOG_CODE(clock_t
                         tm = clock();)

    if ((ret = AndroidBitmap_getInfo(env, bmp, &info)) < 0) {
        loge("AndroidBitmap_getInfo() failed ! error=%d", ret);
        return nullptr;
    }
    logi("color image :: width is %d; height is %d; stride is %d; format is %d;flags is %d",
         info.width, info.height, info.stride, info.format, info.flags);
    if (info.format != ANDROID_BITMAP_FORMAT_RGBA_8888) {
        loge("Bitmap format is not RGBA_8888 !");
        return nullptr;
    }

    // get the basic information of the image
    w = info.width;
    h = info.height;

    jobject newBitmap;
    jclass bitmapCls = env->GetObjectClass(bmp);

    ret = AndroidBitmap_lockPixels(env, bmp, (void **) &row);
    if (ret < 0) {
        loge("AndroidBitmap_lockPixels() failed ! error=%d", ret);
        return nullptr;
    }

    CGESharedGLContext *glContext = CGESharedGLContext::create();
    if (glContext == nullptr) {
        loge("Create Context Failed!");
        return bmp;
    }

    glContext->makecurrent();

    {
        ImageHandler handler;
        handler.initWithRawBufferData(row, w, h, CGE_FORMAT_RGBA_INT8, false);
        AndroidBitmap_unlockPixels(env, bmp);

        CGEMutipleEffectFilter *filter = new CGEMutipleEffectFilter;

        filter->setTextureLoadFunction(cgeGlobalTextureLoadFunc, &texLoadArg);

        const char *strConfig = env->GetStringUTFChars(config, 0);
        filter->initWithEffectString(strConfig);
        env->ReleaseStringUTFChars(config, strConfig);

        filter->setIntensity(intensity);

        handler.addImageFilter(filter);
        handler.processingFilters();

        jmethodID createBitmapFunction = env->GetStaticMethodID(bitmapCls, "createBitmap",
                                                                "(IILandroid/graphics/Bitmap$Config;)Landroid/graphics/Bitmap;");
        jstring configName = env->NewStringUTF("ARGB_8888");
        jclass bitmapConfigClass = env->FindClass("android/graphics/Bitmap$Config");
        jmethodID valueOfBitmapConfigFunction = env->GetStaticMethodID(bitmapConfigClass, "valueOf",
                                                                       "(Ljava/lang/String;)Landroid/graphics/Bitmap$Config;");
        jobject bitmapConfig = env->CallStaticObjectMethod(bitmapConfigClass,
                                                           valueOfBitmapConfigFunction, configName);
        env->DeleteLocalRef(configName);
        newBitmap = env->CallStaticObjectMethod(bitmapCls, createBitmapFunction, info.width,
                                                info.height, bitmapConfig);

        ret = AndroidBitmap_lockPixels(env, newBitmap, (void **) &row);
        if (ret < 0) {
            loge("AndroidBitmap_lockPixels() failed ! error=%d", ret);
            // AndroidBitmap_unlockPixels(env, bmp);
            return nullptr;
        }

        handler.getOutputBufferData(row, CGE_FORMAT_RGBA_INT8);
        AndroidBitmap_unlockPixels(env, newBitmap);
    }

    logi("unlocked pixels, function totalTime: %g s",
                 (clock() - tm) / (float) CLOCKS_PER_SEC);

    delete glContext;
    return newBitmap;
}

/*
 * Class:     org_wysaid_nativePort_CGENativeLibrary
 * Method:    filterImage_MultipleEffectsWriteBack
 * Signature: (Landroid/graphics/Bitmap;Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL
Java_org_wysaid_nativePort_CGENativeLibrary_cgeFilterImage_1MultipleEffectsWriteBack
        (JNIEnv *env, jclass cls, jobject bmp, jstring config, jfloat intensity) {
    CGETexLoadArg texLoadArg;
    texLoadArg.env = env;
    texLoadArg.cls = cls;

    AndroidBitmapInfo info;
    int w, h, ret;
    char *row;

    CGE_LOG_CODE(clock_t
                         tm = clock();)

    if ((ret = AndroidBitmap_getInfo(env, bmp, &info)) < 0) {
        loge("AndroidBitmap_getInfo() failed ! error=%d", ret);
        return;
    }
    logi("color image :: width is %d; height is %d; stride is %d; format is %d;flags is %d",
         info.width, info.height, info.stride, info.format, info.flags);
    if (info.format != ANDROID_BITMAP_FORMAT_RGBA_8888) {
        loge("Bitmap format is not RGBA_8888 !");
        return;
    }
    // get the basic information of the image
    w = info.width;
    h = info.height;

    ret = AndroidBitmap_lockPixels(env, bmp, (void **) &row);

    if (ret < 0) {
        loge("AndroidBitmap_lockPixels() failed ! error=%d", ret);
        return;
    }

    CGESharedGLContext *glContext = CGESharedGLContext::create();

    if (glContext == nullptr) {
        loge("Create Context Failed!");
        return;
    }

    glContext->makecurrent();

    {
        ImageHandler handler;
        handler.initWithRawBufferData(row, w, h, CGE_FORMAT_RGBA_INT8, false);

        CGEMutipleEffectFilter *filter = new CGEMutipleEffectFilter;

        filter->setTextureLoadFunction(cgeGlobalTextureLoadFunc, &texLoadArg);

        const char *strConfig = env->GetStringUTFChars(config, 0);
        filter->initWithEffectString(strConfig);
        env->ReleaseStringUTFChars(config, strConfig);

        filter->setIntensity(intensity);

        handler.addImageFilter(filter);
        handler.processingFilters();

        logi("Reading results....");
        handler.getOutputBufferData(row, CGE_FORMAT_RGBA_INT8);
        logi("Reading results OK....");
        logi("unlocking pixels....");
        AndroidBitmap_unlockPixels(env, bmp);
        logi("unlocked pixels, function totalTime: %g s",
                     (clock() - tm) / (float) CLOCKS_PER_SEC);
    }

    delete glContext;

}

JNIEXPORT jlong JNICALL Java_org_wysaid_nativePort_CGENativeLibrary_cgeCreateFilterWithConfig
        (JNIEnv *env, jclass, jstring config) {
    CGETexLoadArg texLoadArg;
    texLoadArg.env = env;
    texLoadArg.cls = env->FindClass("org/wysaid/nativePort/CGENativeLibrary");

    const char *configStr = env->GetStringUTFChars(config, 0);

    CGEMutipleEffectFilter *filter = new CGEMutipleEffectFilter;
    filter->setTextureLoadFunction(cgeGlobalTextureLoadFunc, &texLoadArg);

    if (!filter->initWithEffectString(configStr)) {
        delete filter;
        filter = nullptr;
    }

    env->ReleaseStringUTFChars(config, configStr);

    return (jlong) filter;
}

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGENativeLibrary_cgeDeleteFilterWithAddress
        (JNIEnv *env, jclass, jlong addr) {
    delete (ImageFilterInterfaceAbstract *) addr;
}

// filterType: normal, keep_ratio, tile

JNIEXPORT jlong JNICALL Java_org_wysaid_nativePort_CGENativeLibrary_cgeCreateBlendFilter
        (JNIEnv *env, jclass, jint blendMode, jint texID, jint texWidth, jint texHeight,
         jint filterType, jfloat intensity) {
    CGEBlendWithResourceFilter *filter = nullptr;

    switch (filterType) {
        case 0:
            filter = new CGEBlendWithResourceFilter();
            logi("Creating normal blend filter...");
            break;
        case 1:
            filter = new CGEBlendKeepRatioFilter();
            logi("Creating keep-ratio blend filter...");
            break;
        case 2:
            filter = new CGEBlendTileFilter();
            logi("Creating tile blend filter...");
            break;
        default:
            loge("Invalid filter type!\n");
            return 0;
    }

    if (filter->initWithMode((CGETextureBlendMode) blendMode)) {
        filter->setSamplerID(texID);
        filter->setTexSize(texWidth, texHeight);
        filter->setIntensity(intensity);
    } else {
        delete filter;
        filter = nullptr;
    }

    return (jlong) filter;
}


}







