# motion-game

motion-game is a cross-platform mobile game engine for RubyMotion. It lets you write mobile games in Ruby for iOS and Android.

## Features

 * **Use Ruby :-)** motion-game exposes a pure Ruby API to write games for mobile devices.
 * **100% cross-platform**: motion-game projects are fully cross-platform for iOS and Android. One codebase runs everywhere.
 * **Fully-featured**: motion-game has sprites, animations, particles, device sensors / events, scene graph / director, UI widgets, etc.
 * **Solid foundations**: the motion-game API is implemented using popular and stable opensource libraries, such as [cocos2d-x](http://www.cocos2d-x.org/), [box2d](http://box2d.org/), and more. motion-game projects are also based on RubyMotion which offers portable Ruby runtimes for iOS and Android as well as static compilation of Ruby code.
 * **Native compilation**: your Ruby code will be compiled into optimized native code for each platform you target. There is no interpreter and the original Ruby code will not be in the app.
 * **Platform APIs access**: if you need it, you can call the entire set of public iOS or Android APIs from Ruby code as well.

## Getting Started

 * [RubyMotion](http://rubymotion.com) is required. A starter version can be downloaded for free. You also need to set up your computer for mobile development (iOS and/or Android). Follow the [Getting Started](http://rubymotion.com/developers) guides after installation.
 * Install motion-game: `$ gem install motion-game`

## Hello World

```
$ motion create --template=motion-game HelloWorld
$ cd HelloWorld
$ rake ios:simulator
$ rake ios:device
$ rake android:emulator
$ rake android:device
```

## Samples

Check out the [HipByte/motion-game-samples](https://github.com/HipByte/motion-game-samples) GitHub repository for samples.

## About

motion-game is developed by HipByte, the company that makes RubyMotion. It is currently available free of charge.
