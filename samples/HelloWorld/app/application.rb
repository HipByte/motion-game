class Application < MC::Application
  def start
    director = MC::Director.shared
    director.run(HelloWorld.new)
  end
end
