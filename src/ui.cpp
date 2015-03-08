#include "rubymotion.h"

#include <ui/UIWidget.h>
#include <ui/UIText.h>
#include <ui/UIButton.h>
#include <ui/UISlider.h>
#include <ui/UIScrollView.h>
#include <ui/UIListView.h>

static VALUE rb_mUI = Qnil;

/// @class UI::Widget < Node
/// The base class for all UI widgets. You should not instantiate this class
/// directly but use a subclass instead.

static VALUE rb_cUIWidget = Qnil;

#define WIDGET(obj) _COCOS_WRAP_GET(obj, cocos2d::ui::Widget)

/// @group Properties

/// @property #enabled?
/// Whether the widget is enabled. When enabled, a widget can be
/// touched or clicked. By default, a widget is enabled.
/// @return [Boolean] whether the widget is enabled.

static VALUE
widget_enabled(VALUE rcv, SEL sel)
{
    return WIDGET(rcv)->isEnabled() ? Qtrue : Qfalse;
}

static VALUE
widget_enabled_set(VALUE rcv, SEL sel, VALUE val)
{
    WIDGET(rcv)->setEnabled(RTEST(val));
    return val;
}

/// @property #highlighted?
/// Whether the widget is highlighted. By default, a widget is not highlighted.
/// @return [Boolean] whether the widget is highlighted.

static VALUE
widget_highlighted(VALUE rcv, SEL sel)
{
    return WIDGET(rcv)->isHighlighted() ? Qtrue : Qfalse;
}

static VALUE
widget_highlighted_set(VALUE rcv, SEL sel, VALUE val)
{
    WIDGET(rcv)->setHighlighted(RTEST(val));
    return val;
}

/// @group Events

/// @method #on_touch
/// Configures a block to be called when a touch event is received on the
/// widget. 
/// @yield the given block will be called when the event is received.
/// @return [Widget] the receiver.

static VALUE
widget_on_touch(VALUE rcv, SEL sel)
{
    VALUE block = rb_current_block();
    if (block == Qnil) {
	rb_raise(rb_eArgError, "block not given");
    }
    block = rb_retain(block); // FIXME need release...

    WIDGET(rcv)->addTouchEventListener(
	    [block](cocos2d::Ref *ref,
		    cocos2d::ui::Widget::TouchEventType event_type) {
		rb_block_call(block, 0, NULL);
	    });
    return rcv;
}

/// @class UI::Text < UI::Widget

/// @group Constructors

static VALUE rb_cUIText = Qnil;

#define TEXT(obj) _COCOS_WRAP_GET(obj, cocos2d::ui::Text)

/// @method #initialize(text='', font='', font_size=0)
/// Creates a new Text widget with optional content, font name and size.
/// @param text [String] content for the text widget.
/// @param font [String] name of the font the text widget should use.
/// @param font_size [Integer] size of the font the text widget should use.

static VALUE
text_new(VALUE rcv, SEL sel, int argc, VALUE *argv)
{
    VALUE str = Qnil, font_name = Qnil, font_size = Qnil;

    rb_scan_args(argc, argv, "03", &str, &font_name, &font_size);

    cocos2d::ui::Text *text = cocos2d::ui::Text::create();

    if (str != Qnil) {
	text->setString(RSTRING_PTR(str));
    }
    if (font_name != Qnil) {
	text->setFontName(RSTRING_PTR(font_name));
    }
    if (font_size != Qnil) {
	text->setFontSize(NUM2LONG(font_size));
    }

    return rb_class_wrap_new(text, rcv);
}

/// @group Properties

/// @property #text
/// @return [String] content of the widget.

static VALUE
text_text(VALUE rcv, SEL sel)
{
    return RSTRING_NEW(TEXT(rcv)->getString().c_str());
}

static VALUE
text_text_set(VALUE rcv, SEL sel, VALUE val)
{
    TEXT(rcv)->setString(RSTRING_PTR(val));
    return val;
}

/// @property #text_color
/// @return [Color] color of the text part of the widget.

static VALUE
text_text_color(VALUE rcv, SEL sel)
{
    return rb_cccolor4_to_obj(TEXT(rcv)->getTextColor());
}

static VALUE
text_text_color_set(VALUE rcv, SEL sel, VALUE val)
{
    TEXT(rcv)->setTextColor(rb_any_to_cccolor4(val));
    return val;
}

/// @property #font
/// @return [String] name of the font used by the widget.

static VALUE
text_font(VALUE rcv, SEL sel)
{
    return RSTRING_NEW(TEXT(rcv)->getFontName().c_str());
}

static VALUE
text_font_set(VALUE rcv, SEL sel, VALUE val)
{
    TEXT(rcv)->setFontName(RSTRING_PTR(val));
    return val;
}

