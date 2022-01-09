package org.wysaid.nativePort;

import java.nio.ShortBuffer;

/**
 * Created by wangyang on 15/7/29.
 */

//A wrapper for native class FrameRecorder
public class FrameRecorder extends FrameRenderer {

    static {
        NativeLibraryLoader.load();
    }

    public FrameRecorder() {
        super(0); //avoid multiple creation.
        nativeAddress = nativeCreateRecorder();
    }

    /////////////////视频录制相关////////////////////

    public boolean startRecording(int fps, String filename) {
        return startRecording(fps, 1650000, filename);
    }

    public boolean startRecording(int fps, int bitRate, String filename) {
        if (nativeAddress != 0)
            return nativeStartRecording(nativeAddress, fps, filename, bitRate);
        return false;
    }

    public boolean isRecordingStarted() {
        if (nativeAddress != 0)
            return nativeIsRecordingStarted(nativeAddress);
        return false;
    }

    public boolean endRecording(boolean shouldSave) {
        if (nativeAddress != 0)
            return nativeEndRecording(nativeAddress, shouldSave);
        return false;
    }

    public void pauseRecording() {
        if (nativeAddress != 0)
            nativePauseRecording(nativeAddress);
    }

    //Not completed by now
//    public boolean isRecordingPaused() {
//        if(mNativeAddress != 0)
//            return nativeIsRecordingPaused(mNativeAddress);
//        return false;
//    }
//
//    public boolean resumeRecording() {
//        if(mNativeAddress != 0)
//            return nativeResumeRecording(mNativeAddress);
//        return false;
//    }

    public double getTimestamp() {
        if (nativeAddress != 0)
            return nativeGetTimestamp(nativeAddress);
        return 0.0;
    }

    public double getVideoStreamtime() {
        if (nativeAddress != 0)
            return nativeGetVideoStreamtime(nativeAddress);
        return 0.0;
    }

    public double getAudioStreamtime() {
        if (nativeAddress != 0)
            return nativeGetAudioStreamtime(nativeAddress);
        return 0.0;
    }

    public void setTempDir(String dir) {
        if (nativeAddress != 0)
            nativeSetTempDir(nativeAddress, dir);
    }

    //需要置于GPU绘制线程
    public void recordImageFrame() {
        if (nativeAddress != 0)
            nativeRecordImageFrame(nativeAddress);
    }

    //需要自行loop
    public void recordAudioFrame(ShortBuffer audioBuffer, int bufferLen) {
        if (nativeAddress != 0)
            nativeRecordAudioFrame(nativeAddress, audioBuffer, bufferLen);
    }

    public void setGlobalFilter(String config) {
        if (nativeAddress != 0)
            nativeSetGlobalFilter(nativeAddress, config);
    }

    public void setBeautifyFilter() {
        if (nativeAddress != 0)
            nativeSetBeautifyFilter(nativeAddress);
    }

    public void setGlobalFilterIntensity(float intensity) {
        if (nativeAddress != 0)
            nativeSetGlobalFilterIntensity(nativeAddress, intensity);
    }

    public void isGlobalFilterEnabled() {
        if (nativeAddress != 0)
            nativeIsGlobalFilterEnabled(nativeAddress);
    }

    /////////////////      private         ///////////////////////

    private native long nativeCreateRecorder();

    /////////////////视频录制相关////////////////////
    private native boolean nativeStartRecording(long holder, int fps, String filename, int bitRate);

    private native boolean nativeIsRecordingStarted(long holder);

    private native boolean nativeEndRecording(long holder, boolean shouldSave);

    private native void nativePauseRecording(long holder);

    //    private native boolean nativeIsRecordingPaused(long holder);
//    private native boolean nativeResumeRecording(long holder);
    private native double nativeGetTimestamp(long holder);

    private native double nativeGetVideoStreamtime(long holder);

    private native double nativeGetAudioStreamtime(long holder);

    private native void nativeSetTempDir(long holder, String dir);

    private native void nativeRecordImageFrame(long holder);

    private native void nativeRecordAudioFrame(long holder, ShortBuffer audioBuffer, int bufferLen);

    private native void nativeSetGlobalFilter(long holder, String config);

    private native void nativeSetBeautifyFilter(long holder);

    private native void nativeSetGlobalFilterIntensity(long holder, float intensity);

    private native void nativeIsGlobalFilterEnabled(long holder);
}
