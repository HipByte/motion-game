class Application < MG::Application
  def start
    MG::Director.shared.run(MainScene.new)
  end

  def resume
    # Will be Called when app will move to active state from background.
  end

  def pause
    # Will be Called when app will move to background from active state.
  end
end