/// @property #font_size
/// @return [Integer] size of the font used by the widget.

static VALUE
text_font_size(VALUE rcv, SEL sel)
{
    return LONG2NUM(TEXT(rcv)->getFontSize());
}

static VALUE
text_font_size_set(VALUE rcv, SEL sel, VALUE val)
{
    TEXT(rcv)->setFontSize(NUM2LONG(val));
    return val;
}

/// @property #area_size
/// @return [Size] the text rendering area size.

static VALUE
text_area_size(VALUE rcv, SEL sel)
{
    return rb_ccsize_to_obj(TEXT(rcv)->getTextAreaSize());
}

static VALUE
text_area_size_set(VALUE rcv, SEL sel, VALUE val)
{
    TEXT(rcv)->setTextAreaSize(rb_any_to_ccsize(val));
    return val;
}

/// @property #vertical_align
/// @return [:top, :center, :bottom] the vertical alignment of the text.

static VALUE sym_top = Qnil, sym_center = Qnil, sym_bottom = Qnil,
	     sym_left = Qnil, sym_right = Qnil;

static VALUE
text_vertical_align(VALUE rcv, SEL sel)
{
    switch (TEXT(rcv)->getTextVerticalAlignment()) {
	case cocos2d::TextVAlignment::TOP:
	    return sym_top;
	case cocos2d::TextVAlignment::CENTER:
	    return sym_center;
	case cocos2d::TextVAlignment::BOTTOM:
	    return sym_bottom;
	default:
	    abort();
    }
}

static VALUE
text_vertical_align_set(VALUE rcv, SEL sel, VALUE val)
{
    cocos2d::TextVAlignment align;
    if (val == sym_top) {
	align = cocos2d::TextVAlignment::TOP;
    }
    else if (val == sym_center) {
	align = cocos2d::TextVAlignment::CENTER;
    }
    else if (val == sym_bottom) {
	align = cocos2d::TextVAlignment::BOTTOM;
    }
    else {
	rb_raise(rb_eArgError, "expected :top, :center or :bottom symbol");
    }
    TEXT(rcv)->setTextVerticalAlignment(align);
    return val;
}

/// @property #horizontal_align
/// @return [:left, :center, :right] the horizontal alignment of the text.

static VALUE
text_horizontal_align(VALUE rcv, SEL sel)
{
    switch (TEXT(rcv)->getTextHorizontalAlignment()) {
	case cocos2d::TextHAlignment::LEFT:
	    return sym_left;
	case cocos2d::TextHAlignment::CENTER:
	    return sym_center;
	case cocos2d::TextHAlignment::RIGHT:
	    return sym_right;
	default:
	    abort();
    }
}

static VALUE
text_horizontal_align_set(VALUE rcv, SEL sel, VALUE val)
{
    cocos2d::TextHAlignment align;
    if (val == sym_left) {
	align = cocos2d::TextHAlignment::LEFT;
    }
    else if (val == sym_center) {
	align = cocos2d::TextHAlignment::CENTER;
    }
    else if (val == sym_right) {
	align = cocos2d::TextHAlignment::RIGHT;
    }
    else {
	rb_raise(rb_eArgError, "expected :left, :center or :right symbol");
    }
    TEXT(rcv)->setTextHorizontalAlignment(align);
    return val;
}

/// @class UI::Button < UI::Widget
/// A button widget. The {#on_touch} method can be used to set a callback when
/// the button is activated. Example:
///   button = UI::Button.new("Touch me!")
///   button.on_touch { puts "touched!" }

/// @group Constructors

static VALUE rb_cUIButton = Qnil;

#define BUTTON(obj) _COCOS_WRAP_GET(obj, cocos2d::ui::Button)

/// @method #initialize(title='')
/// Creates a new Button widget with an optional title.
/// @param title [String] title for the button.

static VALUE
button_new(VALUE rcv, SEL sel, int argc, VALUE *argv)
{
    VALUE title = Qnil;

    rb_scan_args(argc, argv, "01", &title);

    cocos2d::ui::Button *button = cocos2d::ui::Button::create();

    if (title != Qnil) {
	button->setTitleText(RSTRING_PTR(title));
    }

    return rb_class_wrap_new(button, rcv);
}

/// @group Properties

/// @property #text
/// @return [String] title of the button.

static VALUE
button_text(VALUE rcv, SEL sel)
{
    return RSTRING_NEW(BUTTON(rcv)->getTitleText().c_str());
}

static VALUE
button_text_set(VALUE rcv, SEL sel, VALUE val)
{
    BUTTON(rcv)->setTitleText(RSTRING_PTR(val));
    return val;
}

/// @property #text_color
/// @return [Color] color of the title.

