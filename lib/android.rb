#$:.unshift("/Library/RubyMotion/lib")
$:.unshift("/Users/lrz/src/RubyMotion/lib")
require 'motion/project/template/android'

Motion::Project::App.setup do |app|
  app.api_version = '18'
  app.build_dir = 'build/android'
  app.assets_dirs = ['resources']
  app.resources_dirs = []
  app.files.concat(Dir.glob(File.join(File.dirname(__FILE__), 'android/*.rb')))
  app.libs += ['-lz', *Dir.glob('../../build/android/*.a')]
  app.custom_init_funcs << 'Init_Fluency'
end
