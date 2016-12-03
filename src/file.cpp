#include "rubymotion.h"
#include "motion-game.h"

/// @class File

VALUE rb_cFileUtils = Qnil;

/// @method .read
/// Open a file and return its content
/// @return [String] the content.

static VALUE
file_read(VALUE rcv, SEL sel, VALUE val)
{
    std::string filename = RSTRING_PTR(StringValue(val));
    auto content = cocos2d::FileUtils::getInstance()->getStringFromFile(filename);
    return RSTRING_NEW(content.c_str());
}

extern "C"
void
Init_FileUtils(void)
{
    rb_cFileUtils = rb_define_class_under(rb_mMC, "File", rb_cObject);

    rb_define_singleton_method(rb_cFileUtils, "read", file_read, 1);
}
