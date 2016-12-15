#include "rubymotion.h"
#include "motion-game.h"

/// @class Sprite < Node

VALUE rb_cSprite = Qnil;

/// @group Spritesheets

/// @method .load(file_name)
/// Loads all sprites from the content of +file_name+, which should be
/// the name of a property list spritesheet file in the application's resource
/// directory. Once a spritesheet file is loaded, individual sprites can be
/// created using {Sprite#initialize} by providing the name of the frame.
/// Sprite frames files can be created with a visual editor such as
/// TexturePacker.
/// @param file_name [String] the name of the sprite frames property list file.
/// @return [nil]

static VALUE
sprite_load(VALUE rcv, SEL sel, VALUE plist_path)
{
    cocos2d::SpriteFrameCache::getInstance()->addSpriteFramesWithFile(
	    RSTRING_PTR(StringValue(plist_path)));
    return Qnil;
}

/// @group Constructors

/// @method #initialize(sprite_name)
/// Creates a new sprite object from +sprite_name+, which must be either the
/// name of a standalone image file in the application's resource directory
/// or the name of a sprite frame which was loaded from a spritesheet using
/// {load}.
/// @param sprite_name [String] the name of the sprite to create.

static VALUE
sprite_new(VALUE rcv, SEL sel, VALUE name)
{
    std::string name_str = RSTRING_PTR(StringValue(name));
    cocos2d::Sprite *sprite = NULL;

    // Are we trying to retrieve a sprite frame?
    cocos2d::SpriteFrame *sprite_frame =
	cocos2d::SpriteFrameCache::getInstance()->getSpriteFrameByName(
		name_str);
    if (sprite_frame != NULL) {
	sprite = cocos2d::Sprite::createWithSpriteFrame(sprite_frame);
    }
    else {
	// A regular sprite file.
	sprite = cocos2d::Sprite::create(name_str);
    }
    assert(sprite != NULL); // TODO raise exception
    return rb_cocos2d_object_new(sprite, rcv);
}

/// @group Actions

static VALUE
run_action(VALUE rcv, cocos2d::FiniteTimeAction *action)
{
    VALUE block = rb_current_block();
    if (block != Qnil) {
	block = rb_retain(block); // FIXME need release...
	auto call_funcn =
	    cocos2d::CallFuncN::create([block](cocos2d::Node *node) {
		rb_block_call(block, 0, NULL);
	    });
	action = cocos2d::Sequence::create(action, call_funcn, (void *)0);
    }
    SPRITE(rcv)->runAction(action);
    return rcv;
}

/// @method #move_by(delta_location, interval)
/// Moves the position of the receiver to a new location determined by the
/// sum of the current location and the given +delta_location+ object.
/// @param delta_location [Point] a point that will be added to the receiver's
///   current location.
/// @param interval [Float] the animation interval.
/// @return [Sprite] the receiver.
/// @yield if passed a block, the block will be called for the action.

static VALUE
sprite_move_by(VALUE rcv, SEL sel, VALUE delta_location, VALUE interval)
{
    return run_action(rcv, cocos2d::MoveBy::create(NUM2DBL(interval),
		    rb_any_to_ccvec2(delta_location)));
}

/// @method #move_to(location, interval)
/// Moves the position of the receiver to a new given location.
/// @param location [Point] where the receiver should be moved to.
/// @param interval [Float] the animation interval.
/// @return [Sprite] the receiver.
/// @yield if passed a block, the block will be called for the action.

static VALUE
sprite_move_to(VALUE rcv, SEL sel, VALUE location, VALUE interval)
{
    return run_action(rcv, cocos2d::MoveTo::create(NUM2DBL(interval),
		    rb_any_to_ccvec2(location)));
}

/// @method #rotate_by(delta_angle, interval)
/// Rotates the position of the receiver to a new angle determined by the
/// sum of the current rotation and the given +delta_angle+ object.
/// @param delta_angle [Float] the angle to add to the current rotation
/// @param interval [Float] the animation interval.
/// @return [Sprite] the receiver.
/// @yield if passed a block, the block will be called for the action.

