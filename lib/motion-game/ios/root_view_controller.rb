class RootViewController < UIViewController
  def shouldAutorotate
    true
  end

  def didRotateFromInterfaceOrientation(orientation)
    super
    MC::Application.shared._screen_size_changed
  end

  def prefersStatusBarHidden
    true
  end
end
