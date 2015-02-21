class RootViewController < UIViewController
  def supportedInterfaceOrientations
    case MC::Application.orientation
      when :portrait
        UIInterfaceOrientationMaskPortrait
      when :landscape_left
        UIInterfaceOrientationMaskLandscapeLeft
      when :landscape_right
        UIInterfaceOrientationMaskLandscapeRight
      when :portrait_upside_down
        UIInterfaceOrientationMaskPortraitUpsideDown
      when :landscape
        UIInterfaceOrientationMaskLandscape
      when :all
        UIInterfaceOrientationMaskAll
      when :landscape_right
        UIInterfaceOrientationMaskLandscapeRight
      when :all_but_upside_down
        UIInterfaceOrientationMaskAllButUpsideDown
    end
  end

  def shouldAutorotate
    true
  end

  def didRotateFromInterfaceOrientation(orientation)
    super
    MC::Application.instance._screen_size_changed
  end

  def prefersStatusBarHidden
    true
  end
end
