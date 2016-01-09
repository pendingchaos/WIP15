#ifndef LIB_INSPECT_ACTIONS_H
#define LIB_INSPECT_ACTIONS_H
#include "libinspect.h"
#include "shared/uint.h"

void inspect_act_gen_tex(inspect_gl_state_t* state, uint id);
void inspect_act_del_tex(inspect_gl_state_t* state, uint id);
void inspect_act_tex_params(inspect_gl_state_t* state, uint id, inspect_gl_tex_params_t* params);
void inspect_act_tex_data(inspect_gl_state_t* state, uint id, size_t mipmap, size_t w, size_t h, const void* data);
void inspect_act_gen_buf(inspect_gl_state_t* state, uint id);
void inspect_act_del_buf(inspect_gl_state_t* state, uint id);
void inspect_act_buf_data(inspect_gl_state_t* state, uint id, size_t size, const void* data, uint usage);
void inspect_act_buf_sub_data(inspect_gl_state_t* state, uint id, size_t offset, size_t size, const void* data);
void inspect_act_new_shdr(inspect_gl_state_t* state, uint id, uint type);
void inspect_act_del_shdr(inspect_gl_state_t* state, uint id);
void inspect_act_shdr_source(inspect_gl_state_t* state, uint id, size_t count, const char *const* sources);
void inspect_act_new_prog(inspect_gl_state_t* state, uint id);
void inspect_act_del_prog(inspect_gl_state_t* state, uint id);
void inspect_act_attach_shdr(inspect_gl_state_t* state, uint prog, uint shdr);
void inspect_act_detach_shdr(inspect_gl_state_t* state, uint prog, uint shdr);
void inspect_act_set_shdr_info_log(inspect_gl_state_t* state, uint id, const char* info_log);
void inspect_act_set_prog_info_log(inspect_gl_state_t* state, uint id, const char* info_log);
void inspect_act_add_prog_uniform(inspect_gl_state_t* state, uint id, uint fake, uint real);
void inspect_act_gen_vao(inspect_gl_state_t* state, uint id);
void inspect_act_del_vao(inspect_gl_state_t* state, uint id);
void inspect_act_set_vao(inspect_gl_state_t* state, uint id, size_t count, inspect_vertex_attrib_t* attribs);
void inspect_act_gen_fb(inspect_gl_state_t* state, uint id);
void inspect_act_del_fb(inspect_gl_state_t* state, uint id);
void inspect_act_fb_depth(inspect_gl_state_t* state, uint fb, uint tex, uint level);
void inspect_act_fb_stencil(inspect_gl_state_t* state, uint fb, uint tex, uint level);
void inspect_act_fb_depth_stencil(inspect_gl_state_t* state, uint fb, uint tex, uint level);
void inspect_act_fb_color(inspect_gl_state_t* state, uint fb, uint attachment, uint tex, uint level);
void inspect_act_gen_rb(inspect_gl_state_t* state, uint id);
void inspect_act_del_rb(inspect_gl_state_t* state, uint id);
void inspect_act_set_rb(inspect_gl_state_t* state, inspect_rb_t* data);
void inspect_act_gen_sync(inspect_gl_state_t* state, uint64_t id);
void inspect_act_del_sync(inspect_gl_state_t* state, uint64_t id);
void inspect_act_set_sync(inspect_gl_state_t* state, inspect_sync_t* data);
void inspect_act_gen_query(inspect_gl_state_t* state, uint id);
void inspect_act_del_query(inspect_gl_state_t* state, uint id);
void inspect_act_set_query(inspect_gl_state_t* state, inspect_query_t* data);
#endif
