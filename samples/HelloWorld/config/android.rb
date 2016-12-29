# -*- coding: utf-8 -*-

begin
  require 'bundler'
  Bundler.require(:default, :android)
rescue LoadError
end

Motion::Project::App.setup do |app|
  # Use `rake android:config' to see complete project settings.
  app.name = 'HelloWorld'
  app.vm_debug_logs = true
end
