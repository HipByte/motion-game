COCOS2D_PATH = File.expand_path('ext/cocos2d-x-3.3')
RM_VM_PATH = '/Users/lrz/src/RubyMotion/vm'
XCODE_PATH = '/Applications/Xcode.app'
XCODE_IOS_SDK = '8.1'
XCODE_IOS_DEPLOYMENT_TARGET = '7.0'

def cocos_build
  options = {}

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
    options[platform] = { :cc => cc, :cxx => cxx, :cflags => cflags, :cxxflags => cflags + " -std=c++11" }
  end

  build_dir = File.expand_path(File.join('build', 'ios'))
  objs_dir = File.join(build_dir, 'objs')
  objs = []

  compile_obj = lambda do |src_path, add_flags, platform|
    obj_path = File.join(objs_dir, platform, src_path + '.o')
    obj_dirname = File.dirname(obj_path)
    mkdir_p obj_dirname unless File.exist?(obj_dirname)
    if !File.exist?(obj_path) or File.mtime(src_path) > File.mtime(obj_path)
      platform_options = options[platform]
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

  ['iPhoneSimulator', 'iPhoneOS'].each do |platform|
    Dir.chdir(File.join(COCOS2D_PATH, 'cocos')) do
      pats = %w{2d 3d base editor-support math network physics renderer storage ui deprecated}.map { |x| "#{x}/**/*.{c,cpp,m,mm}" }
      pats << "*.{c,cpp}"
      pats << "audio/*.{c,cpp}"
      pats << "audio/{apple,ios}/**/*.{c,cpp,m,mm}"
      pats << "platform/*.{c,cpp}"
      pats << "platform/{apple,ios}/**/*.{c,cpp,m,mm}"
      files = pats.map { |x| Dir.glob(x) }.flatten.uniq
      files.each do |src_path|
        next if src_path == 'base/CCUserDefault-android.cpp'
        next if src_path == 'ui/UIVideoPlayer-android.cpp'
        next if src_path == 'ui/UIWebViewImpl-android.cpp'
        next if src_path == 'ui/UIWebView.cpp'
        add_flags = '-I. -I.. -Ieditor-support -Iplatform -Ideprecated -I2d -I../extensions -I../external -I../external/freetype2/include/ios -I../external/edtaa3func -I../external/tinyxml2 -I../external/ConvertUTF -I../external/unzip -I../external/curl/include/ios -I../external/websockets/include/ios -I../external/chipmunk/include/chipmunk -I../external/xxhash -I../external/png/include/ios -I../external/tiff/include/ios -I../external/jpeg/include/ios -I../external/webp/include/ios'
        if src_path == 'ui/UIWebView.cpp'
          add_flags << ' -ObjC++'
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
        Dir.glob('*.{c,cpp,m,mm}').each do |src_path|
          objs << compile_obj.call(src_path, add_flags, platform)
        end
      end
    end
  
    Dir.chdir('src') do
      add_flags = "-I. -Werror -I#{COCOS2D_PATH}/cocos -I#{COCOS2D_PATH}/cocos/audio/include -I#{RM_VM_PATH} -I#{RM_VM_PATH}/include -DMACRUBY_STATIC -DNO_LIBAUTO"
      Dir.glob('*.{c,cpp}').each do |src_path|
        objs << compile_obj.call(src_path, add_flags, platform)
      end
    end
  end

  lib = File.join(build_dir, 'libmotion-cocos.a')
  if !File.exist?(lib) or objs.any? { |x| File.mtime(x) > File.mtime(lib) }
    rm_rf lib
    sh "/usr/bin/ar rcu #{lib} #{objs.join(' ')}"
    sh "/usr/bin/ranlib #{lib}"
  end

  Dir.glob(File.join(COCOS2D_PATH, 'external/**/prebuilt/ios/*.a')).each do |lib|
    next if lib.include?('lua')
    lib_dest = File.join(build_dir, File.basename(lib))
    if !File.exist?(lib_dest) or File.mtime(lib) > File.mtime(lib_dest)
      install lib, lib_dest
    end
  end
end

namespace 'build' do
  desc 'Build for iOS simulator and device'
  task 'ios' do
    cocos_build
  end

  desc 'Build everything'
  task 'all' => ['build:ios']
end
