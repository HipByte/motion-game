#ifndef __RUBYMOTION_H_
#define __RUBYMOTION_H_

#include "cocos2d.h"

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
    
#define rb_retain(obj) \
    ({ \
	VALUE _obj = (VALUE)obj; \
	rb_objc_retain((void *)_obj); \
	_obj; \
    })
#define rb_release(obj) \
    ({ \
	VALUE _obj = (VALUE)obj; \
	rb_objc_release((void *)_obj); \
	_obj; \
    })

#define rb_current_block() \
    ({ \
	VALUE _b = (VALUE)rb_vm_current_block(); \
	_b == 0 ? Qnil : _b; \
    })

#define rb_block_call(block, argc, argv) \
    rb_vm_block_eval((rb_vm_block_t *)block, argc, argv)

#define RSTRING_NEW(cstr) rb_str_new2(cstr)

#elif CC_TARGET_OS_ANDROID

#include "runtime.h"

#define NUM2DBL NUM2DOUBLE
#define DBL2NUM DOUBLE2NUM

#define rb_obj_is_kind_of(obj, klass) rb_vm_kind_of(obj, (VALUE)klass)

#define rb_selector(str) sel_registerName(str)

#define rb_send rb_dispatch

#define rb_define_method(klass, name, imp, arity) \
    rb_define_method((jclass)klass, name, arity, (IMP)imp)

#define rb_define_singleton_method(klass, name, imp, arity) \
    rb_define_static_method((jclass)klass, name, arity, (IMP)imp)

#define rb_define_module_under(mod, name) \
    (VALUE)rb_define_module_under((jclass)mod, name)

#define rb_define_class_under(module, name, super) \
    rb_retain(rb_vm_define_ruby_class(name, (VALUE)super, (VALUE)module))

#define rb_class_wrap_new(ptr, klass) rb_object_new((jclass)klass, ptr)
#define rb_class_wrap_get_ptr(obj) rb_object_ptr(obj)

#define rb_retain(obj) (VALUE)VM_GLOBAL(obj)
#define rb_release(obj) \
    ({ \
	VALUE _obj = obj; \
	VALUE _local = VM_LOCAL(_obj); \
	VM_GLOBAL_DELETE(_obj); \
	_local; \
    })

#define RSTRING_NEW(cstr) rb_str_new2(cstr)
#define RSTRING_PTR(str) rb_str_to_stdstring(str).c_str()

#define rb_current_block() rb_vm_current_block_object()
#define rb_block_call(block, argc, argv) \
    rb_vm_block_dispatch(RB_BLOCK_PTR(block), argc, argv)

#endif

