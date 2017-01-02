#include "rubymotion.h"
#include "motion-game.h"

static VALUE rb_mEvents = Qnil;
VALUE rb_cAcceleration = Qnil;
VALUE rb_cTouch = Qnil;

/// @class Events::Acceleration < Object
/// This class represents an event received from the accelerometer sensor of
/// the device, usually from the {Scene#on_accelerate} method.

#define ACC(obj) _COCOS_WRAP_GET(obj, cocos2d::Acceleration)

/// @property-readonly #x
/// @return [Float] the x coordinate of the acceleration event.

static VALUE
acc_x(VALUE rcv, SEL sel)
{
    return DBL2NUM(ACC(rcv)->x);
}

/// @property-readonly #y
/// @return [Float] the y coordinate of the acceleration event.

static VALUE
acc_y(VALUE rcv, SEL sel)
{
    return DBL2NUM(ACC(rcv)->y);
}

/// @property-readonly #z
/// @return [Float] the z coordinate of the acceleration event.

static VALUE
acc_z(VALUE rcv, SEL sel)
{
    return DBL2NUM(ACC(rcv)->z);
}

/// @property-readonly #timestamp
/// @return [Float] the timestamp of the acceleration event.

static VALUE
acc_timestamp(VALUE rcv, SEL sel)
{
    return DBL2NUM(ACC(rcv)->timestamp);
}

/// @class Events::Touch < Object
/// This class represents a touch event receive from the device, usually from
/// the {Scene#on_touch_begin} method.

#define TOUCH(obj) _COCOS_WRAP_GET(obj, cocos2d::Touch)

/// @property-readonly #location
/// @return [Point] the current location of the touch event.

static VALUE
touch_location(VALUE rcv, SEL sel)
{
    return rb_ccvec2_to_obj(TOUCH(rcv)->getLocation());
}

/// @property-readonly #start_location
/// @return [Point] the start_location of the touch event.

static VALUE
touch_start_location(VALUE rcv, SEL sel)
{
    return rb_ccvec2_to_obj(TOUCH(rcv)->getStartLocation());
}

extern "C"
void
Init_Events(void)
{
    rb_mEvents = rb_define_module_under(rb_mMC, "Events");

    rb_cAcceleration = rb_define_class_under(rb_mEvents, "Acceleration", rb_cObject);

    rb_define_method(rb_cAcceleration, "x", acc_x, 0);
    rb_define_method(rb_cAcceleration, "y", acc_y, 0);
    rb_define_method(rb_cAcceleration, "z", acc_z, 0);
    rb_define_method(rb_cAcceleration, "timestamp", acc_timestamp, 0);

    rb_cTouch = rb_define_class_under(rb_mEvents, "Touch", rb_cObject);

    rb_define_method(rb_cTouch, "location", touch_location, 0);
    rb_define_method(rb_cTouch, "start_location", touch_start_location, 0);
}
