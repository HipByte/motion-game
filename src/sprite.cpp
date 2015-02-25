#include "rubymotion.h"

/// @class Sprite < Node

VALUE rb_cSprite = Qnil;

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

extern "C"
void
Init_Sprite(void)
{
    rb_cSprite = rb_define_class_under(rb_mMC, "Sprite", rb_cNode);

    rb_define_singleton_method(rb_cSprite, "load", sprite_load, 1);
    rb_define_singleton_method(rb_cSprite, "new", sprite_new, 1);
}
