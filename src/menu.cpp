#include "rubymotion.h"

VALUE rb_cMenu = Qnil;

static VALUE
menu_alloc(VALUE rcv, SEL sel)
{
    cocos2d::Menu *menu = cocos2d::Menu::create();
    return rb_class_wrap_new((void *)menu, rcv);
}

static VALUE
menu_image_item(VALUE rcv, SEL sel, VALUE normal_image, VALUE selected_image)
{
    VALUE block = rb_current_block();
    if (block == Qnil) {
	rb_raise(rb_eArgError, "block not given");
    }
    block = rb_retain(block); // FIXME need release...

    cocos2d::MenuItemImage *item = cocos2d::MenuItemImage::create(
	    RSTRING_PTR(StringValue(normal_image)), RSTRING_PTR(StringValue(selected_image)),
	    [block](cocos2d::Ref *sender) {
		rb_block_call(block, 0, NULL);
	    });
    MENU(rcv)->addChild(item);
    return rcv;
}

extern "C"
void
Init_Menu(void)
{
    rb_cMenu = rb_define_class_under(rb_mMC, "Menu", rb_cNode);

    rb_define_singleton_method(rb_cMenu, "alloc", menu_alloc, 0);
    rb_define_method(rb_cMenu, "image_item", menu_image_item, 2); 
}
