#include "rubymotion.h"

VALUE rb_cAction = Qnil;
VALUE rb_cMoveBy = Qnil;
VALUE rb_cMoveTo = Qnil;
VALUE rb_cRotateBy = Qnil;
VALUE rb_cRotateTo = Qnil;
VALUE rb_cScaleBy = Qnil;
VALUE rb_cScaleTo = Qnil;
VALUE rb_cSkewBy = Qnil;
VALUE rb_cSkewTo = Qnil;
VALUE rb_cJumpBy = Qnil;
VALUE rb_cJumpTo = Qnil;
VALUE rb_cTintBy = Qnil;
VALUE rb_cTintTo = Qnil;
VALUE rb_cFadeTo = Qnil;
VALUE rb_cFadeIn = Qnil;
VALUE rb_cFadeOut = Qnil;
VALUE rb_cBlink = Qnil;
VALUE rb_cSequence = Qnil;
VALUE rb_cSpawn = Qnil;

/// @class Action < Object
/// @method #reverse
/// Reverses the action
/// @return [Action] the reversed action.
static VALUE
action_reverse(VALUE rcv, SEL sel)
{
    ACTION(rcv)->reverse();
    return rcv;
}

/// @group Properties
/// @property #done?
/// Whether the action is done.
/// @return [Boolean] whether the action is done.
static VALUE
action_done(VALUE rcv, SEL sel)
{
    return ACTION(rcv)->isDone() ? Qtrue : Qfalse;
}

/// @class MoveBy < Action
/// @group Constructors
/// @method #initialize(position, interval)
/// Creates an action that will move the position of the receiver to a new location 
/// determined by the sum of the current location and the given +delta_location+ object.
/// @param delta_location [Point] a point that will be added to the receiver's
///   current location.
/// @param interval [Float] the animation interval.
/// @return [MoveBy] the action.
static VALUE
move_by_new(VALUE rcv, SEL sel, VALUE position, VALUE interval)
{
    auto action = cocos2d::MoveBy::create(NUM2DBL(interval), rb_any_to_ccvec2(position));        
    return rb_class_wrap_new((void *)action, rb_cAction);
}

/// @class MoveTo < Action
/// @group Constructors
/// @method #initialize(position, interval)
/// Creates an action that will move the position of the receiver to a new given location.
/// @param location [Point] where the receiver should be moved to.
/// @param interval [Float] the animation interval.
/// @return [MoveTo] the action.
static VALUE
move_to_new(VALUE rcv, SEL sel, VALUE position, VALUE interval)
{
    auto action = cocos2d::MoveTo::create(NUM2DBL(interval), rb_any_to_ccvec2(position));        
    return rb_class_wrap_new((void *)action, rb_cAction);
}

/// @class JumpBy < Action
/// @group Constructors
/// @method #initialize(delta_location, height, jumps, interval)
/// Creates an action that will jump the receiver to a new location 
/// determined by the sum of the current location and the given +delta_location+ object.
/// @param delta_location [Point] a point that will be added to the receiver's
///   current location during jumping.
/// @param height [Float] the height of the jump
/// @param jumps [Float] the number of jumps
/// @param interval [Float] the animation interval.
/// @return [JumpBy] the action.
static VALUE
jump_by_new(VALUE rcv, SEL sel, VALUE delta_location, VALUE height, VALUE jumps, VALUE interval)
{
    auto action = cocos2d::JumpBy::create(NUM2DBL(interval), rb_any_to_ccvec2(delta_location), NUM2DBL(height), NUM2INT(jumps));        
    return rb_class_wrap_new((void *)action, rb_cAction);
}

/// @class JumpTo < Action
/// @group Constructors
/// @method #initialize(position, height, jumps, interval)
/// Creates an action that will jump the receiver to a new location.
/// @param location [Point] a point that the receiver will jump to
/// @param height [Float] the height of the jump
/// @param jumps [Float] the number of jumps
/// @param interval [Float] the animation interval.
/// @return [JumpTo] the action.
static VALUE
jump_to_new(VALUE rcv, SEL sel, VALUE location, VALUE height, VALUE jumps, VALUE interval)
{
    auto action = cocos2d::JumpTo::create(NUM2DBL(interval), rb_any_to_ccvec2(location), NUM2DBL(height), NUM2INT(jumps));        
    return rb_class_wrap_new((void *)action, rb_cAction);
}

