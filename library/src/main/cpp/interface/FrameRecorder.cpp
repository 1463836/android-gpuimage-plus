/*
* cgeFrameRecorder.cpp
*
*  Created on: 2015-7-29
*      Author: Wang Yang
*        Mail: admin@wysaid.org
*/

#define _CGE_USE_FFMPEG_
#ifdef _CGE_USE_FFMPEG_

#include "FrameRecorder.h"
#include "UtilFunctions.h"
#include "MultipleEffects.h"

#include <chrono>
#include <new>

#if defined(DEBUG) || defined(_DEBUG)

#define CHECK_RECORDER_STATUS \
if(fastFrameHandler == nullptr)\
{\
    loge("CGEFrameRecorder is not initialized!!\n");\
    return;\
}

#else

#define CHECK_RECORDER_STATUS

#endif

namespace CGE {
    ///////////////////////////////////////

    ImageDataWriteThread::ImageDataWriteThread() : bufferSize(0), m_task(nullptr) {

    }

    ImageDataWriteThread::~ImageDataWriteThread() {
        clearBuffers();
    }

    void ImageDataWriteThread::clearBuffers() {
        for (auto data : totalCaches) {
            delete[] data.buffer;
        }

        totalCaches.clear();
        m_data4Read = decltype(m_data4Read)();
        data4Write = decltype(data4Write)();
    }

    ImageDataWriteThread::DataCache ImageDataWriteThread::getData4Read() {
        DataCache cache;
        readMutex.lock();
        if (m_data4Read.empty()) {
            cache.buffer = nullptr;
            // logi("read data hungry");
        } else {
            cache = m_data4Read.front();
            m_data4Read.pop();
        }
        readMutex.unlock();
        return cache;
    }

    void ImageDataWriteThread::putData4Read(const DataCache &data) {
        readMutex.lock();
        m_data4Read.push(data);
        readMutex.unlock();
        if (!isActive())
            run();
    }

    ImageDataWriteThread::DataCache ImageDataWriteThread::getData4Write() {
        DataCache cache;
        writeMutex.lock();
        if (data4Write.empty()) {
            if (totalCaches.size() < MAX_DATA_CACHE_LEN) {
//                assert(bufferSize != 0); // bufferSize must be set!

                //增加申请内存失败判断
                cache.buffer = new(std::nothrow) unsigned char[bufferSize];

                if (cache.buffer == nullptr) {
                    loge("Fatal Error: Not enough memory!");
                }

                totalCaches.push_back(cache);
                logi("Cache grow: %d", (int) totalCaches.size());
            } else {
                cache.buffer = nullptr;
                logi("write data hungry, cache size: %d", (int) totalCaches.size());
            }
        } else {
            cache = data4Write.front();
            data4Write.pop();
        }
        writeMutex.unlock();
        return cache;
    }

    void ImageDataWriteThread::putData4Write(const DataCache &data) {
        writeMutex.lock();
        data4Write.push(data);
        writeMutex.unlock();
    }

    bool ImageDataWriteThread::hasData4Read() {
        bool canRead;
        readMutex.lock();
        canRead = !m_data4Read.empty();
        readMutex.unlock();
        return canRead;
    }

    bool ImageDataWriteThread::hasData4Write() {
        if (totalCaches.size() < MAX_DATA_CACHE_LEN)
            return true;

        bool canWrite;
        writeMutex.lock();
        canWrite = !data4Write.empty();
        writeMutex.unlock();
        return canWrite;
    }

    void ImageDataWriteThread::runTask() {
        assert(m_task != nullptr); //task should not be nullptr

        while (1) {
            auto data = getData4Read();
            if (data.buffer == nullptr)
                break;

            m_task(data);

            putData4Write(data);
        }
    }

    ///////////////////////////////////////

    FrameRecorder::FrameRecorder() : recordThread(nullptr), recordImageThread(nullptr),
                                     encoder(nullptr), offscreenContext(nullptr),
                                     globalFilter(nullptr) {

        recordingWork = [&](void *pts) {

            CHECK_RECORDER_STATUS;

            auto bufferCache = recordImageThread->getData4Write();

            if (bufferCache.buffer == nullptr)
                return;

            // auto tm = getCurrentTimeMillis();

            if (offscreenContext != nullptr)
                offscreenContext->makecurrent();

            glViewport(0, 0, dstSize.width, dstSize.height);

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glClear(GL_COLOR_BUFFER_BIT);

            resultMutex.lock();
            textureDrawCache->drawTexture(fastFrameHandler->getBufferTextureID());
            // cacheDrawer->drawTexture(fastFrameHandler.getTargetTextureID());
            glFinish();
            // loge("draw texture time: %g", (getCurrentTimeMillis() - tm));
            resultMutex.unlock();

            glReadPixels(0, 0, dstSize.width, dstSize.height, GL_RGBA, GL_UNSIGNED_BYTE,
                         bufferCache.buffer);

            // loge("readpixel time: %g", (getCurrentTimeMillis() - tm));
            bufferCache.pts = (long) pts;
            recordImageThread->putData4Read(bufferCache);

        };
    }

