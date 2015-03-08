#include "rubymotion.h"

/// @class Scene < Node
/// This class represents a scene, an independent screen or stage of the
/// application workflow. A scene is responsible for handling events from the
/// device and also starting the game loop.
/// An application must have at least one scene, and the +Scene+ class is
/// designed to be subclassed.

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

/// @group Update Loop

/// @method #start_update
/// Starts the update loop. The +#update+ method will be called on this object
/// for every frame.
/// @return [Scene] the receiver.

static VALUE
layer_start_update(VALUE rcv, SEL sel)
{
    LAYER(rcv)->scheduleUpdate();
    return rcv;
}

/// @method #stop_update
/// Stops the update loop. The +#update+ method will no longer be called on
/// this object.
/// @return [Scene] the receiver.

static VALUE
layer_stop_update(VALUE rcv, SEL sel)
{
    LAYER(rcv)->unscheduleUpdate();
    return rcv;
}

/// @method #update(delta)
/// The update loop method. Subclasses can provide a custom implementation of
/// this method. The default implementation is empty.
/// @param delta [Float] a value representing the amount of time, in seconds,
///   since the last time this method was called.
/// @return [Scene] the receiver.

static VALUE
layer_update(VALUE rcv, SEL sel, VALUE delta)
{
    // Do nothing.
    return rcv;
}

/// @group Events

/// @method #on_touch_begin
/// Starts listening for touch begin events on the receiver.
/// @yield [Events::Touch] the given block will be yield when a touch begin
///   event is received.
/// @return [Scene] the receiver.

static VALUE
layer_on_touch_begin(VALUE rcv, SEL sel)
{
    VALUE block = rb_current_block();
    if (block == Qnil) {
	rb_raise(rb_eArgError, "block not given");
    }
    block = rb_retain(block); // FIXME need release...

    auto listener = cocos2d::EventListenerTouchOneByOne::create();
    listener->onTouchBegan = [block](cocos2d::Touch *touch,
		cocos2d::Event *event) -> bool {
	VALUE touch_obj = rb_class_wrap_new((void *)touch,
		rb_cTouch);
	return RTEST(rb_block_call(block, 1, &touch_obj));
    };

    LAYER(rcv)->getEventDispatcher()->addEventListenerWithSceneGraphPriority(
	    listener, LAYER(rcv));
    return rcv;
}

/// @method #on_accelerate
/// Starts listening for accelerometer events on the receiver.
/// @yield [Events::Acceleration] the given block will be yield when an
///   accelerometer event is received from the device.
/// @return [Scene] the receiver.

static VALUE
layer_on_accelerate(VALUE rcv, SEL sel)
{
    VALUE block = rb_current_block();
    if (block == Qnil) {
	rb_raise(rb_eArgError, "block not given");
    }
    block = rb_retain(block); // FIXME need release...

    cocos2d::Device::setAccelerometerEnabled(true);
    auto listener = cocos2d::EventListenerAcceleration::create(
	    [block](cocos2d::Acceleration *acc, cocos2d::Event *event) {
	    VALUE acc_obj = rb_class_wrap_new((void *)acc,
		    rb_cAcceleration);
	    rb_block_call(block, 1, &acc_obj);
	});

    LAYER(rcv)->getEventDispatcher()->addEventListenerWithSceneGraphPriority(
	    listener, LAYER(rcv));
    return rcv;
}

extern "C"
void
Init_Layer(void)
{
    rb_cLayer = rb_define_class_under(rb_mMC, "Scene", rb_cNode);

    rb_define_singleton_method(rb_cLayer, "alloc", layer_alloc, 0);
    rb_define_method(rb_cLayer, "initialize", layer_initialize, 0);
    rb_define_method(rb_cLayer, "start_update", layer_start_update, 0);
    rb_define_method(rb_cLayer, "stop_update", layer_stop_update, 0);
    rb_define_method(rb_cLayer, "update", layer_update, 1);
    rb_define_method(rb_cLayer, "on_touch_begin", layer_on_touch_begin, 0);
    rb_define_method(rb_cLayer, "on_accelerate", layer_on_accelerate, 0);
}
