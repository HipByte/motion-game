#include "rubymotion.h"

/// @class Label < Node

VALUE rb_cLabel = Qnil;

/// @method #initialize(text, font, font_size)
/// Creates a new label object, with a given text and font attributes.
/// @param text [String] the label title.
/// @param font [String] the name of the font to use.
/// @param font_size [Float] the size of the font ot use.

static VALUE
label_new(VALUE rcv, SEL sel, VALUE text, VALUE font, VALUE fontsize)
{
    cocos2d::Label *label = cocos2d::Label::createWithSystemFont(
	    RSTRING_PTR(text), RSTRING_PTR(font), NUM2DBL(fontsize));
    return rb_class_wrap_new((void *)label, rcv);
}

extern "C"
void
Init_Label(void)
{
    rb_cLabel = rb_define_class_under(rb_mMC, "Label", rb_cNode);

    rb_define_singleton_method(rb_cLabel, "new", label_new, 3);
}
