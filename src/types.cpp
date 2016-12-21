#include "rubymotion.h"
#include "motion-game.h"

/// @class Point < Object
/// A point represents a location in a two-dimensional coordinate system using
/// +x+ and +y+ variables.
///
/// When calling a method that expects a +Point+ object, a 2-element +Array+
/// can be passed instead, as a convenience shortcut. For example,
///   node.location = [10, 20]
/// is the same as
///   point = MG::Point.new
///   point.x = 10
///   point.y = 20
///   node.location = point

VALUE rb_cPoint = Qnil;

extern "C"
VALUE
rb_ccvec2_to_obj(cocos2d::Vec2 _vec2)
{
    cocos2d::Vec2 *vec2 = new cocos2d::Vec2(_vec2);
    // cocos2d::Vec2 does not inherite cocos2d::Ref.
    return rb_class_wrap_new(vec2, rb_cPoint);
}

static VALUE
point_alloc(VALUE rcv, SEL sel)
{
    return rb_ccvec2_to_obj(cocos2d::Vec2());
}

/// @property #x
/// @return [Float] the x coordinate of the point.

static VALUE
point_x(VALUE rcv, SEL sel)
{
    return DBL2NUM(VEC2(rcv)->x);
}

static VALUE
point_x_set(VALUE rcv, SEL sel, VALUE x)
{
    VEC2(rcv)->x = NUM2DBL(x);
    return x;
}

/// @property #y
/// @return [Float] the y coordinate of the point.