static VALUE
button_text_color(VALUE rcv, SEL sel)
{
    return rb_cccolor3_to_obj(BUTTON(rcv)->getTitleColor());
}

static VALUE
button_text_color_set(VALUE rcv, SEL sel, VALUE val)
{
    BUTTON(rcv)->setTitleColor(rb_any_to_cccolor3(val));
    return val;
}

/// @property #font
/// @return [String] name of the font used for the button title.

static VALUE
button_font(VALUE rcv, SEL sel)
{
    return RSTRING_NEW(BUTTON(rcv)->getTitleText().c_str());
}

static VALUE
button_font_set(VALUE rcv, SEL sel, VALUE val)
{
    BUTTON(rcv)->setTitleText(RSTRING_PTR(val));
    return val;
}

/// @property #font_size
/// @return [Integer] size of the font used by for button title.

static VALUE
button_font_size(VALUE rcv, SEL sel)
{
    return LONG2NUM(BUTTON(rcv)->getTitleFontSize());
}

static VALUE
button_font_size_set(VALUE rcv, SEL sel, VALUE val)
{
    BUTTON(rcv)->setTitleFontSize(NUM2LONG(val));
    return val;
}

/// @class UI::Slider < UI::Widget

/// @group Constructors

static VALUE rb_cUISlider = Qnil;

#define SLIDER(obj) _COCOS_WRAP_GET(obj, cocos2d::ui::Slider)

/// @method #initialize
/// Creates a new Slider widget.

static VALUE
slider_new(VALUE rcv, SEL sel)
{
    return rb_class_wrap_new(cocos2d::ui::Slider::create(), rcv);
}

/// @group Properties

/// @property #progress
/// @return [Integer] the progress direction of the slider, as a percentage
///   value from +1+ to +100+.

static VALUE
slider_progress(VALUE rcv, SEL sel)
{
    return LONG2NUM(SLIDER(rcv)->getPercent());
}

static VALUE
slider_progress_set(VALUE rcv, SEL sel, VALUE val)
{
    SLIDER(rcv)->setPercent(NUM2LONG(val));
    return val;
}

/// @class UI::Scroll < UI::Widget

/// @group Constructors

static VALUE rb_cUIScroll = Qnil;

#define SCROLL(obj) _COCOS_WRAP_GET(obj, cocos2d::ui::ScrollView)

/// @method #initialize
/// Creates a new Scroll widget.

static VALUE
scroll_new(VALUE rcv, SEL sel)
{
    return rb_class_wrap_new(cocos2d::ui::ScrollView::create(), rcv);
}

/// @group Properties

/// @property #direction
/// @return [:none, :vertical, :horizontal, :both] the direction of the scroll
///   view.

static VALUE sym_none = Qnil, sym_vertical = Qnil, sym_horizontal = Qnil,
	     sym_both = Qnil;

static VALUE
scroll_direction(VALUE rcv, SEL sel)
{
    switch (SCROLL(rcv)->getDirection()) {
	case cocos2d::ui::ScrollView::Direction::NONE:
	    return sym_none;
	case cocos2d::ui::ScrollView::Direction::VERTICAL:
	    return sym_vertical;
	case cocos2d::ui::ScrollView::Direction::HORIZONTAL:
	    return sym_horizontal;
	case cocos2d::ui::ScrollView::Direction::BOTH:
	    return sym_both;
	default:
	    abort();
    }
}

static VALUE
scroll_direction_set(VALUE rcv, SEL sel, VALUE val)
{
    cocos2d::ui::ScrollView::Direction dir;
    if (val == sym_none) {
	dir = cocos2d::ui::ScrollView::Direction::NONE;
    }
    else if (val == sym_vertical) {
	dir = cocos2d::ui::ScrollView::Direction::VERTICAL;
    }
    else if (val == sym_horizontal) {
	dir = cocos2d::ui::ScrollView::Direction::HORIZONTAL;
    }
    else if (val == sym_both) {
	dir = cocos2d::ui::ScrollView::Direction::BOTH;
    }
    else {
	rb_raise(rb_eArgError,
		"expected :none, :vertical, :horizontal or :both symbols");
    }
    SCROLL(rcv)->setDirection(dir);
    return val;
}

/// @property #inner_size
/// @return [Size] the inner container size of the scroll view, which must be
///    larger or equal than the size of the scroll view itself.

static VALUE
scroll_inner_size(VALUE rcv, SEL sel)
{
    return rb_ccsize_to_obj(SCROLL(rcv)->getInnerContainerSize());
}

static VALUE
scroll_inner_size_set(VALUE rcv, SEL sel, VALUE val)
{
    SCROLL(rcv)->setInnerContainerSize(rb_any_to_ccsize(val));
    return val;
}

/// @class UI::List < UI::Scroll

