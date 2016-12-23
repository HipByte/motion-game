module MG
  class MainScene < Scene
    def initialize
      @random = Random.new

      director = Director.shared
      visible_size = director.size
      visible_origin = director.origin  
   
      # Add a 'close' icon to exit the game. 
      menu = Menu.new
      menu.image_item('CloseNormal.png', 'CloseSelected.png') { director.end }
      menu.position = [visible_origin.x + visible_size.width - 25, visible_origin.y + 25]
      add menu, 1

      # Load the sprites sheet.
      Sprite.load('Sprites.plist')

      # Create the ship sprite.
      @ship = Sprite.new('SpaceFlier_sm_1.png')
      @ship.position = [visible_size.width * 0.1, visible_size.height * 0.5]
      add @ship, 1
      @ship_y = @ship.position.y

      # Create the background node.
      @background = Parallax.new
      add @background, 0

      @space_dust1 = Sprite.new('bg_front_spacedust.png')
      @space_dust2 = Sprite.new('bg_front_spacedust.png')
      planetSunrise = Sprite.new('bg_planetsunrise.png')
      galaxy = Sprite.new('bg_galaxy.png')
      spatialAnomaly1 = Sprite.new('bg_spacialanomaly.png')
      spatialAnomaly2 = Sprite.new('bg_spacialanomaly2.png')

      dust_speed = [0.1, 0.1]
      bg_speed = [0.05, 0.05]
      @background.add @space_dust1, 0, dust_speed, [0, visible_size.height / 2]
      @background.add @space_dust2, 0, dust_speed, [@space_dust1.size.width, visible_size.height / 2]
      @background.add galaxy, -1, bg_speed, [0, visible_size.height * 0.7]
      @background.add planetSunrise, -1, bg_speed, [600, visible_size.height * 0]
      @background.add spatialAnomaly1, -1, bg_speed, [900, visible_size.height * 0.3]
      @background.add spatialAnomaly2, -1, bg_speed, [1500, visible_size.height * 0.9]

      %w{Stars1.plist Stars2.plist Stars3.plist}.each { |path| add Particle.new(path) }

      # Asteroids.
      @asteroids = []
      15.times do
        asteroid = Sprite.new('asteroid.png')
        asteroid.visible = false
        @asteroids << asteroid
        add asteroid
      end
      @next_asteroid_spawn = @next_asteroid = 0

      # Ship lasers.
      @ship_lasers = []
      5.times do
        ship_laser = Sprite.new('laserbeam_blue.png')
        ship_laser.visible = false
        @ship_lasers << ship_laser
        add ship_laser
      end
      @next_ship_laser = 0

      # Enable events.
      on_touch_begin { fire_ship_laser }
      # FIXME : the below line does not work on tvOS
      # on_accelerate { |acc| calculate_ship_position(acc) }

      # Start background music.
      Audio.play 'background_music', true

      # Start the game loop.
      @lives = 3
      @score = 0
      start_update
    end

    def update(delta)
      win_size = Director.shared.size

      # Move background space dusts.
      width = @space_dust1.size.width
      pos1 = @space_dust1.position
      pos2 = @space_dust2.position
      movement = 5.0
      pos1.x -= movement
      pos2.x -= movement
      if pos1.x <= -(width * 0.5)
        pos1.x = pos2.x + width
      end
      if pos2.x <= -(width * 0.5)
        pos2.x = pos1.x + width
      end
      @space_dust1.position = pos1
      @space_dust2.position = pos2

      # Move ship according to accelerometer.
      max_y = win_size.height - (@ship.size.height / 2)
      min_y = @ship.size.height / 2
      @ship.position = [@ship.position.x, [[@ship.position.y + (@ship_y * delta), min_y].max, max_y].min]

      # Make asteroids appear.
      now = Time.now
      current_usecs = (now.to_i * 1000) + (now.usec / 1000)
      if current_usecs > @next_asteroid_spawn
        @next_asteroid_spawn = (@random.rand(0.5..1.2) * 1000).to_i + current_usecs

        rand_y = @random.rand(0..(win_size.height))
        rand_duration = @random.rand(2..10)

        move_asteroid = @asteroids[@next_asteroid]
        @next_asteroid += 1
        @next_asteroid = 0 if @next_asteroid == @asteroids.size

        move_asteroid.position = [win_size.width + move_asteroid.size.width / 2, rand_y]
        move_asteroid.visible = true
        move_asteroid.move_by([-win_size.width - move_asteroid.size.width, 0], rand_duration) { move_asteroid.visible = false }
      end

      # Collisions.
      @asteroids.each do |asteroid|
        next unless asteroid.visible?
        @ship_lasers.each do |ship_laser|
          next unless ship_laser.visible?
          if ship_laser.intersects?(asteroid)
            Audio.play('explosion_large')
            ship_laser.visible = asteroid.visible = false
            @score += 1
          end
        end
        if @ship.intersects?(asteroid)
          asteroid.visible = false
          @ship.blink(9, 1)
          @lives -= 1
        end
      end

      game_over if @lives == 0
    end

    def fire_ship_laser
      Audio.play 'laser_ship'

      ship_laser = @ship_lasers[@next_ship_laser]
      @next_ship_laser += 1
      @next_ship_laser = 0 if @next_ship_laser == @ship_lasers.size

      ship_laser.position = @ship.position + [ship_laser.size.width / 2.0, 0]
      ship_laser.visible = true
      ship_laser.move_by [Director.shared.size.width, 0], 0.5 { ship_laser.visible = false }
    end

    def calculate_ship_position(acc)
      filtering_factor = 0.1
      rest_accel_x = -0.6
      ship_max_points_per_sec = Director.shared.size.height * 0.5
      max_diff_x = 0.2

      x = acc.y
      rolling_x = x * filtering_factor
      rolling_x += rolling_x * (1.0 - filtering_factor)
      accel_x = x - rolling_x
      accel_diff = accel_x - rest_accel_x
      accel_fraction = accel_diff / max_diff_x
      @ship_y = ship_max_points_per_sec * accel_fraction
    end

    def game_over
      win_size = Director.shared.size

      label = Button.new("Game Over!\nYour score is #{@score}\nTap to restart")
      label.font = 'Arial'
      label.font_size = 42
      label.position = [win_size.width / 2, win_size.height / 2]
      label.on_touch { |type| restart if type == :end }
      add label

      stop_update
    end

    def restart
      Director.shared.replace GameScene.new
    end
  end
end
