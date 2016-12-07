#include "rubymotion.h"
#include "motion-game.h"

VALUE
rb_cocos2d_object_new(cocos2d::Ref *ptr, VALUE klass)
{
    ptr->retain();
    return rb_class_wrap_new(ptr, klass);
}
