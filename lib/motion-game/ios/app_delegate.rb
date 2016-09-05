class AppDelegate
  def application(application, didFinishLaunchingWithOptions:launchOptions)
    @app = Application.new # important to initialize the engine

    @window = UIWindow.alloc.initWithFrame(UIScreen.mainScreen.bounds)

    rootViewController = RootViewController.alloc.init
    rootViewController.wantsFullScreenLayout = true
    glview = CCEAGLView.viewWithFrame(@window.bounds)
    rootViewController.view = glview

    @window.rootViewController = rootViewController
    @window.makeKeyAndVisible

    MG::Director.shared._set_glview(glview)
    MG::Application.shared.run

    true
  end

  def applicationWillEnterForeground(application)
    director = MG::Director.shared
    director.resume
    director.start_animation
    @app.resume if @app.respond_to?(:resume)
  end

  def applicationWillResignActive(application)
    director = MG::Director.shared
    director.pause
    director.stop_animation
    @app.send(:pause) if @app.respond_to?(:pause)
  end
end
