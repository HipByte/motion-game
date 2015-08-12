$:.unshift("/Library/RubyMotion/lib")
require 'motion/project/template/android'

Motion::Project::App.setup do |app|
  app.api_version = '16'
  app.build_dir = 'build/android'
  app.assets_dirs = ['resources']
  app.resources_dirs = []
  app.files.concat(Dir.glob(File.join(File.dirname(__FILE__), 'android/*.rb')))
  app.vendor_project :jar => File.join(File.dirname(__FILE__), '../../build/android/motion-cocos.jar')
  app.archs = ['armv5te', 'x86']

  %w{armeabi x86}.each do |arch|
    app.libs[arch] += %w{motion-cocos chipmunk crypto curl freetype jpeg png ssl tiff webp websockets}.map { |x| File.join(File.dirname(__FILE__), "../../build/android/#{arch}/lib#{x}.a") } + ['-lz', '-latomic', '-lEGL', '-lGLESv2', '-lOpenSLES', '-landroid'] # The order of these libraries is very important for the linker to find all symbols.
  end
  app.custom_init_funcs << 'Init_Fluency'

  app.manifest.add_child('uses-feature')['android:glEsVersion'] = '0x00020000'
  app.manifest.child('application').child('activity') do |main_activity|
    main_activity['android:screenOrientation'] = 'landscape'
    main_activity['android:theme'] = '@android:style/Theme.NoTitleBar.Fullscreen'
  end
end
