Gem::Specification.new do |spec|
  spec.name        = 'motion-game'
  spec.version     = '1.0'
  spec.date        = Date.today
  spec.summary     = 'Cross-platform mobile game engine for RubyMotion'
  spec.description = "motion-cocoapods allows you to write cross-platform native mobile games in Ruby."
  spec.author      = 'Laurent Sansonetti'
  spec.email       = 'lrz@hipbyte.com'
  spec.homepage    = 'http://www.rubymotion.com/game'
  spec.license     = 'Proprietary'
  spec.files       = Dir.glob('lib/**/*.rb') + Dir.glob('build/ios/*.a') + Dir.glob('build/android/*.jar') + Dir.glob('build/android/{armeabi,x86}/*.a') + Dir.glob('doc/**/*') + Dir.glob('template/**/*')
  spec.metadata    = { "rubymotion_template_dir" => "template" }
end
