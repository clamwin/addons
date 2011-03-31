#!/usr/bin/env python
from distutils.core import setup, Extension
from sys import platform
from os import environ

sources = ['pyhelpers.c', 'toolhelprobj.c']
if platform == 'win32':
    sources.append('win32ops.c')
    environ['DISTUTILS_USE_SDK'] = '1'
    environ['MSSdk'] = '.'

pyhelpers = Extension('pyhelpers', sources=sources)

setup (name = 'pyhelpers',
       version = '1.0',
       author = 'Gianluigi Tiesi',
       author_email = 'sherpya@netfarm.it',
       license ='GPL',
       keywords="python, win32, dbghelp",
       ext_modules = [ pyhelpers ])
