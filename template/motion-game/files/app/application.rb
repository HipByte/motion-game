class Application < MG::Application
  def start
    MG::Director.shared.run(MainScene.new)
  end
end
