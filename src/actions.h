#ifndef LIB_INSPECT_ACTIONS_H
#define LIB_INSPECT_ACTIONS_H
#include "libinspect.h"

void inspect_act_gen_tex(inspect_gl_state_t* state, unsigned int id);
void inspect_act_del_tex(inspect_gl_state_t* state, unsigned int id);
void inspect_act_tex_params(inspect_gl_state_t* state, unsigned int id, inspect_gl_tex_params_t* params);
void inspect_act_tex_data(inspect_gl_state_t* state, unsigned int id, size_t mipmap, size_t size, const void* data);
void inspect_act_gen_buf(inspect_gl_state_t* state, unsigned int id);
void inspect_act_del_buf(inspect_gl_state_t* state, unsigned int id);
void inspect_act_buf_data(inspect_gl_state_t* state, unsigned int id, size_t size, const void* data, unsigned int usage);
void inspect_act_buf_sub_data(inspect_gl_state_t* state, unsigned int id, size_t offset, size_t size, const void* data);
void inspect_act_new_shdr(inspect_gl_state_t* state, unsigned int id, unsigned int type);
void inspect_act_del_shdr(inspect_gl_state_t* state, unsigned int id);
void inspect_act_shdr_source(inspect_gl_state_t* state, unsigned int id, size_t count, const char *const* sources);
void inspect_act_new_prog(inspect_gl_state_t* state, unsigned int id);
void inspect_act_del_prog(inspect_gl_state_t* state, unsigned int id);
void inspect_act_attach_shdr(inspect_gl_state_t* state, unsigned int prog, unsigned int shdr);
void inspect_act_detach_shdr(inspect_gl_state_t* state, unsigned int prog, unsigned int shdr);
void inspect_act_set_shdr_info_log(inspect_gl_state_t* state, unsigned int id, const char* info_log);
void inspect_act_set_prog_info_log(inspect_gl_state_t* state, unsigned int id, const char* info_log);
#endif
