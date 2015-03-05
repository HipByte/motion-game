#include "rubymotion.h"

/// @class Node < Object

VALUE rb_cNode = Qnil;
static VALUE rb_cParallaxNode = Qnil;
static VALUE rb_mEvents = Qnil;
static VALUE rb_cAcceleration = Qnil;
static VALUE rb_cTouch = Qnil;

/// @group Properties

/// @property #anchor_point
/// The anchor point of the node, as a set of percentage coordinates.
/// The anchor point represents where the node will be attached to its parent,
/// and is normalized as a percentage. +[0, 0]+ means the bottom-left corner,
/// and +[1, 1]+ the top-right corner. You can also use values lower than +0+
/// and higher than +1+. The default anchor point value is +[0.5, 0.5]+, which
/// means the center of the parent.
/// @return [Point] the anchor point of the node.

static VALUE
node_anchor_point(VALUE rcv, SEL sel)
{
    return rb_ccvec2_to_obj(NODE(rcv)->getAnchorPoint());
}

static VALUE
node_anchor_point_set(VALUE rcv, SEL sel, VALUE val)
{
    NODE(rcv)->setAnchorPoint(rb_any_to_ccvec2(val));
    return val;
}

/// @property #position
/// @return [Point] the +[x, y]+ position of the node in its parent's
///   coordinate system.

static VALUE
node_position(VALUE rcv, SEL sel)
{
    return rb_ccvec2_to_obj(NODE(rcv)->getPosition());
}

static VALUE
node_position_set(VALUE rcv, SEL sel, VALUE val)
{
    NODE(rcv)->setPosition(rb_any_to_ccvec2(val));
    return val;
}

/// @property #size
/// @return [Size] the content size of the node.

static VALUE
node_size(VALUE rcv, SEL sel)
{
    return rb_ccsize_to_obj(NODE(rcv)->getContentSize());
}

static VALUE
node_size_set(VALUE rcv, SEL sel, VALUE val)
{
    NODE(rcv)->setContentSize(rb_any_to_ccsize(val));
    return val;
}

/// @property #visible?
/// @return [Boolean] whether the node should be visible. The default value is
///   true.

static VALUE
node_visible_set(VALUE rcv, SEL sel, VALUE val)
{
    NODE(rcv)->setVisible(RTEST(val));
    return val;
}

static VALUE
node_visible(VALUE rcv, SEL sel)
{
    return NODE(rcv)->isVisible() ? Qtrue : Qfalse;
}

/// @property #opacity
/// @return [Float] the opacity level of the node, as a Float from the
///   +0.0+ to +1.0+ range.

static VALUE
node_opacity(VALUE rcv, SEL sel)
{
    return BYTE2NUM(NODE(rcv)->getOpacity());
}

static VALUE
node_opacity_set(VALUE rcv, SEL sel, VALUE val)
{
    NODE(rcv)->setOpacity(NUM2BYTE(val));
    return val;
}

/// @property #color
/// @return [Color] the color of the node.

static VALUE
node_color(VALUE rcv, SEL sel)
{
    return rb_cccolor3_to_obj(NODE(rcv)->getColor());
}

static VALUE
node_color_set(VALUE rcv, SEL sel, VALUE val)
{
    NODE(rcv)->setColor(rb_any_to_cccolor3(val));
    return val;
}

/// @group Miscellaneous

/// @method #intersects?(node)
/// @param node [Node] a given Node object.
/// @return [Boolean] whether the receiver's bounding box intersects with the given node's bounding box.

static VALUE
node_intersects(VALUE rcv, SEL sel, VALUE node)
{
    return NODE(rcv)->getBoundingBox().intersectsRect(
	    NODE(node)->getBoundingBox()) ? Qtrue : Qfalse;
}

/// @group Events

/// @method #listen(event)
/// Starts listening for the given +event+ on the receiver, and yields the
/// given block when it happens.
/// @param event [Symbol] the event to listen to.
///   Possible events are:
///   - +:touch_begin+:  when a touch event begins. An {Events::Touch} object
///     will be passed to the block.
///   - +:accelerate+: when an accelerometer event happens. An
///     {Events::Acceleration} object will be passed to the block.
/// @yield [*arg] the given block will be called when the given event is
///   received. The block might receive arguments depending of the event.
/// @return [Node] the receiver.

static VALUE
node_listen(VALUE rcv, SEL sel, VALUE event)
{
    VALUE block = rb_current_block();
    if (block == Qnil) {
	rb_raise(rb_eArgError, "block not given");
    }
    block = rb_retain(block); // FIXME need release...

    cocos2d::EventListener *listener = NULL;
    const char *event_str = rb_sym2name(event);

    if (strcmp(event_str, "touch_begin") == 0) {
	auto touch_listener = cocos2d::EventListenerTouchOneByOne::create();
	touch_listener->onTouchBegan = [block](cocos2d::Touch *touch,
		cocos2d::Event *event) -> bool {
	    VALUE touch_obj = rb_class_wrap_new((void *)touch,
		    rb_cTouch);
	    return RTEST(rb_block_call(block, 1, &touch_obj));
	};
	listener = touch_listener;
    }
    else if (strcmp(event_str, "accelerate") == 0) {
	cocos2d::Device::setAccelerometerEnabled(true);
	listener = cocos2d::EventListenerAcceleration::create(
		[block](cocos2d::Acceleration *acc, cocos2d::Event *event) {
		VALUE acc_obj = rb_class_wrap_new((void *)acc,
			rb_cAcceleration);
		rb_block_call(block, 1, &acc_obj);
	    });
    }
    else {
	rb_raise(rb_eArgError, "invalid event name");
    }

    NODE(rcv)->getEventDispatcher()->addEventListenerWithSceneGraphPriority(
	    listener, NODE(rcv));
    return rcv;
}