#if defined(__cplusplus)
extern "C" {
#endif

#define NUM2BYTE(val) \
    ({ \
	int _byte = (NUM2DBL(val) * 255); \
	std::min(std::max(_byte, 0), 255); \
    })
#define BYTE2NUM(val) (DBL2NUM(val / 255.0))

extern VALUE rb_mMC;
extern VALUE rb_cDirector;
extern VALUE rb_cApplication;
extern VALUE rb_cNode;
extern VALUE rb_cTouch;
extern VALUE rb_cAcceleration;
extern VALUE rb_cScene;
extern VALUE rb_cMenu;
extern VALUE rb_cLabel;
extern VALUE rb_cSprite;
extern VALUE rb_cParticle;
extern VALUE rb_cAudio;
extern VALUE rb_cPoint;
extern VALUE rb_cSize;
extern VALUE rb_cColor;

#define _COCOS_WRAP_GET(obj, type) ((type *)rb_class_wrap_get_ptr(obj))

#define APPLICATION(obj) _COCOS_WRAP_GET(obj, cocos2d::Application)
#define DIRECTOR(obj) _COCOS_WRAP_GET(obj, cocos2d::Director)
#define NODE(obj) _COCOS_WRAP_GET(obj, cocos2d::Node)
#define SCENE(obj) _COCOS_WRAP_GET(obj, cocos2d::Scene)
#define SPRITE(obj) _COCOS_WRAP_GET(obj, cocos2d::Sprite)
#define SCENE(obj) _COCOS_WRAP_GET(obj, cocos2d::Scene)
#define MENU(obj) _COCOS_WRAP_GET(obj, cocos2d::Menu)
#define AUDIO(obj) _COCOS_WRAP_GET(obj, CocosDenshion::SimpleAudioEngine)
#define VEC2(obj) _COCOS_WRAP_GET(obj, cocos2d::Vec2)
#define SIZE(obj) _COCOS_WRAP_GET(obj, cocos2d::Size)
#define COLOR(obj) _COCOS_WRAP_GET(obj, cocos2d::Color4B)

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

static inline cocos2d::Color3B
rb_sym_to_cccolor3(VALUE sym)
{
    const char *str = rb_sym2name(sym);
    int red = 0, green = 0, blue = 0;
    if (strcmp(str, "white") == 0) {
	red = green = blue = 255;
    }
    else if (strcmp(str, "black") == 0) {
	red = green = blue = 0;
    }
    else if (strcmp(str, "red") == 0) {
	red = 255;
	green = blue = 0;
    }
    else if (strcmp(str, "green") == 0) {
	green = 255;
	blue = red = 0;
    }
    else if (strcmp(str, "blue") == 0) {
	blue = 255;
	green = red = 0;
    }
    else {
	rb_raise(rb_eArgError, "invalid symbol `%s' for color", str);
    }
    return cocos2d::Color3B(red, green, blue);
}

static inline cocos2d::Color3B
rb_any_to_cccolor3(VALUE obj)
{
    if (rb_obj_is_kind_of(obj, rb_cArray)) {
	if (RARRAY_LEN(obj) != 3) {
	    rb_raise(rb_eArgError, "expected Array of 3 elements");
	}
	return cocos2d::Color3B(
		NUM2BYTE(RARRAY_AT(obj, 0)),
		NUM2BYTE(RARRAY_AT(obj, 1)),
		NUM2BYTE(RARRAY_AT(obj, 2)));
    }
    else if (rb_obj_is_kind_of(obj, rb_cSymbol)) {
	return rb_sym_to_cccolor3(obj);
    }
    else if (rb_obj_is_kind_of(obj, rb_cColor)) {
	auto color = COLOR(obj);
	return cocos2d::Color3B(color->r, color->g, color->b);
    }
    rb_raise(rb_eArgError, "expected Array, Symbol or Color");
}

static inline cocos2d::Color4B
rb_any_to_cccolor4(VALUE obj)
{
    if (rb_obj_is_kind_of(obj, rb_cArray)) {
	if (RARRAY_LEN(obj) != 4) {
	    rb_raise(rb_eArgError, "expected Array of 4 elements");
	}
	return cocos2d::Color4B(
		NUM2BYTE(RARRAY_AT(obj, 0)),
		NUM2BYTE(RARRAY_AT(obj, 1)),
		NUM2BYTE(RARRAY_AT(obj, 2)),
		NUM2BYTE(RARRAY_AT(obj, 3)));
    }
    else if (rb_obj_is_kind_of(obj, rb_cSymbol)) {
	auto color = rb_sym_to_cccolor3(obj);
	return cocos2d::Color4B(color.r, color.g, color.b, 255);
    }
    else if (rb_obj_is_kind_of(obj, rb_cColor)) {
	return *COLOR(obj);
    }
    rb_raise(rb_eArgError, "expected Array, Symbol or Color");
}

VALUE rb_cccolor4_to_obj(cocos2d::Color4B obj);

static inline VALUE
rb_cccolor3_to_obj(cocos2d::Color3B obj)
{
    return rb_cccolor4_to_obj(cocos2d::Color4B(obj.r, obj.g, obj.b, 255)); 
}

#if defined(__cplusplus)
}
#endif

#endif // __RUBYMOTION_H_
