#include "rubymotion.h"
#include "motion-game.h"

#if CC_TARGET_OS_ANDROID
# include "platform/android/jni/JniHelper.h"
#endif

VALUE rb_mMC = Qnil;

#if CC_TARGET_OS_IPHONE || CC_TARGET_OS_APPLETV
#include <unordered_map>
static std::unordered_map<VALUE, std::pair<void*, int>> new_funcs;

static VALUE
singleton_new(VALUE klass, SEL sel, int argc, VALUE *argv)
{
    std::unordered_map<VALUE, std::pair<void*, int>>::iterator iter;
    Class k = (Class)klass;
    while (k != NULL) {
	iter = new_funcs.find((VALUE)k);
	if (iter != new_funcs.end()) {
	    break;
	}
	k = class_getSuperclass(k);
    }
    assert(k != NULL);
    auto f = iter->second;
    void *func = f.first;
    int arity = f.second;
    if (arity != -1 && arity != argc) {
	rb_raise(rb_eArgError, "wrong number of arguments (%d for %d)",
		argc, arity);
    }

    VALUE (*imp)(VALUE, SEL, ...) = (VALUE (*)(VALUE, SEL, ...))func;
    if (arity == -1) {
	return (*imp)(klass, sel, argc, argv);
    }

    switch (argc) {
      case 0:
	return (*imp)(klass, sel);
      case 1:
	return (*imp)(klass, sel, argv[0]);
      case 2:
	return (*imp)(klass, sel, argv[0], argv[1]);
      case 3:
	return (*imp)(klass, sel, argv[0], argv[1], argv[2]);
      case 4:
	return (*imp)(klass, sel, argv[0], argv[1], argv[2], argv[3]);
      case 5:
	return (*imp)(klass, sel, argv[0], argv[1], argv[2], argv[3],
		argv[4]);
      case 6:
	return (*imp)(klass, sel, argv[0], argv[1], argv[2], argv[3],
		argv[4], argv[5]);
      case 7:
	return (*imp)(klass, sel, argv[0], argv[1], argv[2], argv[3],
		argv[4], argv[5], argv[6]);
      case 8:
	return (*imp)(klass, sel, argv[0], argv[1], argv[2], argv[3],
		argv[4], argv[5], argv[6], argv[7]);
      case 9:
	return (*imp)(klass, sel, argv[0], argv[1], argv[2], argv[3],
		argv[4], argv[5], argv[6], argv[7], argv[8]);
      case 10:
	return (*imp)(klass, sel, argv[0], argv[1], argv[2], argv[3],
		argv[4], argv[5], argv[6], argv[7], argv[8], argv[9]);
      default:
	rb_raise(rb_eArgError, "doesn't support passing more than 10 arguments");
    }
}
#endif

void
rb_define_constructor0(VALUE klass, void *func, int arity)
{
#if (CC_TARGET_OS_IPHONE || CC_TARGET_OS_APPLETV) && !defined(__arm64__)
    // FIXME : [#67] On arm64 device, it causes an exception.
    new_funcs[klass] = std::make_pair(func, arity);
    rb_define_singleton_method(klass, "new", singleton_new, -1);
#else
    rb_define_singleton_method(klass, "new", func, arity);
#endif
}

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
    INIT_MODULE(GLView)
    INIT_MODULE(Events)
    INIT_MODULE(Node)
    INIT_MODULE(Layer)
    INIT_MODULE(Menu)
    INIT_MODULE(Sprite)
    INIT_MODULE(Action)
    INIT_MODULE(Particle)
    INIT_MODULE(Audio)
    INIT_MODULE(Types)
    INIT_MODULE(UI)
    INIT_MODULE(FileUtils)

#undef INIT_MODULE
#undef ADD_FRAME
}
