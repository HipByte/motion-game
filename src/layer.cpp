#include "rubymotion.h"
#include "motion-game.h"
#include <dlfcn.h>

/// @class Scene < Node
/// This class represents a scene, an independent screen or stage of the
/// application workflow. A scene is responsible for handling events from the
/// device, providing a physics world for the sprites, and also starting the
/// game loop.
/// An application must have at least one scene, and the +Scene+ class is
/// designed to be subclassed.

VALUE rb_cScene = Qnil;

#if CC_TARGET_OS_IPHONE && TARGET_IPHONE_SIMULATOR
extern "C" {
    void rb_repl_new(VALUE);
}
#endif

enum mc_Scene_EventType {
    ON_BEGIN,
    ON_MOVE,
    ON_END,
    ON_CANCEL
};

class mc_Scene : public cocos2d::LayerColor {
    public:
	cocos2d::Scene *scene;
	VALUE obj;
	SEL update_sel;
    cocos2d::EventListenerTouchOneByOne *touch_listener;

    mc_Scene() {
	obj = Qnil;
	touch_listener = NULL;
#if CC_TARGET_OS_IPHONE || CC_TARGET_OS_APPLETV
	update_sel = rb_selector("update:");
#else
	update_sel = rb_selector("update");
#endif
    }

    static mc_Scene *create(void) {
	auto scene = new mc_Scene();
	scene->initWithColor(cocos2d::Color4B::BLACK);
	return scene;
    }

    virtual void update(float delta) {
	LayerColor::update(delta);
	VALUE arg = DBL2NUM(delta);
	rb_send(obj, update_sel, 1, &arg);
    }

    void setBackgroundColor(cocos2d::Color3B color) {
	setColor(color);
	updateColor();
    }

#if CC_TARGET_OS_IPHONE && TARGET_IPHONE_SIMULATOR
    virtual void onEnter() {
	cocos2d::LayerColor::onEnter();
	rb_repl_new(this->obj);
    }
#endif
};

#define SCENE(obj) _COCOS_WRAP_GET(obj, mc_Scene)

extern "C"
cocos2d::Scene *
rb_any_to_scene(VALUE obj)
{
    if (rb_obj_is_kind_of(obj, rb_cScene)) {
	return SCENE(obj)->scene;
    }
    rb_raise(rb_eArgError, "expected Scene object");
}