/// @group Constructors

static VALUE rb_cUIList = Qnil;

#define LIST(obj) _COCOS_WRAP_GET(obj, cocos2d::ui::ListView)

/// @method #initialize
/// Creates a new List widget.

static VALUE
list_new(VALUE rcv, SEL sel)
{
    return rb_class_wrap_new(cocos2d::ui::ListView::create(), rcv);
}

extern "C"
void
Init_UI(void)
{
    rb_mUI = rb_define_module_under(rb_mMC, "UI");

    rb_cUIWidget = rb_define_class_under(rb_mUI, "Widget", rb_cNode);

    rb_define_method(rb_cUIWidget, "enabled?", widget_enabled, 0);
    rb_define_method(rb_cUIWidget, "enabled=", widget_enabled_set, 1);
    rb_define_method(rb_cUIWidget, "highlighted?", widget_highlighted, 0);
    rb_define_method(rb_cUIWidget, "highlighted=", widget_highlighted_set, 1);
    rb_define_method(rb_cUIWidget, "on_touch", widget_on_touch, 0);

    rb_cUIText = rb_define_class_under(rb_mUI, "Text", rb_cUIWidget);

    rb_define_singleton_method(rb_cUIText, "new", text_new, -1);
    rb_define_method(rb_cUIText, "text", text_text, 0);
    rb_define_method(rb_cUIText, "text=", text_text_set, 1);
    rb_define_method(rb_cUIText, "text_color", text_text_color, 0);
    rb_define_method(rb_cUIText, "text_color=", text_text_color_set, 1);
    rb_define_method(rb_cUIText, "font", text_font, 0);
    rb_define_method(rb_cUIText, "font=", text_font_set, 1);
    rb_define_method(rb_cUIText, "font_size", text_font_size, 0);
    rb_define_method(rb_cUIText, "font_size=", text_font_size_set, 1);
    rb_define_method(rb_cUIText, "area_size", text_area_size, 0);
    rb_define_method(rb_cUIText, "area_size=", text_area_size_set, 1);
    rb_define_method(rb_cUIText, "vertical_align", text_vertical_align, 0);
    rb_define_method(rb_cUIText, "vertical_align=",
	    text_vertical_align_set, 1);
    rb_define_method(rb_cUIText, "horizontal_align", text_horizontal_align, 0);
    rb_define_method(rb_cUIText, "horizontal_align=",
	    text_horizontal_align_set, 1);

    sym_top = rb_name2sym("top");
    sym_center = rb_name2sym("center");
    sym_bottom = rb_name2sym("bottom");
    sym_left = rb_name2sym("left");
    sym_right = rb_name2sym("right");

    rb_cUIButton = rb_define_class_under(rb_mUI, "Button", rb_cUIWidget);

    rb_define_singleton_method(rb_cUIButton, "new", button_new, -1);
    rb_define_method(rb_cUIButton, "text", button_text, 0);
    rb_define_method(rb_cUIButton, "text=", button_text_set, 1);
    rb_define_method(rb_cUIButton, "text_color", button_text_color, 0);
    rb_define_method(rb_cUIButton, "text_color=", button_text_color_set, 1);
    rb_define_method(rb_cUIButton, "font", button_font, 0);
    rb_define_method(rb_cUIButton, "font=", button_font_set, 1);
    rb_define_method(rb_cUIButton, "font_size", button_font_size, 0);
    rb_define_method(rb_cUIButton, "font_size=", button_font_size_set, 1);

    rb_cUISlider = rb_define_class_under(rb_mUI, "Slider", rb_cUIWidget);

    rb_define_singleton_method(rb_cUISlider, "new", slider_new, 0);
    rb_define_method(rb_cUISlider, "progress", slider_progress, 0);
    rb_define_method(rb_cUISlider, "progress=", slider_progress_set, 1);

    rb_cUIScroll = rb_define_class_under(rb_mUI, "Scroll", rb_cUIWidget);

    rb_define_singleton_method(rb_cUIScroll, "new", scroll_new, 0);
    rb_define_method(rb_cUIScroll, "direction", scroll_direction, 0);
    rb_define_method(rb_cUIScroll, "direction=", scroll_direction_set, 1);
    rb_define_method(rb_cUIScroll, "inner_size", scroll_inner_size, 0);
    rb_define_method(rb_cUIScroll, "inner_size=", scroll_inner_size_set, 1);

    sym_none = rb_name2sym("none");
    sym_vertical = rb_name2sym("vertical");
    sym_horizontal = rb_name2sym("horizontal");
    sym_both = rb_name2sym("both");

    rb_cUIList = rb_define_class_under(rb_mUI, "List", rb_cUIScroll);
    rb_define_singleton_method(rb_cUIList, "new", list_new, 0);

}
