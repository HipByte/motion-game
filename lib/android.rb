#$:.unshift("/Library/RubyMotion/lib")
$:.unshift("/Users/lrz/src/RubyMotion/lib")
require 'motion/project/template/android'

Motion::Project::App.setup do |app|
  app.api_version = '18'
  app.build_dir = 'build/android'
  app.assets_dirs = ['resources']
  app.resources_dirs = []
  app.files.concat(Dir.glob(File.join(File.dirname(__FILE__), 'android/*.rb')))
  app.vendor_project :jar => '../../build/android/motion-cocos.jar'
  app.libs += %w{motion-cocos chipmunk crypto curl freetype jpeg png ssl tiff webp websockets}.map { |x| "../../build/android/lib#{x}.a" } + ['-lz', '-latomic', '-lEGL', '-lGLESv2', '-landroid'] # The order of these libraries is very important for the linker to find all symbols.
  app.custom_init_funcs << 'Init_Fluency'
end
