#include "rubymotion.h"
#include "motion-game.h"

#include <ui/UIWidget.h>
#include <ui/UIText.h>
#include <ui/UIButton.h>
#include <ui/UISlider.h>
#include <ui/UIScrollView.h>
#include <ui/UIListView.h>
#include <ui/UIWebView.h>

static VALUE sym_absolute = Qnil, sym_relative = Qnil, sym_none = Qnil,
	     sym_vertical = Qnil, sym_horizontal = Qnil, sym_both = Qnil;

/// @class Widget < Node
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

/// @property #touch_enabled?
/// Whether the widget is touch enabled. When touch enabled, a widget supports
/// on_touch. By default, a widget is not touch enabled.
/// @return [Boolean] whether the widget is touch enabled.

static VALUE
widget_touch_enabled(VALUE rcv, SEL sel)
{
    return WIDGET(rcv)->isTouchEnabled() ? Qtrue : Qfalse;
}

static VALUE
widget_touch_enabled_set(VALUE rcv, SEL sel, VALUE val)
{
    WIDGET(rcv)->setTouchEnabled(RTEST(val));
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
/// @yield [Symbol] the given block will be called when the event
///   is received with a +Symbol+ that describes the type of event, which can
///   be +:begin+, +:move+, +:end+ or +:cancel+.
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
		VALUE sym = Qnil;
		switch (event_type) {
		case cocos2d::ui::Widget::TouchEventType::BEGAN:
		    sym = rb_name2sym("begin");
		    break;
		case cocos2d::ui::Widget::TouchEventType::MOVED:
		    sym = rb_name2sym("move");
		    break;
		case cocos2d::ui::Widget::TouchEventType::ENDED:
		    sym = rb_name2sym("end");
		    break;
		case cocos2d::ui::Widget::TouchEventType::CANCELED:
		    sym = rb_name2sym("cancel");
		    break;
		}
		rb_block_call(block, 1, &sym);
	    });
    return rcv;
}

#if 0
static VALUE
widget_size(VALUE rcv, SEL sel)
{
    return rb_ccsize_to_obj(WIDGET(rcv)->getContentSize());
}

static VALUE
widget_size_set(VALUE rcv, SEL sel, VALUE val)
{
    WIDGET(rcv)->setContentSize(rb_any_to_ccsize(val));
    return val;
}
#endif

/// @class Text < Widget

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
	text->setString(RSTRING_PTR(StringValue(str)));
    }
    if (font_name != Qnil) {
	text->setFontName(RSTRING_PTR(StringValue(font_name)));
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
    TEXT(rcv)->setString(RSTRING_PTR(StringValue(val)));
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
    TEXT(rcv)->setFontName(RSTRING_PTR(StringValue(val)));
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

/// @class Button < Widget
/// A button widget. The {#on_touch} method can be used to set a callback when
/// the button is activated. Example:
///   button = Button.new("Touch me!")
///   button.on_touch { |type| puts "touched!" if type == :end }

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
	button->setTitleText(RSTRING_PTR(StringValue(title)));
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
    BUTTON(rcv)->setTitleText(RSTRING_PTR(StringValue(val)));
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
    return RSTRING_NEW(BUTTON(rcv)->getTitleFontName().c_str());
}

