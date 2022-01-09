package org.wysaid.nativePort;

/**
 * Created by wangyang on 15/11/26.
 */

//A wrapper for native class FrameRecorder
public class FrameRenderer {

    static {
        NativeLibraryLoader.load();
    }

    protected long nativeAddress;

    public FrameRenderer() {
        nativeAddress = nativeCreateRenderer();
    }

    //Avoid 'nativeCreateRenderer' being called.
    protected FrameRenderer(int dummy) {

    }

    //srcWidth&srcheight stands for the external_texture's width&height
    //dstWidth&dstHeight stands for the recording resolution (default 640*480)
    //dstWidth/dstHeight should not be changed after "init()" is called.
    //srcWidth/srcHeight could be changed by calling "srcResize" function.
    public boolean init(int srcWidth, int srcHeight, int dstWidth, int dstHeight) {
        if (nativeAddress != 0)
            return nativeInit(nativeAddress, srcWidth, srcHeight, dstWidth, dstHeight);
        return false;
    }

    //Will affect the framebuffer
    public void update(int textureOES, float[] transformMatrix) {
        if (nativeAddress != 0)
            nativeUpdate(nativeAddress, textureOES, transformMatrix);
    }

    public void runProc() {
        if (nativeAddress != 0)
            nativeRunProc(nativeAddress);
    }

    //Won't affect the framebuffer
    //the arguments means the viewport.
    public void render(int x, int y, int width, int height) {
        if (nativeAddress != 0)
            nativeRender(nativeAddress, x, y, width, height);
    }

    public void drawCache() {
        if (nativeAddress != 0)
            nativeDrawCache(nativeAddress);
    }

    //set the rotation of the camera texture
    public void setSrcRotation(float rad) {
        if (nativeAddress != 0)
            nativeSetSrcRotation(nativeAddress, rad);
    }

    //set the flip/scaling for the camera texture
    public void setSrcFlipScale(float x, float y) {
        if (nativeAddress != 0)
            nativeSetSrcFlipScale(nativeAddress, x, y);
    }

    //set the render result's rotation
    public void setRenderRotation(float rad) {
        if (nativeAddress != 0)
            nativeSetRenderRotation(nativeAddress, rad);
    }

    //set the render result's flip/scaling
    public void setRenderFlipScale(float x, float y) {
        if (nativeAddress != 0)
            nativeSetRenderFlipScale(nativeAddress, x, y);
    }

    //initialize the filters width config string
    public void setFilterWidthConfig(final String config) {
        if (nativeAddress != 0)
            nativeSetFilterWithConfig(nativeAddress, config);
    }

    //set the mask rotation (radian)
    public void setMaskRotation(float rot) {
        if (nativeAddress != 0)
            nativeSetMaskRotation(nativeAddress, rot);
    }

    //set the mask flipscale
    public void setMaskFlipScale(float x, float y) {
        if (nativeAddress != 0)
            nativeSetMaskFlipScale(nativeAddress, x, y);

    }


    //set the intensity of the filter
    public void setFilterIntensity(float value) {
        if (nativeAddress != 0)
            nativeSetFilterIntensity(nativeAddress, value);
    }

    public void srcResize(int width, int height) {
        if (nativeAddress != 0)
            nativeSrcResize(nativeAddress, width, height);
    }

    public void release() {
        if (nativeAddress != 0) {
            nativeRelease(nativeAddress);
            nativeAddress = 0;
        }
    }


    public void setMaskTexture(int texID, float aspectRatio) {
        if (nativeAddress != 0)
            nativeSetMaskTexture(nativeAddress, texID, aspectRatio);
    }

    public void setMaskTextureRatio(float aspectRatio) {
        if (nativeAddress != 0)
            nativeSetMaskTextureRatio(nativeAddress, aspectRatio);
    }

    public int queryBufferTexture() {
        if (nativeAddress != 0)
            return nativeQueryBufferTexture(nativeAddress);
        return 0;
    }

    public long getImageHandler() {
        return nativeGetImageHandler(nativeAddress);
    }

    public void bindImageFBO() {
        nativeBindImageFBO(nativeAddress);
    }

    public void swapImageFBO() {
        nativeSwapBufferFBO(nativeAddress);
    }

    //nativeFilter 为 JNI 下的 CGEImageFilterInterfaceAbstract 或者其子类.
    public void processWithFilter(long nativeFilter) {
        nativeProcessWithFilter(nativeAddress, nativeFilter);
    }

    //用于加入自定义滤镜
    public void setNativeFilter(long nativeFilter) {
        nativeSetFilterWithAddr(nativeAddress, nativeFilter);
    }

    /////////////////      protected         ///////////////////////

    protected native long nativeCreateRenderer();

    protected native boolean nativeInit(long holder, int srcWidth, int srcHeight, int dstWidth, int dstHeight);

    protected native void nativeUpdate(long holder, int textureOES, float[] transformMatrix);

    protected native void nativeRunProc(long holder);

    protected native void nativeRender(long holder, int x, int y, int width, int height);

    protected native void nativeDrawCache(long holder);

    protected native void nativeSetSrcRotation(long holder, float rad);

    protected native void nativeSetSrcFlipScale(long holder, float x, float y);

    protected native void nativeSetRenderRotation(long holder, float rad);

    protected native void nativeSetRenderFlipScale(long holder, float x, float y);

    protected native void nativeSetFilterWithConfig(long holder, String config);

    protected native void nativeSetFilterIntensity(long holder, float value);

    protected native void nativeSetMaskRotation(long holder, float value);

    protected native void nativeSetMaskFlipScale(long holder, float x, float y);

    protected native void nativeSrcResize(long holder, int width, int height);

    protected native void nativeSetMaskTexture(long holder, int texID, float aspectRatio);

    protected native void nativeSetMaskTextureRatio(long holder, float aspectRatio);

    protected native void nativeRelease(long holder);

    protected native int nativeQueryBufferTexture(long holder);

    protected native long nativeGetImageHandler(long holder);

    protected native void nativeBindImageFBO(long holder);

    protected native void nativeSwapBufferFBO(long holder);

    //辅助方法
    protected native void nativeProcessWithFilter(long holder, long nativeFilter);

    //特殊用法, 谨慎使用, 使用不当可能造成程序运行异常.
    protected native void nativeSetFilterWithAddr(long holder, long filter);
}
