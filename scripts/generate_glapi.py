#!/usr/bin/env python
# -*- coding: utf-8 -*-
from glapi.glapi import *

versions = [(1, 0),
            (1, 1),
            (1, 2),
            (1, 3),
            (1, 4),
            (1, 5),
            (2, 0),
            (2, 1),
            (3, 0),
            (3, 1),
            (3, 2),
            (3, 3),
            (4, 0),
            (4, 1),
            (4, 2),
            (4, 3),
            (4, 4),
            (4, 5),
            (4, 6)]

ver_to_mask = {(1, 0):"gl1_0",
               (1, 1):"gl1_1",
               (1, 2):"gl1_2",
               (1, 3):"gl1_3",
               (1, 4):"gl1_4",
               (1, 5):"gl1_5",
               (2, 0):"gl2_0",
               (2, 1):"gl2_1",
               (3, 0):"gl3_0",
               (3, 1):"gl3_1",
               (3, 2):"gl3_2",
               (3, 3):"gl3_3",
               (4, 0):"gl4_0",
               (4, 1):"gl4_1",
               (4, 2):"gl4_2",
               (4, 3):"gl4_3",
               (4, 4):"gl4_4",
               (4, 5):"gl4_5",
               (4, 6):"gl4_6"}

output = open("../src/shared/glapi.c", "w")

output.write("""//Generated by generate_glapi.py. Do not edit. Edit generate_glapi.py instead.
#include "glapi.h"

#pragma GCC diagnostic ignored "-Wincompatible-pointer-types"
#pragma GCC diagnostic ignored "-Woverflow"

""")

next_req_id = 0

i = 0
for name, group in group_dict.iteritems():
    for vn, vv in group.vals.iteritems():
        ver_mask = "|".join([ver_to_mask[ver] for ver in versions[versions.index(group.minvers[vn]):]])
        
        output.write("static const glapi_requirements_t req_%d = {%s, NULL, true, true};\n" %
                     (next_req_id, ver_mask))
        
        output.write("static const glapi_group_entry_t entry_%d_%d = {&req_%d, %du, \"%s\"};\n" %
                     (i, id(vn), next_req_id, vv, vn))
        
        next_req_id += 1
    
    output.write("static const glapi_group_entry_t* entries_%d[] = {%s};\n" %
                 (i,
                  ", ".join(["&entry_%d_%d" % (i, id(vn)) for vn in group.vals.keys()])))
    
    output.write("static const glapi_group_t group_%s = {%s, \"%s\", %d, entries_%d};\n" %
                 (group.name,
                  str(group.bitmask).lower(),
                  name,
                  len(group.vals.keys()),
                  i))
    
    i += 1

i = 0

for name, func in func_dict.iteritems():
    for arg in func.params:
        group = ("&group_%s" % arg.group.name) if arg.group != None else "NULL"
        
        output.write("static const glapi_arg_t arg_%d = {\"%s\", %s};\n" %
                     (id(arg),
                      arg.name,
                      group))
    
    if func.minver != None:
        ver_mask = "|".join([ver_to_mask[ver] for ver in versions[versions.index(func.minver):]])
    else:
        ver_mask = "glnone"
    
    output.write("static const glapi_arg_t* args_%d[] = {%s};\n" %
                 (i,
                  ", ".join(["&arg_%d" % id(arg) for arg in func.params])))
    
    output.write("static const glapi_requirements_t req_%d = {%s, NULL, %s, %s};\n" %
                 (next_req_id,
                  ver_mask,
                  "true",
                  "true"))
    
    output.write("static const glapi_function_t func_%d = {&req_%d, \"%s\", %d, args_%d};\n" %
                 (id(func),
                  next_req_id,
                  name,
                  len(func.params),
                  i))
    
    next_req_id += 1
    
    i += 1

output.write("static glapi_group_t*const* groups[] = {%s};\n" %
             ", ".join(["&group_%s" % group.name for group in group_dict.values()]))

output.write("static glapi_function_t** funcs[] = {%s};\n" %
             ", ".join(["&func_%d" % id(func) for func in func_dict.values()]))

output.write("const glapi_t glapi = {%d, groups, %d, funcs};\n" %
             (len(group_dict),
              len(func_dict)))
