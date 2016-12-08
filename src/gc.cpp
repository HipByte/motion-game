#include "rubymotion.h"
#include "motion-game.h"

VALUE
rb_cocos2d_object_new(cocos2d::Ref *ptr, VALUE klass)
{
    ptr->retain();
    return rb_class_wrap_new(ptr, klass);
}

#if CC_TARGET_OS_IPHONE || CC_TARGET_OS_APPLETV
#define FINALIZER_SELECTOR "dealloc"

static IMP
rb_objc_method_swizzling(Class klass, SEL sel, IMP new_imp)
{
    Method method = class_getInstanceMethod(klass, sel);
    if (method == NULL) {
	abort();
    }
    IMP old_imp = method_getImplementation(method);
    class_replaceMethod(klass, sel, new_imp, method_getTypeEncoding(method));
    return old_imp;
}

class Finalizer {
  private:
    SEL sel_finalize;
    std::map<VALUE, void*> super;

  public:
    Finalizer() {
	sel_finalize = sel_registerName(FINALIZER_SELECTOR);
    }

    void* super_finalizer(VALUE obj) {
	VALUE klass = RB_GET_CLASS((VALUE)obj);
	return (void*)super[klass];
    }

    void register_handler(VALUE klass, void *func) {
	IMP super_func = rb_objc_method_swizzling((Class)klass, sel_finalize, (IMP)func);
	super[klass] = (void*)super_func;
    }
};

static Finalizer m_Finalizer = Finalizer();

static void
cocos2d_object_handler(void *rcv, SEL sel)
{
    cocos2d::Ref *ptr = _COCOS_WRAP_GET(rcv, cocos2d::Ref);
    ptr->autorelease();

    IMP super_func = (IMP)m_Finalizer.super_finalizer((VALUE)rcv);
    if (super_func != NULL) {
	((void(*)(void *, SEL))super_func)(rcv, sel);
    }
}
#endif

void
rb_register_cocos2d_object_finalizer(VALUE klass)
{
#if CC_TARGET_OS_IPHONE || CC_TARGET_OS_APPLETV

    m_Finalizer.register_handler(klass, (void*)cocos2d_object_handler);

#elif CC_TARGET_OS_ANDROID

    // TODO

#endif
}