/// @class RotateBy < Action
/// @group Constructors
/// @method #initialize(delta_angle, interval)
/// Creates an action that will rotates the position of the receiver 
/// to a new angle determined by the
/// sum of the current rotation and the given +delta_angle+ object.
/// @param delta_angle [Float] the angle to add to the current rotation
/// @param interval [Float] the animation interval.
/// @return [Sprite] the receiver.
static VALUE
rotate_by_new(VALUE rcv, SEL sel, VALUE delta_angle, VALUE interval)
{
    auto action = cocos2d::RotateBy::create(NUM2DBL(interval), NUM2DBL(delta_angle));        
    return rb_class_wrap_new((void *)action, rb_cAction);
}

/// @class RotateTo < Action
/// @group Constructors
/// @method #initialize(angle, interval)
/// Creates an action that will rotate the angle of the receiver to a new angle
/// by modifying it's rotation attribute.
/// @param angle [Float] the receiver should be rotated to.
/// @param interval [Float] the animation interval.
/// @return [RotateTo] the action.
static VALUE
rotate_to_new(VALUE rcv, SEL sel, VALUE angle, VALUE interval)
{
    auto action = cocos2d::RotateTo::create(NUM2DBL(interval), NUM2DBL(angle));        
    return rb_class_wrap_new((void *)action, rb_cAction);
}

/// @class ScaleBy < Action
/// @group Constructors
/// @method #initialize(scale, interval)
/// Creates an action that will scale the receiver
/// @param scale [Float] the receiver should be scaled by
/// @param interval [Float] the animation interval.
/// @return [ScaleBy] the action.
static VALUE
scale_by_new(VALUE rcv, SEL sel, VALUE scale, VALUE interval)
{
    auto action = cocos2d::ScaleBy::create(NUM2DBL(interval), NUM2DBL(scale));        
    return rb_class_wrap_new((void *)action, rb_cAction);
}

/// @class ScaleTo < Action
/// @group Constructors
/// @method #initialize(scale, interval)
/// Creates an action that will scale the receiver
/// @param scale [Float] the receiver should be scaled to
/// @param interval [Float] the animation interval.
/// @return [ScaleTo] the action.
static VALUE
scale_to_new(VALUE rcv, SEL sel, VALUE scale, VALUE interval)
{
    auto action = cocos2d::ScaleTo::create(NUM2DBL(interval), NUM2DBL(scale));        
    return rb_class_wrap_new((void *)action, rb_cAction);
}

/// @class SkewBy < Action
/// @group Constructors
/// @method #initialize(delta_x_angle, delta_y_angle, interval)
/// Creates an action that skews a Node object to given angles by modifying it's 
/// skewX and skewY attributes by delta_x_angle and delta_y_angle.
/// @param delta_x_angle [Float] the skew X delta angle
/// @param delta_y_angle [Float] the skew Y delta angle
/// @param interval [Float] the animation interval.
/// @return [SkewBy] the action.
static VALUE
skew_by_new(VALUE rcv, SEL sel, VALUE delta_x_angle, VALUE delta_y_angle, VALUE interval)
{
    auto action = cocos2d::SkewBy::create(NUM2DBL(interval), NUM2DBL(delta_x_angle), NUM2DBL(delta_y_angle));        
    return rb_class_wrap_new((void *)action, rb_cAction);
}

/// @class SkewTo < Action
/// @group Constructors
/// @method #initialize(x_angle, y_angle, interval)
/// Creates an action that skews a Node object to given angles by modifying it's 
/// skewX and skewY attributes
/// @param x_angle [Float] the Skew X Angle
/// @param y_angle [Float] the Skew Y Angle
/// @param interval [Float] the animation interval.
/// @return [SkewTp] the action.
static VALUE
skew_to_new(VALUE rcv, SEL sel, VALUE x_angle, VALUE y_angle, VALUE interval)
{
    auto action = cocos2d::SkewTo::create(NUM2DBL(interval), NUM2DBL(x_angle), NUM2DBL(y_angle));        
    return rb_class_wrap_new((void *)action, rb_cAction);
}

