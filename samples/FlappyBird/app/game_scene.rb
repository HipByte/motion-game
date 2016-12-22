module MG
  class GameScene < Scene
    BIRD =  1 << 0
    WORLD = 1 << 1

    def initialize
      self.gravity = [0, -900]

      @backgrounds = []

      add_skyline
      add_ground
      add_bird

      @pipe_update = 0
      @random = Random.new

      # On touch, the bird jumps.
      on_touch_begin do
        Audio.play('sfx_wing')
        @bird.velocity = [0, 200]
      end

      # On collision contact, it's game over.
      on_contact_begin do
        Audio.play('sfx_hit')
        puts "game over!11!!!1!!1!1!!1111!!!!!11!!!eleven"
        true
      end

      start_update
    end

    def fly
      Audio.play('sfx_wing')
      @bird.velocity = [0, 200]
    end

    def add_skyline
      x = 0
      @backgrounds << []
      4.times do
        skyline = Sprite.new('skyline.png')
        skyline.position = [x, Director.shared.size.height / 2.0]
        add skyline, 0
        x += skyline.size.width - 5
        @backgrounds.last << skyline
      end
    end

    def add_ground
      x = 0
      @backgrounds << []
      4.times do
        ground = Sprite.new('ground.png')
        ground.attach_physics_box
        ground.dynamic = false
        ground.category_mask = WORLD
        ground.contact_mask = BIRD
        ground.position = [x, 30]
        add ground, 2
        x += ground.size.width - 5
        @backgrounds.last << ground
      end
    end

    def add_bird
      @bird = Sprite.new('bird_one.png')
      @bird.attach_physics_box
      @bird.category_mask = BIRD
      @bird.contact_mask = WORLD
      @bird.position = [100, Director.shared.size.height / 2]
      @bird.animate(['bird_one.png', 'bird_two.png', 'bird_three.png'], 0.5, Repeat::FOREVER)
      add @bird
    end

    def add_pipe
      pipe_y = @random.rand(150.0..450.0)
      [['pipe_up.png', 0], ['pipe_down.png', 850]].each do |sprite_name, y_offset|
        pipe = Sprite.new(sprite_name)
        pipe.attach_physics_box
        pipe.category_mask = WORLD
        pipe.contact_mask = BIRD
        pipe.dynamic = false
        pipe.position = [2000, pipe_y + y_offset]
        add pipe, 1
        pipe.move_by([-2000, 0], 4.0) { pipe.delete_from_parent }
      end
    end

    def update(delta)
      # Move background pieces.
      @backgrounds.each do |sprite1, sprite2, sprite3, sprite4|
        width = sprite1.size.width
        pos1 = sprite1.position
        pos2 = sprite2.position
        pos3 = sprite3.position
        pos4 = sprite4.position
        movement = 5.0
        pos1.x -= movement
        pos2.x -= movement
        pos3.x -= movement
        pos4.x -= movement
        if pos1.x <= -(width / 2.0)
          pos1.x = pos4.x + width - 5
        elsif pos2.x <= -(width / 2.0)
          pos2.x = pos1.x + width - 5
        elsif pos3.x <= -(width / 2.0)
          pos3.x = pos2.x + width - 5
        elsif pos4.x <= -(width / 2.0)
          pos4.x = pos3.x + width - 5
        end
        sprite1.position = pos1
        sprite2.position = pos2
        sprite3.position = pos3
        sprite4.position = pos4
      end

      # Rotate bird.
      @bird.rotation = 360 - [[-90, @bird.velocity.y * 0.2 + 60].max, 30].min if @bird

      # Make a pipe appear every 2 seconds.
      @pipe_update += delta
      if @pipe_update >= 2.0
        add_pipe
        @pipe_update = 0
      end
    end
  end
end