static VALUE
scene_alloc(VALUE rcv, SEL sel)
{
    auto layer = mc_Scene::create();

    auto scene = cocos2d::Scene::createWithPhysics();
    scene->addChild(layer);
    layer->scene = scene;

    VALUE obj = rb_cocos2d_object_new(layer, rcv);
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
scene_initialize(VALUE rcv, SEL sel)
{
    return rcv;
}

/// @group Update Loop

/// @method #start_update
/// Starts the update loop. The +#update+ method will be called on this object
/// for every frame.
/// @return [Scene] the receiver.

static VALUE
scene_start_update(VALUE rcv, SEL sel)
{
    SCENE(rcv)->scheduleUpdate();
    return rcv;
}

/// @method #stop_update
/// Stops the update loop. The +#update+ method will no longer be called on
/// this object.
/// @return [Scene] the receiver.

static VALUE
scene_stop_update(VALUE rcv, SEL sel)
{
    SCENE(rcv)->unscheduleUpdate();
    return rcv;
}

/// @method #update(delta)
/// The update loop method. Subclasses can provide a custom implementation of
/// this method. The default implementation is empty.
/// @param delta [Float] a value representing the amount of time, in seconds,
///   since the last time this method was called.
/// @return [Scene] the receiver.

static VALUE
scene_update(VALUE rcv, SEL sel, VALUE delta)
{
    // Do nothing.
    return rcv;
}

/// @method #schedule(delay, repeat=0, interval=0)
/// Schedules a given block for execution.
/// @param delay [Float] the duration of the block, in seconds.
/// @param repeat [Integer] the number of times the block should be repeated.
/// @param interval [Float] the interval between repetitions, in seconds.
/// @yield [Float] the given block will be yield with the delta value,
///   in seconds.
/// @return [String] a token representing the task that can be passed to
///   {#unschedule} when needed.

static VALUE
scene_schedule(VALUE rcv, SEL sel, int argc, VALUE *argv)
{
    VALUE block = rb_current_block();
    if (block == Qnil) {
	rb_raise(rb_eArgError, "block not given");
    }
    block = rb_retain(block); // FIXME need release...

    VALUE interval = Qnil, repeat = Qnil, delay = Qnil;
    rb_scan_args(argc, argv, "12", &delay, &repeat, &interval);

    float interval_c  = RTEST(interval) ? NUM2DBL(interval) : 0;
    unsigned int repeat_c = RTEST(repeat) ? NUM2LONG(repeat) : 0;
    float delay_c = NUM2DBL(delay);
    char key[100];
    snprintf(key, sizeof key, "schedule_lambda_%p", (void *)block);

    SCENE(rcv)->schedule([block](float delta) {
		VALUE delta_obj = DBL2NUM(delta);
		rb_block_call(block, 1, &delta_obj);
	    },
	    interval_c, repeat_c, delay_c, key);

    return RSTRING_NEW(key);
}

/// @method #unschedule(key)
/// Unschedules a task that's currently running.
/// @param key [String] a token representing the task to unschedule,
///   returned by {#schedule}.
/// @return [Scene] the receiver.

static VALUE
scene_unschedule(VALUE rcv, SEL sel, VALUE key)
{
    SCENE(rcv)->unschedule(RSTRING_PTR(StringValue(key)));
    return rcv;
}

/// @group Events

static VALUE
scene_add_listener(VALUE rcv, cocos2d::EventListener *listener)
{
    auto scene = SCENE(rcv);
    scene->getEventDispatcher()->addEventListenerWithSceneGraphPriority(
	    listener, scene);
    return rcv;
}

static VALUE
scene_on_touch_event(VALUE rcv, SEL sel, mc_Scene_EventType type)
{
    VALUE block = rb_current_block();
    if (block == Qnil) {
	rb_raise(rb_eArgError, "block not given");
    }
    block = rb_retain(block); // FIXME need release...

    auto scene = SCENE(rcv);
    if (scene->touch_listener == NULL) {
	scene->touch_listener = cocos2d::EventListenerTouchOneByOne::create();
    }
    else {
	scene->getEventDispatcher()->removeEventListener(scene->touch_listener);
    }
    auto lambda = [block](cocos2d::Touch *touch, cocos2d::Event *event) -> bool {
	VALUE touch_obj = rb_cocos2d_object_new(touch, rb_cTouch);
	return RTEST(rb_block_call(block, 1, &touch_obj));
    };

    switch (type) {
      case ON_BEGIN:
	scene->touch_listener->onTouchBegan = lambda;
	break;
      case ON_MOVE:
	scene->touch_listener->onTouchMoved = lambda;
	break;
      case ON_END:
	scene->touch_listener->onTouchEnded = lambda;
	break;
      case ON_CANCEL:
	scene->touch_listener->onTouchCancelled = lambda;
	break;
    }

    return scene_add_listener(rcv, scene->touch_listener);
}

/// @method #on_touch_begin
/// Starts listening for touch begin events on the receiver.
/// @yield [Events::Touch] the given block will be yield when a touch begin
///   event is received.
/// @return [Scene] the receiver.

static VALUE
scene_on_touch_begin(VALUE rcv, SEL sel)
{
    return scene_on_touch_event(rcv, sel, mc_Scene_EventType::ON_BEGIN);
}

/// @method #on_touch_end
/// Starts listening for touch end events on the receiver.
/// @yield [Events::Touch] the given block will be yield when a touch end
///   event is received.
/// @return [Scene] the receiver.

static VALUE
scene_on_touch_end(VALUE rcv, SEL sel)
{
    return scene_on_touch_event(rcv, sel, mc_Scene_EventType::ON_END);
}

/// @method #on_touch_move
/// Starts listening for touch move events on the receiver.
/// @yield [Events::Touch] the given block will be yield when a touch move
///   event is received.
/// @return [Scene] the receiver.

static VALUE
scene_on_touch_move(VALUE rcv, SEL sel)
{
    return scene_on_touch_event(rcv, sel, mc_Scene_EventType::ON_MOVE);
}

/// @method #on_touch_cancel
/// Starts listening for touch cancel events on the receiver.
/// @yield [Events::Touch] the given block will be yield when a touch cancel
///   event is received.
/// @return [Scene] the receiver.

static VALUE
scene_on_touch_cancel(VALUE rcv, SEL sel)
{
    return scene_on_touch_event(rcv, sel, mc_Scene_EventType::ON_CANCEL);
}
/// @method #on_accelerate
/// Starts listening for accelerometer events on the receiver.
/// @yield [Events::Acceleration] the given block will be yield when an
///   accelerometer event is received from the device.
/// @return [Scene] the receiver.

static VALUE
scene_on_accelerate(VALUE rcv, SEL sel)
{
#if CC_TARGET_OS_APPLETV
    rb_raise(rb_eRuntimeError, "Not supported in tvOS");
#else
    VALUE block = rb_current_block();
    if (block == Qnil) {
	rb_raise(rb_eArgError, "block not given");
    }
    block = rb_retain(block); // FIXME need release...

    cocos2d::Device::setAccelerometerEnabled(true);
    auto listener = cocos2d::EventListenerAcceleration::create(
	[block](cocos2d::Acceleration *acc, cocos2d::Event *event) {
	    VALUE acc_obj = rb_cocos2d_object_new(acc, rb_cAcceleration);
	    rb_block_call(block, 1, &acc_obj);
	});

    return scene_add_listener(rcv, listener);
#endif
}

/// @method #on_contact_begin
/// Starts listening for contact begin events from the physics engine.
/// @yield [Events::PhysicsContact] the given block will be yield when a
///   contact event is received from the physics engine.
/// @return [Scene] the receiver.

static VALUE
scene_on_contact_begin(VALUE rcv, SEL sel)
{
    VALUE block = rb_current_block();
    if (block == Qnil) {
	rb_raise(rb_eArgError, "block not given");
    }
    block = rb_retain(block); // FIXME need release...

    auto listener = cocos2d::EventListenerPhysicsContact::create();
    listener->onContactBegin = [block](cocos2d::PhysicsContact &contact) -> bool {
	return RTEST(rb_block_call(block, 0, NULL));
    };

    return scene_add_listener(rcv, listener);
}

/// @group Properties

/// @property #gravity
/// @return [Point] the gravity of the scene's physics world.

static VALUE
scene_gravity(VALUE rcv, SEL sel)
{
    return rb_ccvec2_to_obj(SCENE(rcv)->scene->getPhysicsWorld()->getGravity());
}

static VALUE
scene_gravity_set(VALUE rcv, SEL sel, VALUE arg)
{
    SCENE(rcv)->scene->getPhysicsWorld()->setGravity(rb_any_to_ccvec2(arg));
    return rcv;
}

/// @property #debug_physics?
/// @return [Boolean] whether the physics engine should draw debug lines.

static VALUE
scene_debug_physics(VALUE rcv, SEL sel)
{
    return SCENE(rcv)->scene->getPhysicsWorld()->getDebugDrawMask()
	== cocos2d::PhysicsWorld::DEBUGDRAW_NONE ? Qfalse : Qtrue;
}

static VALUE
scene_debug_physics_set(VALUE rcv, SEL sel, VALUE arg)
{
    SCENE(rcv)->scene->getPhysicsWorld()->setDebugDrawMask(RTEST(arg)
	    ? cocos2d::PhysicsWorld::DEBUGDRAW_ALL
	    : cocos2d::PhysicsWorld::DEBUGDRAW_NONE);
    return arg;
}

static VALUE
scene_color_set(VALUE rcv, SEL sel, VALUE val)
{
    SCENE(rcv)->setBackgroundColor(rb_any_to_cccolor3(val));
    return val;
}

extern "C"
void
Init_Layer(void)
{
    rb_cScene = rb_define_class_under(rb_mMC, "Scene", rb_cNode);

    rb_define_singleton_method(rb_cScene, "alloc", scene_alloc, 0);
    rb_define_method(rb_cScene, "initialize", scene_initialize, 0);
    rb_define_method(rb_cScene, "start_update", scene_start_update, 0);
    rb_define_method(rb_cScene, "stop_update", scene_stop_update, 0);
    rb_define_method(rb_cScene, "update", scene_update, 1);
    rb_define_method(rb_cScene, "schedule", scene_schedule, -1);
    rb_define_method(rb_cScene, "unschedule", scene_unschedule, 1);
    rb_define_method(rb_cScene, "on_touch_begin", scene_on_touch_begin, 0);
    rb_define_method(rb_cScene, "on_touch_end", scene_on_touch_end, 0);
    rb_define_method(rb_cScene, "on_touch_move", scene_on_touch_move, 0);
    rb_define_method(rb_cScene, "on_touch_cancel", scene_on_touch_cancel, 0);
    rb_define_method(rb_cScene, "on_accelerate", scene_on_accelerate, 0);
    rb_define_method(rb_cScene, "on_contact_begin", scene_on_contact_begin, 0);
    rb_define_method(rb_cScene, "gravity", scene_gravity, 0);
    rb_define_method(rb_cScene, "gravity=", scene_gravity_set, 1);
    rb_define_method(rb_cScene, "debug_physics?", scene_debug_physics, 0);
    rb_define_method(rb_cScene, "debug_physics=", scene_debug_physics_set, 1);
    rb_define_method(rb_cScene, "color=", scene_color_set, 1);
}
