#include "rubymotion.h"
#include "motion-game.h"

/// @class GLView < Object

VALUE rb_cGLView = Qnil;

/// @method #design_resolution_size(width, height, policy)
/// Set the design resolution size.
/// @param width [Float] Design resolution width.
/// @param height [Float] Design resolution height.
/// @param policy [Integer] The resolution policy desired, you may choose:
///   1. {ResolutionPolicy::EXACT_FIT} - Fill screen by stretch-to-fit.
///   2. {ResolutionPolicy::NO_BORDER} - Full screen without black border.
///   3. {ResolutionPolicy::SHOW_ALL}  - Full screen with black border.
/// @return [self] the receiver.

static VALUE
glview_design_resolution_size(VALUE rcv, SEL sel, VALUE width, VALUE height, VALUE policy)
{
    GLVIEW(rcv)->setDesignResolutionSize(NUM2DBL(width), NUM2DBL(height), (ResolutionPolicy)NUM2INT(policy));
    return rcv;
}

/// @class ResolutionPolicy < Object

static VALUE rb_cResolutionPolicy = Qnil;

/// @constant EXACT_FIT = 0
/// Fill screen by stretch-to-fit.
/// @constant NO_BORDER = 1
/// Full screen without black border.
/// @constant SHOW_ALL = 2
/// Full screen with black border.

extern "C"
void
Init_GLView(void)
{
    rb_cGLView = rb_define_class_under(rb_mMC, "GLView", rb_cObject);
    rb_register_cocos2d_object_finalizer(rb_cGLView);

    rb_define_method(rb_cGLView, "design_resolution_size", glview_design_resolution_size, 3);

    rb_cResolutionPolicy = rb_define_class_under(rb_mMC, "ResolutionPolicy", rb_cObject);
    rb_define_const(rb_cResolutionPolicy, "EXACT_FIT", LONG2NUM(ResolutionPolicy::EXACT_FIT));
    rb_define_const(rb_cResolutionPolicy, "NO_BORDER", LONG2NUM(ResolutionPolicy::NO_BORDER));
    rb_define_const(rb_cResolutionPolicy, "SHOW_ALL", LONG2NUM(ResolutionPolicy::SHOW_ALL));
}