static VALUE
button_font_set(VALUE rcv, SEL sel, VALUE val)
{
    BUTTON(rcv)->setTitleFontName(RSTRING_PTR(StringValue(val)));
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

/// @property #zoom_scale
/// @return [Float] the value with which the button will zoom when the user
///   presses it.

static VALUE
button_zoom_scale(VALUE rcv, SEL sel)
{
    return DBL2NUM(BUTTON(rcv)->getZoomScale());
}

static VALUE
button_zoom_scale_set(VALUE rcv, SEL sel, VALUE val)
{
    BUTTON(rcv)->setZoomScale(NUM2DBL(val));
    return val;
}

static VALUE
button_load_texture_normal(VALUE rcv, SEL sel, VALUE val)
{
  BUTTON(rcv)->loadTextureNormal(RSTRING_PTR(StringValue(val)));
  return val;
}

static VALUE
button_load_texture_pressed(VALUE rcv, SEL sel, VALUE val)
{
  BUTTON(rcv)->loadTexturePressed(RSTRING_PTR(StringValue(val)));
  return val;
}

static VALUE
button_load_texture_disabled(VALUE rcv, SEL sel, VALUE val)
{
  BUTTON(rcv)->loadTextureDisabled(RSTRING_PTR(StringValue(val)));
  return val;
}

/// @class Slider < Widget

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

/// @class Layout < Widget

/// @group Constructors

static VALUE rb_cUILayout = Qnil;

#define LAYOUT(obj) _COCOS_WRAP_GET(obj, cocos2d::ui::Layout)

/// @method #initialize
/// Creates a new List widget.

static VALUE
layout_new(VALUE rcv, SEL sel)
{
    return rb_class_wrap_new(cocos2d::ui::Layout::create(), rcv);
}

/// @group Properties

/// @property #type
/// @return [:absolute, :vertical, :horizontal, :relative] the layout type.

static VALUE
layout_type(VALUE rcv, SEL sel)
{
    switch (LAYOUT(rcv)->getLayoutType()) {
	case cocos2d::ui::Layout::Type::ABSOLUTE:
	    return sym_absolute;
	case cocos2d::ui::Layout::Type::VERTICAL:
	    return sym_vertical;
	case cocos2d::ui::Layout::Type::HORIZONTAL:
	    return sym_horizontal;
	case cocos2d::ui::Layout::Type::RELATIVE:
	    return sym_relative;
	default:
	    abort();
    }
    return rcv;
}

static VALUE
layout_type_set(VALUE rcv, SEL sel, VALUE arg)
{
    if (arg == sym_absolute) {
	LAYOUT(rcv)->setLayoutType(cocos2d::ui::Layout::Type::ABSOLUTE);
    }
    else if (arg == sym_vertical) {
	LAYOUT(rcv)->setLayoutType(cocos2d::ui::Layout::Type::VERTICAL);
    }
    else if (arg == sym_horizontal) {
	LAYOUT(rcv)->setLayoutType(cocos2d::ui::Layout::Type::HORIZONTAL);
    }
    else if (arg == sym_relative) {
	LAYOUT(rcv)->setLayoutType(cocos2d::ui::Layout::Type::RELATIVE);
    }
    else {
	rb_raise(rb_eArgError, "expected :absolute, :vertical, :horizontal "\
		"or :relative symbols");
    }
    return arg;
}

/// @property #background_color
/// @return [Color] the background color of the widget.

static VALUE
layout_background_color(VALUE rcv, SEL sel)
{
    return rb_cccolor3_to_obj(LAYOUT(rcv)->getBackGroundColor());
}

static VALUE
layout_background_color_set(VALUE rcv, SEL sel, VALUE color)
{
    auto layout = LAYOUT(rcv);
    layout->setBackGroundColorType(
	    cocos2d::ui::Layout::BackGroundColorType::SOLID);
    layout->setBackGroundColor(rb_any_to_cccolor3(color));
    return color;
}

/// @property #clipping?
/// @return [Boolean] whether the layout can clip its content and children
///   (default is false).

static VALUE
layout_clipping(VALUE rcv, SEL sel)
{
    return LAYOUT(rcv)->isClippingEnabled() ? Qtrue : Qfalse;
}

static VALUE
layout_clipping_set(VALUE rcv, SEL sel, VALUE arg)
{
    LAYOUT(rcv)->setClippingEnabled(RTEST(arg));
    return arg;
}

static VALUE
layout_add(VALUE rcv, SEL sel, VALUE widget)
{
    LAYOUT(rcv)->addChild(WIDGET(widget));
    return widget;
}

/// @class Scroll < Layout

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

/// @property-readonly #inner_container
/// @return [Layout] the inner container of the scroll view.

static VALUE
scroll_inner_container(VALUE rcv, SEL sel)
{
    auto layout = SCROLL(rcv)->getInnerContainer();
    return layout == NULL
	? Qnil : rb_class_wrap_new((void *)layout, rb_cUILayout);
}

/// @class List < Scroll

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

/// @group Managing Items

/// @method #add_item(widget)
/// Adds a new item to the end of the list.
/// @param widget [Widget] the item to add.
/// @return [List] the receiver.

static VALUE
list_add_item(VALUE rcv, SEL sel, VALUE widget)
{
    auto w = WIDGET(widget);
    w->setTouchEnabled(true);
    LIST(rcv)->pushBackCustomItem(w);
    return rcv;
}

/// @method #insert_item(index, widget)
/// Inserts a new item at the given index in the list.
/// @param index [Integer] the index where to add the item.
/// @param widget [Widget] the item to add.
/// @return [List] the receiver.

static VALUE
list_insert_item(VALUE rcv, SEL sel, VALUE index, VALUE widget)
{
    auto w = WIDGET(widget);
    w->setTouchEnabled(true);
    LIST(rcv)->insertCustomItem(w, NUM2LONG(index));
    return rcv;
}

/// @method #item_at(index)
/// Retrieves the item at the given index.
/// @param index [Integer] the index to look up.
/// @return [Widget] the item at the given index, or +nil+ if there isn't any.

static VALUE
list_item_at(VALUE rcv, SEL sel, VALUE index)
{
    // TODO
    return Qnil;
}

/// @method #delete_item(index)
/// Deletes the item at the given index.
/// @param index [Integer] the index to look up.
/// @return [List] the receiver.

static VALUE
list_delete_item(VALUE rcv, SEL sel, VALUE index)
{
    LIST(rcv)->removeItem(NUM2LONG(index));
    return rcv;
}

/// @method #clear_items
/// Removes all items in the list.
/// @return [List] the receiver.

static VALUE
list_clear_items(VALUE rcv, SEL sel)
{
    LIST(rcv)->removeAllItems();
    return rcv;
}

/// @group Selection

/// @method #on_selection
/// Configures a block to be called when an item is selected in the list view.
/// @yield [Integer] the given block will be called when an item is selected,
///   passing the index of the selection as the argument.
/// @return [List] the receiver.

static VALUE
list_on_selection(VALUE rcv, SEL sel)
{
    VALUE block = rb_current_block();
    if (block == Qnil) {
	rb_raise(rb_eArgError, "block not given");
    }
    block = rb_retain(block); // FIXME need release...

    auto list = LIST(rcv);
    list->addEventListener(
	    [block, list](cocos2d::Ref *ref,
		    cocos2d::ui::ListView::EventType event_type) {
		if (event_type ==
			cocos2d::ui::ListView::EventType::\
			ON_SELECTED_ITEM_END) {
		    VALUE index = LONG2NUM(list->getCurSelectedIndex());
		    rb_block_call(block, 1, &index);
		}
	    });
    return rcv;
}

/// @method #selected_item
/// @return [Integer] the index of the currently selected item.

static VALUE
list_selected_item(VALUE rcv, SEL sel)
{
    return LONG2NUM(LIST(rcv)->getCurSelectedIndex());
}

/// @group Properties

/// @property #items_margin
/// @return [Float] the margin between items in the list.

static VALUE
list_items_margin(VALUE rcv, SEL sel)
{
    return DBL2NUM(LIST(rcv)->getItemsMargin());
}

static VALUE
list_items_margin_set(VALUE rcv, SEL sel, VALUE margin)
{
    LIST(rcv)->setItemsMargin(NUM2DBL(margin));
    return margin;
}

#if !CC_TARGET_OS_APPLETV

/// @class Web < Widget

/// @group Constructors

static VALUE rb_cUIWeb = Qnil;

#define WEB(obj) _COCOS_WRAP_GET(obj, cocos2d::experimental::ui::WebView)

/// @method #initialize
/// Creates a new Web widget.

static VALUE
web_new(VALUE rcv, SEL sel)
{
    return rb_class_wrap_new(cocos2d::experimental::ui::WebView::create(), rcv);
}

/// @group Loading Data

/// @method #load_html(str, baseurl)
/// Loads a given HTML data into the widget.
/// @param str [String] the HTML string to load.
/// @param baseurl [String] the base URL for the content.
/// @return [Web] the receiver.

static VALUE
web_load_html(VALUE rcv, SEL sel, VALUE str, VALUE baseurl)
{
    WEB(rcv)->loadHTMLString(RSTRING_PTR(StringValue(str)), RSTRING_PTR(StringValue(baseurl)));
    return rcv;
}

/// @method #load_url(url)
/// Loads a given URL into the widget.
/// @param url [String] the URL to load.
/// @return [Web] the receiver.

static VALUE
web_load_url(VALUE rcv, SEL sel, VALUE str)
{
    WEB(rcv)->loadURL(RSTRING_PTR(StringValue(str)));
    return rcv;
}

/// @method #load_file(path)
/// Loads a given file into the widget.
/// @param path [String] the file to load.
/// @return [Web] the receiver.

static VALUE
web_load_file(VALUE rcv, SEL sel, VALUE str)
{
    WEB(rcv)->loadFile(RSTRING_PTR(StringValue(str)));
    return rcv;
}

/// @method #stop
/// Stops the current loading.
/// @return [Web] the receiver.

static VALUE
web_stop(VALUE rcv, SEL sel)
{
    WEB(rcv)->stopLoading();
    return rcv;
}

/// @method #reload
/// Reloads the current context.
/// @return [Web] the receiver.

static VALUE
web_reload(VALUE rcv, SEL sel)
{
    WEB(rcv)->reload();
    return rcv;
}

/// @group JavaScript Interface

/// @method #evaluate(expr)
/// Evaluates the given JavaScript expression.
/// @param expr [String] a JavaScript expression to evaluate.
/// @return [Web] the receiver.

static VALUE
web_evaluate(VALUE rcv, SEL sel, VALUE expr)
{
    WEB(rcv)->evaluateJS(RSTRING_PTR(StringValue(expr)));
    return rcv;
}
#endif

extern "C"
void
Init_UI(void)
{
    rb_cUIWidget = rb_define_class_under(rb_mMC, "Widget", rb_cNode);

    rb_define_method(rb_cUIWidget, "enabled?", widget_enabled, 0);
    rb_define_method(rb_cUIWidget, "enabled=", widget_enabled_set, 1);
    rb_define_method(rb_cUIWidget, "touch_enabled?", widget_touch_enabled, 0);
    rb_define_method(rb_cUIWidget, "touch_enabled=", widget_touch_enabled_set, 1);
    rb_define_method(rb_cUIWidget, "highlighted?", widget_highlighted, 0);
    rb_define_method(rb_cUIWidget, "highlighted=", widget_highlighted_set, 1);
    //rb_define_method(rb_cUIWidget, "size", widget_size, 0);
    //rb_define_method(rb_cUIWidget, "size=", widget_size_set, 1);
    rb_define_method(rb_cUIWidget, "on_touch", widget_on_touch, 0);

    rb_cUIText = rb_define_class_under(rb_mMC, "Text", rb_cUIWidget);

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

    rb_cUIButton = rb_define_class_under(rb_mMC, "Button", rb_cUIWidget);

    rb_define_singleton_method(rb_cUIButton, "new", button_new, -1);
    rb_define_method(rb_cUIButton, "text", button_text, 0);
    rb_define_method(rb_cUIButton, "text=", button_text_set, 1);
    rb_define_method(rb_cUIButton, "text_color", button_text_color, 0);
    rb_define_method(rb_cUIButton, "text_color=", button_text_color_set, 1);
    rb_define_method(rb_cUIButton, "font", button_font, 0);
    rb_define_method(rb_cUIButton, "font=", button_font_set, 1);
    rb_define_method(rb_cUIButton, "font_size", button_font_size, 0);
    rb_define_method(rb_cUIButton, "font_size=", button_font_size_set, 1);
    rb_define_method(rb_cUIButton, "zoom_scale", button_zoom_scale, 0);
    rb_define_method(rb_cUIButton, "zoom_scale=", button_zoom_scale_set, 1);
    rb_define_method(rb_cUIButton, "load_texture_normal", button_load_texture_normal, 1);
    rb_define_method(rb_cUIButton, "load_texture_pressed", button_load_texture_pressed, 1);
    rb_define_method(rb_cUIButton, "load_texture_disabled", button_load_texture_disabled, 1);

    rb_cUISlider = rb_define_class_under(rb_mMC, "Slider", rb_cUIWidget);

    rb_define_singleton_method(rb_cUISlider, "new", slider_new, 0);
    rb_define_method(rb_cUISlider, "progress", slider_progress, 0);
    rb_define_method(rb_cUISlider, "progress=", slider_progress_set, 1);

    rb_cUILayout = rb_define_class_under(rb_mMC, "Layout", rb_cUIWidget);

    rb_define_singleton_method(rb_cUILayout, "new", layout_new, 0);
    rb_define_method(rb_cUILayout, "type", layout_type, 0);
    rb_define_method(rb_cUILayout, "type=", layout_type_set, 1);
    rb_define_method(rb_cUILayout, "background_color",
	    layout_background_color, 0);
    rb_define_method(rb_cUILayout, "background_color=",
	    layout_background_color_set, 1);
    rb_define_method(rb_cUILayout, "clipping=", layout_clipping_set, 1);
    rb_define_method(rb_cUILayout, "clipping?", layout_clipping, 0);
    rb_define_method(rb_cUILayout, "add", layout_add, 1);

    sym_absolute = rb_name2sym("absolute");
    sym_relative = rb_name2sym("relative");

    rb_cUIScroll = rb_define_class_under(rb_mMC, "Scroll", rb_cUILayout);

    rb_define_singleton_method(rb_cUIScroll, "new", scroll_new, 0);
    rb_define_method(rb_cUIScroll, "direction", scroll_direction, 0);
    rb_define_method(rb_cUIScroll, "direction=", scroll_direction_set, 1);
    rb_define_method(rb_cUIScroll, "inner_size", scroll_inner_size, 0);
    rb_define_method(rb_cUIScroll, "inner_size=", scroll_inner_size_set, 1);
    rb_define_method(rb_cUIScroll, "inner_container",
	    scroll_inner_container, 0);

    sym_none = rb_name2sym("none");
    sym_vertical = rb_name2sym("vertical");
    sym_horizontal = rb_name2sym("horizontal");
    sym_both = rb_name2sym("both");

    rb_cUIList = rb_define_class_under(rb_mMC, "List", rb_cUIScroll);

    rb_define_singleton_method(rb_cUIList, "new", list_new, 0);
    rb_define_method(rb_cUIList, "add_item", list_add_item, 1);
    rb_define_method(rb_cUIList, "insert_item", list_insert_item, 2);
    rb_define_method(rb_cUIList, "item_at", list_item_at, 1);
    rb_define_method(rb_cUIList, "delete_item", list_delete_item, 1);
    rb_define_method(rb_cUIList, "clear_items", list_clear_items, 0);
    rb_define_method(rb_cUIList, "on_selection", list_on_selection, 0);
    rb_define_method(rb_cUIList, "selected_item", list_selected_item, 0);
    rb_define_method(rb_cUIList, "items_margin", list_items_margin, 0);
    rb_define_method(rb_cUIList, "items_margin=", list_items_margin_set, 1);

#if !CC_TARGET_OS_APPLETV
    rb_cUIWeb = rb_define_class_under(rb_mMC, "Web", rb_cUIWidget);

    rb_define_singleton_method(rb_cUIWeb, "new", web_new, 0);
    rb_define_method(rb_cUIWeb, "load_html", web_load_html, 2);
    rb_define_method(rb_cUIWeb, "load_url", web_load_url, 1);
    rb_define_method(rb_cUIWeb, "load_file", web_load_file, 1);
    rb_define_method(rb_cUIWeb, "stop", web_stop, 0);
    rb_define_method(rb_cUIWeb, "reload", web_reload, 0);
    rb_define_method(rb_cUIWeb, "evaluate", web_reload, 1);
#endif
}
