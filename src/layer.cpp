#include "rubymotion.h"

/// @class Scene < Node
/// This class represents a scene, an independent screen or stage of the
/// application workflow. A scene is responsible for handling events from the
/// device, providing a physics world for the sprites, and also starting the
/// game loop.
/// An application must have at least one scene, and the +Scene+ class is
/// designed to be subclassed.

VALUE rb_cScene = Qnil;

class mc_Scene : public cocos2d::LayerColor {
    public:
	cocos2d::Scene *scene;
	VALUE obj;
	SEL update_sel;

    mc_Scene() {
	obj = Qnil;
#if CC_TARGET_OS_IPHONE
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

/// @group Events

static VALUE
scene_add_listener(VALUE rcv, cocos2d::EventListener *listener)
{
    auto scene = SCENE(rcv);
    scene->getEventDispatcher()->addEventListenerWithSceneGraphPriority(
	    listener, scene);
    return rcv;
}

/// @method #on_touch_begin
/// Starts listening for touch begin events on the receiver.
/// @yield [Events::Touch] the given block will be yield when a touch begin
///   event is received.
/// @return [Scene] the receiver.

static VALUE
scene_on_touch_begin(VALUE rcv, SEL sel)
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

    return scene_add_listener(rcv, listener);
}

/// @method #on_accelerate
/// Starts listening for accelerometer events on the receiver.
/// @yield [Events::Acceleration] the given block will be yield when an
///   accelerometer event is received from the device.
/// @return [Scene] the receiver.

static VALUE
scene_on_accelerate(VALUE rcv, SEL sel)
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

    return scene_add_listener(rcv, listener);
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
    listener->onContactBegin = [block](cocos2d::PhysicsContact &contact)
	    -> bool {
//	VALUE touch_obj = rb_class_wrap_new((void *)touch,
//		rb_cTouch);
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
    rb_define_method(rb_cScene, "on_touch_begin", scene_on_touch_begin, 0);
    rb_define_method(rb_cScene, "on_accelerate", scene_on_accelerate, 0);
    rb_define_method(rb_cScene, "on_contact_begin", scene_on_contact_begin, 0);
    rb_define_method(rb_cScene, "gravity", scene_gravity, 0);
    rb_define_method(rb_cScene, "gravity=", scene_gravity_set, 1);
    rb_define_method(rb_cScene, "debug_physics?", scene_debug_physics, 0);
    rb_define_method(rb_cScene, "debug_physics=", scene_debug_physics_set, 1);
    rb_define_method(rb_cScene, "color=", scene_color_set, 1);
}
