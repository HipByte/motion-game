#include "rubymotion.h"

/// @class Particle < Node

VALUE rb_cParticle = Qnil;

/// @method #initialize(file_name)
/// Creates a Particle object with the content of +file_name+, which should
/// be the name of a property list file in the application's resource
/// directory. Particle files can be created manually or with a visual editor
/// such as Particle Designer.
/// @param file_name [String] the name of the property list particle file.

static VALUE
particle_new(VALUE rcv, SEL sel, VALUE name)
{
    cocos2d::ParticleSystem *particle = cocos2d::ParticleSystemQuad::create(
	    RSTRING_PTR(name));
    assert(particle != NULL);
    return rb_class_wrap_new((void *)particle, rcv);
}

extern "C"
void
Init_Particle(void)
{
    rb_cParticle = rb_define_class_under(rb_mMC, "Particle", rb_cNode);

    rb_define_singleton_method(rb_cParticle, "new", particle_new, 1);
}

