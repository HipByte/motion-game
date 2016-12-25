# motion-game

[![Gem Version](https://badge.fury.io/rb/motion-game.svg)](https://badge.fury.io/rb/motion-game)

motion-game is a cross-platform mobile game engine for RubyMotion. It lets you write mobile games in Ruby for iOS, tvOS and Android.

motion-game is currently in **beta**. Please give it a try and report problems you find to us.

## Features

 * **Use Ruby :-)** motion-game exposes a pure Ruby API to write games for mobile devices. You can write a video game all in Ruby.
 * **100% cross-platform**: motion-game projects are fully cross-platform for iOS, tvOS and Android. One codebase runs everywhere.
 * **Fully-featured**: motion-game has audio, sprites, animations, particles, device sensors / events, scene graph / director, UI widgets, etc.
 * **Solid foundations**: the motion-game API is implemented using popular and stable opensource libraries, such as [cocos2d-x](http://www.cocos2d-x.org/), [box2d](http://box2d.org/), and more. motion-game projects are also based on RubyMotion which offers portable Ruby runtimes for iOS and Android as well as static compilation of Ruby code.
 * **Native compilation**: your Ruby code will be compiled into optimized native code for each platform you target. There is no interpreter and the original Ruby code will not be in the app.
 * **Platform APIs access**: if you need it, you can call the entire set of public iOS, tvOS or Android APIs from Ruby code as well.

## Getting Started

### Installing RubyMotion

[RubyMotion 4.7+](http://rubymotion.com) is required. A starter version can be downloaded for free. You also need to set up your computer for mobile development (iOS and/or tvOS and/or Android). Follow the [Getting Started](http://rubymotion.com/developers) guides after installation.

### Installing motion-game

#### Binaries

motion-game is available as a gem:

```
$ gem install motion-game
```

#### Source

You can build your own copy of motion-game:

- Make sure you have your system set up for Android develpment: `motion android-setup --api_version=16`.

```
$ git clone https://github.com/HipByte/motion-game.git && cd motion-game
$ git submodule update --init
$ bundle
$ rake build:setup

$ rake gem
$ gem install motion-game-x.x.gem
```

#### Developing Locally

Once you have the source locally. Create a motion-game project as a sibling to the `motion-game` directory using the `motion create --template=motion-game PROJECTNAME` command.

Update the reference to `motion-game` in your `Gemfile` to point to your local instance of `motion-game`:

    gem 'motion-game', path: '../motion-game'

Chances are all of you changes will be done in the `/src` directory (since it'll probably be adding missing Cocos2dx methods).

Once you've made your change in the correct `.cpp` file, run `rake build:ios`/`rake build:android` to test your changes locally (running `rake gem` is a longer process which you don't have to do).

Here are some examples of how to add missing apis to motion-game:

- [add Draw#triange](https://github.com/HipByte/motion-game/commit/3c32771be11c5715a4922ba45914207b2c6f4d38)
- [add Draw#line](https://github.com/HipByte/motion-game/commit/972fd115d3ef2816c19618b14823363356ca85b1)
- [add Draw#clear](https://github.com/HipByte/motion-game/commit/98cc463724153bae1481a9364ef3f166e15f8c0f)

### Hello World

```
$ motion create --template=motion-game HelloWorld
$ cd HelloWorld
```

#### iOS

```
$ rake ios:simulator
$ rake ios:device
```

#### tvOS

```
$ rake tvos:simulator
$ rake tvos:device
```

#### Android

```
$ rake android:emulator
$ rake android:device
```

### API reference

The whole framework API is documented. The [API reference](http://www.rubydoc.info/gems/motion-game/1.1.10) is available online.

You can also build the API reference locally:

```
$ rake doc
$ open doc/index.html
```

### Samples

The [samples](https://github.com/HipByte/motion-game/tree/master/samples) directory contains sample projects.

## License

Copyright (c) 2015, HipByte (info@hipbyte.com) and contributors. 
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met: 

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer. 
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution. 

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
