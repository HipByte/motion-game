require 'date'

Gem::Specification.new do |spec|
  spec.name        = 'motion-game'
  spec.version     = '1.1.14'
  spec.summary     = 'Cross-platform mobile game engine for RubyMotion'
  spec.description = "motion-game allows you to write cross-platform native mobile games in Ruby."
  spec.author      = 'HipByte'
  spec.email       = 'info@hipbyte.com'
  spec.homepage    = 'https://github.com/HipByte/motion-game'
  spec.license     = 'BSD-2-Clause'

  spec.files       = ['README.md', '.document', 'doc/API_reference.rb']
  spec.files      += Dir.glob('lib/**/*.rb') + Dir.glob('template/**/*')
  # Add libraries for iOS
  spec.files      += Dir.glob('build/ios/*.a')
  # Add libraries for tvOS
  spec.files      += Dir.glob('build/tvos/*.a')
  # Add libraries for Android
  spec.files      += Dir.glob('build/android/*.jar') + Dir.glob('build/android/{armeabi,x86}/*.a')

  spec.metadata    = { "rubymotion_template_dir" => "template" }
end
