#include "rubymotion.h"

VALUE rb_cParticle = Qnil;

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

