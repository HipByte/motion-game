#include "rubymotion.h"
#include "motion-game.h"

/// @class Node < Object
/// Node is the base class of objects in the scene graph. You should not
/// instantiate this class directly but use a subclass instead.

VALUE rb_cNode = Qnil;
static VALUE rb_cParallaxNode = Qnil;
static VALUE rb_cDrawNode = Qnil;

static VALUE
node_alloc(VALUE rcv, SEL sel)
{
    return rb_cocos2d_object_new(cocos2d::Node::create(), rcv);
}

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

/// @method #visible=(value)
/// Set whether the node is visible.
/// @param value [Boolean] true if display the node.

static VALUE
node_visible_set(VALUE rcv, SEL sel, VALUE val)
{
    NODE(rcv)->setVisible(RTEST(val));
    return val;
}

/// @method #visible?
/// @return [Boolean] whether the node should be visible. The default value is
///   true.

static VALUE
node_visible(VALUE rcv, SEL sel)
{
    return NODE(rcv)->isVisible() ? Qtrue : Qfalse;
}

/// @property #alpha
/// @return [Float] the opacity (alpha) level of the node, as a Float from the
///   +0.0+ to +1.0+ range.

static VALUE
node_alpha(VALUE rcv, SEL sel)
{
    return BYTE2NUM(NODE(rcv)->getOpacity());
}

static VALUE
node_alpha_set(VALUE rcv, SEL sel, VALUE val)
{
    NODE(rcv)->setOpacity(NUM2BYTE(val));
    return val;
}

/// @property #z_index
/// @return [Integer] the local z-order index of the receiver in the scene
///   graph, which will determine its priority when rendering the scene.

static VALUE
node_z_index(VALUE rcv, SEL sel)
{
    return LONG2NUM(NODE(rcv)->getLocalZOrder());
}

