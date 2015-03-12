#include "rubymotion.h"

/// @class Sprite < Node

VALUE rb_cSprite = Qnil;

/// @group Spritesheets

/// @method .load(file_name)
/// Loads all sprites from the content of +file_name+, which should be
/// the name of a property list spritesheet file in the application's resource
//  directory. Once a spritesheet file is loaded, its frames can be created
/// using {Sprite#initialize}.
/// Sprite frames files can be created with a visual editor such as
/// TexturePacker.
/// @param file_name [String] the name of the sprite frames property list file.
/// @return [nil]

static VALUE
sprite_load(VALUE rcv, SEL sel, VALUE plist_path)
{
    cocos2d::SpriteFrameCache::getInstance()->addSpriteFramesWithFile(
	    RSTRING_PTR(plist_path));
    return Qnil;
}

/// @group Constructors

/// @method #initialize(sprite_name)
/// Creates a new sprite object from the name of +sprite_name+. The sprite
/// must either exist as a standalone image file in the application's resource
/// directory, or must have been loaded from a spritesheet using #{load}.
/// @param sprite_name [String] the name of the sprite to create.

static VALUE
sprite_new(VALUE rcv, SEL sel, VALUE name)
{
    std::string name_str = RSTRING_PTR(name);
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
    return rb_class_wrap_new((void *)sprite, rcv);
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

static VALUE
sprite_move_by(VALUE rcv, SEL sel, VALUE position, VALUE interval)
{
    return run_action(rcv, cocos2d::MoveBy::create(NUM2DBL(interval),
		rb_any_to_ccvec2(position)));
}

/// @method #move_to(location, interval)
/// Moves the position of the receiver to a new given location.
/// @param location [Point] where the receiver should be moved to.
/// @param interval [Float] the animation interval.
/// @return [Sprite] the receiver.

static VALUE
sprite_move_to(VALUE rcv, SEL sel, VALUE position, VALUE interval)
{
    return run_action(rcv, cocos2d::MoveTo::create(NUM2DBL(interval),
		rb_any_to_ccvec2(position)));
}

/// @method #blink(number_of_blinks, interval)
/// Blinks the receiver.
/// @param number_of_blinks [Integer] the number of times the receiver should
///   blink.
/// @param interval [Float] the animation interval.
/// @return [Sprite] the receiver.

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
/// @param frame_names [Array<String>] an array of frame names to load and use
///   for the animation.
/// @param delay [Float] the delay in seconds between each frame animation.
/// @param loops [Integer, Symbol] the number of times the animation should
///   loop. If given the +:forever+ symbol, the animation will loop forever.
/// @return [Sprite] the receiver.

static VALUE
sprite_animate(VALUE rcv, SEL sel, int argc, VALUE *argv)
{
    VALUE frame_names = Qnil, delay = Qnil, loops = Qnil;

    rb_scan_args(argc, argv, "21", &frame_names, &delay, &loops);

    cocos2d::Vector<cocos2d::SpriteFrame *> frames;
    auto texture_cache = cocos2d::Director::getInstance()->getTextureCache();
    for (int i = 0, count = RARRAY_LEN(frame_names); i < count; i++) {
	std::string frame_name = RSTRING_PTR(RARRAY_AT(frame_names, i));
	auto texture = texture_cache->addImage(frame_name);
	assert(texture != NULL);
	cocos2d::Rect rect = cocos2d::Rect::ZERO;
	rect.size = texture->getContentSize();
	frames.pushBack(cocos2d::SpriteFrame::createWithTexture(texture,
		    rect));
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

extern "C"
void
Init_Sprite(void)
{
    rb_cSprite = rb_define_class_under(rb_mMC, "Sprite", rb_cNode);

    rb_define_singleton_method(rb_cSprite, "load", sprite_load, 1);
    rb_define_singleton_method(rb_cSprite, "new", sprite_new, 1);
    rb_define_method(rb_cSprite, "move_by", sprite_move_by, 2);
    rb_define_method(rb_cSprite, "move_to", sprite_move_to, 2);
    rb_define_method(rb_cSprite, "blink", sprite_blink, 2);
    rb_define_method(rb_cSprite, "animate", sprite_animate, -1);
}
