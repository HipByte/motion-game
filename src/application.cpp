#include "rubymotion.h"

/// @class Application < Object

VALUE rb_cApplication = Qnil;
static VALUE mc_application_instance = Qnil;

class mc_Application : private cocos2d::Application {
    public:
	VALUE obj;
	SEL start_sel;

    mc_Application() {
	obj = Qnil;
	start_sel = rb_selector("start");
    }

    virtual bool
    applicationDidFinishLaunching() {
	rb_send(obj, start_sel, 0, NULL);
	return true;
    }

    virtual void
    applicationDidEnterBackground() {

    }

    virtual void
    applicationWillEnterForeground() {

    }
};

/// @method .shared
/// @return [Application] the shared Application instance.

static VALUE
application_instance(VALUE rcv, SEL sel)
{
    if (mc_application_instance == Qnil) {
	VALUE obj = rb_class_wrap_new(
		(void *)cocos2d::Application::getInstance(), rb_cApplication);
	mc_application_instance = rb_retain(obj);
    }
    return mc_application_instance;
}

static VALUE
application_alloc(VALUE rcv, SEL sel)
{
    mc_Application *app = new mc_Application();
    VALUE obj = rb_class_wrap_new((void *)app, rcv);
    app->obj = obj;
    return obj;
}

static VALUE
application_run(VALUE rcv, SEL sel)
{
    APPLICATION(rcv)->run();
    return rcv;
}

/// @method #start
/// This method is called when the application finished launching. This method
/// is empty by default, and you are responsible to provide a custom
/// implementation that will create the interface of your game.
/// @return [Application] the receiver.

static VALUE
application_start(VALUE rcv, SEL sel)
{
    // Do nothing.
    return rcv;
}

static VALUE
application_screen_size_changed(VALUE rcv, SEL sel)
{
    cocos2d::GLView *glview = cocos2d::Director::getInstance()->getOpenGLView();
    if (glview != NULL) {
	cocos2d::Size size = glview->getFrameSize();
	cocos2d::Application::getInstance()->applicationScreenSizeChanged(
		size.width, size.height);
    }
    return rcv;
}

extern "C"
void
Init_Application(void)
{
    rb_cApplication = rb_define_class_under(rb_mMC, "Application", rb_cObject);

    rb_define_singleton_method(rb_cApplication, "shared",
	    application_instance, 0);
    rb_define_singleton_method(rb_cApplication, "alloc", application_alloc, 0);
    rb_define_method(rb_cApplication, "run", application_run, 0);
    rb_define_method(rb_cApplication, "start", application_start, 0);

    // Internal.
    rb_define_method(rb_cApplication, "_screen_size_changed",
	    application_screen_size_changed, 0);
}

#if CC_TARGET_OS_ANDROID
// extern C++
void
cocos_android_app_init(JNIEnv* env, jobject thiz)
{
    JniFrame frame;

    // Create the app instance here.
    VALUE app_klass = rb_const_get(rb_cObject, "Application");
    assert(app_klass != Qnil);

    mc_Application *app = new mc_Application();
    VALUE obj = rb_class_wrap_new((void *)app, app_klass);
    app->obj = rb_retain(obj);
    mc_application_instance = rb_retain(app->obj);
}
#endif
