#ifndef __RUBYMOTION_H_
#define __RUBYMOTION_H_

#include "cocos2d.h"

#if defined(__cplusplus)
extern "C" {
#endif

#if CC_TARGET_OS_IPHONE

#include <objc/runtime.h>
#include "macruby_internal.h"
#include "ruby/intern.h"
#include "ruby/node.h"
#include "ruby/io.h"
#include "id.h"
#include "vm.h"

#define rb_define_method(klass, name, imp, arity) \
    rb_objc_define_method(klass, name, (void *)imp, arity)
#define rb_define_singleton_method(klass, name, imp, arity) \
    rb_define_method(RB_GET_CLASS(klass), name, imp, arity)

#define rb_selector(str) sel_registerName(str)

#define rb_send(rcv, sel, argc, argv) rb_vm_call(rcv, sel, argc, argv)

struct rb_class_ptr {
    VALUE klass;
    VALUE flags;
    void *ptr;
};

static inline
VALUE
rb_class_wrap_new(void *ptr, VALUE klass)
{
    struct rb_class_ptr *obj =
	(struct rb_class_ptr *)class_createInstance((Class)klass,
		sizeof(struct rb_class_ptr));
    assert(obj != NULL);
    obj->klass = klass;
    obj->flags = T_NATIVE;
    obj->ptr = ptr;
    return (VALUE)obj;
}

#define rb_class_wrap_get_ptr(obj) \
    ((struct rb_class_ptr *)obj)->ptr
    
#define rb_obj_retain(obj) (VALUE)rb_objc_retain((void *)obj)
#define rb_obj_release(obj) (VALUE)rb_objc_release((void *)obj)

#endif

extern VALUE rb_mMC;
extern VALUE rb_cDirector;
extern VALUE rb_cApplication;
extern VALUE rb_cNode;
extern VALUE rb_cLayer;
extern VALUE rb_cMenu;
extern VALUE rb_cLabel;
extern VALUE rb_cSprite;
extern VALUE rb_cAudio;
extern VALUE rb_cPoint;
extern VALUE rb_cSize;

#define _COCOS_WRAP_GET(obj, type) ((type *)rb_class_wrap_get_ptr(obj))

#define APPLICATION(obj) _COCOS_WRAP_GET(obj, cocos2d::Application)
#define DIRECTOR(obj) _COCOS_WRAP_GET(obj, cocos2d::Director)
#define NODE(obj) _COCOS_WRAP_GET(obj, cocos2d::Node)
#define LAYER(obj) _COCOS_WRAP_GET(obj, cocos2d::Layer)
#define SCENE(obj) _COCOS_WRAP_GET(obj, cocos2d::Scene)
#define MENU(obj) _COCOS_WRAP_GET(obj, cocos2d::Menu)
#define VEC2(obj) _COCOS_WRAP_GET(obj, cocos2d::Vec2)
#define SIZE(obj) _COCOS_WRAP_GET(obj, cocos2d::Size)

static inline cocos2d::Vec2
rb_any_to_ccvec2(VALUE obj)
{
    if (rb_obj_is_kind_of(obj, rb_cArray)) {
	if (RARRAY_LEN(obj) != 2) {
	    rb_raise(rb_eArgError, "expected Array of 2 elements");
	}
	return cocos2d::Vec2(NUM2DBL(RARRAY_AT(obj, 0)),
		NUM2DBL(RARRAY_AT(obj, 1)));
    }
    else if (rb_obj_is_kind_of(obj, rb_cPoint)) {
	return *VEC2(obj);
    }
    rb_raise(rb_eArgError, "expected Array or Point");
}

VALUE rb_ccvec2_to_obj(cocos2d::Vec2 obj);

static inline cocos2d::Size
rb_any_to_ccsize(VALUE obj)
{
    if (rb_obj_is_kind_of(obj, rb_cArray)) {
	if (RARRAY_LEN(obj) != 2) {
	    rb_raise(rb_eArgError, "expected Array of 2 elements");
	}
	return cocos2d::Size(NUM2DBL(RARRAY_AT(obj, 0)),
		NUM2DBL(RARRAY_AT(obj, 1)));
    }
    else if (rb_obj_is_kind_of(obj, rb_cSize)) {
	return *SIZE(obj);
    }
    rb_raise(rb_eArgError, "expected Array or Size");
}

VALUE rb_ccsize_to_obj(cocos2d::Size obj);

#if defined(__cplusplus)
}
#endif

#endif // __RUBYMOTION_H_
