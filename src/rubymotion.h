#ifndef __RUBYMOTION_H_
#define __RUBYMOTION_H_

#include "cocos2d.h"

#if defined(__cplusplus)
extern "C" {
#endif

#define VALUE		unsigned long
#define SIGNED_VALUE	long
#define ID		unsigned long

#define Qfalse  0x0
#define Qtrue   0x2
#define Qnil    0x4
#define Qundef  0x6

#if CC_TARGET_OS_IPHONE || CC_TARGET_OS_APPLETV
#include "rubymotion-apple.h"
#elif CC_TARGET_OS_ANDROID
#include "rubymotion-android.h"
#endif

extern VALUE rb_cArray;
extern VALUE rb_cSymbol;
extern VALUE rb_cObject;
extern VALUE rb_cInteger;
extern VALUE rb_eArgError;
extern VALUE rb_eRuntimeError;

void rb_raise(VALUE exc, const char *fmt, ...) __attribute__ ((noreturn));
int rb_scan_args(int, const VALUE*, const char*, ...);
VALUE rb_ary_new(void);
VALUE rb_ary_push(VALUE, VALUE);
VALUE rb_name2sym(const char *);
ID rb_intern(const char *name);

#define RTEST(obj) \
    ({ \
	VALUE _rtest_obj = (VALUE)obj; \
	_rtest_obj != Qfalse && _rtest_obj != Qnil; \
    })

VALUE rb_str_new2(const char *);
#define RSTRING_NEW(cstr) rb_str_new2(cstr)

#define NUM2BYTE(val) \
    ({ \
	int _byte = (NUM2DBL(val) * 255); \
	std::min(std::max(_byte, 0), 255); \
    })
#define BYTE2NUM(val) (DBL2NUM(val / 255.0))


#if defined(__cplusplus)
}
#endif

#endif // __RUBYMOTION_H_
