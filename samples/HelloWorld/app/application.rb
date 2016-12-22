class Application < MG::Application
  def start
    director = MG::Director.shared
    director.run(MG::HelloWorld.new)
  end
end
