#ifndef __MOTION_GAME_H_
#define __MOTION_GAME_H_

#if defined(__cplusplus)
extern "C" {
#endif

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
#define SPRITE(obj) _COCOS_WRAP_GET(obj, cocos2d::Sprite)
#define MENU(obj) _COCOS_WRAP_GET(obj, cocos2d::Menu)
#define AUDIO(obj) _COCOS_WRAP_GET(obj, CocosDenshion::SimpleAudioEngine)
#define VEC2(obj) _COCOS_WRAP_GET(obj, cocos2d::Vec2)
#define SIZE(obj) _COCOS_WRAP_GET(obj, cocos2d::Size)
#define COLOR(obj) _COCOS_WRAP_GET(obj, cocos2d::Color4B)
#define ACTION(obj) _COCOS_WRAP_GET(obj, cocos2d::Action)
#define ACTION_INTERVAL(obj) _COCOS_WRAP_GET(obj, cocos2d::ActionInterval)
#define FINITE_TIME_ACTION(obj) _COCOS_WRAP_GET(obj, cocos2d::FiniteTimeAction)

VALUE rb_cocos2d_object_new(cocos2d::Ref *ptr, VALUE klass);

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

cocos2d::Scene *rb_any_to_scene(VALUE obj);

#if defined(__cplusplus)
}
#endif

#endif // __MOTION_GAME_H_
