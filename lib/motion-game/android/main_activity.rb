class MainActivity < Org::Cocos2dx::Lib::Cocos2dxActivity
  def onResume
    super
    director = MG::Director.shared
    director.resume
    director.start_animation
  end

  def onPause
    super
    director = MG::Director.shared
    director.pause
    director.stop_animation
  end    
end
