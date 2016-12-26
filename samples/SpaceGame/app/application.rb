class Application < MG::Application
  def start
    director = MG::Director.shared
    size = director.size
    director.glview.design_resolution_size(size.width, size.height, MG::ResolutionPolicy::SHOW_ALL)
    director.content_scale_factor = 750.0 / size.height
    director.show_stats = true
    director.run(MG::MainScene.new)
  end

  def resume
    # Will be called when app will move to active state from background.
  end

  def pause
    # Will be called when app will move to background from active state.
  end
end