/// @class TintBy < Action
/// @group Constructors
/// @method #initialize(delta_red, delta_green, delta_blue, interval)
/// Creates an action that tints a Node from current tint to a custom one.
/// @param delta_red [Float] 
/// @param delta_green [Float] 
/// @param delta_blue [Float] 
/// @param interval [Float] the animation interval.
/// @return [TintBy] the action.
static VALUE
tint_by_new(VALUE rcv, SEL sel, VALUE delta_red, VALUE delta_green, VALUE delta_blue, VALUE interval)
{
    auto action = cocos2d::TintBy::create(NUM2DBL(interval), 
      NUM2BYTE(delta_red), NUM2BYTE(delta_green), NUM2BYTE(delta_blue));        
    return rb_class_wrap_new((void *)action, rb_cAction);
}

/// @class TintTo < Action
/// @group Constructors
/// @method #initialize(red, green, blue, interval)
/// Creates an action that tints the receiver from its current tint to a custom one.
/// @param red [Float] 
/// @param green [Float] 
/// @param blue [Float] 
/// @param interval [Float] the animation interval.
/// @return [TintTo] the action.
static VALUE
tint_to_new(VALUE rcv, SEL sel, VALUE red, VALUE green, VALUE blue, VALUE interval)
{
    auto action = cocos2d::TintTo::create(NUM2DBL(interval), NUM2BYTE(r), NUM2BYTE(g), NUM2BYTE(b));        
    return rb_class_wrap_new((void *)action, rb_cAction);
}

/// @class FadeTo < Action
/// @group Constructors
/// @method #initialize(opacity, interval)
/// Creates an action Fades the receiver. It modifies the opacity from the current opacity
/// to a custom one.
/// @param opacity [Integer] the opacity to fade the object to (between 0-255)
/// @param interval [Float] the animation interval.
/// @return [FadeTo] the action.
static VALUE
fade_to_new(VALUE rcv, SEL sel, VALUE opacity, VALUE interval)
{
    auto action = cocos2d::FadeTo::create(NUM2DBL(interval), NUM2BYTE(opacity));        
    return rb_class_wrap_new((void *)action, rb_cAction);
}

/// @class FadeIn < Action
/// @group Constructors
/// @method #initialize(interval)
/// Creates an action Fades In the receiver. It modifies the opacity from the 0 to 255.
/// @param interval [Float] the animation interval.
/// @return [FadeIn] the action.
static VALUE
fade_in_new(VALUE rcv, SEL sel, VALUE interval)
{
    auto action = cocos2d::FadeIn::create(NUM2DBL(interval));        
    return rb_class_wrap_new((void *)action, rb_cAction);
}

/// @class FadeOut < Action
/// @group Constructors
/// @method #initialize(interval)
/// Creates an action Fades Out the receiver. It modifies the opacity from the 255 to 0.
/// @param interval [Float] the animation interval.
/// @return [FadeOut] the action.
static VALUE
fade_out_new(VALUE rcv, SEL sel, VALUE interval)
{
    auto action = cocos2d::FadeOut::create(NUM2DBL(interval));        
    return rb_class_wrap_new((void *)action, rb_cAction);
}

/// @class Blink < Action
/// @group Constructors
/// @method #initialize(blinks, interval)
/// Creates an action that Blinks the receiver by modifying it's visible attribute.
/// @param blinks [Integer] the number of times to blink.
/// @param interval [Float] the animation interval.
/// @return [Blink] the action.
static VALUE
blink_new(VALUE rcv, SEL sel, VALUE blinks, VALUE interval)
{
    auto action = cocos2d::Blink::create(NUM2DBL(interval), NUM2INT(blinks));        
    return rb_class_wrap_new((void *)action, rb_cAction);
}

/// @class Sequence < Action
/// @group Constructors
/// @method #initialize(actions)
/// Creates an action that runs actions sequentially, 
/// one after another on the receiver.
/// @param actions [Action[]] the array of actions to run in sequence
/// @return [Sequence] the action.
static VALUE
sequence_new(VALUE rcv, SEL sel, VALUE actions)
{
    cocos2d::Vector<cocos2d::FiniteTimeAction *> actionsVector;
    
    for (int i = 0, count = RARRAY_LEN(actions); i < count; i++) {
    actionsVector.pushBack(FINITE_TIME_ACTION(RARRAY_AT(actions, i)));
    }
    
    auto action = cocos2d::Sequence::create(actionsVector);        
    return rb_class_wrap_new((void *)action, rb_cAction);
}

