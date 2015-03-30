#include "rubymotion.h"
#include <AudioEngine.h>

/// @class Audio < Object

VALUE rb_cAudio = Qnil;

struct mc_Audio {
    int audio_id;
};

#define AUDIO_ENGINE cocos2d::experimental::AudioEngine

#define AUDIO_ID(obj) \
    ((_COCOS_WRAP_GET(obj, struct mc_Audio))->audio_id)

/// @group Constructors

/// @method .play(path, loop=false, volume=1.0)
/// Creates a new Audio object based on a sound file at the given path and
/// immediately plays it.
/// @param path [String] the path of the sound file that should be played.
/// @param loop [Boolean] whether the sound file playback should loop.
/// @param volume [Float] the audio volume that should be used to play this
///   this sound file, as a +0.0+ to +1.0+ Float range.  
/// @return [Audio] an Audio instance.

static VALUE
audio_play(VALUE rcv, SEL sel, int argc, VALUE *argv)
{
    VALUE path = Qnil, loop = Qnil, volume = Qnil;

    rb_scan_args(argc, argv, "12", &path, &loop, &volume);

    std::string str = RSTRING_PTR(path);
    if (str.find('.') == std::string::npos) {
#if CC_TARGET_OS_IPHONE
	str.append(".caf");
#else  
	str.append(".wav");
#endif
    }

    auto audio = new struct mc_Audio();
    audio->audio_id = AUDIO_ENGINE::play2d(str,
	loop == Qnil ? false : RTEST(loop),
	volume == Qnil ? 1.0 : NUM2DBL(volume));
    return rb_class_wrap_new((void *)audio, rb_cAudio);
}

/// @group Properties

/// @property #loop?
/// @return [Boolean] whether the sound file should loop.

static VALUE
audio_loop(VALUE rcv, SEL sel)
{
    return AUDIO_ENGINE::isLoop(AUDIO_ID(rcv)) ? Qtrue : Qfalse;
}

static VALUE
audio_loop_set(VALUE rcv, SEL sel, VALUE flag)
{
    AUDIO_ENGINE::setLoop(AUDIO_ID(rcv), RTEST(flag)); 
    return flag;
}

/// @property #volume
/// @return [Float] the volume of the sound file, from a 0.0 to 1.0 Float range.

static VALUE
audio_volume(VALUE rcv, SEL sel)
{
    return DBL2NUM(AUDIO_ENGINE::getVolume(AUDIO_ID(rcv)));
}

static VALUE
audio_volume_set(VALUE rcv, SEL sel, VALUE val)
{
    AUDIO_ENGINE::setVolume(AUDIO_ID(rcv), NUM2DBL(val)); 
    return val;
}

/// @property #current_position
/// @return [Float] the position where to play the sound file.

static VALUE
audio_current_position(VALUE rcv, SEL sel)
{
    return DBL2NUM(AUDIO_ENGINE::getCurrentTime(AUDIO_ID(rcv)));
}

static VALUE
audio_current_position_set(VALUE rcv, SEL sel, VALUE val)
{
    AUDIO_ENGINE::setCurrentTime(AUDIO_ID(rcv), NUM2DBL(val)); 
    return val;
}

/// @property-readonly #duration
/// @return [Float] the duration left in the sound file.

static VALUE
audio_duration(VALUE rcv, SEL sel)
{
    return DBL2NUM(AUDIO_ENGINE::getDuration(AUDIO_ID(rcv)));
}

/// @group Playback

/// @method #resume
/// Resumes playing the sound file.
/// @return [Audio] the receiver.

static VALUE
audio_resume(VALUE rcv, SEL sel)
{
    AUDIO_ENGINE::resume(AUDIO_ID(rcv));
    return rcv;
}

/// @method #pause
/// Pauses the sound file.
/// @return [Audio] the receiver.

static VALUE
audio_pause(VALUE rcv, SEL sel)
{
    AUDIO_ENGINE::pause(AUDIO_ID(rcv));
    return rcv;
}

/// @method #stop
/// Stops the sound file.
/// @return [Audio] the receiver.

static VALUE
audio_stop(VALUE rcv, SEL sel)
{
    AUDIO_ENGINE::stop(AUDIO_ID(rcv));
    return rcv;
}

/// @method #playing?
/// @return [Boolean] whether the sound file is being played.

static VALUE
audio_playing(VALUE rcv, SEL sel)
{
    return AUDIO_ENGINE::getState(AUDIO_ID(rcv))
	== AUDIO_ENGINE::AudioState::PLAYING ? Qtrue : Qfalse;
}

/// @method #paused?
/// @return [Boolean] whether the sound file is being paused.

static VALUE
audio_paused(VALUE rcv, SEL sel)
{
    return AUDIO_ENGINE::getState(AUDIO_ID(rcv))
	== AUDIO_ENGINE::AudioState::PAUSED ? Qtrue : Qfalse;
}

extern "C"
void
Init_Audio(void)
{
    rb_cAudio = rb_define_class_under(rb_mMC, "Audio", rb_cObject);

    rb_define_singleton_method(rb_cAudio, "play", audio_play, -1);
    rb_define_method(rb_cAudio, "loop?", audio_loop, 0);
    rb_define_method(rb_cAudio, "loop=", audio_loop_set, 1);
    rb_define_method(rb_cAudio, "volume", audio_volume, 0);
    rb_define_method(rb_cAudio, "volume=", audio_volume_set, 1);
    rb_define_method(rb_cAudio, "current_position", audio_current_position, 0);
    rb_define_method(rb_cAudio, "current_position=",
	    audio_current_position_set, 1);
    rb_define_method(rb_cAudio, "duration", audio_duration, 0);
    rb_define_method(rb_cAudio, "resume", audio_resume, 0);
    rb_define_method(rb_cAudio, "pause", audio_pause, 0);
    rb_define_method(rb_cAudio, "stop", audio_stop, 0);
    rb_define_method(rb_cAudio, "playing?", audio_playing, 0);
    rb_define_method(rb_cAudio, "paused?", audio_paused, 0);
}
