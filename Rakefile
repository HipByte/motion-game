COCOS2D_PATH = File.expand_path('ext/cocos2d-x-3.3')
RM_VM_PATH = '/Users/lrz/src/RubyMotion/vm'

XCODE_PATH = '/Applications/Xcode.app'
XCODE_IOS_SDK = '8.1'
XCODE_IOS_DEPLOYMENT_TARGET = '7.0'

ANDROID_NDK_PATH = File.expand_path('~/.rubymotion-android/ndk')
ANDROID_SDK_PATH = File.expand_path('~/.rubymotion-android/sdk')
ANDROID_API = '16'

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
    BUILD_OPTIONS[platform] = { :cc => cc, :cxx => cxx, :cflags => cflags, :cxxflags => cflags + " -std=c++11 -Wno-inconsistent-missing-override" }
  end
end
begin
  toolchain_bin = File.join(ANDROID_NDK_PATH, 'toolchains/llvm-3.5/prebuilt/darwin-x86_64/bin')
  # Android ARM
  cflags = "-no-canonical-prefixes -target armv5te-none-linux-androideabi -march=armv5te -mthumb -msoft-float -marm -gcc-toolchain \"#{ANDROID_NDK_PATH}/toolchains/arm-linux-androideabi-4.8/prebuilt/darwin-x86_64\" -mtune=xscale -MMD -MP -fpic -ffunction-sections -funwind-tables -fexceptions -fstack-protector -fno-strict-aliasing -fno-omit-frame-pointer -DANDROID -I\"#{ANDROID_NDK_PATH}/platforms/android-#{ANDROID_API}/arch-arm/usr/include\" -Wformat -Werror=format-security -DCC_TARGET_OS_ANDROID"
  BUILD_OPTIONS['android-arm'] = { :cc => File.join(toolchain_bin, 'clang'), :cxx => File.join(toolchain_bin, 'clang++'), :cflags => cflags, :cxxflags => cflags + " -std=c++11 -I\"#{ANDROID_NDK_PATH}/sources/cxx-stl/gnu-libstdc++/4.9/include\" -I\"#{ANDROID_NDK_PATH}/sources/cxx-stl/gnu-libstdc++/4.9/libs/armeabi/include\" -I\"#{ANDROID_NDK_PATH}/sources/cpufeatures\"" }
  # Android Intel
  cflags = "-no-canonical-prefixes -msoft-float -target i686-none-linux-android -gcc-toolchain \"#{ANDROID_NDK_PATH}/toolchains/x86-4.8/prebuilt/darwin-x86_64\" -MMD -MP -fpic -ffunction-sections -funwind-tables -fexceptions -fstack-protector -fno-strict-aliasing -O0 -g3 -fno-omit-frame-pointer -DANDROID -I\"#{ANDROID_NDK_PATH}/platforms/android-#{ANDROID_API}/arch-x86/usr/include\" -Wformat -Werror=format-security -DCC_TARGET_OS_ANDROID"
  BUILD_OPTIONS['android-x86'] = { :cc => File.join(toolchain_bin, 'clang'), :cxx => File.join(toolchain_bin, 'clang++'), :cflags => cflags, :cxxflags => cflags + " -std=c++11 -I\"#{ANDROID_NDK_PATH}/sources/cxx-stl/gnu-libstdc++/4.9/include\" -I\"#{ANDROID_NDK_PATH}/sources/cxx-stl/gnu-libstdc++/4.9/libs/x86/include\" -I\"#{ANDROID_NDK_PATH}/sources/cpufeatures\"" }
end

def build_project(platforms, platform_code, build_dir)
  build_dir = File.expand_path(build_dir)
  objs_dir = File.join(build_dir, 'objs')

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

  objs = []
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

    next unless platform_code == 'android'

    toolchain_bin = "#{ANDROID_NDK_PATH}/toolchains/x86-4.9/prebuilt/darwin-x86_64/i686-linux-android/bin"
    ar = toolchain_bin + "/ar"
    ranlib = toolchain_bin + "/ranlib"
  
    lib_dir = File.join(build_dir,
      case platform
        when 'android-x86'
          'x86'
        when 'android-arm'
          'armeabi'
      end)
    lib = File.join(lib_dir, 'libmotion-cocos.a')
    if !File.exist?(lib) or objs.any? { |x| File.mtime(x) > File.mtime(lib) }
      rm_rf lib
      sh "#{ar} rcu #{lib} #{objs.join(' ')}"
      sh "#{ranlib} #{lib}"
    end

    objs.clear
  end

  if platform_code != 'android'
    ar = '/usr/bin/ar'
    ranlib = '/usr/bin/ranlib'

    lib_dir = build_dir
    lib = File.join(lib_dir, 'libmotion-cocos.a')
    if !File.exist?(lib) or objs.any? { |x| File.mtime(x) > File.mtime(lib) }
      rm_rf lib
      sh "#{ar} rcu #{lib} #{objs.join(' ')}"
      sh "#{ranlib} #{lib}"
    end
  end

  prebuild_platforms = []
  if platform_code == 'android'
    prebuild_platforms += ['android/armeabi', 'android/x86']
  else
    prebuild_platforms += [platform_code]
  end
  prebuild_platforms.each do |prebuild_platform|
    Dir.glob(File.join(COCOS2D_PATH, "external/**/prebuilt/#{prebuild_platform}/*.a")).each do |lib|
      next if lib.include?('lua')
      prebuild_build_dir = File.join(build_dir, prebuild_platform.include?('/') ? File.basename(prebuild_platform) : '')
      lib_dest = File.join(prebuild_build_dir, File.basename(lib))
      if !File.exist?(lib_dest) or File.mtime(lib) > File.mtime(lib_dest)
        mkdir_p File.dirname(lib_dest)
        install lib, lib_dest
      end
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
    build_project(['android-arm', 'android-x86'], 'android', 'build/android')
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
    io.puts "  trace = Rake.application.options.trace == true"
    io.puts "  system \"/usr/bin/rake -r \\\"#\{File.dirname(__FILE__)\}/#\{platform\}.rb\\\" -f \\\"config/#\{platform\}.rb\\\" \\\"#\{task\}\\\" #\{trace ? '--trace' : ''\}\" or exit 1"
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

