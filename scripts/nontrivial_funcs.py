#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Finds potential non-trivial functions.
import glxml
import string

gl = glxml.GL(False)

class Function(object):
    def __init__(self, func):
        self.versions = []
        self.extension = None
        self.func = func

functions = {}

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
            (4, 5)]

for k, v in gl.functions.iteritems():
    functions[k] = Function(v)

for ver_num in versions:
    ver = gl.versions[ver_num]
    vers = versions[versions.index(ver_num):]
    
    for func in ver.new_functions:
        for other_ver in vers:
            functions[func].versions.append(other_ver)
    
    for func in ver.removed_functions:
        for other_ver in vers:
            functions[func].versions.remove(other_ver)

for k, v in gl.extensions.iteritems():
    for func in v.functions:
        if not func.startswith("glX"):
            functions[func].extension = k

funcs = {}
funcs_noext = []

objects = ["GLsync",
           "GLDEBUGPROC",
           "GLDEBUGPROCARB",
           "GLeglImageOES",
           "GLvdpauSurfaceNV",
           "GLXDEBUGPROCKHR",
           "GLDEBUGPROCAMD",
           "GLXPixmap",
           "GLXWindow",
           "GLXPbuffer",
           "GLXDrawable",
           "GLXVideoDeviceNV",
           "Pixmap",
           "Window",
           "Font",
           "Colormap",
           "GLXContextID",
           "GLXFBConfig",
           "GLXVideoCaptureDeviceNV",
           "GLXFBConfigSGIX",
           "GLXPbufferSGIX",
           "GLXVideoSourceSGIX",
           "GLXContext",
           "GLuint",
           "GLhandleARB"]

for name, func in functions.iteritems():
    if name.startswith("glUniform") or name.startswith("glVertexAttrib"):
        continue
    
    nontrivial = False
    
    if name.startswith("glGet"):
        nontrivial = True
    
    for param in func.func.params:
        if param.type_ in objects and param.group == None:
            nontrivial = True
        elif "*" in param.type_:
            nontrivial = True
    
    if func.func.returnType in objects:
        nontrivial = True
    elif "*" in func.func.returnType:
        nontrivial = True
    
    if not nontrivial:
        continue
    
    postfix = ""
    for c in name[::-1]:
        if c in string.ascii_uppercase:
            postfix += c
        else:
            break
    
    postfix = postfix[::-1]
    
    if postfix.startswith("D"):
        postfix = postfix[1:]
    
    content = name + "(" + ", ".join([param.name for param in func.func.params]) + ")"
    
    if len(func.versions) != 0:
        ver = func.versions[-1]
        
        content = "GL %d.%d " % ver + content
    
    if postfix == "":
        funcs_noext.append(content)
    elif postfix in funcs:
        funcs[postfix].append(content)
    else:
        funcs[postfix] = [content]

print "OpenGL:"

funcs_noext.sort()

for func in funcs_noext:
    print "    "+func

for ext, funcs_ in funcs.iteritems():
    print ext + ":"
    funcs_.sort()
    for func in funcs_:
        print "    "+func
