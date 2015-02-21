Motion::Project::App.setup do |app|
  # Use `rake config' to see complete project settings.
  app.deployment_target = '7.0'
  app.files.concat(%w{app_delegate root_view_controller}.map { |x| File.join(File.dirname(__FILE__), "#{x}.rb") })
  app.frameworks += ['CoreMotion', 'AVFoundation', 'GameController', 'MediaPlayer', 'OpenAL']
  app.libs += ['-lz', '-lsqlite3']
  app.vendor_project '../../build/ios', :static, :force_load => true
  app.custom_init_funcs << 'Init_Fluency'
end