static VALUE
sprite_rotate_by(VALUE rcv, SEL sel, VALUE delta_angle, VALUE interval)
{
    return run_action(rcv, cocos2d::RotateBy::create(NUM2DBL(interval),
		    NUM2DBL(delta_angle)));
}

/// @method #rotate_to(angle, interval)
/// Rotates the angle of the receiver to a new angle certain angle
/// by modifying it's rotation attribute.
/// @param angle [Float] the receiver should be rotated to.
/// @param interval [Float] the animation interval.
/// @return [Sprite] the receiver.
/// @yield if passed a block, the block will be called for the action.

static VALUE
sprite_rotate_to(VALUE rcv, SEL sel, VALUE angle, VALUE interval)
{
    return run_action(rcv, cocos2d::RotateTo::create(NUM2DBL(interval),
		    NUM2DBL(angle)));
}

/// @method #blink(number_of_blinks, interval)
/// Blinks the receiver.
/// @param number_of_blinks [Integer] the number of times the receiver should
///   blink.
/// @param interval [Float] the animation interval.
/// @return [Sprite] the receiver.
/// @yield if passed a block, the block will be called for the action.

static VALUE
sprite_blink(VALUE rcv, SEL sel, VALUE blinks, VALUE interval)
{
    return run_action(rcv, cocos2d::Blink::create(NUM2DBL(interval),
		    NUM2INT(blinks)));
}

/// @method #animate(frame_names, delay, loops=1)
/// Starts an animation where the sprite display frame will be changed to
/// the given frames in +sprite_frames_names+ based on the given +delay+ and
/// repeated +loops+ times.
/// @param frame_names [Array<String>] an array of sprite frames to load and
///   use for the animation, which can be either the names of standalone image
///   files in the application's resource directory or the names of sprite
///   frames loaded from a spritesheet using {load}.
/// @param delay [Float] the delay in seconds between each frame animation.
/// @param loops [Integer, Symbol] the number of times the animation should
///   loop. If given the +:forever+ symbol, the animation will loop forever.
/// @return [Sprite] the receiver.
/// @yield if passed a block, the block will be called for the action.

static VALUE
sprite_animate(VALUE rcv, SEL sel, int argc, VALUE *argv)
{
    VALUE frame_names = Qnil, delay = Qnil, loops = Qnil;

    rb_scan_args(argc, argv, "21", &frame_names, &delay, &loops);

    cocos2d::Vector<cocos2d::SpriteFrame *> frames;
    auto texture_cache = cocos2d::Director::getInstance()->getTextureCache();
    for (int i = 0, count = RARRAY_LEN(frame_names); i < count; i++) {
	VALUE name = RARRAY_AT(frame_names, i);
	std::string frame_name = RSTRING_PTR(StringValue(name));
	auto texture = texture_cache->addImage(frame_name);
	cocos2d::SpriteFrame *frame = NULL;
	if (texture != NULL) {
	    cocos2d::Rect rect = cocos2d::Rect::ZERO;
	    rect.size = texture->getContentSize();
	    frame = cocos2d::SpriteFrame::createWithTexture(texture, rect);
	}
	else {
	    frame = cocos2d::SpriteFrameCache::getInstance()->getSpriteFrameByName(frame_name);
	}
	assert(frame != NULL);
	frames.pushBack(frame);
    }

    int loops_i = 1;
    bool forever = false;
    if (loops != Qnil) {
	if (rb_obj_is_kind_of(loops, rb_cInteger)) {
	    loops_i = NUM2LONG(loops);
	}
	else if (loops == rb_name2sym("forever")) {
	    forever = true;
	}
    }

    auto animation = cocos2d::Animation::createWithSpriteFrames(frames,
	    NUM2DBL(delay), loops_i);
    cocos2d::ActionInterval *action = cocos2d::Animate::create(animation);
    if (forever) {
	action = cocos2d::RepeatForever::create(action);
    }
    return run_action(rcv, action);
}


