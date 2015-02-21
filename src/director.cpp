#include "rubymotion.h"

VALUE rb_cDirector = Qnil;
static VALUE mc_director_instance = Qnil;

static VALUE
director_instance(VALUE rcv, SEL sel)
{
    if (mc_director_instance == Qnil) {
	mc_director_instance = rb_class_wrap_new(
		(void *)cocos2d::Director::getInstance(), rb_cDirector);
	rb_obj_retain(mc_director_instance);
    }
    return mc_director_instance;
}

static VALUE
director_view_set(VALUE rcv, SEL sel, VALUE obj)
{
    cocos2d::GLView *glview =
	cocos2d::GLViewImpl::createWithEAGLView((void *)obj);
    DIRECTOR(rcv)->setOpenGLView(glview);
    return obj;
}

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

static VALUE
director_run(VALUE rcv, SEL sel, VALUE obj)
{
    DIRECTOR(rcv)->runWithScene(obj_to_scene(obj));
    return obj;
}

static VALUE
director_replace(VALUE rcv, SEL sel, VALUE obj)
{
    DIRECTOR(rcv)->replaceScene(obj_to_scene(obj));
    return obj;
}

static VALUE
director_end(VALUE rcv, SEL sel)
{
    DIRECTOR(rcv)->end();
#if CC_TARGET_OS_IPHONE
    exit(0);
#endif
    return rcv;
}

static VALUE
director_origin(VALUE rcv, SEL sel)
{
    return rb_ccvec2_to_obj(DIRECTOR(rcv)->getVisibleOrigin());
}

static VALUE
director_size(VALUE rcv, SEL sel)
{
    return rb_ccsize_to_obj(DIRECTOR(rcv)->getVisibleSize());
}

static VALUE
director_show_stats(VALUE rcv, SEL sel, VALUE val)
{
    DIRECTOR(rcv)->setDisplayStats(RTEST(val));
    return val;
}

extern "C"
void
Init_Director(void)
{
    rb_cDirector = rb_define_class_under(rb_mMC, "Director", rb_cObject);
    rb_define_singleton_method(rb_cDirector, "instance", director_instance, 0);
    rb_define_method(rb_cDirector, "run", director_run, 1);
    rb_define_method(rb_cDirector, "replace", director_replace, 1);
    rb_define_method(rb_cDirector, "end", director_end, 0);
    rb_define_method(rb_cDirector, "origin", director_origin, 0);
    rb_define_method(rb_cDirector, "size", director_size, 0);
    rb_define_method(rb_cDirector, "show_stats=", director_show_stats, 1);

    // Internal.
    rb_define_method(rb_cDirector, "_set_glview", director_view_set, 1);
}
