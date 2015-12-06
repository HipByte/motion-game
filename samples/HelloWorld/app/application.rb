class Application < MG::Application
  def start
    director = MG::Director.shared
    director.run(HelloWorld.new)
  end
end
