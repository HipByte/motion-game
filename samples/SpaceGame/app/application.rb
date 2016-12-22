class Application < MG::Application
  def start
    director = MG::Director.shared
    director.show_stats = true
    director.run(MG::GameScene.new)
  end
end
