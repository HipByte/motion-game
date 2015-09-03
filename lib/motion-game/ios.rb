$:.unshift("/Library/RubyMotion/lib")
require 'motion/project/template/ios'

Motion::Project::App.setup do |app|
  app.build_dir = 'build/ios'
  app.deployment_target = '7.0' unless Motion::Project::Config.starter?
  app.files.concat(Dir.glob(File.join(File.dirname(__FILE__), 'ios/*.rb')))
  app.frameworks += ['CoreMotion', 'AVFoundation', 'GameController', 'MediaPlayer', 'OpenAL']
  app.libs += ['-lz', '-lsqlite3']
  app.vendor_project File.join(File.dirname(__FILE__), '../../build/ios'), :static, :force_load => true
  app.custom_init_funcs << 'Init_Fluency'

  app.info_plist['UISupportedInterfaceOrientations'] = ['UIInterfaceOrientationLandscapeRight', 'UIInterfaceOrientationLandscapeLeft']
end
