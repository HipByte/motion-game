#include "rubymotion.h"

VALUE rb_cNode = Qnil;
static VALUE rb_cParallaxNode = Qnil;
static VALUE rb_cAcceleration = Qnil;

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
    return child;
}

static VALUE
node_start_update(VALUE rcv, SEL sel)
{
    NODE(rcv)->scheduleUpdate();
    return rcv;
}

static VALUE
node_stop_update(VALUE rcv, SEL sel)
{
    NODE(rcv)->unscheduleUpdate();
    return rcv;
}

static VALUE
node_listen(VALUE rcv, SEL sel, VALUE event)
{
    rb_vm_block_t *block = rb_vm_current_block();
    if (block == NULL) {
	rb_raise(rb_eArgError, "block not given");
    }
    rb_obj_retain(block); // FIXME need release...

    cocos2d::EventListener *listener = NULL;
    const char *event_str = rb_sym2name(event);

    if (strcmp(event_str, "on_touch_begin") == 0) {
	auto touch_listener = cocos2d::EventListenerTouchOneByOne::create();
	touch_listener->onTouchBegan = [block](cocos2d::Touch *touch,
		cocos2d::Event *event) -> bool {
	    return RTEST(rb_vm_block_eval(block, 0, NULL));
	};
	listener = touch_listener;
    }
    else if (strcmp(event_str, "accelerate") == 0) {
	cocos2d::Device::setAccelerometerEnabled(true);
	listener = cocos2d::EventListenerAcceleration::create(
		[block](cocos2d::Acceleration *acc, cocos2d::Event *event) {
		VALUE acc_obj = rb_class_wrap_new((void *)acc,
			rb_cAcceleration);
		rb_vm_block_eval(block, 1, &acc_obj);
	    });
    }
    else {
	rb_raise(rb_eArgError, "invalid event name");
    }

    NODE(rcv)->getEventDispatcher()->addEventListenerWithSceneGraphPriority(
	    listener, NODE(rcv));
    return rcv;
}

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

static VALUE
node_intersects(VALUE rcv, SEL sel, VALUE node)
{
    return NODE(rcv)->getBoundingBox().intersectsRect(
	    NODE(node)->getBoundingBox()) ? Qtrue : Qfalse;
}

static VALUE
run_action(VALUE rcv, cocos2d::FiniteTimeAction *action)
{
    rb_vm_block_t *block = rb_vm_current_block();
    if (block != NULL) {
	rb_obj_retain(block); // FIXME need release...
	action = cocos2d::Sequence::create(action,
		cocos2d::CallFuncN::create([block](cocos2d::Node *node) {
		    rb_vm_block_eval(block, 0, NULL);
		}), NULL);
    }
    NODE(rcv)->runAction(action);
    return rcv;
}

static VALUE
node_move_by(VALUE rcv, SEL sel, VALUE position, VALUE interval)
{
    return run_action(rcv, cocos2d::MoveBy::create(NUM2DBL(interval),
		rb_any_to_ccvec2(position)));
}

static VALUE
node_blink(VALUE rcv, SEL sel, VALUE blinks, VALUE interval)
{
    return run_action(rcv, cocos2d::Blink::create(NUM2DBL(interval),
		NUM2INT(blinks)));
}

#define PNODE(obj) _COCOS_WRAP_GET(obj, cocos2d::ParallaxNode)

static VALUE
pnode_alloc(VALUE rcv, SEL sel)
{
    cocos2d::ParallaxNode *pnode = cocos2d::ParallaxNode::create();
    return rb_class_wrap_new((void *)pnode, rcv);
}

static VALUE
pnode_add(VALUE rcv, SEL sel, VALUE child, VALUE z, VALUE parallax_ratio,
	VALUE position_offset)
{
    PNODE(rcv)->addChild(NODE(child), NUM2INT(z),
	    rb_any_to_ccvec2(parallax_ratio),
	    rb_any_to_ccvec2(position_offset));
    return child;
}

#define ACC(obj) _COCOS_WRAP_GET(obj, cocos2d::Acceleration)

static VALUE
acc_x(VALUE rcv, SEL sel)
{
    return DBL2NUM(ACC(rcv)->x);
}

static VALUE
acc_y(VALUE rcv, SEL sel)
{
    return DBL2NUM(ACC(rcv)->y);
}

static VALUE
acc_z(VALUE rcv, SEL sel)
{
    return DBL2NUM(ACC(rcv)->z);
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
    rb_define_method(rb_cNode, "add", node_add, -1);
    rb_define_method(rb_cNode, "start_update", node_start_update, 0);
    rb_define_method(rb_cNode, "stop_update", node_stop_update, 0);
    rb_define_method(rb_cNode, "listen", node_listen, 1);
    rb_define_method(rb_cNode, "visible=", node_visible_set, 1);
    rb_define_method(rb_cNode, "visible?", node_visible, 0);
    rb_define_method(rb_cNode, "intersects?", node_intersects, 1);
    rb_define_method(rb_cNode, "move_by", node_move_by, 2);
    rb_define_method(rb_cNode, "blink", node_blink, 2);

    rb_cParallaxNode = rb_define_class_under(rb_mMC, "ParallaxNode", rb_cNode);

    rb_define_singleton_method(rb_cParallaxNode, "alloc", pnode_alloc, 0);
    rb_define_method(rb_cParallaxNode, "add", pnode_add, 4);

    rb_cAcceleration = rb_define_class_under(rb_mMC, "Acceleration",
	    rb_cObject);

    rb_define_method(rb_cAcceleration, "x", acc_x, 0);
    rb_define_method(rb_cAcceleration, "y", acc_y, 0);
    rb_define_method(rb_cAcceleration, "z", acc_z, 0);
}
