#include "rubymotion.h"

/// @class Scene < Node

VALUE rb_cLayer = Qnil;

class mc_Layer : private cocos2d::Layer {
    public:
	VALUE obj;
	SEL update_sel;

    mc_Layer() {
	obj = Qnil;
#if CC_TARGET_OS_IPHONE
	update_sel = rb_selector("update:");
#else
	update_sel = rb_selector("update");
#endif
    }

    virtual void update(float delta) {
	VALUE arg = DBL2NUM(delta);
	rb_send(obj, update_sel, 1, &arg);
    }

    virtual bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event) {
	printf("onTouchBegan\n");
	return true;
    }
};

static VALUE
layer_alloc(VALUE rcv, SEL sel)
{
    mc_Layer *layer = new mc_Layer();
    VALUE obj = rb_class_wrap_new((void *)layer, rcv);
    layer->obj = rb_retain(obj);
    return obj;
}

/// @group Constructors

/// @method #initialize
/// The default initializer. Subclasses can construct the scene interface in
/// this method, as well as providing an implementation for {#update}, then
/// run the update loop by calling {#start_update}.
/// @return [Scene] the receiver.

static VALUE
layer_initialize(VALUE rcv, SEL sel)
{
    return rcv;
}

extern "C"
void
Init_Layer(void)
{
    rb_cLayer = rb_define_class_under(rb_mMC, "Scene", rb_cNode);

    rb_define_singleton_method(rb_cLayer, "alloc", layer_alloc, 0);
    rb_define_method(rb_cLayer, "initialize", layer_initialize, 0);
}
