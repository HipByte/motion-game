#include "rubymotion.h"
#include <SimpleAudioEngine.h>

VALUE rb_cAudio = Qnil;

static const char *
audio_path(VALUE path)
{
    std::string str = RSTRING_PTR(path);
#if CC_TARGET_OS_IPHONE
    str.append(".caf");
#else  
    str.append(".wav");
#endif
    return str.c_str();
}

static VALUE
audio_background(VALUE rcv, SEL sel, int argc, VALUE *argv)
{
    VALUE path = Qnil;
    VALUE loop = Qtrue;

    rb_scan_args(argc, argv, "11", &path, &loop);
   
    CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic(
	    audio_path(path), RTEST(loop)); 
    return Qtrue;
}

static VALUE
audio_effect(VALUE rcv, SEL sel, VALUE path)
{
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(
	    audio_path(path)); 
    return Qtrue;
}

extern "C"
void
Init_Audio(void)
{
    rb_cAudio = rb_define_class_under(rb_mMC, "Audio", rb_cObject);

    rb_define_singleton_method(rb_cAudio, "background", audio_background, -1);
    rb_define_singleton_method(rb_cAudio, "effect", audio_effect, 1);
}
