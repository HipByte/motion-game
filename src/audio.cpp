#include "rubymotion.h"
#include <SimpleAudioEngine.h>

/// @class Audio < Object

VALUE rb_cAudio = Qnil;
static VALUE mc_audio_instance = Qnil;

/// @group Constructors

/// @method .shared
/// @return [Audio] the shared Audio instance.

static VALUE
audio_instance(VALUE rcv, SEL sel)
{
    if (mc_audio_instance == Qnil) {
	VALUE obj = rb_class_wrap_new(
		(void *)CocosDenshion::SimpleAudioEngine::getInstance(),
		rb_cAudio);
	mc_audio_instance = rb_retain(obj);
    }
    return mc_audio_instance;
}

static std::string
audio_path(VALUE path)
{
    std::string str = RSTRING_PTR(path);
    if (str.find('.') == std::string::npos) {
#if CC_TARGET_OS_IPHONE
	str.append(".caf");
#else  
	str.append(".wav");
#endif
    }
    return str;
}

/// @group Audio Playback

/// @method #background(file_name, loop=true)
/// Plays background music.
/// @param file_name [String] Name of the audio file to play, without its file
///   extension. The file should reside in the application's resource
///   directory.
/// @param loop [true, false] Whether the audio file should loop.
/// @return [Audio] the receiver.

static VALUE
audio_background(VALUE rcv, SEL sel, int argc, VALUE *argv)
{
    VALUE path = Qnil;
    VALUE loop = Qtrue;

    rb_scan_args(argc, argv, "11", &path, &loop);
   
    AUDIO(rcv)->playBackgroundMusic(audio_path(path).c_str(), RTEST(loop));
    return rcv;
}

/// @method #effect(file_name)
/// Plays a sound effect.
/// @param file_name [String] Name of the audio file to play, without its file
///   extension. The file should reside in the application's resource
///   directory.
/// @return [Audio] the receiver.

static VALUE
audio_effect(VALUE rcv, SEL sel, VALUE path)
{
    AUDIO(rcv)->playEffect(audio_path(path).c_str());
    return rcv;
}

extern "C"
void
Init_Audio(void)
{
    rb_cAudio = rb_define_class_under(rb_mMC, "Audio", rb_cObject);

    rb_define_singleton_method(rb_cAudio, "shared", audio_instance, 0);
    rb_define_method(rb_cAudio, "background", audio_background, -1);
    rb_define_method(rb_cAudio, "effect", audio_effect, 1);
}
