COCOS2D_PATH = File.expand_path('ext/cocos2d-x-3.3')
RM_VM_PATH = '/Users/lrz/src/RubyMotion/vm'

XCODE_PATH = '/Applications/Xcode.app'
XCODE_IOS_SDK = '8.1'
XCODE_IOS_DEPLOYMENT_TARGET = '7.0'

ANDROID_NDK_PATH = '/Users/lrz/src/android-ndk-r10c'
ANDROID_SDK_PATH = '/Users/lrz/src/android-sdk-macosx'
ANDROID_API = '18'

BUILD_OPTIONS = {}
begin
  # iOS
  toolchain_bin = XCODE_PATH + '/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin'
  cc = toolchain_bin + '/clang'
  cxx = toolchain_bin + '/clang++'
  cflags_gen = lambda do |platform_apple_str|
    sdk_path = "#{XCODE_PATH}/Contents/Developer/Platforms/#{platform_apple_str}.platform/Developer/SDKs/#{platform_apple_str}#{XCODE_IOS_SDK}.sdk"
    cflags = "-isysroot \"#{sdk_path}\" -F#{sdk_path}/System/Library/Frameworks -fobjc-legacy-dispatch -fobjc-abi-version=2 -DCC_TARGET_OS_IPHONE"
    if platform_apple_str == 'iPhoneSimulator'
      cflags << " -arch i386 -arch x86_64 -mios-simulator-version-min=#{XCODE_IOS_DEPLOYMENT_TARGET}"
    else
      cflags << " -arch armv7 -arch armv7s -arch arm64 -mios-version-min=#{XCODE_IOS_DEPLOYMENT_TARGET}"
    end
   cflags
  end
  %w{iPhoneSimulator iPhoneOS}.each do |platform|
    cflags = cflags_gen.call(platform)
    BUILD_OPTIONS[platform] = { :cc => cc, :cxx => cxx, :cflags => cflags, :cxxflags => cflags + " -std=c++11" }
  end
end
begin
  # Android
  toolchain_bin = File.join(ANDROID_NDK_PATH, 'toolchains/llvm-3.4/prebuilt/darwin-x86_64/bin')
  cflags = "-no-canonical-prefixes -target armv5te-none-linux-androideabi -march=armv5te -mthumb -msoft-float -marm -gcc-toolchain \"#{ANDROID_NDK_PATH}/toolchains/arm-linux-androideabi-4.8/prebuilt/darwin-x86_64\" -mtune=xscale -MMD -MP -fpic -ffunction-sections -funwind-tables -fexceptions -fstack-protector -fno-strict-aliasing -fno-omit-frame-pointer -DANDROID -I\"#{ANDROID_NDK_PATH}/platforms/android-#{ANDROID_API}/arch-arm/usr/include\" -Wformat -Werror=format-security -DCC_TARGET_OS_ANDROID"
  BUILD_OPTIONS['android'] = { :cc => File.join(toolchain_bin, 'clang'), :cxx => File.join(toolchain_bin, 'clang++'), :cflags => cflags, :cxxflags => cflags + " -std=c++11 -I\"#{ANDROID_NDK_PATH}/sources/cxx-stl/gnu-libstdc++/4.9/include\" -I\"#{ANDROID_NDK_PATH}/sources/cxx-stl/gnu-libstdc++/4.9/libs/armeabi/include\" -I\"#{ANDROID_NDK_PATH}/sources/cpufeatures\"" }
end