/// @method #flipped_horizontally=(value)
/// Sets whether the sprite should be flipped horizontally or not. It only flips
/// the texture of the sprite, and not the texture of the sprite's children.
/// Also, flipping the texture doesn't alter the anchorPoint.
/// @param value [Boolean] true if the sprite should be flipped horizontally, false otherwise.

/// @method #flipped_x=(value)
/// This is alias method of {#flipped_horizontally=}.

static VALUE
sprite_flipped_horizontally_set(VALUE rcv, SEL sel, VALUE arg)
{
    SPRITE(rcv)->setFlippedX(RTEST(arg));
    return arg;
}

/// @method #flipped_vertically=(value)
/// Sets whether the sprite should be flipped vertically or not. It only flips
/// the texture of the sprite, and not the texture of the sprite's children.
/// Also, flipping the texture doesn't alter the anchorPoint.
/// @param value [Boolean] true if the sprite should be flipped vertically, false otherwise.

/// @method #flipped_y=(value)
/// This is alias method of {#flipped_vertically=}.

static VALUE
sprite_flipped_vertically_set(VALUE rcv, SEL sel, VALUE arg)
{
    SPRITE(rcv)->setFlippedY(RTEST(arg));
    return arg;
}

/// @group Physics

static cocos2d::PhysicsBody *
need_physics(VALUE rcv)
{
    auto physics = SPRITE(rcv)->getPhysicsBody();
    if (physics == NULL) {
	rb_raise(rb_eRuntimeError, "receiver does not have a physics body");
    }
    return physics;
}

/// @method #attach_physics_box(size=nil)
/// Attaches a physics body with a box shape to the sprite.
/// @param size [Size] the size of the box. If +nil+ is given, the size of the
///   sprite, retrieved with {#size}, will be used instead.
/// @return [Sprite] the receiver.

static VALUE
sprite_attach_physics_box(VALUE rcv, SEL sel, int argc, VALUE *argv)
{
    VALUE size = Qnil;
    rb_scan_args(argc, argv, "01", &size);

    auto sprite = SPRITE(rcv);
    auto ccsize = sprite->getContentSize();
    if (size != Qnil) {
	ccsize = rb_any_to_ccsize(size);
    }
    auto physics = cocos2d::PhysicsBody::createBox(ccsize);
    SPRITE(rcv)->setPhysicsBody(physics);
    return rcv;
}

/// @method #apply_impulse(force)
/// Applies a continuous force to the sprite body.
/// @param force [Point] the force to apply.
/// @return [Sprite] the receiver.

static VALUE
sprite_apply_impulse(VALUE rcv, SEL sel, VALUE force)
{
    need_physics(rcv)->applyImpulse(rb_any_to_ccvec2(force));
    return rcv;
}

/// @method #apply_force(force)
/// Applies an immediate force to the sprite body.
/// @param force [Point] the force to apply.
/// @return [Sprite] the receiver.

static VALUE
sprite_apply_force(VALUE rcv, SEL sel, VALUE force)
{
    need_physics(rcv)->applyForce(rb_any_to_ccvec2(force));
    return rcv;
}

/// @endgroup

/// @property #mass
/// @return [Float] the body mass of the sprite.

static VALUE
sprite_mass(VALUE rcv, SEL sel)
{
    return DBL2NUM(need_physics(rcv)->getMass());
}

static VALUE
sprite_mass_set(VALUE rcv, SEL sel, VALUE arg)
{
    need_physics(rcv)->setMass(NUM2DBL(arg));
    return arg;
}

/// @method #gravitates?
/// @return [Boolean] whether the sprite should be affected by the scene's
///   gravitational force. The default is +true+.

