#!/usr/bin/env python
# -*- coding: utf-8 -*-
# File   : .ycm_extra_osx_conf.py
# License: MIT/X11
# Author : Dries Pauwels <2mjolk@gmail.com>
# Date   : do 13 sep 2018 10:24
import os
import ycm_core

# These are the compilation flags that will be used in case there's no
# compilation database set (by default, one is not set).
# CHANGE THIS LIST OF FLAGS. YES, THIS IS THE DROID YOU HAVE BEEN LOOKING FOR.
flags = [
    '-Wall',
    '-Wextra',
    '-Werror',
    '-Wno-long-long',
    '-Wno-variadic-macros',
    '-fexceptions',
    '-DNDEBUG',
    '-DUSE_CLANG_COMPLETER',
    # THIS IS IMPORTANT! Without a "-std=<something>" flag, clang won't know which
    # language to use when compiling headers. So it will guess. Badly. So C++
    # headers will be compiled as C headers. You don't want that so ALWAYS specify
    # a "-std=<something>".
    # For a C project, you would set this to something like 'c99' instead of
    # 'c++11'.
    '-std=gnu11',
    # ...and the same thing goes for the magic -x option which specifies the
    # language that the files to be compiled are written in. This is mostly
    # relevant for c++ headers.
    # For a C project, you would set this to 'c' instead of 'c++'.
    '-x', 'c'
    # This path will only work on OS X, but extra paths that don't exist are not
    # harmful
    #'-isystem',
    #'/usr/local/include',
    #'-I',
    #'/usr/local/include',
    #'-I',
    #'include',
    #'-I',
    #'.'
]

def FlagsForFile( filename, **kwargs ):
    return {
        'flags': flags,
        'do_cache': True
    }