task 'archive' do
  archive_dir = '/tmp/motion-game'
  rm_rf archive_dir
  mkdir_p archive_dir
  files = []
  files += Dir.glob('build/{ios,android}/**/*.{a,jar}')
  files += Dir.glob('lib/**/*.rb')
  files += Dir.glob('doc/**/*').reject { |x| File.directory?(x) }
  files += Dir.glob('samples/**/*').reject { |x| x.include?('build') or File.directory?(x) }
  files.each do |path|
    dest = File.join(archive_dir, File.dirname(path))
    mkdir_p dest
    install path, dest
  end
  Dir.chdir(File.dirname(archive_dir)) do
    rm_rf 'motion-game.zip'
    sh "/usr/bin/zip -r motion-game.zip #{File.basename(archive_dir)}"
  end
  mv File.join(File.dirname(archive_dir), 'motion-game.zip'), '.'
end

class DocAPIGen
  def initialize
    @classes = []
  end

  def parse(path)
    txt = File.read(path)
    current_class = nil
    current_node = nil
    txt.lines.select { |x| x.start_with?('/// ') }.each do |line|
      line = line[4..-1].rstrip
      if md = line.match(/@class\s+(.+)/)
        current_class = current_node = add_class(md[1])
      elsif md = line.match(/@method\s+(.+)/)
        current_node = add_method(current_class, md[1])
      elsif md = line.match(/@property\s+(.+)/)
        current_node = add_property(current_class, md[1])
      elsif md = line.match(/@property-readonly\s+(.+)/)
        current_node = add_property(current_class, md[1], :reader)
      elsif md = line.match(/@property-writeonly\s+(.+)/)
        current_node = add_property(current_class, md[1], :writer)
      elsif md = line.match(/@group\s+(.+)/)
        current_node = add_group(current_class, md[1])
      else
        current_node[:doc] << line << "\n"
      end
    end
  end

  def to_ruby
    require 'stringio'
    io = StringIO.new
    io.puts "# MG stands for Motion Game. All classes part of the framework are defined under that module."
    io.puts "module MG"
    io.puts "# The MG::Events module contains classes that represent events received from the game engine. You typically never instantiate these classes yourself."
    io.puts "module Events; end"
    @classes.each do |klass|
      io.puts doc_comment(klass, 0)
      io.puts "class #{klass[:def]}"
      klass[:nodes].each do |node|
        io.puts doc_comment(node, 2)
        case node[:type]
          when :property
            io.puts "  attr_#{node[:mode]} :#{node[:sel]}"
          when :cmethod
            io.puts "  def self.#{node[:sel]}; end"
          when :imethod
            io.puts "  def #{node[:sel]}; end"
          when :group
            io.puts "  # @group #{node[:name]}"
        end
        io.puts
      end
      io.puts "end"
    end
    io.puts "end"
    io
  end

  private

  def add_class(definition)
   klass = { :def => definition, :doc => '', :nodes => [] }
   @classes << klass
   klass
  end

  def add_method(klass, definition)
    method = { :sel => definition[1..-1], :doc => '' }
    type = case definition[0]
      when '.'
        :cmethod
      when '#'
        :imethod
      else
        raise "expected method definition `#{definition}' to start with '.' or '#'" 
    end
    method[:type] = type
    klass[:nodes] << method
    method
  end

  def add_property(klass, definition, mode = :accessor)
    if definition[0] != '#'
      raise "expected property definition `#{definition}' to start with '#'"
    end
    property = { :type => :property, :sel => definition[1..-1], :mode => mode, :doc => '' }
    klass[:nodes] << property
    property
  end

  def add_group(klass, definition)
    group = { :type => :group, :name => definition, :doc => '' }
    klass[:nodes] << group
    group
  end

  def doc_comment(node, level)
    node[:doc].lines.map { |x| (' ' * level) + "# " + x }.join
  end
end

desc 'Generate API reference'
task 'doc' do
  # Generate documentation in 'doc' directory.
  api_gen = DocAPIGen.new
  Dir.glob('src/*.cpp').each { |path| api_gen.parse(path) }
  rm_rf 'doc'
  mkdir_p 'doc'
  File.open('/tmp/fake.rb', 'w') { |io| io.write(api_gen.to_ruby.string) }
  sh "yard doc -o ./doc /tmp/fake.rb"

  # Remove references to fake.rb.
  Dir.glob('doc/**/*.html').each do |html|
    txt = File.read(html)
    txt.gsub!(/\/tmp\/fake\.rb/, 'motion-game')
    File.open(html, 'w') { |io| io.write(txt) }
  end

  # Remove "show source" links.
  txt = File.read('doc/js/app.js')
  txt.sub!(/\$\(createSourceLinks\);/, '')
  File.open('doc/js/app.js', 'w') do |io|
    io.write(txt)
  end
end

desc 'Create motion-game.gem file'
task 'gem' do
  sh "gem build motion-game.gemspec"
end
