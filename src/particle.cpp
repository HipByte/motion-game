#include "rubymotion.h"
#include "motion-game.h"

/// @class Particle < Node

VALUE rb_cParticle = Qnil;

#define PARTICLE(obj) _COCOS_WRAP_GET(obj, cocos2d::ParticleSystemQuad)

/// @group Constructors

/// @method #initialize(file_name=nil)
/// Creates a Particle object. If +file_name+ is given, it should be the name
//  of a property list file in the application's resource directory. Particle
/// files can be created manually or with a visual editor such as Particle
/// Designer. If +file_name+  is not given an empty Particle object will be
/// created.
/// @param file_name [String] the name of the property list particle file.

static VALUE
particle_new(VALUE rcv, SEL sel, int argc, VALUE *argv)
{
    VALUE name = Qnil;

    rb_scan_args(argc, argv, "01", &name);

    cocos2d::ParticleSystem *particle = NULL;
    if (RTEST(name)) {
	particle = cocos2d::ParticleSystemQuad::create(RSTRING_PTR(StringValue(name)));
    }
    else {
	particle = cocos2d::ParticleSystemQuad::createWithTotalParticles(50);
	particle->setAutoRemoveOnFinish(true);
    }
    return rb_cocos2d_object_new(particle, rcv);
}

/// @property-writeonly #texture
/// @return [String] the path of the texture file.

static VALUE
particle_texture_set(VALUE rcv, SEL sel, VALUE path)
{
    auto texture_cache = cocos2d::Director::getInstance()->getTextureCache();
    auto texture = texture_cache->addImage(RSTRING_PTR(StringValue(path)));
    auto particle = PARTICLE(rcv);
    particle->setTexture(texture);
    particle->setStartSize(texture->getPixelsWide());
    particle->setEndSize(texture->getPixelsWide());
    return path;
}

/// @property #speed
/// @return [Float] the speed of the particle emitter.

static VALUE
particle_speed(VALUE rcv, SEL sel)
{
    return DBL2NUM(PARTICLE(rcv)->getSpeed());
}

static VALUE
particle_speed_set(VALUE rcv, SEL sel, VALUE val)
{
    PARTICLE(rcv)->setSpeed(NUM2DBL(val));
    return val;
}

/// @property #life
/// @return [Float] the life of each particle.

static VALUE
particle_life(VALUE rcv, SEL sel)
{
    return DBL2NUM(PARTICLE(rcv)->getLife());
}

static VALUE
particle_life_set(VALUE rcv, SEL sel, VALUE val)
{
    PARTICLE(rcv)->setLife(NUM2DBL(val));
    return val;
}

/// @property #life_range
/// @return [Float] the life variation of each particle.

static VALUE
particle_life_range(VALUE rcv, SEL sel)
{
    return DBL2NUM(PARTICLE(rcv)->getLifeVar());
}

static VALUE
particle_life_range_set(VALUE rcv, SEL sel, VALUE val)
{
    PARTICLE(rcv)->setLifeVar(NUM2DBL(val));
    return val;
}

/// @property #angle
/// @return [Float] the angle of each particle.

static VALUE
particle_angle(VALUE rcv, SEL sel)
{
    return DBL2NUM(PARTICLE(rcv)->getAngle());
}

static VALUE
particle_angle_set(VALUE rcv, SEL sel, VALUE val)
{
    PARTICLE(rcv)->setAngle(NUM2DBL(val));
    return val;
}

/// @property #angle_range
/// @return [Float] the angle variation of each particle.

static VALUE
particle_angle_range(VALUE rcv, SEL sel)
{
    return DBL2NUM(PARTICLE(rcv)->getAngleVar());
}

static VALUE
particle_angle_range_set(VALUE rcv, SEL sel, VALUE val)
{
    PARTICLE(rcv)->setAngleVar(NUM2DBL(val));
    return val;
}

/// @property #duration
/// @return [Float] the duration of the particle.

static VALUE
particle_duration(VALUE rcv, SEL sel)
{
    return DBL2NUM(PARTICLE(rcv)->getDuration());
}

