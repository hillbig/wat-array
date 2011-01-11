VERSION = '0.0.6'
APPNAME = 'wat_array'

srcdir = '.'
blddir = 'build'

def set_options(ctx):
  ctx.tool_options('compiler_cxx')
  ctx.tool_options('unittestt')

def configure(ctx):
  ctx.check_tool('compiler_cxx')
  ctx.check_tool('unittestt')	
  ctx.env.CXXFLAGS += ['-O2', '-Wall', '-W', '-g']

import Scripting
Scripting.dist_exts += ['.sh']

def dist_hook():
    import os
    os.remove('googlecode_upload.py')

def build(bld):
  bld.recurse('src test performance_test')
  bld.install_files('/usr/lib/pkgconfig', 'wat_array.pc')