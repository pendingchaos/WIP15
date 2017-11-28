#!/usr/bin/env python
# -*- coding: utf-8 -*-
from glapi.glapi import *
import sys

class FileWriter(object):
    def __init__(self, fileobj, fname):
        self.fileobj = fileobj
        self.fname = fname
        self.lines_written = 0
        
        self.current_fname = fname
        self.current_line = 1
    
    def set_line_spec(self, line, fname):
        # NOTE: This does not escape the filename
        self.fileobj.write('#line %d "%s"\n' % (line, fname))
        self.lines_written += 1
    
    def reset_line_spec(self):
        # NOTE: This does not escape the filename
        self.fileobj.write('#line %d "%s"\n' % (self.lines_written+2, self.fname))
        self.lines_written += 1
    
    def write_lines(self, text, line=None, fname=None):
        if line!=self.current_line or fname!=self.current_fname:
            if line!=None or fname!=None:
                self.set_line_spec(line, fname)
            else:
                self.reset_line_spec()
            self.current_line = line
            self.current_fname = fname
        
        for line in text.split('\n'):
            self.fileobj.write(line + '\n')
            self.lines_written += 1
            if self.current_line != None:
                self.current_line += 1

def generate_replay_wrapper(writer, name, impl, impl_line, impl_fname):
    func = func_dict[name]
    
    writer.write_lines('''#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"

__attribute__((visibility("default"))) void replay_%s(trace_command_t* cmd_) {
    cmd = cmd_;''' % name)
    
    if not name.startswith('glX') and not name.startswith('wip15'):
        writer.write_lines('''    if (!trc_get_current_gl_context(ctx->trace, -1)) {
        trc_add_error(cmd, "No current OpenGL context.");
        return;
    } else {
        trc_obj_t* cur_ctx = trc_get_current_gl_context(ctx->trace, -1);
        ctx->ns = ((const trc_gl_context_rev_t*)trc_obj_get_rev(cur_ctx, -1))->namespace;
        ctx->priv_ns = ((const trc_gl_context_rev_t*)trc_obj_get_rev(cur_ctx, -1))->priv_ns;
    }''')
    else:
        writer.write_lines('    ctx->ns = NULL;')
    
    writer.write_lines('    replay_begin_cmd(ctx, \"%s\", cmd);' % name)
    
    if name == 'glXGetProcAddress':
        writer.write_lines('    %s_t real = &%s;' % (name, name))
    else:
        writer.write_lines('    %s_t real = ((replay_gl_funcs_t*)ctx->_replay_gl)->real_%s;' % (name, name))
    
    for i, param in zip(list(range(len(func.params))), func.params):
        writer.write_lines('    trace_value_t* arg_%s = &cmd->args[%d];' % (param.name, i))
        writer.write_lines(param.dtype.gen_replay_read_code('p_'+param.name, 'arg_'+param.name, param.array_count))
    
    writer.write_lines(impl, impl_line, impl_fname)
    
    #The `goto end;` is to remove "label defined but not used" warnings
    writer.write_lines('goto end; end: ;\n')
    
    for i, param in zip(list(range(len(func.params))), func.params):
        writer.write_lines(param.dtype.gen_replay_finalize_code('p_'+param.name, 'arg_'+param.name, param.array_count))
    
    writer.write_lines('''replay_end_cmd(ctx, \"%s\", cmd);
#undef FUNC
#define FUNC \"%s\"
}
#pragma GCC diagnostic pop''' % (name, name))

input_fname = sys.argv[1]
output_fname = sys.argv[2]
output = open(output_fname, 'w')
writer = FileWriter(output, output_fname);

writer.write_lines('#include "libtrace/replay.h"')

lines = open(input_fname).read().split('\n')
lines = zip([i+1 for i in range(len(lines))], lines)
while len(lines) > 0:
    num, line = lines.pop(0)
    is_replay_decl = line.split('//')[0].rstrip().endswith(':')
    is_replay_decl = is_replay_decl and not (line.startswith(' ') or line.startswith('\t'))
    if is_replay_decl:
        name = line.split('//')[0].rstrip()[:-1]
        impl = ''
        while len(lines) > 0:
            _, peek = lines[0]
            if peek.startswith(' ') or peek.startswith('\t'):
                impl += peek + '\n'
                lines.pop(0)
            else:
                break
        
        generate_replay_wrapper(writer, name, impl, num+1, input_fname)
    else:
        writer.write_lines(line, num, input_fname)
