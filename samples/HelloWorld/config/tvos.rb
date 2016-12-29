# -*- coding: utf-8 -*-

begin
  require 'bundler'
  Bundler.require(:default, :tvos)
rescue LoadError
end

Motion::Project::App.setup do |app|
  # Use `rake tvos:config' to see complete project settings.
  app.name = 'HelloWorld'
end
