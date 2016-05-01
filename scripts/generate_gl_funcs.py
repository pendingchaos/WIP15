#!/usr/bin/env python
# -*- coding: utf-8 -*-
import glapi.glxml

gl = glapi.glxml.GL(False)

output = open('generated_gl_funcs.py', 'w')

vers = [(1, 0), (1, 1), (1, 2), (1, 3), (1, 4), (1, 5), (2, 0), (2, 1),
        (3, 0), (3, 1), (3, 2), (3, 3), (4, 0), (4, 1), (4, 2), (4, 3),
        (4, 4), (4, 5)]

func_names = []
funcs = []
for v in vers:
    for n in gl.versions[v].new_functions:
        if n not in func_names:
            funcs.append(n)
            func_names.append(n)
    
    for r in gl.versions[v].removed_functions:
        funcs.remove(r)

enums = []
for v in vers:
    enums += gl.versions[v].new_enums
    for r in gl.versions[v].removed_enums:
        try: enums.remove(r)
        except ValueError: pass

groups = {}
for f in funcs:
    for p in gl.functions[f].params:
        if p.group != None: groups[p.group] = []

func_minvers = {}
for vn, v in gl.versions.iteritems():
    for gn, g in gl.groups.iteritems():
        if gn not in groups: continue
        for e in g.enumNames:
            if e in v.new_enums: groups[gn].append(e)
    for f in v.new_functions:
        func_minvers[f] = vn

for f in gl.functions.keys():
    if f.startswith('glX'):
        funcs.append(f)

for name, group in groups.iteritems():
    output.write('Group(\'%s\')' % name)
    for en in group:
        ver = (3, 2)
        for v in gl.versions.iteritems():
            if en in v[1].new_enums: 
                ver = v[0]
                break
        
        output.write('.add(\'%s\', %d, (%d, %d))' % (en, gl.enumValues[en], ver[0], ver[1]))
    output.write('\n')

for name in funcs:
    func = gl.functions[name]
    
    try: ver = '(%d, %d)' % (func_minvers[name][0], func_minvers[name][1])
    except: ver = 'None'
    output.write('Func(%s, \'%s\', [' % (ver, name))
    
    s = open('gl_funcs.py').read()
    s = s.replace('Func(\'%s\'' % name, 'Func(%s, \'%s\'' % (ver, name))
    open('gl_funcs.py', 'w').write(s)
    
    params = []
    for param in func.params:
        res = 'Param('
        dtype = param.type_
        
        array_count = 'None'
        if dtype[-1] == ']':
            array_count = str(eval(dtype.split('[')[1][:-1]))
            dtype = dtype.split('[')[0]
        
        if dtype.replace(' ', '') == 'constGLchar*const*':
            res += 'tPointer'
        elif dtype.replace(' ', '') == 'constGLchar**':
            res += 'tPointer'
        elif dtype.replace(' ', '') == 'constGLcharARB**':
            res += 'tPointer'
        elif '*' in dtype:
            if dtype.replace(' ', '') == 'constGLchar*': res += 'tString'
            elif dtype.replace(' ', '') == 'GLchar*': res += 'tMutableString'
            elif 'const' in dtype: res += 'tPointer'
            else: res += 'tMutablePointer'
        else:
            res += 't' + dtype.replace('const', '').replace(' ', '').lstrip().rstrip()
        
        params.append(res + ', \'%s\', %s, %s)' % (param.name, array_count, repr(param.group)))
    
    output.write(', '.join(params) + ']')
    
    if func.returnType != 'void':
        if func.returnType.replace(' ', '') == 'GLchar*':
            output.write(', tString')
        elif '*' in func.returnType:
            output.write(', tPointer')
        else:
            output.write(', t%s' % func.returnType.replace('const', '').replace(' ', '').lstrip().rstrip())
    else:
        output.write(', None')
    
    output.write(')\n')