def build_project(platforms, platform_code, build_dir)
  build_dir = File.expand_path(build_dir)
  objs_dir = File.join(build_dir, 'objs')
  objs = []

  compile_obj = lambda do |src_path, add_flags, platform|
    obj_path = File.join(objs_dir, platform, src_path + '.o')
    obj_dirname = File.dirname(obj_path)
    mkdir_p obj_dirname unless File.exist?(obj_dirname)
    if !File.exist?(obj_path) or File.mtime(src_path) > File.mtime(obj_path)
      platform_options = BUILD_OPTIONS[platform]
      cc = platform_options[:cc]
      cxx = platform_options[:cxx]
      cflags = platform_options[:cflags]
      cxxflags = platform_options[:cxxflags]
      case File.extname(src_path)
        when '.cpp', '.mm'
          sh "#{cxx} #{cxxflags} #{add_flags} -c #{src_path} -o #{obj_path}"
        when '.c', '.m'
          sh "#{cc} #{cflags} #{add_flags} -c #{src_path} -o #{obj_path}"
      end
    end
    obj_path
  end

  platforms.each do |platform|
    file_pattern = platform_code == 'android' ? '*.{c,cpp}' : '*.{c,cpp,m,mm}'
    cocos_platforms_pattern = platform_code == 'android' ? 'android' : 'apple,ios'
    Dir.chdir(File.join(COCOS2D_PATH, 'cocos')) do
      add_flags = "-I. -I.. -Ieditor-support -Iplatform -Ideprecated -I2d -I../extensions -I../external -I../external/edtaa3func -I../external/tinyxml2 -I../external/ConvertUTF -I../external/unzip -I../external/curl/include/#{platform_code} -I../external/websockets/include/#{platform_code} -I../external/chipmunk/include/chipmunk -I../external/xxhash -I../external/png/include/#{platform_code} -I../external/tiff/include/#{platform_code} -I../external/jpeg/include/#{platform_code} -I../external/webp/include/#{platform_code} -I../external/freetype2/include/#{platform_code}"
      if platform_code == 'android'
        add_flags << " -I../external/freetype2/include/android/freetype2"
        add_flags << " -Iplatform/android"
        add_flags << " -Iaudio/include -Iaudio/android"
      end
      pats = %w{2d 3d base editor-support math network physics renderer storage ui deprecated}.map { |x| "#{x}/**/#{file_pattern}" }
      pats << "#{file_pattern}"
      pats << "audio/#{file_pattern}"
      pats << "audio/{#{cocos_platforms_pattern}}/**/#{file_pattern}"
      pats << "platform/#{file_pattern}"
      pats << "platform/{#{cocos_platforms_pattern}}/**/#{file_pattern}"
      files = pats.map { |x| Dir.glob(x) }.flatten.uniq
      files.each do |src_path|
        case platform_code
          when 'ios'
            next if src_path == 'base/CCUserDefault-android.cpp'
            next if src_path == 'base/CCController-android.cpp'
            next if src_path == 'ui/UIVideoPlayer-android.cpp'
            next if src_path == 'ui/UIWebViewImpl-android.cpp'
            next if src_path == 'ui/UIWebView.cpp'
          when 'android'
        end

        objs << compile_obj.call(src_path, add_flags, platform)
      end
    end
  
    ['external/xxhash', 'external/ConvertUTF', 'external/tinyxml2', 'external/unzip', 'external/edtaa3func', 'extensions/GUI/CCControlExtension', 'extensions/GUI/CCScrollView'].each do |dir|
      Dir.chdir(File.join(COCOS2D_PATH , dir)) do
        add_flags = '-DUSE_FILE32API -I.'
        base_headers = '-I' + (0...dir.split('/').size).to_a.map { '..' }.join('/')
        add_flags << ' ' + base_headers
        add_flags << ' ' + base_headers + '/cocos'
        add_flags << ' ' + base_headers + '/cocos/platform'
        Dir.glob(file_pattern).each do |src_path|
          objs << compile_obj.call(src_path, add_flags, platform)
        end
      end
    end
  
    Dir.chdir('src') do
      add_flags = "-I. -Werror -I#{COCOS2D_PATH}/cocos -I#{COCOS2D_PATH}/cocos/audio/include -I#{RM_VM_PATH}"
      case platform_code
        when 'ios'
          add_flags << " -I#{RM_VM_PATH}/include -DMACRUBY_STATIC -DNO_LIBAUTO"
        when 'android'
          add_flags << " -I#{RM_VM_PATH}/java"
      end
      Dir.glob(file_pattern).each do |src_path|
        objs << compile_obj.call(src_path, add_flags, platform)
      end
    end
  end

  ranlib = case platform_code
    when 'ios'
      '/usr/bin/ranlib'
    when 'android'
      "#{ANDROID_NDK_PATH}/toolchains/x86-4.9/prebuilt/darwin-x86_64/i686-linux-android/bin/ranlib"
  end

  lib = File.join(build_dir, 'libmotion-cocos.a')
  if !File.exist?(lib) or objs.any? { |x| File.mtime(x) > File.mtime(lib) }
    rm_rf lib
    sh "/usr/bin/ar rcu #{lib} #{objs.join(' ')}"
    sh "#{ranlib} #{lib}"
  end

  prebuild_platform = platform_code
  prebuild_platform += '/armeabi' if platform_code == 'android'
  Dir.glob(File.join(COCOS2D_PATH, "external/**/prebuilt/#{prebuild_platform}/*.a")).each do |lib|
    next if lib.include?('lua')
    lib_dest = File.join(build_dir, File.basename(lib))
    if !File.exist?(lib_dest) or File.mtime(lib) > File.mtime(lib_dest)
      install lib, lib_dest
    end
  end

  if platform_code == 'android'
    classes_dir = File.join(build_dir, 'classes')
    Dir.chdir(File.join(COCOS2D_PATH, 'cocos/platform/android/java/src')) do
      java_src_files = Dir.glob('*/**/*.java')
      android_jar = "#{ANDROID_SDK_PATH}/platforms/android-#{ANDROID_API}/android.jar"
      mkdir_p classes_dir unless File.exist? classes_dir
      sh "/usr/bin/javac -d \"#{classes_dir}\" -bootclasspath \"#{android_jar}\" #{java_src_files.join(' ')}"
    end
    Dir.chdir(classes_dir) do
      sh "/usr/bin/jar cvf ../motion-cocos.jar ."
    end
  end
end

namespace 'build' do
  desc 'Build for Android'
  task 'android' do
    build_project(['android'], 'android', 'build/android')
  end

  desc 'Build for iOS simulator and device'
  task 'ios' do
    build_project(['iPhoneSimulator', 'iPhoneOS'], 'ios', 'build/ios')
  end

  desc 'Build everything'
  task 'all' => ['build:ios', 'build:android']
end

desc 'Generate lib/shortcuts.rb'
task 'gen_tasks_shortcuts' do
  File.open('lib/shortcuts.rb', 'w') do |io|
    io.puts "# This file has been generated, do not edit by hand.\n"
    io.puts "def invoke_rake(platform, task)"
    io.puts "  system \"/usr/bin/rake -r \\\"#\{File.dirname(__FILE__)\}/#\{platform\}.rb\\\" -f \\\"config/#\{platform\}.rb\\\" \\\"#\{task\}\\\"\" or exit 1"
    io.puts "end"
    %w{ios android}.each do |platform|
      io.puts "namespace '#{platform}' do"
      `/usr/bin/rake -I /Library/RubyMotion/lib -f /Library/RubyMotion/lib/motion/project/template/#{platform}.rb -T`.scan(/rake\s([^\s]+)\s+#\s([^\n]+)/).each do |plat_task, plat_desc|
        io.puts "  desc \"#{plat_desc}\""
        io.puts "  task \"#{plat_task}\" do"
        io.puts "    invoke_rake '#{platform}', '#{plat_task}'"
        io.puts "  end"
      end
      io.puts "end"
    end
  end
end