static VALUE
node_z_index_set(VALUE rcv, SEL sel, VALUE val)
{
    NODE(rcv)->setLocalZOrder(NUM2LONG(val));
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

/// @property #rotation
/// Returns the rotation of the node in degrees. 0 is the default angle.
/// Positive values rotate node clockwise, and negative values for
/// anti-clockwise
/// @return [Float] the rotation of the node in degrees.

static VALUE
node_rotation(VALUE rcv, SEL sel)
{
    return DBL2NUM(NODE(rcv)->getRotation());
}

static VALUE
node_rotation_set(VALUE rcv, SEL sel, VALUE val)
{
    NODE(rcv)->setRotation(NUM2DBL(val));
    return val;
}

/// @property #scale
/// Returns the scaling factor of the node, which multiplies its width, height
/// and depth.
/// @return [Float] the scaling factor.

static VALUE
node_scale(VALUE rcv, SEL sel)
{
    return DBL2NUM(NODE(rcv)->getScale());
}

static VALUE
node_scale_set(VALUE rcv, SEL sel, VALUE val)
{
    NODE(rcv)->setScale(NUM2DBL(val));
    return val;
}

/// @property #name
/// @return [String] a name to easily identify the node in the graph.

static VALUE
node_name(VALUE rcv, SEL sel)
{
    return RSTRING_NEW(NODE(rcv)->getName().c_str());
}

static VALUE
node_name_set(VALUE rcv, SEL sel, VALUE val)
{
    NODE(rcv)->setName(RSTRING_PTR(StringValue(val)));
    return val;
}

/// @method #run_action(action)
/// Run the provided action on the receiver node.
/// @return [self] the receiver.
/// @yield if passed a block, the block will be called for the action.

static VALUE
node_run_action(VALUE rcv, SEL sel, VALUE action)
{
    VALUE block = rb_current_block();
    if (block != Qnil) {
	block = rb_retain(block); // FIXME need release...
	auto call_funcn =
	    cocos2d::CallFuncN::create(
		[block](cocos2d::Node *node) {
		    rb_block_call(block, 0, NULL);
		});
	NODE(rcv)->runAction(cocos2d::Sequence::create(FINITE_TIME_ACTION(action), call_funcn, (void *)0));
    }
    else {
	NODE(rcv)->runAction(ACTION(action));
    }

    return rcv;
}

/// @method #stop_all_actions()
/// Stop all actions running on the node
/// @return [self] the receiver.
static VALUE
node_stop_all_actions(VALUE rcv, SEL sel)
{
    NODE(rcv)->stopAllActions();
    return rcv;
}

/// @method #stop_action(action)
/// Stop the provided action
/// @param action [Action] the action to stop.
/// @return [self] the receiver.
static VALUE
node_stop_action(VALUE rcv, SEL sel, VALUE action)
{
    NODE(rcv)->stopAction(ACTION(action));
    return rcv;
}

/// @method #intersects?(node)
/// @param node [Node] a given Node object.
/// @return [Boolean] whether the receiver's bounding box intersects with the
///   given node's bounding box.

static VALUE
node_intersects(VALUE rcv, SEL sel, VALUE node)
{
    return NODE(rcv)->getBoundingBox().intersectsRect(
	NODE(node)->getBoundingBox()) ? Qtrue : Qfalse;
}

/// @group Container

/// @method #add(node, zpos=0)
/// Adds a child node to the receiver with a local z-order.
/// @param node [Node] the child to add.
/// @param zpos [Integer] the local z-order.
/// @return [self] the receiver.

static VALUE
node_add(VALUE rcv, SEL sel, int argc, VALUE *argv)
{
    VALUE child = Qnil;
    VALUE zpos = Qnil;
    rb_scan_args(argc, argv, "11", &child, &zpos);

    if (zpos == Qnil) {
	rb_add_relationship(rcv, child);
	NODE(rcv)->addChild(NODE(child));
    }
    else {
	rb_add_relationship(rcv, child);
	NODE(rcv)->addChild(NODE(child), NUM2LONG(zpos));
    }
    return rcv;
}


/// @method #clear(cleanup=true)
/// Removes all children nodes from the receiver.
/// @param cleanup [Boolean] cleans all running actions on children before
///   removing them.
/// @return [self] the receiver.

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
/// @return [self] the receiver.

static VALUE
node_delete(VALUE rcv, SEL sel, int argc, VALUE *argv)
{
    VALUE node = Qnil, cleanup = Qnil;
    rb_scan_args(argc, argv, "11", &node, &cleanup);

    NODE(rcv)->removeChild(NODE(node), RTEST(cleanup));
    return rcv;
}

/// @method #parent
/// @return [Node] the parent node, or +nil+ if there isn't one.

static VALUE
node_parent(VALUE rcv, SEL sel)
{
    auto parent = NODE(rcv)->getParent();
    return parent == NULL ? Qnil : rb_cocos2d_object_new(parent, rb_cNode);
}

/// @method #children
/// @return [Array<Node>] an array of +Node+ objects that have been added to
///   the receiver.

static VALUE
node_children(VALUE rcv, SEL sel)
{
    VALUE ary = rb_ary_new();
    auto vector = NODE(rcv)->getChildren();
    for (int i = 0, count = vector.size(); i < count; i++) {
	rb_ary_push(ary, rb_cocos2d_object_new(vector.at(i), rb_cNode));
    }
    return ary;
}

/// @method #delete_from_parent(cleanup=true)
/// Removes the receiver node from its parent.
/// Same as:
///   node.parent.delete(node, cleanup)
/// @param cleanup [Boolean] cleans all running actions on the receiver before
///   removing it from the parent.
/// @return [self] the receiver.

static VALUE
node_delete_from_parent(VALUE rcv, SEL sel, int argc, VALUE *argv)
{
    VALUE cleanup = Qnil;
    rb_scan_args(argc, argv, "01", &cleanup);

    NODE(rcv)->removeFromParentAndCleanup(RTEST(cleanup));
    return rcv;
}

/// @endgroup

/// @method #schedule(delay, repeat=0, interval=0)
/// Schedules a given block for execution.
/// @param delay [Float] the duration of the block, in seconds.
/// @param repeat [Integer] the number of times the block should be repeated.
///   If {Repeat::FOREVER} (or negative value directly) was given, the animation will loop forever.
/// @param interval [Float] the interval between repetitions, in seconds.
/// @yield [Float] the given block will be yield with the delta value,
///   in seconds.
/// @return [String] a token representing the task that can be passed to
///   {#unschedule} when needed.

static VALUE
node_schedule(VALUE rcv, SEL sel, int argc, VALUE *argv)
{
    VALUE block = rb_current_block();
    if (block == Qnil) {
	rb_raise(rb_eArgError, "block not given");
    }
    block = rb_retain(block); // FIXME need release...

    VALUE interval = Qnil, repeat = Qnil, delay = Qnil;
    rb_scan_args(argc, argv, "12", &delay, &repeat, &interval);

    float interval_c  = RTEST(interval) ? NUM2DBL(interval) : 0;
    int tmp_repeat_c = RTEST(repeat) ? NUM2LONG(repeat) : 0;
    unsigned int repeat_c = (tmp_repeat_c >= 0) ? tmp_repeat_c : kRepeatForever;
    float delay_c = NUM2DBL(delay);
    char key[100];
    snprintf(key, sizeof key, "schedule_lambda_%p", (void *)block);

    NODE(rcv)->schedule([block](float delta) {
		VALUE delta_obj = DBL2NUM(delta);
		rb_block_call(block, 1, &delta_obj);
	    },
	    interval_c, repeat_c, delay_c, key);

    return RSTRING_NEW(key);
}

/// @method #schedule_once(delay)
/// Schedules a given block for execution that runs only once, with a delay of 0 or larger.
/// @param delay [Float] the duration of the block, in seconds.
/// @yield [Float] the given block will be yield with the delta value,
///   in seconds.
/// @return [String] a token representing the task that can be passed to
///   {#unschedule} when needed.

static VALUE
node_schedule_once(VALUE rcv, SEL sel, VALUE delay)
{
    VALUE block = rb_current_block();
    if (block == Qnil) {
	rb_raise(rb_eArgError, "block not given");
    }
    block = rb_retain(block); // FIXME need release...

    float delay_c = NUM2DBL(delay);
    char key[100];
    snprintf(key, sizeof key, "schedule_once_lambda_%p", (void *)block);

    NODE(rcv)->scheduleOnce([block](float delta) {
		VALUE delta_obj = DBL2NUM(delta);
		rb_block_call(block, 1, &delta_obj);
	    },
	    delay_c, key);

    return RSTRING_NEW(key);
}

/// @method #unschedule(key)
/// Unschedules a task that's currently running.
/// @param key [String] a token representing the task to unschedule,
///   returned by {#schedule}.
/// @return [self] the receiver.

static VALUE
node_unschedule(VALUE rcv, SEL sel, VALUE key)
{
    NODE(rcv)->unschedule(RSTRING_PTR(StringValue(key)));
    return rcv;
}

/// @method #number_of_running_actions
/// @return [Integer] the number of running actions for the node.

static VALUE
node_number_of_running_actions(VALUE rcv, SEL sel)
{
    return SSIZET2NUM(NODE(rcv)->getNumberOfRunningActions());
}

/// @class Parallax < Node

#define PNODE(obj) _COCOS_WRAP_GET(obj, cocos2d::ParallaxNode)

static VALUE
pnode_alloc(VALUE rcv, SEL sel)
{
    cocos2d::ParallaxNode *pnode = cocos2d::ParallaxNode::create();
    return rb_cocos2d_object_new(pnode, rcv);
}

/// @method #add(node, zpos, parallax_ratio, position_offset)
/// Adds the given Node object to the receiver and configure it in the
/// parallax view.
/// @param node [Node] the node to add to the receiver.
/// @param zpos [Integer] the local z-order.
/// @param parallax_ratio [Point]
/// @param position_offset [Point]
/// @return [Node] the child node.

static VALUE
pnode_add(VALUE rcv, SEL sel, VALUE child, VALUE z, VALUE parallax_ratio,
	VALUE position_offset)
{
    rb_add_relationship(rcv, child);
    PNODE(rcv)->addChild(NODE(child), NUM2INT(z),
	    rb_any_to_ccvec2(parallax_ratio),
	    rb_any_to_ccvec2(position_offset));
    return child;
}

/// @class Draw < Node

#define DRAW(obj) _COCOS_WRAP_GET(obj, cocos2d::DrawNode)

static VALUE
draw_alloc(VALUE rcv, SEL sel)
{
    auto node = cocos2d::DrawNode::create();
    return rb_cocos2d_object_new(node, rcv);
}

/// @group Draw Operations

/// @method #clear
/// Clears drew shapes.
/// @return [self] the receiver.

static VALUE
draw_clear(VALUE rcv, SEL sel)
{
    DRAW(rcv)->clear();
    return rcv;
}

/// @method #dot(pos, radius, color)
/// Draws a filled circle at the given position with the given radius and color.
/// @param pos [Point] the position where to draw.
/// @param radius [Float] the radius of the circle to draw.
/// @param color [Color] the color to use to fill the circle.
/// @return [self] the receiver.

static VALUE
draw_dot(VALUE rcv, SEL sel, VALUE pos, VALUE radius, VALUE color)
{
    DRAW(rcv)->drawDot(rb_any_to_ccvec2(pos), NUM2DBL(radius),
	    cocos2d::Color4F(rb_any_to_cccolor4(color)));
    return rcv;
}

/// @method #rect(origin, destination, color, fill=false)
/// Draws a rectangle at the given position with the given color.
/// @param origin [Point] the position where to start drawing (lower-left).
/// @param destination [Point] the position where to end drawing (higher-right).
/// @param color [Color] the color to use to draw.
/// @param fill [Boolean] whether the rectangle should be filled up.
/// @return [self] the receiver.

static VALUE
draw_rect(VALUE rcv, SEL sel, int argc, VALUE *argv)
{
    VALUE origin = Qnil, destination = Qnil, color = Qnil, fill = Qnil;

    rb_scan_args(argc, argv, "31", &origin, &destination, &color, &fill);

    if (RTEST(fill)) {
	DRAW(rcv)->drawSolidRect(rb_any_to_ccvec2(origin),
		rb_any_to_ccvec2(destination),
		cocos2d::Color4F(rb_any_to_cccolor4(color)));
    }
    else {
	DRAW(rcv)->drawRect(rb_any_to_ccvec2(origin),
		rb_any_to_ccvec2(destination),
		cocos2d::Color4F(rb_any_to_cccolor4(color)));
    }
    return rcv;
}

/// @method #line(origin, destination, thickness, color)
/// Draws a line at the given position with the given color.
/// @param origin [Point] the position where to start drawing (lower-left).
/// @param destination [Point] the position where to end drawing (higher-right).
/// @param thickness [Float] the line thickness.
/// @param color [Color] the color to use to draw.
/// @return [self] the receiver.

static VALUE
draw_line(VALUE rcv, SEL sel, int argc, VALUE *argv)
{
    VALUE origin = Qnil, destination = Qnil, thickness = Qnil, color = Qnil;
    rb_scan_args(argc, argv, "4", &origin, &destination, &thickness, &color);
    DRAW(rcv)->drawSegment(rb_any_to_ccvec2(origin),
	    rb_any_to_ccvec2(destination),
	    NUM2DBL(thickness),
	    cocos2d::Color4F(rb_any_to_cccolor4(color)));
    return rcv;
}

/// @method #triangle(position1, position2, position3, color)
/// Draws a triangle at the given positions with the given color.
/// @param position1 [Point] The triangle vertex point.
/// @param position2 [Point] The triangle vertex point.
/// @param position3 [Point] The triangle vertex point.
/// @param color [Color] the color to use to draw.
/// @return [self] the receiver.

static VALUE
draw_triangle(VALUE rcv, SEL sel, int argc, VALUE *argv)
{
    VALUE position1 = Qnil, position2 = Qnil, position3 = Qnil, color = Qnil;
    rb_scan_args(argc, argv, "4", &position1, &position2, &position3, &color);
    DRAW(rcv)->drawTriangle(rb_any_to_ccvec2(position1),
	    rb_any_to_ccvec2(position2),
	    rb_any_to_ccvec2(position3),
	    cocos2d::Color4F(rb_any_to_cccolor4(color)));
    return rcv;
}

extern "C"
void
Init_Node(void)
{
    rb_cNode = rb_define_class_under(rb_mMC, "Node", rb_cObject);
    // Register finalizer in rb_cNode only for node.cpp because other classes inherit rb_cNode
    rb_register_cocos2d_object_finalizer(rb_cNode);

    rb_define_singleton_method(rb_cNode, "alloc", node_alloc, 0);
    rb_define_method(rb_cNode, "anchor_point", node_anchor_point, 0);
    rb_define_method(rb_cNode, "anchor_point=", node_anchor_point_set, 1);
    rb_define_method(rb_cNode, "position", node_position, 0);
    rb_define_method(rb_cNode, "position=", node_position_set, 1);
    rb_define_method(rb_cNode, "size", node_size, 0);
    rb_define_method(rb_cNode, "size=", node_size_set, 1);
    rb_define_method(rb_cNode, "alpha", node_alpha, 0);
    rb_define_method(rb_cNode, "alpha=", node_alpha_set, 1);
    rb_define_method(rb_cNode, "z_index", node_z_index, 0);
    rb_define_method(rb_cNode, "z_index=", node_z_index_set, 1);
    rb_define_method(rb_cNode, "color", node_color, 0);
    rb_define_method(rb_cNode, "color=", node_color_set, 1);
    rb_define_method(rb_cNode, "rotation", node_rotation, 0);
    rb_define_method(rb_cNode, "rotation=", node_rotation_set, 1);
    rb_define_method(rb_cNode, "scale", node_scale, 0);
    rb_define_method(rb_cNode, "scale=", node_scale_set, 1);
    rb_define_method(rb_cNode, "name", node_name, 0);
    rb_define_method(rb_cNode, "name=", node_name_set, 1);
    rb_define_method(rb_cNode, "add", node_add, -1);
    rb_define_method(rb_cNode, "visible=", node_visible_set, 1);
    rb_define_method(rb_cNode, "visible?", node_visible, 0);
    rb_define_method(rb_cNode, "intersects?", node_intersects, 1);
    rb_define_method(rb_cNode, "clear", node_clear, -1);
    rb_define_method(rb_cNode, "delete", node_delete, -1);
    rb_define_method(rb_cNode, "parent", node_parent, 0);
    rb_define_method(rb_cNode, "children", node_children, 0);
    rb_define_method(rb_cNode, "delete_from_parent", node_delete_from_parent, -1);
    rb_define_method(rb_cNode, "run_action", node_run_action, 1);
    rb_define_method(rb_cNode, "stop_all_actions", node_stop_all_actions, 0);
    rb_define_method(rb_cNode, "schedule", node_schedule, -1);
    rb_define_method(rb_cNode, "schedule_once", node_schedule_once, 1);
    rb_define_method(rb_cNode, "unschedule", node_unschedule, 1);
    rb_define_method(rb_cNode, "number_of_running_actions", node_number_of_running_actions, 0);

    rb_cParallaxNode = rb_define_class_under(rb_mMC, "Parallax", rb_cNode);

    rb_define_singleton_method(rb_cParallaxNode, "alloc", pnode_alloc, 0);
    rb_define_method(rb_cParallaxNode, "add", pnode_add, 4);

    rb_cDrawNode = rb_define_class_under(rb_mMC, "Draw", rb_cNode);

    rb_define_singleton_method(rb_cDrawNode, "alloc", draw_alloc, 0);
    rb_define_method(rb_cDrawNode, "clear", draw_clear, 0);
    rb_define_method(rb_cDrawNode, "dot", draw_dot, 3);
    rb_define_method(rb_cDrawNode, "rect", draw_rect, -1);
    rb_define_method(rb_cDrawNode, "line", draw_line, -1);
    rb_define_method(rb_cDrawNode, "triangle", draw_triangle, -1);
}