/// @class Spawn < Action
/// @group Constructors
/// @method #initialize(actions)
/// Creates an action that spawns actions in parallel on the receiver.
/// @param actions [Action[]] the array of actions to run
/// @return [Spawn] the action.
static VALUE
spawn_new(VALUE rcv, SEL sel, VALUE actions)
{
    cocos2d::Vector<cocos2d::FiniteTimeAction *> actionsVector;
    
    for (int i = 0, count = RARRAY_LEN(actions); i < count; i++) {
    actionsVector.pushBack(FINITE_TIME_ACTION(RARRAY_AT(actions, i)));
    }
    
    auto action = cocos2d::Spawn::create(actionsVector);        
    return rb_class_wrap_new((void *)action, rb_cAction);
}

extern "C"
void
Init_Action(void)
{
    rb_cAction = rb_define_class_under(rb_mMC, "Action", rb_cObject);

    rb_define_method(rb_cAction, "reverse", action_reverse, 0);
    rb_define_method(rb_cAction, "done?", action_done, 0);
    
    rb_cMoveBy = rb_define_class_under(rb_mMC, "MoveBy", rb_cAction);
    rb_define_singleton_method(rb_cMoveBy, "new", move_by_new, 2);

    rb_cMoveTo = rb_define_class_under(rb_mMC, "MoveTo", rb_cAction);
    rb_define_singleton_method(rb_cMoveTo, "new", move_to_new, 2);

    rb_cJumpBy = rb_define_class_under(rb_mMC, "JumpBy", rb_cAction);
    rb_define_singleton_method(rb_cJumpBy, "new", jump_by_new, 4);

    rb_cJumpTo = rb_define_class_under(rb_mMC, "JumpTo", rb_cAction);
    rb_define_singleton_method(rb_cJumpTo, "new", jump_to_new, 4);

    rb_cRotateBy = rb_define_class_under(rb_mMC, "RotateBy", rb_cAction);
    rb_define_singleton_method(rb_cRotateBy, "new", rotate_by_new, 2);

    rb_cRotateTo = rb_define_class_under(rb_mMC, "RotateTo", rb_cAction);
    rb_define_singleton_method(rb_cRotateTo, "new", rotate_to_new, 2);

    rb_cScaleBy = rb_define_class_under(rb_mMC, "ScaleBy", rb_cAction);
    rb_define_singleton_method(rb_cScaleBy, "new", scale_by_new, 2);

    rb_cScaleTo = rb_define_class_under(rb_mMC, "ScaleTo", rb_cAction);
    rb_define_singleton_method(rb_cScaleTo, "new", scale_to_new, 2);

    rb_cTintBy = rb_define_class_under(rb_mMC, "TintBy", rb_cAction);
    rb_define_singleton_method(rb_cTintBy, "new", tint_by_new, 4);

    rb_cTintTo = rb_define_class_under(rb_mMC, "TintTo", rb_cAction);
    rb_define_singleton_method(rb_cTintTo, "new", tint_by_new, 4);

    rb_cFadeTo = rb_define_class_under(rb_mMC, "FadeTo", rb_cAction);
    rb_define_singleton_method(rb_cFadeTo, "new", fade_to_new, 2);

    rb_cFadeIn = rb_define_class_under(rb_mMC, "FadeIn", rb_cAction);
    rb_define_singleton_method(rb_cFadeIn, "new", fade_in_new, 1);

    rb_cFadeOut = rb_define_class_under(rb_mMC, "FadeOut", rb_cAction);
    rb_define_singleton_method(rb_cFadeOut, "new", fade_out_new, 1);

    rb_cBlink = rb_define_class_under(rb_mMC, "Blink", rb_cAction);
    rb_define_singleton_method(rb_cBlink, "new", blink_new, 2);

    rb_cSequence = rb_define_class_under(rb_mMC, "Sequence", rb_cAction);
    rb_define_singleton_method(rb_cSequence, "new", sequence_new, 1);

    rb_cSpawn = rb_define_class_under(rb_mMC, "Spawn", rb_cAction);
    rb_define_singleton_method(rb_cSpawn, "new", spawn_new, 1);
}

// TODO: Add actions
  // Follow
  // Speed
  // Repeat
  // RepeatForever
  // BezierTo/BezierBy
  // DelayTime
  // ReverseTime
  // Animate