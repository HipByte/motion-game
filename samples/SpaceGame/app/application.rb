class Application < MC::Application
  orientation :all_but_upside_down

  def start
    director = MC::Director.instance
    director.show_stats = true
    director.run(GameLayer.new)
  end
end
