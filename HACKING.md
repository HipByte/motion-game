= Before building motion-game:

Patch ``sources/cxx-stl/gnu-libstdc++/4.9/include/cstddef`` with http://reviews.llvm.org/rL201729

= How to build

$ git clone -b tvos https://github.com/elvman/cocos2d-x ext/cocos2d-x
$ python ext/cocos2d-x download-deps.py
$ rake build


