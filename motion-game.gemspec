Gem::Specification.new do |spec|
  spec.name        = 'motion-game'
  spec.version     = '1.1'
  spec.date        = Date.today
  spec.summary     = 'Cross-platform mobile game engine for RubyMotion'
  spec.description = "motion-game allows you to write cross-platform native mobile games in Ruby."
  spec.author      = 'HipByte'
  spec.email       = 'info@hipbyte.com'
  spec.homepage    = 'http://www.rubymotion.com/developers/motion-game'
  spec.license     = 'BSD'
  spec.files       = Dir.glob('lib/**/*.rb') + Dir.glob('build/ios/*.a') + Dir.glob('build/android/*.jar') + Dir.glob('build/android/{armeabi,x86}/*.a') + Dir.glob('doc/**/*') + Dir.glob('template/**/*')
  spec.metadata    = { "rubymotion_template_dir" => "template" }
end
