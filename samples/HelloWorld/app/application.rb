class Application < MC::Application
  def start
    director = MC::Director.instance
    director.run(HelloWorld.new)
  end
end
