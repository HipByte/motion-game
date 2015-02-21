#include "rubymotion.h"

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

static VALUE
point_x(VALUE rcv, SEL sel)
{
    return DBL2NUM(VEC2(rcv)->x);
}

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

static VALUE
point_plus(VALUE rcv, SEL sel, VALUE obj)
{
    return rb_ccvec2_to_obj(*VEC2(rcv) + rb_any_to_ccvec2(obj));
}

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

static VALUE
size_width(VALUE rcv, SEL sel)
{
    return DBL2NUM(SIZE(rcv)->width);
}

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
}
