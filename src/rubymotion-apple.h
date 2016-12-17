#include <objc/runtime.h>
#include <objc/message.h>

void DBGLog(const char *format, ... );
#define debug_log DBGLog

long rb_ary_len(VALUE);
VALUE rb_ary_elt(VALUE, long);
#define RARRAY_LEN(a) (rb_ary_len((VALUE)a))
#define RARRAY_AT(a,i) (rb_ary_elt((VALUE)a, (long)i))

double rb_num2dbl(VALUE);
VALUE rb_float_new(double);
#define NUM2DBL(x) rb_num2dbl((VALUE)(x))
#define DBL2NUM(x) rb_float_new(x)

#define FIXNUM_FLAG	0x01
#define IMMEDIATE_MASK	0x03
#define FIXNUM_P(f) ((((SIGNED_VALUE)(f)) & IMMEDIATE_MASK) == FIXNUM_FLAG)
long rb_num2long(VALUE);
#define FIX2LONG(x) (x >> 2)
#define NUM2LONG(x) (FIXNUM_P(x) ? FIX2LONG(x) : rb_num2long((VALUE)x))
VALUE rb_uint2big(VALUE);
#define LONG2FIX(i) ((VALUE)(((SIGNED_VALUE)(i))<<2 | FIXNUM_FLAG))
#define FIXNUM_MAX (LONG_MAX>>2)
#define POSFIXABLE(f) (((VALUE)f) < FIXNUM_MAX+1)
#define INT2NUM(x) (POSFIXABLE(x) ? LONG2FIX(x) : rb_uint2big(x))
#define LONG2NUM(x) INT2NUM(x)
long rb_num2int(VALUE);
#define FIX2INT(x) ((int)FIX2LONG(x))
#define NUM2INT(x) (FIXNUM_P(x) ? FIX2INT(x) : rb_num2int((VALUE)x))

const char *rb_str_cstr(VALUE);
VALUE rb_string_value(VALUE*);
#define RSTRING_PTR(str) (rb_str_cstr((VALUE)str))
#define StringValue(v) rb_string_value(&(v))

VALUE rb_obj_is_kind_of(VALUE, VALUE);

void rb_objc_define_method(VALUE klass, const char *name, void *imp,
	const int arity);
#define rb_define_method(klass, name, imp, arity) \
    rb_objc_define_method(klass, name, (void *)imp, arity)

#define RB_GET_CLASS(obj) (VALUE)object_getClass((id)obj)
#define rb_define_singleton_method(klass, name, imp, arity) \
    rb_define_method(RB_GET_CLASS(klass), name, imp, arity)

VALUE rb_define_module(const char *);
VALUE rb_define_module_under(VALUE, const char *);

#define rb_selector(str) sel_registerName(str)

VALUE rb_vm_call_s(VALUE self, SEL sel, int argc, const VALUE *argv);
#define rb_send(rcv, sel, argc, argv) rb_vm_call_s(rcv, sel, argc, argv)

struct rb_class_ptr {
    VALUE reserved; // MUST NOT use.
    VALUE flags;
    void *ptr;
};

static inline
VALUE
rb_class_wrap_new(void *ptr, VALUE klass)
{
#define T_NATIVE 0x16
    struct rb_class_ptr *obj =
	(struct rb_class_ptr *)class_createInstance((Class)klass,
		sizeof(struct rb_class_ptr));
    assert(obj != NULL);
    obj->flags = T_NATIVE;
    obj->ptr = ptr;
    objc_msgSend((id)obj, sel_registerName("autorelease"));
    return (VALUE)obj;
}

#define rb_class_wrap_get_ptr(obj) \
    ((struct rb_class_ptr *)obj)->ptr

void *rb_objc_retain(void *addr);
#define rb_retain(obj) \
    ({ \
	VALUE _obj = (VALUE)obj; \
	rb_objc_retain((void *)_obj); \
	_obj; \
    })

void *rb_objc_release(void *addr);
// TODO: should send `autorelease` instead of `release` for stability.
#define rb_release(obj) \
    ({ \
	VALUE _obj = (VALUE)obj; \
	rb_objc_release((void *)_obj); \
	_obj; \
    })

void *rb_vm_current_block(void);
#define rb_current_block() \
    ({ \
	VALUE _b = (VALUE)rb_vm_current_block(); \
	_b == 0 ? Qnil : _b; \
    })

VALUE rb_vm_block_eval(void *block, int argc, const VALUE *argv);
#define rb_block_call(block, argc, argv) \
    rb_vm_block_eval((void *)block, argc, argv)

VALUE rb_define_class_under(VALUE, const char*, VALUE);

const char *rb_sym2name(VALUE sym);

VALUE rb_const_get(VALUE, ID);
