#!/usr/bin/env python
# -*- coding: utf-8 -*-
from glapi.glapi import *
import hashlib

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

extensions = gl.extensions.items()
ext_names = [e[0] for e in extensions]

next_req_id = 0

i = 0
for name, group in group_dict.items():
    for vn, vv in group.vals.items():
        if group.minvers[vn]:
            ver_mask = "|".join([ver_to_mask[ver] for ver in versions[versions.index(group.minvers[vn]):]])
        else:
            ver_mask = "0"
        
        exts = [ext_names.index(ext_name) for ext_name in group.exts[vn]]
        
        output.write("static const size_t req_%d_exts[] = {%s};\n" % (next_req_id, ', '.join([str(v) for v in exts])))
        output.write("static const glapi_requirements_t req_%d = {%s, %d, req_%d_exts};\n" %
                     (next_req_id, ver_mask, len(exts), next_req_id))
        
        output.write("static const glapi_group_entry_t entry_%d_%d = {&req_%d, %du, \"%s\"};\n" %
                     (i, id(vn), next_req_id, vv, vn))
        
        next_req_id += 1
    
    output.write("static const glapi_group_entry_t* entries_%d[] = {%s};\n" %
                 (i,
                  ", ".join(["&entry_%d_%d" % (i, id(vn)) for vn in list(group.vals.keys())])))
    
    output.write("static const glapi_group_t group_%s = {%s, \"%s\", %d, entries_%d};\n" %
                 (group.name,
                  str(group.bitmask).lower(),
                  name,
                  len(list(group.vals.keys())),
                  i))
    
    i += 1

i = 0

def gen_dtype(group, array_count, base):
    group = ("&group_%s" % group.name) if group != None else "NULL"
    is_array = "true" if array_count != None else "false"
    base_str = {'BASE_VOID': 'GLApi_Void',
                'BASE_UNSIGNED_INT': 'GLApi_UInt',
                'BASE_INT': 'GLApi_Int',
                'BASE_PTR': 'GLApi_Ptr',
                'BASE_BOOL': 'GLApi_Boolean',
                'BASE_FLOAT': 'GLApi_Double',
                'BASE_DOUBLE': 'GLApi_Double',
                'BASE_STRING': 'GLApi_Str',
                'BASE_DATA': 'GLApi_Data',
                'BASE_FUNC_PTR': 'GLApi_FunctionPtr',
                'BASE_VARIANT': 'GLApi_Variant'}[base.__class__.base if base else 'BASE_VOID']
    obj_type = 'GLApi_NoObj'
    if base and 'obj_type' in dir(base.__class__):
        obj_type = {'buffer': 'GLApi_Buffer',
                    'sampler': 'GLApi_Sampler',
                    'texture': 'GLApi_Texture',
                    'query': 'GLApi_Query',
                    'framebuffer': 'GLApi_Framebuffer',
                    'renderbuffer': 'GLApi_Renderbuffer',
                    'sync': 'GLApi_Sync',
                    'program': 'GLApi_Program',
                    'program_pipeline': 'GLApi_ProgramPipeline',
                    'shader': 'GLApi_Shader',
                    'vao': 'GLApi_VAO',
                    'transform_feedback': 'GLApi_TransformFeedback',
                    'context': 'GLApi_Context'}[base.__class__.obj_type]
    return '(glapi_dtype_t){%s, %s, %s, %s}' % (group, is_array, base_str, obj_type)

for name, func in func_dict.items():
    for arg in func.params:
        dtype = gen_dtype(arg.group, arg.array_count, arg.dtype)
        output.write("static const glapi_arg_t arg_%d = {\"%s\", %s};\n" %
                     (id(arg), arg.name, dtype))
    
    if func.minver != None:
        ver_mask = "|".join([ver_to_mask[ver] for ver in versions[versions.index(func.minver):]])
    else:
        ver_mask = "glnone"
    
    output.write("static const glapi_arg_t* args_%d[] = {%s};\n" %
                 (i,
                  ", ".join(["&arg_%d" % id(arg) for arg in func.params])))
    
    #TODO: Extensions
    output.write("static const glapi_requirements_t req_%d = {%s, 0, NULL};\n" %
                 (next_req_id, ver_mask))
    
    output.write("static const glapi_function_t func_%d = {&req_%d, \"%s\", %d, args_%d, %s};\n" %
                 (id(func), next_req_id, name, len(func.params), i,
                  gen_dtype(None, None, func.rettype)))
    
    next_req_id += 1
    
    i += 1

for name, ext in extensions:
    output.write("static const glapi_extension_t ext_%d = {\"%s\"};\n" % (id(ext), name))

output.write("static glapi_group_t*const* groups[] = {%s};\n" %
             ", ".join(["&group_%s" % group.name for group in list(group_dict.values())]))

output.write("static glapi_function_t** funcs[] = {%s};\n" %
             ", ".join(["&func_%d" % id(func) for func in list(func_dict.values())]))

output.write("static glapi_extension_t** extensions[] = {%s};\n" %
             ", ".join(["&ext_%d" % id(ext) for ext in list(gl.extensions.values())]))

output.write("const glapi_t glapi = {%d, groups, %d, funcs, %d, extensions};\n" % (len(group_dict), len(func_dict), len(gl.extensions)))