static VALUE
point_y(VALUE rcv, SEL sel)
{
    return DBL2NUM(VEC2(rcv)->y);
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
/// @param point [Point] A coordinate.
/// @return [Point] A new Point object.

static VALUE
point_plus(VALUE rcv, SEL sel, VALUE obj)
{
    return rb_ccvec2_to_obj(*VEC2(rcv) + rb_any_to_ccvec2(obj));
}

/// @method #-(point)
/// Substracts the coordinates of the receiver with the coordinates of the given
/// point object.
/// @param point [Point] A coordinate.
/// @return [Point] A new Point object.

static VALUE
point_minus(VALUE rcv, SEL sel, VALUE obj)
{
    return rb_ccvec2_to_obj(*VEC2(rcv) - rb_any_to_ccvec2(obj));
}

/// @method #distance(point)
/// Calculates the distance between two points.
/// @param point [Point] A point to calculate the distance.
/// @return [Float] the distance.

static VALUE
point_distance(VALUE rcv, SEL sel, VALUE obj)
{
    return DBL2NUM(VEC2(rcv)->getDistance(rb_any_to_ccvec2(obj)));
}

static VALUE
point_inspect(VALUE rcv, SEL sel)
{
    auto vec = VEC2(rcv);
    char buf[100];
    snprintf(buf, sizeof buf, "[%f, %f]", vec->x, vec->y);
    return RSTRING_NEW(buf);
}

/// @class Size < Object
/// A size represents the dimensions of width and height of an object.
///
/// When calling a method that expects a +Size+ object, a 2-element +Array+
/// can be passed instead, as a convenience shortcut. For example,
///   node.size = [200, 400]
/// is the same as
///   size = MG::Size.new
///   size.width = 200
///   size.height = 400
///   node.size = size

VALUE rb_cSize = Qnil;

extern "C"
VALUE
rb_ccsize_to_obj(cocos2d::Size _size)
{
    cocos2d::Size *size = new cocos2d::Size(_size);
    // cocos2d::Size does not inherite cocos2d::Ref.
    return rb_class_wrap_new(size, rb_cSize);
}

static VALUE
size_alloc(VALUE rcv, SEL sel)
{
    return rb_ccsize_to_obj(cocos2d::Size());
}

/// @property #width
/// @return [Float] the size width.

static VALUE
size_width(VALUE rcv, SEL sel)
{
    return DBL2NUM(SIZE(rcv)->width);
}

static VALUE
size_width_set(VALUE rcv, SEL sel, VALUE obj)
{
    SIZE(rcv)->width = NUM2DBL(obj);
    return obj;
}

/// @property #height
/// @return [Float] the size height.

static VALUE
size_height(VALUE rcv, SEL sel)
{
    return DBL2NUM(SIZE(rcv)->height);
}

static VALUE
size_height_set(VALUE rcv, SEL sel, VALUE obj)
{
    SIZE(rcv)->height = NUM2DBL(obj);
    return obj;
}

/// @group Helpers

/// @method #+(size)
/// Adds the dimensions of the receiver with the dimensions of the given
/// size object.
/// @param size [Size] A dimension.
/// @return [Size] a new Size object.

static VALUE
size_plus(VALUE rcv, SEL sel, VALUE obj)
{
    return rb_ccsize_to_obj(*SIZE(rcv) + rb_any_to_ccsize(obj));
}

/// @method #-(size)
/// Substracts the dimensions of the receiver with the dimensions of the given
/// size object.
/// @param size [Size] A dimension.
/// @return [Size] a new Size object.

static VALUE
size_minus(VALUE rcv, SEL sel, VALUE obj)
{
    return rb_ccsize_to_obj(*SIZE(rcv) - rb_any_to_ccsize(obj));
}

/// @method #/(delta)
/// Divides the dimensions of the receiver with the given number.
/// @param delta [Float] the number to divide the receiver with.
/// @return [Size] a new Size object.

static VALUE
size_div(VALUE rcv, SEL sel, VALUE obj)
{
    return rb_ccsize_to_obj(*SIZE(rcv) / NUM2DBL(obj));
}

/// @method #*(delta)
/// Multiplies the dimensions of the receiver with the given number.
/// @param delta [Float] the number to multiply the receiver with.
/// @return [Size] a new Size object.

static VALUE
size_mul(VALUE rcv, SEL sel, VALUE obj)
{
    return rb_ccsize_to_obj(*SIZE(rcv) * NUM2DBL(obj));
}

static VALUE
size_inspect(VALUE rcv, SEL sel)
{
    auto vec = SIZE(rcv);
    char buf[100];
    snprintf(buf, sizeof buf, "[%f, %f]", vec->width, vec->height);
    return RSTRING_NEW(buf);
}

/// @class Color < Object
/// A color represents the color, and sometimes opacity (alpha) of an object.
///
/// When calling a method that expects a +Color+ object, a 3-element +Array+
/// (red-green-blue) or 4-element +Array+ (red-green-blue-alpha) can be passed
/// instead, as a convenience shortcut. For example,
///   node.color = [0.2, 0.3, 0.4]
/// is the same as
///   color = MG::Color.new
///   color.red = 0.2
///   color.green = 0.3
///   color.blue = 0.4
///   node.color = color
/// or
///   node.color = MG::Color.new(0.2, 0.3, 0.4)
/// Alternatively, a +Symbol+ corresponding to a basic color name can be
/// provided. For example,
///   node.color = :red
/// is the same as
///   color = MG::Color.new
///   color.red = 1.0
///   color.green = color.blue = 0
///   node.color = color
/// Currently, the following symbols are supported: +:white+, +:black+, +:red+,
/// +:green+ and +:blue+.
/// The +MG::Color.new+ constructor will return the black color.

VALUE rb_cColor = Qnil;

extern "C"
VALUE
rb_cccolor4_to_obj(cocos2d::Color4B _color)
{
    cocos2d::Color4B *color = new cocos2d::Color4B(_color);
    // cocos2d::Color4B does not inherite cocos2d::Ref.
    return rb_class_wrap_new(color, rb_cColor);
}

/// @group Constructors

/// @method #initialize(*args)
/// @overload initialize
///   Creates a new object with default color set.
///   @return [Color] a Color object.

/// @overload initialize(ary)
///   Creates a new object with 3-element or 4-element +Array+.
///   @param ary [Array] 3-element or 4-element +Array+.
///   @return [Color] a Color object.

/// @overload initialize(red, green, blue, alpha=1.0)
///   Creates a new object.
///   @param red [Float] the red portion of the color, from +0.0+ to +1.0+.
///   @param green [Float] the green portion of the color, from +0.0+ to +1.0+.
///   @param blue [Float] the blue portion of the color, from +0.0+ to +1.0+.
///   @param alpha [Float] the alpha portion of the color, from +0.0+ to +1.0+.
///   @return [Color] a Color object.

/// @endgroup

static VALUE
color_new(VALUE rcv, SEL sel, int argc, VALUE *argv)
{
    switch (argc) {
      case 0:
	return rb_cccolor4_to_obj(cocos2d::Color4B::BLACK);
      case 1:
	// Array
	if (!rb_obj_is_kind_of(argv[0], rb_cArray)) {
	    rb_raise(rb_eArgError, "expected Array");
	}
	return rb_cccolor4_to_obj(rb_any_to_cccolor4(argv[0]));
    }

    VALUE red, green, blue, alpha;
    rb_scan_args(argc, argv, "31", &red, &green, &blue, &alpha);

    GLubyte alpha_c = 255;
    if (argc == 4) {
	alpha_c = NUM2BYTE(alpha);
    }
    return rb_cccolor4_to_obj(cocos2d::Color4B(
		NUM2BYTE(red),
		NUM2BYTE(green),
		NUM2BYTE(blue),
		alpha_c));
}

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

static VALUE
color_inspect(VALUE rcv, SEL sel)
{
    auto color = COLOR(rcv);
    char buf[100];
    snprintf(buf, sizeof buf, "#<Color Red:%f Green:%f Blue:%f Alpha:%f>",
	    color->r/255.0, color->g/255.0, color->b/255.0, color->a/255.0);
    return RSTRING_NEW(buf);
}

extern "C"
void
Init_Types(void)
{
    rb_cPoint = rb_define_class_under(rb_mMC, "Point", rb_cObject);

    rb_define_singleton_method(rb_cPoint, "alloc", point_alloc, 0);
    rb_define_method(rb_cPoint, "x", point_x, 0);
    rb_define_method(rb_cPoint, "x=", point_x_set, 1);
    rb_define_method(rb_cPoint, "y", point_y, 0);
    rb_define_method(rb_cPoint, "y=", point_y_set, 1);
    rb_define_method(rb_cPoint, "+", point_plus, 1);
    rb_define_method(rb_cPoint, "-", point_minus, 1);
    rb_define_method(rb_cPoint, "distance", point_distance, 1);
    rb_define_method(rb_cPoint, "inspect", point_inspect, 0);

    rb_cSize = rb_define_class_under(rb_mMC, "Size", rb_cObject);

    rb_define_singleton_method(rb_cSize, "alloc", size_alloc, 0);
    rb_define_method(rb_cSize, "width", size_width, 0);
    rb_define_method(rb_cSize, "width=", size_width_set, 1);
    rb_define_method(rb_cSize, "height", size_height, 0);
    rb_define_method(rb_cSize, "height=", size_height_set, 1);
    rb_define_method(rb_cSize, "+", size_plus, 1);
    rb_define_method(rb_cSize, "-", size_minus, 1);
    rb_define_method(rb_cSize, "/", size_div, 1);
    rb_define_method(rb_cSize, "*", size_mul, 1);
    rb_define_method(rb_cSize, "inspect", size_inspect, 0);

    rb_cColor = rb_define_class_under(rb_mMC, "Color", rb_cObject);

    rb_define_constructor(rb_cColor, color_new, -1);
    rb_define_method(rb_cColor, "red", color_red, 0);
    rb_define_method(rb_cColor, "red=", color_red_set, 1);
    rb_define_method(rb_cColor, "green", color_green, 0);
    rb_define_method(rb_cColor, "green=", color_green_set, 1);
    rb_define_method(rb_cColor, "blue", color_blue, 0);
    rb_define_method(rb_cColor, "blue=", color_blue_set, 1);
    rb_define_method(rb_cColor, "alpha", color_alpha, 0);
    rb_define_method(rb_cColor, "alpha=", color_alpha_set, 1);
    rb_define_method(rb_cColor, "inspect", color_inspect, 0);
}
