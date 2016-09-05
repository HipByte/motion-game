class MainActivity < Org::Cocos2dx::Lib::Cocos2dxActivity
  def onResume
    super
    if @called_first_resume
      # ignore onResume calling when launched app to call :resume method as well as iOS/tvOS platform
      director = MG::Director.shared
      director.resume
      director.start_animation
      app = MG::Application.shared
      app.send(:resume) if app.respond_to?(:resume)
    end
    @called_first_resume = true
  end

  def onPause
    super
    director = MG::Director.shared
    director.pause
    director.stop_animation
    app = MG::Application.shared
    app.send(:pause) if app.respond_to?(:pause)
  end    
end