/// @group Update Loop

/// @method #start_update
/// Starts the update loop. The +#update+ method will be called on this object
/// for every frame.
/// @return [Node] the receiver.

static VALUE
node_start_update(VALUE rcv, SEL sel)
{
    NODE(rcv)->scheduleUpdate();
    return rcv;
}

/// @method #stop_update
/// Stops the update loop. The +#update+ method will no longer be called on
/// this object.
/// @return [Node] the receiver.

static VALUE
node_stop_update(VALUE rcv, SEL sel)
{
    NODE(rcv)->unscheduleUpdate();
    return rcv;
}

/// @method #update
/// The update loop method. Subclasses can provide a custom implementation of
/// this method. The default implementation is empty.
/// @return [Node] the receiver.

static VALUE
node_update(VALUE rcv, SEL sel)
{
    // Do nothing.
    return rcv;
}

/// @group Actions

static VALUE
run_action(VALUE rcv, cocos2d::FiniteTimeAction *action)
{
    VALUE block = rb_current_block();
    if (block != Qnil) {
	block = rb_retain(block); // FIXME need release...
	auto call_funcn =
	    cocos2d::CallFuncN::create([block](cocos2d::Node *node) {
		rb_block_call(block, 0, NULL);
	    });
	action = cocos2d::Sequence::create(action, call_funcn, (void *)0);
    }
    NODE(rcv)->runAction(action);
    return rcv;
}

/// @method #move_by(delta_location, interval)
/// Moves the position of the receiver to a new location determined by the
/// sum of the current location and the given +delta_location+ object.
/// @param delta_location [Point] a point that will be added to the receiver's
///   current location.
/// @param interval [Float] the animation interval.
/// @return [Node] the receiver.

static VALUE
node_move_by(VALUE rcv, SEL sel, VALUE position, VALUE interval)
{
    return run_action(rcv, cocos2d::MoveBy::create(NUM2DBL(interval),
		rb_any_to_ccvec2(position)));
}

/// @method #blink(number_of_blinks, interval)
/// Blinks the receiver.
/// @param number_of_blinks [Integer] the number of times the receiver should
///   blink.
/// @param interval [Float] the animation interval.
/// @return [Node] the receiver.

static VALUE
node_blink(VALUE rcv, SEL sel, VALUE blinks, VALUE interval)
{
    return run_action(rcv, cocos2d::Blink::create(NUM2DBL(interval),
		NUM2INT(blinks)));
}

/// @group Container

/// @method #add(node, zpos=0)
/// Adds a child node to the receiver with a local z-order.
/// @param node [Node] the child to add.
/// @param zpos [Integer] the local z-order.
/// @return [Node] the receiver.

static VALUE
node_add(VALUE rcv, SEL sel, int argc, VALUE *argv)
{
    VALUE child = Qnil;
    VALUE zpos = Qnil;
    rb_scan_args(argc, argv, "11", &child, &zpos);

    if (zpos == Qnil) {
	NODE(rcv)->addChild(NODE(child));
    }
    else {
	NODE(rcv)->addChild(NODE(child), NUM2LONG(zpos));
    }
    return rcv;
}


/// @method #clear(cleanup=true)
/// Removes all children nodes from the receiver.
/// @param cleanup [Boolean] cleans all running actions on children before
///   removing them.
/// @return [Node] the receiver.

static VALUE
node_clear(VALUE rcv, SEL sel, int argc, VALUE *argv)
{
    VALUE cleanup = Qnil;
    rb_scan_args(argc, argv, "01", &cleanup);

    NODE(rcv)->removeAllChildrenWithCleanup(RTEST(cleanup));
    return rcv;
}

/// @method #delete(node, cleanup=true)
/// Removes the given child node from the receiver.
/// @param cleanup [Boolean] cleans all running actions on child before
///   removing it.
/// @return [Node] the receiver.

static VALUE
node_delete(VALUE rcv, SEL sel, int argc, VALUE *argv)
{
    VALUE node = Qnil, cleanup = Qnil;
    rb_scan_args(argc, argv, "11", &node, &cleanup);

    NODE(rcv)->removeChild(NODE(node), RTEST(cleanup));
    return rcv;
}

/// @class Parallax < Node

#define PNODE(obj) _COCOS_WRAP_GET(obj, cocos2d::ParallaxNode)

