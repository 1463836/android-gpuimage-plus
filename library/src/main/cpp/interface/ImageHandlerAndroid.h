/*
 * cgeImageHandlerAndroid.h
 *
 *  Created on: 2015-12-20
 *      Author: Wang Yang
 *        Mail: admin@wysaid.org
 */

#ifndef _CGE_IMAGEHANDLER_ANDROID_H_
#define _CGE_IMAGEHANDLER_ANDROID_H_

#include "ImageHandler.h"

namespace CGE {
    class CGEImageHandlerAndroid : public CGE::ImageHandler {
    public:
        CGEImageHandlerAndroid();

        ~CGEImageHandlerAndroid();

        bool initWithBitmap(JNIEnv *env, jobject bitmap, bool enableReversion = false);

        jobject getResultBitmap(JNIEnv *env);

        void processingFilters();

        void swapBufferFBO();

    protected:

    };

}


#endif