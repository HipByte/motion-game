#include "rubymotion.h"

VALUE rb_cSprite = Qnil;

static VALUE
sprite_load(VALUE rcv, SEL sel, VALUE plist_path)
{
    cocos2d::SpriteFrameCache::getInstance()->addSpriteFramesWithFile(
	    RSTRING_PTR(plist_path));
    return Qnil;
}

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