static VALUE
sprite_gravitates(VALUE rcv, SEL sel)
{
    return need_physics(rcv)->isGravityEnabled() ? Qtrue : Qfalse;
}

/// @method #gravitates=(value)
/// Set the body is affected by the physics world's gravitational force or not.
/// @param value[Boolean] true if enable gravitational force.

static VALUE
sprite_gravitates_set(VALUE rcv, SEL sel, VALUE arg)
{
    need_physics(rcv)->setGravityEnable(RTEST(arg));
    return arg;
}

/// @method #dynamic?
/// @return [Boolean] whether the sprite body should be dynamic or not in the
///   physics world. The default is +true+, and a dynamic body will affect
///   with gravity.

static VALUE
sprite_dynamic(VALUE rcv, SEL sel)
{
    return need_physics(rcv)->isDynamic() ? Qtrue : Qfalse;
}

/// @method #dynamic=(value)
/// Set dynamic to body.
/// @param value [Boolean] true if the sprite body should be dynamic.

static VALUE
sprite_dynamic_set(VALUE rcv, SEL sel, VALUE arg)
{
    need_physics(rcv)->setDynamic(RTEST(arg));
    return arg;
}

/// @property #friction
/// @return [Float] the linear damping / air friction force on the sprite body.

static VALUE
sprite_friction(VALUE rcv, SEL sel)
{
    return DBL2NUM(need_physics(rcv)->getLinearDamping());
}

static VALUE
sprite_friction_set(VALUE rcv, SEL sel, VALUE arg)
{
    need_physics(rcv)->setLinearDamping(NUM2DBL(arg));
    return arg;
}

/// @property #velocity
/// @return [Point] the velocity force on the sprite body.

static VALUE
sprite_velocity(VALUE rcv, SEL sel)
{
    return rb_ccvec2_to_obj(need_physics(rcv)->getVelocity());
}

static VALUE
sprite_velocity_set(VALUE rcv, SEL sel, VALUE arg)
{
    need_physics(rcv)->setVelocity(rb_any_to_ccvec2(arg));
    return arg;
}

/// @method #resting?
/// @return [Boolean] whether the body is at rest.

static VALUE
sprite_resting(VALUE rcv, SEL sel)
{
    return need_physics(rcv)->isResting() ? Qtrue : Qfalse;
}

/// @method #resting=(value)
/// Set body to rest
/// @param value [Boolean] true if rest the body.

static VALUE
sprite_resting_set(VALUE rcv, SEL sel, VALUE arg)
{
    need_physics(rcv)->setResting(RTEST(arg));
    return arg;
}

/// @property #inertia_moment
/// @return [Float] the moment of inertia of the body.

static VALUE
sprite_inertia_moment(VALUE rcv, SEL sel)
{
    return DBL2NUM(need_physics(rcv)->getMoment());
}

static VALUE
sprite_inertia_moment_set(VALUE rcv, SEL sel, VALUE arg)
{
    need_physics(rcv)->setMoment(NUM2DBL(arg));
    return arg;
}

/// @property #category_mask
/// @return [Integer] physics category mask.

static VALUE
sprite_category_mask(VALUE rcv, SEL sel)
{
    return LONG2NUM(need_physics(rcv)->getCategoryBitmask());
}

static VALUE
sprite_category_mask_set(VALUE rcv, SEL sel, VALUE arg)
{
    need_physics(rcv)->setCategoryBitmask(NUM2LONG(arg));
    return arg;
}

/// @property #contact_mask
/// @return [Integer] physics contact test mask.

static VALUE
sprite_contact_mask(VALUE rcv, SEL sel)
{
    return LONG2NUM(need_physics(rcv)->getContactTestBitmask());
}

static VALUE
sprite_contact_mask_set(VALUE rcv, SEL sel, VALUE arg)
{
    need_physics(rcv)->setContactTestBitmask(NUM2LONG(arg));
    return arg;
}

/// @property #collision_mask
/// @return [Integer] physics collision mask.

