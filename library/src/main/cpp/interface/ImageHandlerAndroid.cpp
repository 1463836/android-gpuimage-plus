/*
 * cgeImageHandlerAndroid.cpp
 *
 *  Created on: 2015-12-20
 *      Author: Wang Yang
 *        Mail: admin@wysaid.org
 */

#include "ImageHandlerAndroid.h"
#include <android/bitmap.h>

namespace CGE {
    void CGEImageHandlerAndroid::processingFilters() {
        if (vecFilters.empty() || fboTextures[0] == 0) {
            glFlush();
            return;
        }

        glDisable(GL_BLEND);
        assert(vertexArrayBuffer != 0);

        glViewport(0, 0, dstImageSize.width, dstImageSize.height);

        for (std::vector<ImageFilterInterfaceAbstract *>::iterator iter = vecFilters.begin();
             iter < vecFilters.end(); ++iter) {
            swapBufferFBO();
            glBindBuffer(GL_ARRAY_BUFFER, vertexArrayBuffer);
            (*iter)->render2Texture(this, fboTextures[1], vertexArrayBuffer);
            glFlush();
        }
        glFinish();
        // glFlush();
    }

    void CGEImageHandlerAndroid::swapBufferFBO() {
        useImageFBO();
        std::swap(fboTextures[0], fboTextures[1]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                               fboTextures[0], 0);

        //为了效率， 高帧率绘制期间不检测错误.
    }

    CGEImageHandlerAndroid::CGEImageHandlerAndroid() {
        logi("CGEImageHandlerAndroid created!\n");
    }

    CGEImageHandlerAndroid::~CGEImageHandlerAndroid() {
        logi("CGEImageHandlerAndroid released!\n");
    }

    bool CGEImageHandlerAndroid::initWithBitmap(JNIEnv *env, jobject bmp, bool enableReversion) {
        AndroidBitmapInfo info;

        int ret = AndroidBitmap_getInfo(env, bmp, &info);

        if (ret < 0) {
            loge("AndroidBitmap_getInfo() failed ! error=%d", ret);
            return false;
        }

        logi(
                "color image :: width is %d; height is %d; stride is %d; format is %d;flags is %d",
                info.width, info.height, info.stride, info.format, info.flags);

        if (info.format != ANDROID_BITMAP_FORMAT_RGBA_8888) {
            loge("Bitmap format is not RGBA_8888 !");
            return false;
        }

        char *row;

        ret = AndroidBitmap_lockPixels(env, bmp, (void **) &row);

        if (ret < 0) {
            loge("AndroidBitmap_lockPixels() failed ! error=%d", ret);
            return false;
        }

        bool flag = initWithRawBufferData(row, info.width, info.height, CGE_FORMAT_RGBA_INT8,
                                          enableReversion);

        AndroidBitmap_unlockPixels(env, bmp);

        return flag;
    }

    jobject CGEImageHandlerAndroid::getResultBitmap(JNIEnv *env) {

        jclass bitmapCls = env->FindClass("android/graphics/Bitmap");

        jmethodID createBitmapFunction = env->GetStaticMethodID(bitmapCls, "createBitmap",
                                                                "(IILandroid/graphics/Bitmap$Config;)Landroid/graphics/Bitmap;");
        jstring configName = env->NewStringUTF("ARGB_8888");
        jclass bitmapConfigClass = env->FindClass("android/graphics/Bitmap$Config");
        jmethodID valueOfBitmapConfigFunction = env->GetStaticMethodID(bitmapConfigClass, "valueOf",
                                                                       "(Ljava/lang/String;)Landroid/graphics/Bitmap$Config;");
        jobject bitmapConfig = env->CallStaticObjectMethod(bitmapConfigClass,
                                                           valueOfBitmapConfigFunction, configName);
        env->DeleteLocalRef(configName);
        jobject newBitmap = env->CallStaticObjectMethod(bitmapCls, createBitmapFunction,
                                                        dstImageSize.width, dstImageSize.height,
                                                        bitmapConfig);

        char *row;

        int ret = AndroidBitmap_lockPixels(env, newBitmap, (void **) &row);

        if (ret < 0) {
            loge("AndroidBitmap_lockPixels() failed ! error=%d", ret);
            return nullptr;
        }

        getOutputBufferData(row, CGE_FORMAT_RGBA_INT8);
        AndroidBitmap_unlockPixels(env, newBitmap);

        return newBitmap;
    }


}












