#include "rubymotion.h"
#if CC_TARGET_OS_ANDROID
# include "platform/android/jni/JniHelper.h"
#endif

VALUE rb_mMC = Qnil;

extern "C"
void
Init_Fluency(void)
{
#if CC_TARGET_OS_ANDROID
    auto java_vm = VM_JAVA_VM();
    assert(java_vm != NULL);
    cocos2d::JniHelper::setJavaVM(java_vm);

#define USE_SYMBOL(x) { extern void *x; void *_sym = x; }
    USE_SYMBOL(Java_org_cocos2dx_lib_GameControllerAdapter_nativeControllerAxisEvent)
    USE_SYMBOL(Java_org_cocos2dx_lib_GameControllerAdapter_nativeControllerButtonEvent)
    USE_SYMBOL(Java_org_cocos2dx_lib_GameControllerAdapter_nativeControllerConnected)
    USE_SYMBOL(Java_org_cocos2dx_lib_GameControllerAdapter_nativeControllerDisconnected)
    USE_SYMBOL(Java_org_cocos2dx_lib_Cocos2dxVideoHelper_nativeExecuteVideoCallback)
    USE_SYMBOL(Java_org_cocos2dx_lib_Cocos2dxWebViewHelper_didFailLoading)
    USE_SYMBOL(Java_org_cocos2dx_lib_Cocos2dxWebViewHelper_didFinishLoading)
    USE_SYMBOL(Java_org_cocos2dx_lib_Cocos2dxWebViewHelper_onJsCallback)
    USE_SYMBOL(Java_org_cocos2dx_lib_Cocos2dxWebViewHelper_shouldStartLoading)
    USE_SYMBOL(Java_org_cocos2dx_lib_Cocos2dxBitmap_nativeInitBitmapDC)
    USE_SYMBOL(Java_org_cocos2dx_lib_Cocos2dxActivity_getGLContextAttrs)
    USE_SYMBOL(Java_org_cocos2dx_lib_Cocos2dxRenderer_nativeInit)
    USE_SYMBOL(Java_org_cocos2dx_lib_Cocos2dxRenderer_nativeOnSurfaceChanged)
    USE_SYMBOL(Java_org_cocos2dx_lib_Cocos2dxAccelerometer_onSensorChanged)
    USE_SYMBOL(Java_org_cocos2dx_lib_Cocos2dxHelper_nativeSetApkPath)
    USE_SYMBOL(Java_org_cocos2dx_lib_Cocos2dxHelper_nativeSetContext)
    USE_SYMBOL(Java_org_cocos2dx_lib_Cocos2dxHelper_nativeSetEditTextDialogResult)
    USE_SYMBOL(Java_org_cocos2dx_lib_Cocos2dxRenderer_nativeDeleteBackward)
    USE_SYMBOL(Java_org_cocos2dx_lib_Cocos2dxRenderer_nativeGetContentText)
    USE_SYMBOL(Java_org_cocos2dx_lib_Cocos2dxRenderer_nativeInsertText)
    USE_SYMBOL(Java_org_cocos2dx_lib_Cocos2dxRenderer_nativeOnPause)
    USE_SYMBOL(Java_org_cocos2dx_lib_Cocos2dxRenderer_nativeOnResume)
    USE_SYMBOL(Java_org_cocos2dx_lib_Cocos2dxRenderer_nativeRender)
    /* USE_SYMBOL(Java_org_cocos2dx_lib_Cocos2dxRenderer_nativeKeyDown) */
    USE_SYMBOL(Java_org_cocos2dx_lib_Cocos2dxRenderer_nativeTouchesBegin)
    USE_SYMBOL(Java_org_cocos2dx_lib_Cocos2dxRenderer_nativeTouchesCancel)
    USE_SYMBOL(Java_org_cocos2dx_lib_Cocos2dxRenderer_nativeTouchesEnd)
    USE_SYMBOL(Java_org_cocos2dx_lib_Cocos2dxRenderer_nativeTouchesMove)
#undef USE_SYMBOL
#endif

    rb_mMC = rb_define_module("MG");

#if CC_TARGET_OS_ANDROID
# define ADD_FRAME Frame _frame
#else
# define ADD_FRAME
#endif

#define INIT_MODULE(x) \
    extern void Init_##x(void); \
    do { \
	ADD_FRAME; \
	Init_##x(); \
    } \
    while (0);

    INIT_MODULE(Application)
    INIT_MODULE(Director)
    INIT_MODULE(Events)
    INIT_MODULE(Node)
    INIT_MODULE(Layer)
    INIT_MODULE(Menu)
    INIT_MODULE(Sprite)
    INIT_MODULE(Particle)
    INIT_MODULE(Audio)
    INIT_MODULE(Types)
    INIT_MODULE(UI)

#undef INIT_MODULE
#undef ADD_FRAME
}
