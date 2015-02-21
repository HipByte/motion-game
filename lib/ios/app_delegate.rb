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

    MC::Director.instance._set_glview(glview)
    MC::Application.instance.run

    true
  end
end