static VALUE
pnode_alloc(VALUE rcv, SEL sel)
{
    cocos2d::ParallaxNode *pnode = cocos2d::ParallaxNode::create();
    return rb_class_wrap_new((void *)pnode, rcv);
}

/// @method #add(node, zpos, parallax_ratio, position_offset)
/// Adds the given Node object to the receiver and configure it in the
/// parallax view.
/// @param node [Node] the node to add to the receiver.
/// @param zpos [Integer] the local z-order.
/// @param parallax_ratio [Point]
/// @param position_offset [Point]

static VALUE
pnode_add(VALUE rcv, SEL sel, VALUE child, VALUE z, VALUE parallax_ratio,
	VALUE position_offset)
{
    PNODE(rcv)->addChild(NODE(child), NUM2INT(z),
	    rb_any_to_ccvec2(parallax_ratio),
	    rb_any_to_ccvec2(position_offset));
    return child;
}

/// @class Events::Acceleration < Object

#define ACC(obj) _COCOS_WRAP_GET(obj, cocos2d::Acceleration)

/// @group Properties

/// @property-readonly #x
/// @return [Float] the x coordinate of the acceleration event.

static VALUE
acc_x(VALUE rcv, SEL sel)
{
    return DBL2NUM(ACC(rcv)->x);
}

/// @property-readonly #y
/// @return [Float] the y coordinate of the acceleration event.

static VALUE
acc_y(VALUE rcv, SEL sel)
{
    return DBL2NUM(ACC(rcv)->y);
}

/// @property-readonly #z
/// @return [Float] the z coordinate of the acceleration event.

static VALUE
acc_z(VALUE rcv, SEL sel)
{
    return DBL2NUM(ACC(rcv)->z);
}

/// @property-readonly #timestamp
/// @return [Float] the timestamp of the acceleration event.

static VALUE
acc_timestamp(VALUE rcv, SEL sel)
{
    return DBL2NUM(ACC(rcv)->timestamp);
}

/// @class Events::Touch < Object

#define TOUCH(obj) _COCOS_WRAP_GET(obj, cocos2d::Touch)

/// @group Properties

/// @property-readonly #location
/// @return [Point] the current location of the touch event.

static VALUE
touch_location(VALUE rcv, SEL sel)
{
    return rb_ccvec2_to_obj(TOUCH(rcv)->getLocation());
}

extern "C"
void
Init_Node(void)
{
    rb_cNode = rb_define_class_under(rb_mMC, "Node", rb_cObject);

    rb_define_method(rb_cNode, "anchor_point", node_anchor_point, 0);
    rb_define_method(rb_cNode, "anchor_point=", node_anchor_point_set, 1);
    rb_define_method(rb_cNode, "position", node_position, 0);
    rb_define_method(rb_cNode, "position=", node_position_set, 1);
    rb_define_method(rb_cNode, "size", node_size, 0);
    rb_define_method(rb_cNode, "size=", node_size_set, 0);
    rb_define_method(rb_cNode, "opacity", node_opacity, 0);
    rb_define_method(rb_cNode, "opacity=", node_opacity_set, 0);
    rb_define_method(rb_cNode, "color", node_color, 0);
    rb_define_method(rb_cNode, "color=", node_color_set, 0);
    rb_define_method(rb_cNode, "add", node_add, -1);
    rb_define_method(rb_cNode, "start_update", node_start_update, 0);
    rb_define_method(rb_cNode, "stop_update", node_stop_update, 0);
    rb_define_method(rb_cNode, "update", node_update, 0);
    rb_define_method(rb_cNode, "listen", node_listen, 1);
    rb_define_method(rb_cNode, "visible=", node_visible_set, 1);
    rb_define_method(rb_cNode, "visible?", node_visible, 0);
    rb_define_method(rb_cNode, "intersects?", node_intersects, 1);
    rb_define_method(rb_cNode, "move_by", node_move_by, 2);
    rb_define_method(rb_cNode, "blink", node_blink, 2);
    rb_define_method(rb_cNode, "clear", node_clear, -1);
    rb_define_method(rb_cNode, "delete", node_delete, -1);

    rb_cParallaxNode = rb_define_class_under(rb_mMC, "Parallax", rb_cNode);

    rb_define_singleton_method(rb_cParallaxNode, "alloc", pnode_alloc, 0);
    rb_define_method(rb_cParallaxNode, "add", pnode_add, 4);

    rb_mEvents = rb_define_module_under(rb_mMC, "Events");

    rb_cAcceleration = rb_define_class_under(rb_mEvents, "Acceleration",
	    rb_cObject);

    rb_define_method(rb_cAcceleration, "x", acc_x, 0);
    rb_define_method(rb_cAcceleration, "y", acc_y, 0);
    rb_define_method(rb_cAcceleration, "z", acc_z, 0);
    rb_define_method(rb_cAcceleration, "timestamp", acc_timestamp, 0);

    rb_cTouch = rb_define_class_under(rb_mEvents, "Touch",
	    rb_cObject);

    rb_define_method(rb_cTouch, "location", touch_location, 0);
}
