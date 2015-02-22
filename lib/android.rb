$:.unshift("/Library/RubyMotion/lib")
require 'motion/project/template/android'

Motion::Project::App.setup do |app|
  app.build_dir = 'build/android'
  app.api_version = '18'
=begin
  app.files.concat(%w{app_delegate root_view_controller}.map { |x| File.join(File.dirname(__FILE__), "#{x}.rb") })
  app.frameworks += ['CoreMotion', 'AVFoundation', 'GameController', 'MediaPlayer', 'OpenAL']
  app.libs += ['-lz', '-lsqlite3']
  app.vendor_project '../../build/ios', :static, :force_load => true
=end
  app.custom_init_funcs << 'Init_Fluency'
end