static VALUE
particle_duration_set(VALUE rcv, SEL sel, VALUE val)
{
    PARTICLE(rcv)->setDuration(NUM2DBL(val));
    return val;
}

/// @property #position_range
/// @return [Point] the position variation of each particle.

static VALUE
particle_position_range(VALUE rcv, SEL sel)
{
    return rb_ccvec2_to_obj(PARTICLE(rcv)->getPosVar());
}

static VALUE
particle_position_range_set(VALUE rcv, SEL sel, VALUE val)
{
    PARTICLE(rcv)->setPosVar(rb_any_to_ccvec2(val));
    return val;
}

/// @property #particle_count
/// @return [Integer] the number of particles to emit.

static VALUE
particle_particle_count(VALUE rcv, SEL sel)
{
    return LONG2NUM(PARTICLE(rcv)->getTotalParticles());
}

static VALUE
particle_particle_count_set(VALUE rcv, SEL sel, VALUE val)
{
    PARTICLE(rcv)->setTotalParticles(NUM2LONG(val));
    return val;
}

/// @property #start_color
/// @return [Color] the color that should be used when the particle starts.

static VALUE
particle_start_color(VALUE rcv, SEL sel)
{
    return rb_cccolor4_to_obj(cocos2d::Color4B(PARTICLE(rcv)->getStartColor()));
}

static VALUE
particle_start_color_set(VALUE rcv, SEL sel, VALUE val)
{
    PARTICLE(rcv)->setStartColor(cocos2d::Color4F(rb_any_to_cccolor4(val)));
    return val;
}

/// @property #end_color
/// @return [Color] the color that should be used when the particle ends.

static VALUE
particle_end_color(VALUE rcv, SEL sel)
{
    return rb_cccolor4_to_obj(cocos2d::Color4B(PARTICLE(rcv)->getEndColor()));
}

static VALUE
particle_end_color_set(VALUE rcv, SEL sel, VALUE val)
{
    PARTICLE(rcv)->setEndColor(cocos2d::Color4F(rb_any_to_cccolor4(val)));
    return val;
}

extern "C"
void
Init_Particle(void)
{
    rb_cParticle = rb_define_class_under(rb_mMC, "Particle", rb_cNode);
    // rb_register_cocos2d_object_finalizer(rb_cParticle); removed because rb_cParticle inherits rb_cNode and it already has finalizer.

    rb_define_singleton_method(rb_cParticle, "new", particle_new, -1);
    rb_define_method(rb_cParticle, "texture=", particle_texture_set, 1);
    rb_define_method(rb_cParticle, "speed", particle_speed, 0);
    rb_define_method(rb_cParticle, "speed=", particle_speed_set, 1);
    rb_define_method(rb_cParticle, "life", particle_life, 0);
    rb_define_method(rb_cParticle, "life=", particle_life_set, 1);
    rb_define_method(rb_cParticle, "life_range", particle_life_range, 0);
    rb_define_method(rb_cParticle, "life_range=", particle_life_range_set, 1);
    rb_define_method(rb_cParticle, "angle", particle_angle, 0);
    rb_define_method(rb_cParticle, "angle=", particle_angle_set, 1);
    rb_define_method(rb_cParticle, "angle_range", particle_angle_range, 0);
    rb_define_method(rb_cParticle, "angle_range=", particle_angle_range_set, 1);
    rb_define_method(rb_cParticle, "duration", particle_duration, 0);
    rb_define_method(rb_cParticle, "duration=", particle_duration_set, 1);
    rb_define_method(rb_cParticle, "position_range", particle_position_range, 0);
    rb_define_method(rb_cParticle, "position_range=", particle_position_range_set, 1);
    rb_define_method(rb_cParticle, "particle_count", particle_particle_count, 0);
    rb_define_method(rb_cParticle, "particle_count=", particle_particle_count_set, 1);
    rb_define_method(rb_cParticle, "start_color", particle_start_color, 0);
    rb_define_method(rb_cParticle, "start_color=", particle_start_color_set, 1);
    rb_define_method(rb_cParticle, "end_color", particle_end_color, 0);
    rb_define_method(rb_cParticle, "end_color=", particle_end_color_set, 1);
}
