module MG
  class HelloWorld < MG::Scene
    def initialize
      label = MG::Text.new("Hello World", "Arial", 96)
      label.anchor_point = [0, 0]
      add label
    end
  end
end
