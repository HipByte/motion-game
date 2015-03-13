class GameScene < MC::Scene
  BIRD =  1 << 0
  WORLD = 1 << 1

  def initialize
    self.gravity = [0, -900]

    @pipe_update = 0
    @random = Random.new
    @backgrounds = []

    # Add skyline.
    x = 0
    @backgrounds << []
    2.times do
      skyline = MC::Sprite.new('skyline.png')
      skyline.position = [x, MC::Director.shared.size.height / 2.0]
      add skyline, -1
      x += skyline.size.width - 5
      @backgrounds.last << skyline 
    end

    # Add ground.
    x = 0
    @backgrounds << []
    2.times do
      ground = MC::Sprite.new('ground.png')
      ground.position = [x, 30]
      ground.attach_physics_box
      ground.dynamic = false
      ground.category_mask = WORLD
      ground.contact_mask = BIRD
      add ground, 2
      x += ground.size.width - 5
      @backgrounds.last << ground 
    end

    # Add bird.
    @bird = MC::Sprite.new('bird_one.png')
    @bird.position = [100, MC::Director.shared.size.height / 2]
    @bird.animate(['bird_one.png', 'bird_two.png', 'bird_three.png'], 0.5, :forever)
    @bird.attach_physics_box
    @bird.category_mask = BIRD
    @bird.contact_mask = WORLD
    add @bird

    # On touch, the bird jumps.
    on_touch_begin do
      MC::Audio.shared.effect('sfx_wing')
      @bird.velocity = [0, 200]
    end

    # On collision contact, it's game over.
    on_contact_begin do
      MC::Audio.shared.effect('sfx_hit')
      puts "game over!"
      true
    end

    start_update
  end

  def update(delta)
    # Move background pieces.
    @backgrounds.each do |sprite1, sprite2|
      width = sprite1.size.width
      pos1 = sprite1.position
      pos2 = sprite2.position
      movement = 5.0
      pos1.x -= movement
      pos2.x -= movement
      if pos1.x <= -(width / 2.0)
        pos1.x = pos2.x + width - 5
      elsif pos2.x <= -(width / 2.0)
        pos2.x = pos1.x + width - 5
      end
      sprite1.position = pos1
      sprite2.position = pos2
    end

    # Rotate bird.
    @bird.rotation = 360 - [[-90, @bird.velocity.y * 0.2 + 60].max, 30].min

    # Make a pipe appear every 2 seconds.
    @pipe_update += delta
    if @pipe_update >= 2.0
      pipe_y = @random.rand(150.0..450.0)
      [['pipe_up.png', 0], ['pipe_down.png', 850]].each do |sprite_name, y_offset|
        pipe = MC::Sprite.new(sprite_name)
        pipe.attach_physics_box
        pipe.category_mask = WORLD
        pipe.contact_mask = BIRD
        pipe.dynamic = false
        pipe.position = [800, pipe_y + y_offset]
        add pipe, 1
        pipe.move_by([-900, 0], 4.0) { pipe.delete_from_parent }
      end
      @pipe_update = 0
    end
  end
end
