#!/usr/bin/env python
# -*- coding: utf-8 -*-
import math
from glapi.glapi import *
from glob import glob

for fname, func in func_dict.items():
    for param in func.params:
        if isinstance(param.dtype, tGLenum) and param.group==None:
            print("Warning: Parameter %s of %s is probably missing a group" % (param.name, fname))

exceptions = {}
for fname, func in func_dict.items():
    for param in func.params:
        if param.dtype.__class__ != tGLuint: continue
        if param.name in exceptions.get(fname, []): continue
        for s in ['buf', 'sampler', 'tex', 'query', 'framebuffer', 'fb', 'renderbuffer', 'rb', 'sync', 'program', 'pipeline', 'shader', 'vao', 'vaobj', 'array', 'id', 'xfb']:
            if s in param.name:
                print 'Note: Parameter %s of %s might be an object name' % (param.name, fname)
                break

replay_func_impls_str = '\n\n'.join([open(f).read() for f in glob('../src/libtrace/replay/*.c')])
implemented = set()

current_name = ""
for line in replay_func_impls_str.split("\n"):
    if line.split('//')[0].rstrip().endswith(":") and (line.startswith("gl") or line.startswith("wip15")):
        if len(current_name) != 0:
            implemented.add(current_name)
        current_name = line.split('//')[0].rstrip()[:-1]
if current_name != "":
    implemented.add(current_name)

versions = [(1, 0), (1, 1), (1, 2), (1, 3), (1, 4), (1, 5), (2, 0), (2, 1), (3, 0), (3, 1),
            (3, 2), (3, 3), (4, 0), (4, 1), (4, 2), (4, 3), (4, 4), (4, 5), (4, 6), None]
num_implemented = {v:0 for v in versions}
num_not_implemented = {v:0 for v in versions}
done = 0
not_done = 0
todo = {v:[] for v in versions}
for func_name, func in func_dict.items():
    if func_name not in implemented:
        todo[func.minver].append(func_name)
        for ver in versions[versions.index(func.minver):]:
            if ver==None and func.minver != None: continue
            num_not_implemented[ver] += 1
        not_done += 1
    else:
        for ver in versions[versions.index(func.minver):]:
            if ver==None and func.minver != None: continue
            num_implemented[ver] += 1
        done += 1

for ver in versions:
    for f in sorted(todo[ver]):
        if ver == None:
            print('TODO other: %s' % f)
        else:
            print('TODO %d.%d: %s' % (ver[0], ver[1], f))

for ver in versions:
    if ver == None:
        print('Other:')
    else:
        print('%d.%d:' % ver)
    print('    %.0f%c functions implemented' % (math.floor(num_implemented[ver]/float(num_implemented[ver]+num_not_implemented[ver])*100), '%'))

print('All:')
print('    %.0f%c' % (math.floor(done/float(done+not_done)*100), '%'))
