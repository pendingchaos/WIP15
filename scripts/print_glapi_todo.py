#!/usr/bin/env python
# -*- coding: utf-8 -*-
import glapi.glxml
from gl_state import *
from glapi.glapi import *

for fname, func in func_dict.iteritems():
    for param in func.params:
        if isinstance(param.dtype, tGLenum) and param.group==None:
            print "Warning: Parameter %s of %s is probably missing a group" % (param.name, fname)

replay_func_impls_str = open("nontrivial_func_impls.c").read()
implemented = set()

current_name = ""
print_func = False
for line in replay_func_impls_str.split("\n"):
    if line.split('//')[0].rstrip().endswith(":") and line.startswith("gl"):
        if len(current_name) != 0:
            implemented.add(current_name)
        current_name = line.split('//')[0].rstrip()[:-1]
if current_name != "":
    implemented.add(current_name)

versions = [(1, 0), (1, 1), (1, 2), (1, 3), (1, 4), (1, 5), (2, 0), (2, 1), (3, 0), (3, 1),
            (3, 2), (3, 3), (4, 0), (4, 1), (4, 2), (4, 3), (4, 4), (4, 5), None]
num_implemented = {v:0 for v in versions}
num_not_implemented = {v:0 for v in versions}
todo = {v:[] for v in versions}
for func_name, func in func_dict.iteritems():
    if func_name not in implemented:
        todo[func.minver].append(func_name)
        for ver in versions[versions.index(func.minver):]:
            num_not_implemented[ver] += 1
    else:
        for ver in versions[versions.index(func.minver):]:
            num_implemented[ver] += 1

for ver in versions:
    for f in todo[ver]:
        if ver == None:
            print 'TODO other: %s' % f
        else:
            print 'TODO %d.%d: %s' % (ver[0], ver[1], f)

for ver in versions:
    if ver == None:
        print 'Other:'
    else:
        print '%d.%d:' % ver
    print '    %.0f%c functions implemented' % (num_implemented[ver]/float(num_implemented[ver]+num_not_implemented[ver])*100, '%')