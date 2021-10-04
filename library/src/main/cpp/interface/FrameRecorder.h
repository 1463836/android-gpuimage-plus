/*
* cgeFrameRecorder.h
*
*  Created on: 2015-7-29
*      Author: Wang Yang
*        Mail: admin@wysaid.org
*/

#if !defined(_CGEFRAMERECORDER_H_) && defined(_CGE_USE_FFMPEG_)
#define _CGEFRAMERECORDER_H_

#include "FrameRenderer.h"
#include "VideoEncoder.h"
#include "ImageFilter.h"

#include <queue>

// cgeFrameRecorder 适用于实时视频录制， 不适宜视频转储。

namespace CGE
{

    class ImageDataWriteThread : public CGEThreadPreemptive
    {
    public:
        ImageDataWriteThread();
        ~ImageDataWriteThread();

        //使用最大N个缓存
        enum { MAX_DATA_CACHE_LEN = 3 };

        struct DataCache
        {
            // DataCache() : buffer(nullptr), pts(-1) {}
            unsigned char* buffer;
            long pts;
        };

        DataCache getData4Read();
        void putData4Read(const DataCache& tex);

        DataCache getData4Write();
        void putData4Write(const DataCache& tex);

        bool hasData4Read();
        bool hasData4Write();

        //设置所需buffer大小
        void setBufferAllocSize(int bufferSize)
        {
            bufferSize = bufferSize;
        }

        void clearBuffers();

        void setTask(std::function<void (const DataCache&)> task)
        {
            m_task = task;
        }

    protected:

        void runTask();

    private:
        //动态大小数据缓存
        std::vector<DataCache> totalCaches;
        std::queue<DataCache> m_data4Read;
        std::queue<DataCache> data4Write;

        std::mutex readMutex;
        std::mutex writeMutex;

        int bufferSize;

        std::function<void (const DataCache&)> m_task;
    };

    ///////////////////////////////////////

    class FrameRecorder : public FrameRenderer
    {
    public:
        FrameRecorder();
        ~FrameRecorder();

        //会影响framebuffer设置。
        // void update(GLuint externalTexture, float* transformMatrix);

        void runProc();

        ////////////////////////////////////////////////////////////
        //        视频录制相关
        // 由于实际录制时不一定能够满足帧率， 当帧率不足时，
        // 将通过计算，进行适当的跳帧以满足音视频同步
        // 如果录制帧率过快， 也将丢弃一些帧以达到同步
        ////////////////////////////////////////////////////////////

        typedef CGEVideoEncoderMP4::AudioSampleData AudioSampleData;

        //记录当前图像帧.
        void recordImageFrame();

        //记录音频帧.
        void recordAudioFrame(const AudioSampleData& data);

        //开始录制视频， 参数为帧率(>0)
        //如果启动录制成功则返回true， 否则返回false
        //filename 必须为一个绝对路径文件地址
        bool startRecording(int fps, const char* filename, int bitRate = 1650000);

        //判断视频录制是否启动 (暂停状态也视为已启动)
        bool isRecordingStarted();

        //结束录制视频
        //如果不保存， 则参数为false， 否则为true
        //返回值代表是否保存成功
        bool endRecording(bool shouldSave);

        //暂停录制视频 (将导致音视频线程挂起)
        void pauseRecording();

        //判断视频录制是否暂停 (未启动状态视为未暂停)
        bool isRecordingPaused();

        //继续录制视频
        //返回值表示是否resume成功
        bool resumeRecording();

        //获取当前录制的视频时长
        double getRecordingTimestamp();

        double getVideoStreamTime();
        double getAudioStreamTime();

        //美化相关

        void setGlobalFilter(const char* config, CGETextureLoadFun texLoadFunc, void* loadArg);
        void setGlobalFilterIntensity(float intensity);
        inline ImageFilterInterfaceAbstract* getGlobalFilter() {
            return globalFilter;
        }

    protected:
        void _createOffscreenContext();

        // 录制相关...

        //线程池仅包含一个线程， 作为录制线程使用
        CGEThreadPool* recordThread;
        CGESharedGLContext* offscreenContext; //录制线程使用的离屏context

        std::function<void (void*)> recordingWork;

        ImageDataWriteThread* recordImageThread;

        double recordingTimestamp, m_lastRecordingTime;
        CGEVideoEncoderMP4* encoder;

        int currentPTS;
        int recordFPS;
        bool isRecording, _isRecordingPaused;

        // 人脸跟踪相关
        ImageFilterInterfaceAbstract* globalFilter;
    };

}

#endif