    FrameRecorder::~FrameRecorder() {
//        logi("CGEFrameRecorder::~CGEFrameRecorder");


        endRecording(false);

        if (recordThread != nullptr) {
            logi("m_recordThread kill before...");

            bool bShoudWait = true;

            recordThread->run(CGEThreadPool::Work([&](void *) {
                logi("Delete offscreen context...");
                delete offscreenContext;
                offscreenContext = nullptr;
                bShoudWait = false;
            }));

            while (bShoudWait || recordThread->isActive()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }

            logi("m_recordThread kill after...");

            recordThread->quit();

            delete recordThread;
            recordThread = nullptr;
        }
    }

    // void CGEFrameRecorder::update(GLuint externalTexture, float* transformMatrix)
    // {
    // 	CHECK_RECORDER_STATUS;

    // 	fastFrameHandler->useImageFBO();
    // 	glViewport(m_viewport[0], m_viewport[1], m_viewport[2], m_viewport[3]);

    // 	m_textureDrawerExtOES->setTransform(transformMatrix);
    // 	m_textureDrawerExtOES->drawTexture(externalTexture);
    // }

    void FrameRecorder::runProc() {
        //processingFilters may change targetTextureID&bufferTextureID, lock to ensure safety.
        std::unique_lock<std::mutex> uniqueLock(resultMutex);

        if (globalFilter != nullptr) {
            fastFrameHandler->processingWithFilter(globalFilter);
        }

        fastFrameHandler->processingFilters();

        if (isRecordingStarted() && !_isRecordingPaused) {

            //The first frame must be recorded.
            if (recordingTimestamp == 0.0) {
                recordingTimestamp = 0.0001; //set to 0.0001 ms, stands for started.
                m_lastRecordingTime = getCurrentTimeMillis();
                logi("first frame...");
            } else {
                double currentTime = getCurrentTimeMillis();
                recordingTimestamp += currentTime - m_lastRecordingTime;
                m_lastRecordingTime = currentTime;
                // logi("time stamp %g...", m_recordingTimestamp);
            }

            int ptsInFact = recordingTimestamp * (recordFPS / 1000.0);

            if (ptsInFact < currentPTS) {
                logi("too fast, drop frame...");
                return;
            } else if (ptsInFact > currentPTS + 3) {
                logi("too slow, jump frame...");
                currentPTS = ptsInFact;
            } else {
                // logi("fine speed...");
                if (currentPTS == ptsInFact)
                    currentPTS = ptsInFact + 1;
                else
                    currentPTS = ptsInFact;
            }

            if (recordThread != nullptr) {
                fastFrameHandler->useImageFBO();
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                                       fastFrameHandler->getBufferTextureID(), 0);

                glViewport(0, 0, dstSize.width, dstSize.height);
                textureDrawCache->drawTexture(fastFrameHandler->getTargetTextureID());
                glFinish();
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                                       fastFrameHandler->getTargetTextureID(), 0);

                if (recordThread->isActive() && recordThread->totalWorks() != 0)
                    return;

                recordThread->run(
                        CGEThreadPool::Work(recordingWork, (void *) (size_t) currentPTS));
            } else {
                auto bufferCache = recordImageThread->getData4Write();

                if (bufferCache.buffer != nullptr) {
                    // auto tm = getCurrentTimeMillis();

                    fastFrameHandler->useImageFBO();

                    // loge("draw texture time: %g", (getCurrentTimeMillis() - tm));

                    glReadPixels(0, 0, dstSize.width, dstSize.height, GL_RGBA, GL_UNSIGNED_BYTE,
                                 bufferCache.buffer);

                    // loge("readpixel time: %g", (getCurrentTimeMillis() - tm));
                    bufferCache.pts = currentPTS;
                    recordImageThread->putData4Read(bufferCache);
                }
            }
        }
    }

    /////////////  视频录制相关  ///////////////

    void FrameRecorder::recordImageFrame() {

    }

    void FrameRecorder::recordAudioFrame(const AudioSampleData &data) {
        encoder->record(data);
    }

    bool FrameRecorder::startRecording(int fps, const char *filename, int bitRate) {
        delete encoder;
        encoder = new CGEVideoEncoderMP4();
        encoder->setRecordDataFormat(CGEVideoEncoderMP4::FMT_RGBA8888);
        if (!encoder->init(filename, fps, dstSize.width, dstSize.height, true, bitRate)) {
            delete encoder;
            encoder = nullptr;
            loge("CGEFrameRecorder::startRecording - start recording failed!");
            return false;
        }

        logw("encoder created!");

        if (offscreenContext == nullptr || recordThread == nullptr)
            _createOffscreenContext(); //offscreen context will be created from another thread.

        int bufferLen = dstSize.width * dstSize.height * 4;

        recordImageThread = new ImageDataWriteThread();

        recordImageThread->setBufferAllocSize(bufferLen);

        recordImageThread->setTask([&](const ImageDataWriteThread::DataCache &data) {

            // auto tm = getCurrentTimeMillis();

            CGEVideoEncoderMP4::ImageData imageData;
            imageData.width = dstSize.width;
            imageData.height = dstSize.height;
            imageData.linesize[0] = dstSize.width * 4;
            imageData.data[0] = data.buffer;
            imageData.pts = data.pts;

            // loge("PTS: %d", (int)data.pts);

            if (!encoder->record(imageData)) {
                loge("record frame failed!");
            }

            // loge("pts sequence: %d, time: %g", (int)data.pts, getCurrentTimeMillis() - tm);
        });

        recordFPS = fps;
        currentPTS = -1;
        isRecording = true;
        _isRecordingPaused = false;
        recordingTimestamp = 0.0;

        logw("CGEFrameRecorder::startRecording... %s",filename);

        return true;
    }

    bool FrameRecorder::isRecordingStarted() {
        return encoder != nullptr && isRecording;
    }

    bool FrameRecorder::endRecording(bool shouldSave) {
        isRecording = false;

//        logi("Waiting for the recording threads...");

        //wait for recoding over.
        while ((recordThread != nullptr && recordThread->isActive()) ||
               (recordImageThread != nullptr && recordImageThread->isActive()))
            std::this_thread::sleep_for(std::chrono::milliseconds(1));

        delete recordImageThread;
        recordImageThread = nullptr;

//        logi("threads sync.");

        if (encoder == nullptr)
            return false;

        bool ret = true;
        logi("CGEFrameRecorder::endRecording...");

        if (shouldSave) {
            ret = encoder->save();
        } else {
            encoder->drop();
        }

        logi("delete encoder...");

        delete encoder;
        encoder = nullptr;

        logi("CGEFrameRecorder::endRecording OK...");
        return ret;
    }

    void FrameRecorder::pauseRecording() {
        //Not provided by now.
        // m_isRecordingPaused = true;
        loge("Pause function is not completed by now!!");
    }

    bool FrameRecorder::isRecordingPaused() {
        return encoder != nullptr && _isRecordingPaused;
    }

    bool FrameRecorder::resumeRecording() {
        if (encoder == nullptr)
            return false;
        _isRecordingPaused = false;
        return true;
    }

    double FrameRecorder::getRecordingTimestamp() {
        return recordingTimestamp;
    }

    double FrameRecorder::getVideoStreamTime() {
        return encoder->getVideoStreamTime();
    }

    double FrameRecorder::getAudioStreamTime() {
        return encoder->getAudioStreamTime();
    }

    void FrameRecorder::_createOffscreenContext() {
        EGLContext eglContext = eglGetCurrentContext();

        if (eglContext == EGL_NO_CONTEXT) {
            loge("Context creation must be in the GL thread!");
            return;
        }

        if (recordThread == nullptr)
            recordThread = new CGEThreadPool();

        recordThread->run(CGEThreadPool::Work([&](void *) {

            delete offscreenContext;
            offscreenContext = CGESharedGLContext::create(eglContext, dstSize.width,
                                                          dstSize.height,
                                                          CGESharedGLContext::RECORDABLE_ANDROID);
            if (offscreenContext == nullptr) {
                loge("CGESharedGLContext : RECORDABLE_ANDROID is not supported!");
                offscreenContext = CGESharedGLContext::create(eglContext, dstSize.width,
                                                              dstSize.height,
                                                              CGESharedGLContext::PBUFFER);
                if (offscreenContext == nullptr)
                    loge("Fatal Error: Create Context Failed!");
            }

            if (offscreenContext != nullptr) {
                glViewport(0, 0, dstSize.width, dstSize.height);
                logi("Info from offscreen context thread (begin)....");
                printGLInfo();
                logi("Info from offscreen context thread (end)....");
            } else {
                loge("Create OpenGL child thread failed! The device may be too old!");
            }
        }));

        while (recordThread->isActive())
            std::this_thread::sleep_for(std::chrono::milliseconds(1));

        //创建shared context失败， 将不使用OpenGL子线程
        //OpenGL Child thread will not be used.
        if (offscreenContext == nullptr) {
            recordThread->quit();
            recordThread = nullptr;
        }
    }

    void FrameRecorder::setGlobalFilter(const char *config, CGETextureLoadFun texLoadFunc,
                                        void *loadArg) {
        delete globalFilter;
        globalFilter = nullptr;

        if (config == nullptr || *config == '\0') {
            return;
        }

        auto *filter = new CGEMutipleEffectFilter();

        filter->setTextureLoadFunction(texLoadFunc, loadArg);

        if (!filter->initWithEffectString(config)) {
            delete filter;
            return;
        }

        if (filter->isWrapper()) {
            auto &&v = filter->getFilters();
            if (!v.empty())
                globalFilter = v[0];
        } else {
            globalFilter = filter;
        }
    }

    void FrameRecorder::setGlobalFilterIntensity(float intensity) {
        if (globalFilter != nullptr) {
            globalFilter->setIntensity(intensity);
        } else {
            loge("You must set a tracking filter first!\n");
        }
    }

}

#endif








