#include "rubymotion.h"

VALUE rb_mMC = Qnil;

extern "C"
void
Init_Fluency(void)
{
    rb_mMC = rb_define_module("MC");

#define INIT_MODULE(x) \
    extern void Init_##x(void); \
    Init_##x();

    INIT_MODULE(Application)
    INIT_MODULE(Director)
    INIT_MODULE(Node)
    INIT_MODULE(Layer)
    INIT_MODULE(Label)
    INIT_MODULE(Menu)
    INIT_MODULE(Sprite)
    INIT_MODULE(Audio)
    INIT_MODULE(Types)

#undef INIT_MODULE
}
