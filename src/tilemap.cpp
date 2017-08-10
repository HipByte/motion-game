#include "rubymotion.h"
#include "motion-game.h"

/// @class TileMap < Node

VALUE rb_cTileMap = Qnil;

/// @method .load(tmx_filename)
/// Load the specfied TMX file and return a TileMap object.
/// @param tmx_filename [String] the filename of the TMX file.
/// @return [TileMap]

static VALUE
tilemap_load(VALUE rcv, SEL sel, VALUE tmx_filename)
{
    std::string filename_str = RSTRING_PTR(StringValue(tmx_filename));
    auto map = cocos2d::TMXTiledMap::create(filename_str);
    if (map == NULL) {
      rb_raise(rb_eRuntimeError, "Can't create TileMap with `%s'. Need a proper TMX file.", filename_str.c_str());
    }
    return rb_cocos2d_object_new(map, rcv);
}

/// @method #layer(name)
/// Return the layer for the given name.
/// @return [TMXLayer] TMX layer.

static cocos2d::TMXLayer *
tilemap_layer(VALUE rcv, SEL sel, VALUE name)
{
    return TILEMAP(rcv)->getLayer(RSTRING_PTR(StringValue(name)));
}

/// @property #tile_size
/// @return [Size] the tile size.

static VALUE
tilemap_tile_size(VALUE rcv, SEL sel)
{
    return rb_ccsize_to_obj(TILEMAP(rcv)->getTileSize());
}

extern "C"
void
Init_TileMap(void)
{
    rb_cTileMap = rb_define_class_under(rb_mMC, "TileMap", rb_cNode);
    rb_define_singleton_method(rb_cTileMap, "load", tilemap_load, 1);
    rb_define_method(rb_cTileMap, "layer", tilemap_layer, 1);
    rb_define_method(rb_cTileMap, "tile_size", tilemap_tile_size, 0);
}
