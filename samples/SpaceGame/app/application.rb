class Application < MC::Application
  def start
    director = MC::Director.shared
    director.show_stats = true
    director.run(GameScene.new)
  end
end
