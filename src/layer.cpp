#include "rubymotion.h"

VALUE rb_cLayer = Qnil;

class mc_Layer : private cocos2d::Layer {
    public:
	VALUE obj;

    mc_Layer() {
	obj = Qnil;
    }

    virtual void update(float delta) {
	VALUE arg = DBL2NUM(delta);
	rb_send(obj, rb_selector("update:"), 1, &arg);
    }
};

static VALUE
layer_alloc(VALUE rcv, SEL sel)
{
    mc_Layer *layer = new mc_Layer();
    VALUE obj = rb_class_wrap_new((void *)layer, rcv);
    layer->obj = obj;
    return obj;
}

static VALUE
layer_initialize(VALUE rcv, SEL sel)
{
    //LAYER(rcv)->init();
    return rcv;
}

extern "C"
void
Init_Layer(void)
{
    rb_cLayer = rb_define_class_under(rb_mMC, "Layer", rb_cNode);

    rb_define_singleton_method(rb_cLayer, "alloc", layer_alloc, 0);
    rb_define_method(rb_cLayer, "initialize", layer_initialize, 0);
}
