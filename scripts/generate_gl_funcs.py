#!/usr/bin/env python
# -*- coding: utf-8 -*-
import glxml

gl = glxml.GL(False)

output = open('generated_gl_funcs.py', 'w')

vers = [(1, 0), (1, 1), (1, 2), (1, 3), (1, 4), (1, 5), (2, 0), (2, 1),
        (3, 0), (3, 1), (3, 2), (3, 3), (4, 0), (4, 1), (4, 2), (4, 3),
        (4, 4), (4, 5)]

funcs = []
for v in vers:
    funcs += gl.versions[v].new_functions
    for r in gl.versions[v].removed_functions:
        funcs.remove(r)

for name in funcs:
    func = gl.functions[name]
    
    output.write('Func(\'%s\', [' % name)
    
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
        
        params.append(res + ', \'%s\', %s)' % (param.name, array_count))
    
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
