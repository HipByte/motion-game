#include "rubymotion.h"

/// @class Director < Object
/// Director is a shared object that takes care of navigating between scenes.

VALUE rb_cDirector = Qnil;
static VALUE mc_director_instance = Qnil;

/// @group Constructors

/// @method .shared
/// @return [Director] the shared Director instance.

static VALUE
director_instance(VALUE rcv, SEL sel)
{
    if (mc_director_instance == Qnil) {
	VALUE obj = rb_class_wrap_new(
		(void *)cocos2d::Director::getInstance(), rb_cDirector);
	mc_director_instance = rb_retain(obj);
    }
    return mc_director_instance;
}

#if CC_TARGET_OS_IPHONE
static VALUE
director_view_set(VALUE rcv, SEL sel, VALUE obj)
{
    cocos2d::GLView *glview =
	cocos2d::GLViewImpl::createWithEAGLView((void *)obj);
    DIRECTOR(rcv)->setOpenGLView(glview);
    return obj;
}
#endif

/// @group Managing Scenes

static cocos2d::Scene *
obj_to_scene(VALUE obj)
{
    cocos2d::Scene *scene = NULL;
    if (rb_obj_is_kind_of(obj, rb_cLayer)) {
	scene = cocos2d::Scene::create();
	scene->addChild(LAYER(obj));
    }
    assert(scene != NULL);
    return scene;
}

/// @method #run(scene)
/// Runs the given scene object.
/// @param scene [Scene] the scene to run.
/// @return [Director] the receiver.

static VALUE
director_run(VALUE rcv, SEL sel, VALUE obj)
{
    DIRECTOR(rcv)->runWithScene(obj_to_scene(obj));
    return rcv;
}

/// @method #replace(scene)
/// Replaces the current scene with a new one. The running scene will be
/// terminated.
/// @param scene [Scene] the scene to replace the current one with.
/// @return [Director] the receiver.

static VALUE
director_replace(VALUE rcv, SEL sel, VALUE obj)
{
    DIRECTOR(rcv)->replaceScene(obj_to_scene(obj));
    return rcv;
}

/// @method #end
/// Ends the scene execution. This also has the effect of quitting the
/// application.
/// @return [Director] the receiver.

static VALUE
director_end(VALUE rcv, SEL sel)
{
    DIRECTOR(rcv)->end();
    exit(0);
    return rcv;
}

/// @group Properties

/// @property-readonly #origin
/// @return [Point] the visible origin of the director view in points.

static VALUE
director_origin(VALUE rcv, SEL sel)
{
    return rb_ccvec2_to_obj(DIRECTOR(rcv)->getVisibleOrigin());
}

/// @property-readonly #size
/// @return [Size] the visible size of the director view in points.

static VALUE
director_size(VALUE rcv, SEL sel)
{
    return rb_ccsize_to_obj(DIRECTOR(rcv)->getVisibleSize());
}

/// @property #show_stats?
/// Controls whether the FPS (frame-per-second) statistic label is displayed
/// in the bottom-left corner of the director view. By default it is hidden.
/// @return [Boolean] whether the FPS label is displayed.

static VALUE
director_show_stats_set(VALUE rcv, SEL sel, VALUE val)
{
    DIRECTOR(rcv)->setDisplayStats(RTEST(val));
    return val;
}

static VALUE
director_show_stats(VALUE rcv, SEL sel)
{
    return DIRECTOR(rcv)->isDisplayStats() ? Qtrue : Qfalse;
}

extern "C"
void
Init_Director(void)
{
    rb_cDirector = rb_define_class_under(rb_mMC, "Director", rb_cObject);

    rb_define_singleton_method(rb_cDirector, "shared", director_instance, 0);
    rb_define_method(rb_cDirector, "run", director_run, 1);
    rb_define_method(rb_cDirector, "replace", director_replace, 1);
    rb_define_method(rb_cDirector, "end", director_end, 0);
    rb_define_method(rb_cDirector, "origin", director_origin, 0);
    rb_define_method(rb_cDirector, "size", director_size, 0);
    rb_define_method(rb_cDirector, "show_stats=", director_show_stats_set, 1);
    rb_define_method(rb_cDirector, "show_stats?", director_show_stats, 0);

    // Internal.
#if CC_TARGET_OS_IPHONE
    rb_define_method(rb_cDirector, "_set_glview", director_view_set, 1);
#endif
}