static VALUE
sprite_collision_mask(VALUE rcv, SEL sel)
{
    return LONG2NUM(need_physics(rcv)->getCollisionBitmask());
}

static VALUE
sprite_collision_mask_set(VALUE rcv, SEL sel, VALUE arg)
{
    need_physics(rcv)->setCollisionBitmask(NUM2LONG(arg));
    return arg;
}

extern "C"
void
Init_Sprite(void)
{
    rb_cSprite = rb_define_class_under(rb_mMC, "Sprite", rb_cNode);
    // rb_register_cocos2d_object_finalizer(rb_cSprite); removed because rb_cSprite inherits rb_cNode and it already has finalizer.

    rb_define_singleton_method(rb_cSprite, "load", sprite_load, 1);
    rb_define_singleton_method(rb_cSprite, "new", sprite_new, 1);
    rb_define_method(rb_cSprite, "move_by", sprite_move_by, 2);
    rb_define_method(rb_cSprite, "move_to", sprite_move_to, 2);
    rb_define_method(rb_cSprite, "rotate_by", sprite_rotate_by, 2);
    rb_define_method(rb_cSprite, "rotate_to", sprite_rotate_to, 2);
    rb_define_method(rb_cSprite, "blink", sprite_blink, 2);
    rb_define_method(rb_cSprite, "animate", sprite_animate, -1);
    rb_define_method(rb_cSprite, "flipped_horizontally=", sprite_flipped_horizontally_set, 1);
    rb_define_method(rb_cSprite, "flipped_x=", sprite_flipped_horizontally_set, 1);
    rb_define_method(rb_cSprite, "flipped_vertically=", sprite_flipped_vertically_set, 1);
    rb_define_method(rb_cSprite, "flipped_y=", sprite_flipped_vertically_set, 1);
    rb_define_method(rb_cSprite, "attach_physics_box", sprite_attach_physics_box, -1);
    rb_define_method(rb_cSprite, "apply_impulse", sprite_apply_impulse, 1);
    rb_define_method(rb_cSprite, "apply_force", sprite_apply_force, 1);
    rb_define_method(rb_cSprite, "mass", sprite_mass, 0);
    rb_define_method(rb_cSprite, "mass=", sprite_mass_set, 1);
    rb_define_method(rb_cSprite, "gravitates?", sprite_gravitates, 0);
    rb_define_method(rb_cSprite, "gravitates=", sprite_gravitates_set, 1);
    rb_define_method(rb_cSprite, "dynamic?", sprite_dynamic, 0);
    rb_define_method(rb_cSprite, "dynamic=", sprite_dynamic_set, 1);
    rb_define_method(rb_cSprite, "friction", sprite_friction, 0);
    rb_define_method(rb_cSprite, "friction=", sprite_friction_set, 1);
    rb_define_method(rb_cSprite, "velocity", sprite_velocity, 0);
    rb_define_method(rb_cSprite, "velocity=", sprite_velocity_set, 1);
    rb_define_method(rb_cSprite, "resting?", sprite_resting, 0);
    rb_define_method(rb_cSprite, "resting=", sprite_resting_set, 1);
    rb_define_method(rb_cSprite, "inertia_moment", sprite_inertia_moment, 0);
    rb_define_method(rb_cSprite, "inertia_moment=", sprite_inertia_moment_set, 1);
    rb_define_method(rb_cSprite, "category_mask", sprite_category_mask, 0);
    rb_define_method(rb_cSprite, "category_mask=", sprite_category_mask_set, 1);
    rb_define_method(rb_cSprite, "collision_mask", sprite_collision_mask, 0);
    rb_define_method(rb_cSprite, "collision_mask=", sprite_collision_mask_set, 1);
    rb_define_method(rb_cSprite, "contact_mask", sprite_contact_mask, 0);
    rb_define_method(rb_cSprite, "contact_mask=", sprite_contact_mask_set, 1);
}
