#include "rubymotion.h"

/// @class Point < Object

VALUE rb_cPoint = Qnil;

extern "C"
VALUE
rb_ccvec2_to_obj(cocos2d::Vec2 _vec2)
{
    cocos2d::Vec2 *vec2 = new cocos2d::Vec2(_vec2);
    return rb_class_wrap_new((void *)vec2, rb_cPoint);
}

static VALUE
point_alloc(VALUE rcv, SEL sel)
{
    return rb_ccvec2_to_obj(cocos2d::Vec2());
}

/// @group Properties

/// @property #x
/// @return [Float] the x coordinate of the point.

static VALUE
point_x(VALUE rcv, SEL sel)
{
    return DBL2NUM(VEC2(rcv)->x);
}

/// @property #y
/// @return [Float] the y coordinate of the point.

static VALUE
point_y(VALUE rcv, SEL sel)
{
    return DBL2NUM(VEC2(rcv)->y);
}

static VALUE
point_x_set(VALUE rcv, SEL sel, VALUE x)
{
    VEC2(rcv)->x = NUM2DBL(x);
    return x;
}

static VALUE
point_y_set(VALUE rcv, SEL sel, VALUE y)
{
    VEC2(rcv)->y = NUM2DBL(y);
    return y;
}

/// @group Helpers

/// @method #+(point)
/// Adds the coordinates of the receiver with the coordinates of the given
/// point object.
/// @param point [Point]
/// @return [Point] A new Point object.

static VALUE
point_plus(VALUE rcv, SEL sel, VALUE obj)
{
    return rb_ccvec2_to_obj(*VEC2(rcv) + rb_any_to_ccvec2(obj));
}

/// @class Size < Object

VALUE rb_cSize = Qnil;

extern "C"
VALUE
rb_ccsize_to_obj(cocos2d::Size _size)
{
    cocos2d::Size *size = new cocos2d::Size(_size);
    return rb_class_wrap_new((void *)size, rb_cSize);
}

static VALUE
size_alloc(VALUE rcv, SEL sel)
{
    return rb_ccsize_to_obj(cocos2d::Size());
}

/// @group Properties

/// @property #width
/// @return [Float] the size width.

static VALUE
size_width(VALUE rcv, SEL sel)
{
    return DBL2NUM(SIZE(rcv)->width);
}

/// @property #height
/// @return [Float] the size height.

static VALUE
size_height(VALUE rcv, SEL sel)
{
    return DBL2NUM(SIZE(rcv)->height);
}

static VALUE
size_width_set(VALUE rcv, SEL sel, VALUE obj)
{
    SIZE(rcv)->width = NUM2DBL(obj);
    return obj;
}

static VALUE
size_height_set(VALUE rcv, SEL sel, VALUE obj)
{
    SIZE(rcv)->height = NUM2DBL(obj);
    return obj;
}

/// @class Color < Object

VALUE rb_cColor = Qnil;

extern "C"
VALUE
rb_cccolor4_to_obj(cocos2d::Color4B _color)
{
    cocos2d::Color4B *color = new cocos2d::Color4B(_color);
    return rb_class_wrap_new((void *)color, rb_cColor);
}

static VALUE
color_alloc(VALUE rcv, SEL sel)
{
    return rb_cccolor4_to_obj(cocos2d::Color4B());
}

/// @group Properties

/// @property #red
/// @return [Float] the red portion of the color, from +0.0+ to +1.0+.

static VALUE
color_red(VALUE rcv, SEL sel)
{
    return BYTE2NUM(COLOR(rcv)->r);
}

static VALUE
color_red_set(VALUE rcv, SEL sel, VALUE val)
{
    COLOR(rcv)->r = NUM2BYTE(val);
    return val;
}

/// @property #green
/// @return [Float] the green portion of the color, from +0.0+ to +1.0+.

static VALUE
color_green(VALUE rcv, SEL sel)
{
    return BYTE2NUM(COLOR(rcv)->g);
}

static VALUE
color_green_set(VALUE rcv, SEL sel, VALUE val)
{
    COLOR(rcv)->g = NUM2BYTE(val);
    return val;
}

/// @property #blue
/// @return [Float] the blue portion of the color, from +0.0+ to +1.0+.

static VALUE
color_blue(VALUE rcv, SEL sel)
{
    return BYTE2NUM(COLOR(rcv)->b);
}

static VALUE
color_blue_set(VALUE rcv, SEL sel, VALUE val)
{
    COLOR(rcv)->b = NUM2BYTE(val);
    return val;
}

/// @property #alpha
/// @return [Float] the alpha portion of the color, from +0.0+ to +1.0+.

static VALUE
color_alpha(VALUE rcv, SEL sel)
{
    return BYTE2NUM(COLOR(rcv)->a);
}

static VALUE
color_alpha_set(VALUE rcv, SEL sel, VALUE val)
{
    COLOR(rcv)->a = NUM2BYTE(val);
    return val;
}

extern "C"
void
Init_Types(void)
{
    rb_cPoint = rb_define_class_under(rb_mMC, "Point", rb_cObject);

    rb_define_singleton_method(rb_cPoint, "alloc", point_alloc, 0);
    rb_define_method(rb_cPoint, "x", point_x, 0);
    rb_define_method(rb_cPoint, "y", point_y, 0);
    rb_define_method(rb_cPoint, "x=", point_x_set, 1);
    rb_define_method(rb_cPoint, "y=", point_y_set, 1);
    rb_define_method(rb_cPoint, "+", point_plus, 1);

    rb_cSize = rb_define_class_under(rb_mMC, "Size", rb_cObject);

    rb_define_singleton_method(rb_cSize, "alloc", size_alloc, 0);
    rb_define_method(rb_cSize, "width", size_width, 0);
    rb_define_method(rb_cSize, "height", size_height, 0);
    rb_define_method(rb_cSize, "width=", size_width_set, 1);
    rb_define_method(rb_cSize, "height=", size_height_set, 1);

    rb_cColor = rb_define_class_under(rb_mMC, "Color", rb_cObject);

    rb_define_singleton_method(rb_cColor, "alloc", color_alloc, 0);
    rb_define_method(rb_cColor, "red", color_red, 0);
    rb_define_method(rb_cColor, "green", color_green, 0);
    rb_define_method(rb_cColor, "blue", color_blue, 0);
    rb_define_method(rb_cColor, "alpha", color_alpha, 0);
    rb_define_method(rb_cColor, "red=", color_red_set, 1);
    rb_define_method(rb_cColor, "green=", color_green_set, 1);
    rb_define_method(rb_cColor, "blue=", color_blue_set, 1);
    rb_define_method(rb_cColor, "alpha=", color_alpha_set, 1);
}
