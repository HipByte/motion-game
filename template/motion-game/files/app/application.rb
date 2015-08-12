class Application < MC::Application
  def start
    MC::Director.shared.run(MainScene.new)
  end
end
