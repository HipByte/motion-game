class RootViewController < UIViewController
  def shouldAutorotate
    true
  end

  def didRotateFromInterfaceOrientation(orientation)
    super
    MG::Application.shared._screen_size_changed
  end

  def prefersStatusBarHidden
    true
  end
end
