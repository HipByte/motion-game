#include "rubymotion.h"

/// @class Node < Object
/// Node is the base class of objects in the scene graph. You should not
/// instantiate this class directly but use a subclass instead.

VALUE rb_cNode = Qnil;
static VALUE rb_cParallaxNode = Qnil;

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

/// @method #parent
/// @return [Node] the parent node, or +nil+ if there isn't one.

static VALUE
node_parent(VALUE rcv, SEL sel)
{
    auto parent = NODE(rcv)->getParent();
    return parent == NULL ? Qnil : rb_class_wrap_new((void *)parent, rb_cNode);
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
	rb_ary_push(ary, rb_class_wrap_new((void *)vector.at(i), rb_cNode));
    }
    return ary;
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
    rb_define_method(rb_cNode, "size=", node_size_set, 1);
    rb_define_method(rb_cNode, "opacity", node_opacity, 0);
    rb_define_method(rb_cNode, "opacity=", node_opacity_set, 1);
    rb_define_method(rb_cNode, "z_index", node_z_index, 0);
    rb_define_method(rb_cNode, "z_index=", node_z_index_set, 1);
    rb_define_method(rb_cNode, "color", node_color, 0);
    rb_define_method(rb_cNode, "color=", node_color_set, 1);
    rb_define_method(rb_cNode, "add", node_add, -1);
    rb_define_method(rb_cNode, "visible=", node_visible_set, 1);
    rb_define_method(rb_cNode, "visible?", node_visible, 0);
    rb_define_method(rb_cNode, "intersects?", node_intersects, 1);
    rb_define_method(rb_cNode, "clear", node_clear, -1);
    rb_define_method(rb_cNode, "delete", node_delete, -1);
    rb_define_method(rb_cNode, "parent", node_parent, 0);
    rb_define_method(rb_cNode, "children", node_children, 0);

    rb_cParallaxNode = rb_define_class_under(rb_mMC, "Parallax", rb_cNode);

    rb_define_singleton_method(rb_cParallaxNode, "alloc", pnode_alloc, 0);
    rb_define_method(rb_cParallaxNode, "add", pnode_add, 4);
}
