#include "libtrace/replay/utils.h"

static void gen_program_pipelines(size_t count, const GLuint* real, const GLuint* fake, bool create) {
    trc_gl_program_pipeline_rev_t rev;
    rev.has_object = create;
    rev.active_program = (trc_obj_ref_t){NULL};
    rev.vertex_program = (trc_obj_ref_t){NULL};
    rev.fragment_program = (trc_obj_ref_t){NULL};
    rev.geometry_program = (trc_obj_ref_t){NULL};
    rev.tess_control_program = (trc_obj_ref_t){NULL};
    rev.tess_eval_program = (trc_obj_ref_t){NULL};
    rev.compute_program = (trc_obj_ref_t){NULL};
    rev.validation_status = -1;
    
    for (size_t i = 0; i < count; ++i) {
        rev.real = real[i];
        trc_create_named_obj(ctx->priv_ns, TrcProgramPipeline, fake[i], &rev);
    }
}

bool program_has_stage(trc_obj_t* program, GLenum stage) {
    const trc_gl_program_rev_t* rev = trc_obj_get_rev(program, -1);
    trc_obj_t*const* linked = trc_map_data(rev->linked, TRC_MAP_READ);
    bool found = false;
    for (size_t i = 0; i < rev->linked->size/sizeof(linked[0]); i++) {
        trc_obj_t* ls = linked[i];
        const trc_gl_shader_rev_t* shader = trc_obj_get_rev(ls, rev->link_revision);
        if (shader->type != stage) continue;
        found = true;
        break;
    }
    trc_unmap_data(linked);
    return found;
}

trc_obj_t* get_active_program_for_stage(GLenum stage) {
    trc_obj_t* program = gls_get_bound_program();
    if (program) return program_has_stage(program, stage) ? program : NULL;
    
    trc_obj_t* pipeline = gls_get_bound_pipeline();
    if (!pipeline) return NULL;
    
    const trc_gl_program_pipeline_rev_t* rev = trc_obj_get_rev(pipeline, -1); 
    trc_obj_t* res = NULL;
    switch (stage) {
    case GL_VERTEX_SHADER: res = rev->vertex_program.obj; break;
    case GL_FRAGMENT_SHADER: res = rev->fragment_program.obj; break;
    case GL_GEOMETRY_SHADER: res = rev->geometry_program.obj; break;
    case GL_TESS_CONTROL_SHADER: res = rev->tess_control_program.obj; break;
    case GL_TESS_EVALUATION_SHADER: res = rev->tess_eval_program.obj; break;
    case GL_COMPUTE_SHADER: res = rev->compute_program.obj; break;
    default: break;
    }
    
    return res && program_has_stage(res, stage) ? res : NULL;
}

trc_obj_t* get_active_program() {
    trc_obj_t* program = gls_get_bound_program();
    if (program) return program;
    trc_obj_t* pipeline = gls_get_bound_pipeline();
    if (!pipeline) return NULL;
    const trc_gl_program_pipeline_rev_t* rev = trc_obj_get_rev(pipeline, -1); 
    return rev->active_program.obj;
}

glCreateShader: //GLenum p_type
    GLuint real_shdr = F(glCreateShader)(p_type);
    GLuint fake = trc_get_uint(&cmd->ret)[0];
    trc_gl_shader_rev_t rev;
    rev.real = real_shdr;
    rev.sources = trc_create_data(ctx->trace, 0, NULL, 0);
    rev.info_log = trc_create_data(ctx->trace, 1, "", 0);
    rev.type = p_type;
    trc_create_named_obj(ctx->ns, TrcShader, fake, &rev);

glDeleteShader: //GLuint p_shader
    if (p_shader == 0) RETURN;
    if (!p_shader_rev) ERROR("Invalid shader name");
    real(p_shader_rev->real);
    delete_obj(p_shader, TrcShader);

glShaderSource: //GLuint p_shader, GLsizei p_count, const GLchar*const* p_string, const GLint* p_length
    if (p_count < 0) ERROR("count cannot be negative");
    if (!p_shader_rev) ERROR("Invalid shader name");
    
    size_t res_sources_size = 0;
    char* res_sources = NULL;
    if (arg_length->type == Type_Ptr) {
        real(p_shader_rev->real, p_count, p_string, NULL);
        for (GLsizei i = 0; i < p_count; i++) {
            res_sources = realloc(res_sources, res_sources_size+strlen(p_string[i])+1);
            memset(res_sources+res_sources_size, 0, strlen(p_string[i])+1);
            strcpy(res_sources+res_sources_size, p_string[i]);
            res_sources_size += strlen(p_string[i]) + 1;
        }
    } else {
        const int64_t* length = trc_get_int(arg_length);
        GLint* length_int = replay_alloc(p_count*sizeof(GLint));
        for (GLsizei i = 0; i < p_count; i++) length_int[i] = length[i];
        real(p_shader_rev->real, p_count, p_string, length_int);
        for (GLsizei i = 0; i < p_count; i++) {
            res_sources = realloc(res_sources, res_sources_size+length[i]+1);
            memset(res_sources+res_sources_size, 0, length[i]+1);
            memcpy(res_sources+res_sources_size, p_string[i], length[i]);
            res_sources[res_sources_size+length[i]+1] = 0;
            res_sources_size += length[i] + 1;
        }
    }
    
    trc_gl_shader_rev_t shdr = *p_shader_rev;
    shdr.sources = trc_create_data_no_copy(ctx->trace, res_sources_size, res_sources, 0);
    set_shader(&shdr);

glCompileShader: //GLuint p_shader
    if (!p_shader_rev) ERROR("Invalid shader name");
    
    real(p_shader_rev->real);
    
    trc_gl_shader_rev_t shdr = *p_shader_rev;
    
    GLint len;
    F(glGetShaderiv)(p_shader_rev->real, GL_INFO_LOG_LENGTH, &len);
    shdr.info_log = trc_create_data(ctx->trace, len+1, NULL, 0);
    char* info_log = trc_map_data(shdr.info_log, TRC_MAP_REPLACE);
    F(glGetShaderInfoLog)(p_shader_rev->real, len+1, NULL, info_log);
    trc_unmap_data(info_log);
    
    set_shader(&shdr);
    
    GLint status;
    F(glGetShaderiv)(p_shader_rev->real, GL_COMPILE_STATUS, &status);
    if (!status) ERROR("Failed to compile shader");

glCreateProgram: //
    GLuint real_program = F(glCreateProgram)();
    GLuint fake = trc_get_uint(&cmd->ret)[0];
    trc_gl_program_rev_t rev;
    rev.real = real_program;
    trc_data_t* empty_data = trc_create_data(ctx->trace, 0, NULL, 0);
    rev.root_uniform_count = 0;
    rev.uniforms = empty_data;
    rev.uniform_data = empty_data;
    rev.vertex_attribs = empty_data;
    rev.uniform_blocks = empty_data;
    for (size_t i = 0; i < 6; i++) rev.subroutines[i] = empty_data;
    for (size_t i = 0; i < 6; i++) rev.subroutine_uniforms[i] = empty_data;
    rev.shaders = empty_data;
    rev.linked = empty_data;
    rev.info_log = trc_create_data(ctx->trace, 1, "", 0);
    rev.binary_retrievable_hint = -1;
    rev.link_revision = -1;
    rev.separable = false;
    rev.link_status = -1;
    rev.validation_status = -1;
    trc_create_named_obj(ctx->ns, TrcProgram, fake, &rev);

glDeleteProgram: //GLuint p_program
    if (p_program == 0) RETURN;
    if (!p_program_rev) ERROR("Invalid program name");
    real(p_program_rev->real);
    
    size_t shader_count = p_program_rev->shaders->size / sizeof(trc_obj_ref_t);
    trc_obj_ref_t* shaders = trc_map_data(p_program_rev->shaders, TRC_MAP_READ);
    for (size_t i = 0; i < shader_count; i++)
        trc_del_obj_ref(shaders[i]);
    trc_unmap_data(shaders);
    
    trc_gl_program_rev_t newrev = *p_program_rev;
    newrev.shaders = trc_create_data(ctx->trace, 0, NULL, 0);
    set_program(&newrev);
    
    delete_obj(p_program, TrcProgram);

glProgramParameteri: //GLuint p_program, GLenum p_pname, GLint p_value
    if (!p_program_rev) ERROR("Invalid program name");
    trc_gl_program_rev_t newrev = *p_program_rev;
    switch (p_pname) {
    case GL_PROGRAM_BINARY_RETRIEVABLE_HINT: newrev.binary_retrievable_hint = p_value ? 1 : 0; break;
    case GL_PROGRAM_SEPARABLE: newrev.separable = p_value; break;
    }
    set_program(&newrev);
    real(p_program, p_pname, p_value);

glAttachShader: //GLuint p_program, GLuint p_shader
    if (!p_program_rev) ERROR("Invalid program name");
    if (!p_shader_rev) ERROR("Invalid shader name");
    
    real(p_program_rev->real, p_shader_rev->real);
    
    trc_gl_program_rev_t program = *p_program_rev;
    trc_gl_program_rev_t old = program;
    const trc_gl_shader_rev_t* shader = p_shader_rev;
    
    size_t shader_count = program.shaders->size / sizeof(trc_obj_ref_t);
    trc_obj_ref_t* src = trc_map_data(old.shaders, TRC_MAP_READ);
    
    for (size_t i = 0; i < shader_count; i++) {
        if (src[i].obj == p_shader_rev->head.obj) ERROR("Shader is already attached");
    }
    
    program.shaders = trc_create_data(ctx->trace, (shader_count+1)*sizeof(trc_obj_ref_t), NULL, TRC_DATA_NO_ZERO);
    
    trc_obj_ref_t* dest = trc_map_data(program.shaders, TRC_MAP_REPLACE);
    memcpy(dest, src, shader_count*sizeof(trc_obj_ref_t));
    memset(&dest[shader_count], 0, sizeof(trc_obj_ref_t));
    dest[shader_count].obj = p_shader_rev->head.obj;
    trc_grab_obj(p_shader_rev->head.obj);
    trc_unmap_data(src);
    trc_unmap_data(dest);
    
    set_program(&program);

glDetachShader: //GLuint p_program, GLuint p_shader
    if (!p_program_rev) ERROR("Invalid program name");
    if (!p_shader_rev) ERROR("Invalid shader name");
    
    real(p_program_rev->real, p_shader_rev->real);
    
    trc_gl_program_rev_t program = *p_program_rev;
    trc_gl_program_rev_t old = program;
    
    size_t shader_count = program.shaders->size / sizeof(trc_obj_ref_t);
    program.shaders = trc_create_data(ctx->trace, (shader_count-1)*sizeof(trc_obj_ref_t), NULL, TRC_DATA_NO_ZERO);
    
    trc_obj_ref_t* dest = trc_map_data(program.shaders, TRC_MAP_REPLACE);
    trc_obj_ref_t* src = trc_map_data(old.shaders, TRC_MAP_READ);
    size_t next = 0;
    bool found = false;
    for (size_t i = 0; i < shader_count; i++) {
        if (src[i].obj == p_shader_rev->head.obj) {
            found = true;
            trc_del_obj_ref(src[i]);
            continue;
        }
        dest[next++] = src[i];
    }
    trc_unmap_data(src);
    trc_unmap_data(dest);
    if (!found) ERROR("Shader is not attached to program");
    
    set_program(&program);

glBindAttribLocation: //GLuint p_program, GLuint p_index, const GLchar* p_name
    if (!p_program_rev) ERROR("Invalid program name");
    real(p_program_rev->real, p_index, p_name);

glUniformBlockBinding: //GLuint p_program, GLuint p_uniformBlockIndex, GLuint p_uniformBlockBinding
    const trc_gl_program_rev_t* rev_ptr = get_program(p_program);
    if (!rev_ptr) ERROR("Invalid program name");
    trc_gl_program_rev_t rev = *rev_ptr;
    if (p_uniformBlockBinding >= gls_get_state_int(GL_MAX_UNIFORM_BUFFER_BINDINGS, 0))
        ERROR("Invalid binding");
    uint uniform_block_count = rev.uniform_blocks->size / sizeof(trc_gl_program_uniform_block_t);
    trc_gl_program_uniform_block_t* blocks = trc_map_data(rev.uniform_blocks, TRC_MAP_READ);
    for (uint i = 0; i < uniform_block_count; i++) {
        if (blocks[i].fake == p_uniformBlockIndex) {
            real(p_program, blocks[i].real, p_uniformBlockBinding);
            trc_gl_program_rev_t newrev = rev;
            newrev.uniform_blocks = trc_create_data(ctx->trace, rev.uniform_blocks->size, blocks, 0);
            trc_gl_program_uniform_block_t* newblocks = trc_map_data(newrev.uniform_blocks, TRC_MAP_MODIFY);
            newblocks[i].binding = p_uniformBlockBinding;
            trc_unmap_data(newblocks);
            set_program(&newrev);
            goto success;
        }
    }
    trc_add_error(cmd, "No such uniform block");
    success:
    trc_unmap_data(blocks);

glUniformSubroutinesuiv: //GLenum p_shadertype, GLsizei p_count, const GLuint* p_indices
    trc_obj_t* program = get_active_program_for_stage(p_shadertype);
    if (!program) ERROR("No program is current for stage");
    GLint reqCount, subroutineCount;
    const trc_gl_program_rev_t* program_rev = trc_obj_get_rev(program, -1);
    F(glGetProgramStageiv)(program_rev->real, p_shadertype, GL_ACTIVE_SUBROUTINE_UNIFORM_LOCATIONS, &reqCount);
    F(glGetProgramStageiv)(program_rev->real, p_shadertype, GL_ACTIVE_SUBROUTINES, &subroutineCount);
    if (p_count != reqCount) ERROR("Count must be GL_ACTIVE_SUBROUTINE_UNIFORM_LOCATIONS");
    for (GLsizei i = 0; i < p_count; i++) {
        if (p_indices[i] >= subroutineCount)
            ERROR("Index at %d is not a valid subroutine\n", i);
    }
    real(p_shadertype, p_count, p_indices);
    //TODO: Store the indices

typedef struct link_program_extra_t {
    char* name;
    uint32_t val;
    uint32_t stage;
    uint32_t stage_idx;
} link_program_extra_t;

static uint link_program_extra(const char* name, size_t* i, link_program_extra_t* res) {
    trace_extra_t* extra = trc_get_extrai(cmd, name, (*i)++);
    if (!extra) return -1; //-1=End
    
    data_reader_t dr = dr_new(extra->size, extra->data);
    uint32_t len;
    if (!dr_read_le(&dr, 4, &res->val, 4, &res->stage, 4, &len, -1))
        ERROR2(1, "Invalid %s extra", name); //1=skip
    res->name = calloc(len+1, 1);
    if (!dr_read(&dr, len, res->name)) {
        free(res->name);
        ERROR2(1, "Invalid %s extra", name); //1=skip
    }
    
    switch (res->stage) {
    case GL_VERTEX_SHADER: res->stage_idx = 0; break;
    case GL_TESS_CONTROL_SHADER: res->stage_idx = 1; break;
    case GL_TESS_EVALUATION_SHADER: res->stage_idx = 2; break;
    case GL_GEOMETRY_SHADER: res->stage_idx = 3; break;
    case GL_FRAGMENT_SHADER: res->stage_idx = 4; break;
    case GL_COMPUTE_SHADER: res->stage_idx = 5; break;
    case 0: res->stage_idx = 0; break;
    default: ERROR2(1, "Invalid %s extra", name); //1=skip
    }
    
    return 0; //0=Use
}

static int get_type_columns(GLenum type) {
    switch (type) {
    case GL_FLOAT_MAT2:
    case GL_FLOAT_MAT2x3:
    case GL_FLOAT_MAT2x4:
    case GL_DOUBLE_MAT2:
    case GL_DOUBLE_MAT2x3:
    case GL_DOUBLE_MAT2x4: return 2;
    case GL_FLOAT_MAT3:
    case GL_FLOAT_MAT3x2:
    case GL_FLOAT_MAT3x4:
    case GL_DOUBLE_MAT3:
    case GL_DOUBLE_MAT3x2:
    case GL_DOUBLE_MAT3x4: return 3;
    case GL_FLOAT_MAT4:
    case GL_FLOAT_MAT4x2:
    case GL_FLOAT_MAT4x3:
    case GL_DOUBLE_MAT4:
    case GL_DOUBLE_MAT4x2:
    case GL_DOUBLE_MAT4x3: return 4;
    default: return 1;
    }
}

//TODO: The get_program_* functions are all very similar
static trc_data_t* get_program_vertex_attribs(GLuint real_program) {
    size_t vertex_attrib_count = 0;
    trc_gl_program_vertex_attrib_t* vertex_attribs = NULL;
    size_t i = 0;
    int res;
    link_program_extra_t extra;
    while ((res=link_program_extra("replay/program/vertex_attrib", &i, &extra))!=-1) {
        if (res != 0) continue;
        
        GLint real_loc = F(glGetAttribLocation)(real_program, extra.name);
        if (real_loc < 0) {
            trc_add_error(cmd, "Nonexistent or inactive vertex attribute while adding vertex attribute '%s'", extra.name);
            goto continue_loop;
        }
        
        trc_gl_program_vertex_attrib_t attrib;
        attrib.name = trc_create_data(ctx->trace, strlen(extra.name)+1, extra.name, 0);
        attrib.fake = extra.val;
        attrib.real = real_loc;
        attrib.type = 0xffffffff;
        
        GLint count;
        F(glGetProgramiv)(real_program, GL_ACTIVE_ATTRIBUTES, &count);
        for (GLint i = 0; i < count; i++) {
            char name_buf[strlen(extra.name)+1];
            GLsizei len;
            GLint size;
            GLenum type;
            F(glGetActiveAttrib)(real_program, i, strlen(extra.name)+1, &len, &size, &type, name_buf);
            if (len==strlen(extra.name) && strcmp(name_buf, extra.name)==0) {
                attrib.type = type;
                break;
            }
        }
        if (attrib.type == 0xffffffff) {
            trc_add_error(cmd, "Nonexistent or inactive vertex attribute while adding vertex attribute '%s'", extra.name);
            goto continue_loop;
        }
        
        attrib.locations_used = get_type_columns(attrib.type);
        
        size_t attrib_size = sizeof(trc_gl_program_vertex_attrib_t);
        vertex_attribs = realloc(vertex_attribs, (vertex_attrib_count+1)*attrib_size);
        vertex_attribs[vertex_attrib_count++] = attrib;
        
        continue_loop:
        free(extra.name);
    }
    
    size_t size = vertex_attrib_count * sizeof(trc_gl_program_vertex_attrib_t);
    return trc_create_data_no_copy(ctx->trace, size, vertex_attribs, 0);
}

static trc_data_t* get_program_uniform_blocks(GLuint real_program) {
    size_t uniform_block_count = 0;
    trc_gl_program_uniform_block_t* uniform_blocks = NULL;
    size_t i = 0;
    int res;
    link_program_extra_t extra;
    while ((res=link_program_extra("replay/program/uniform_block", &i, &extra))!=-1) {
        if (res != 0) continue;
        GLint real_idx = F(glGetUniformBlockIndex)(real_program, extra.name);
        if (real_idx < 0) {
            trc_add_error(cmd, "Nonexistent or inactive uniform block while adding uniform block %s", extra.name);
        } else {
            uniform_blocks = realloc(uniform_blocks, (uniform_block_count+1)*sizeof(trc_gl_program_uniform_block_t));
            trc_gl_program_uniform_block_t block;
            memset(&block, 0, sizeof(block)); //initialize padding to zero - it might be compressed
            block.name = trc_create_data(ctx->trace, strlen(extra.name)+1, extra.name, 0);
            block.real = real_idx;
            block.fake = extra.val;
            block.binding = 0;
            uniform_blocks[uniform_block_count++] = block;
        }
        free(extra.name);
    }
    
    return trc_create_data_no_copy(ctx->trace, uniform_block_count*sizeof(trc_gl_program_uniform_block_t), uniform_blocks, 0);
}

static void get_program_subroutines(GLuint real_program, trc_data_t** datas) {
    //TODO: Check for extensions
    bool compute_supported = gls_get_ver() >= 430;
    bool tesselation_supported = gls_get_ver() >= 400;
    
    size_t subroutine_count[6] = {0};
    uint* subroutines[6] = {0};
    size_t i = 0;
    int res;
    link_program_extra_t extra;
    while ((res=link_program_extra("replay/program/subroutine", &i, &extra))!=-1) {
        if (res != 0) continue;
        if ((extra.stage==GL_COMPUTE_SHADER && !compute_supported) ||
            ((extra.stage==GL_TESS_CONTROL_SHADER||extra.stage==GL_TESS_EVALUATION_SHADER) && !tesselation_supported)) {
            trc_add_error(cmd, "Unsupported shader stage while adding subroutine %s", extra.name);
            continue;
        }
        GLint real_idx = F(glGetSubroutineIndex)(real_program, extra.stage, extra.name);
        if (real_idx == GL_INVALID_INDEX) {
            trc_add_error(cmd, "Nonexistent or inactive subroutine while adding subroutine %s", extra.name);
        } else {
            subroutines[extra.stage_idx] = realloc(subroutines[extra.stage_idx], (subroutine_count[extra.stage_idx]+1)*sizeof(uint)*2);
            subroutines[extra.stage_idx][subroutine_count[extra.stage_idx]*2] = real_idx;
            subroutines[extra.stage_idx][subroutine_count[extra.stage_idx]++*2+1] = extra.val;
        }
        free(extra.name);
    }
    
    for (size_t i = 0; i < 6; i++)
        datas[i] = trc_create_data_no_copy(ctx->trace, subroutine_count[i]*2*sizeof(uint), subroutines[i], 0);
}

static void get_program_subroutine_uniforms(GLuint real_program, trc_data_t** datas) {
    //TODO: Check for extensions
    bool compute_supported = gls_get_ver() >= 430;
    bool tesselation_supported = gls_get_ver() >= 400;
    
    //"_uniform" not included to keep names short
    size_t subroutine_count[6] = {0};
    uint* subroutines[6] = {0};
    size_t i = 0;
    int res;
    link_program_extra_t extra;
    while ((res=link_program_extra("replay/program/subroutine_uniform", &i, &extra))!=-1) {
        if (res != 0) continue;
        if ((extra.stage==GL_COMPUTE_SHADER && !compute_supported) ||
            ((extra.stage==GL_TESS_CONTROL_SHADER||extra.stage==GL_TESS_EVALUATION_SHADER) && !tesselation_supported)) {
            trc_add_error(cmd, "Unsupported shader stage while adding subroutine uniform %s", extra.name);
            continue;
        }
        GLint real_idx = F(glGetSubroutineUniformLocation)(real_program, extra.stage, extra.name);
        if (real_idx == GL_INVALID_INDEX) {
            trc_add_error(cmd, "Nonexistent or inactive subroutine while adding subroutine uniform %s", extra.name);
        } else {
            subroutines[extra.stage_idx] = realloc(subroutines[extra.stage_idx], (subroutine_count[extra.stage_idx]+1)*sizeof(uint)*2);
            subroutines[extra.stage_idx][subroutine_count[extra.stage_idx]*2] = real_idx;
            subroutines[extra.stage_idx][subroutine_count[extra.stage_idx]++*2+1] = extra.val;
        }
        free(extra.name);
    }
    
    for (size_t i = 0; i < 6; i++)
        datas[i] = trc_create_data_no_copy(ctx->trace, subroutine_count[i]*2*sizeof(uint), subroutines[i], 0);
}

typedef struct uniform_spec_state_t {
    trc_replay_context_t* ctx;
    data_reader_t dr;
    trc_gl_uniform_t* uniforms;
    size_t next_index;
    size_t storage_used;
    uint32_t spec_count;
    uint32_t root_count;
    GLuint real_program;
} uniform_spec_state_t;

static bool read_normal_uniform(uniform_spec_state_t* state, trc_gl_uniform_t* spec) {
    if (!dr_read_le(&state->dr, 4, &spec->fake_loc, -1)) return false;
    
    uint8_t dim[2];
    if (!dr_read(&state->dr, 2, dim)) return false;
    if (dim[0]>4 || dim[1]>4) return false;
    spec->dtype.dim[0] = dim[0];
    spec->dtype.dim[1] = dim[1];
    
    return true;
}

static bool read_texture_uniform(uniform_spec_state_t* state, trc_gl_uniform_t* spec) {
    if (!dr_read_le(&state->dr, 4, &spec->fake_loc, -1)) return false;
    
    uint8_t params[5];
    if (!dr_read(&state->dr, 5, params)) return false;
    if (params[0] > 5) return false;
    if (params[4]!=0 && params[4]!=2 && params[4]!=3) return false;
    spec->dtype.tex_type = params[0];
    spec->dtype.tex_shadow = params[1];
    spec->dtype.tex_array = params[2];
    spec->dtype.tex_multisample = params[3];
    spec->dtype.tex_dtype = params[4];
    
    return true;
}

static bool read_uniform_spec(uniform_spec_state_t* state, uint specindex,
                              str_t var_name_base, uint parent);

static bool read_composite_uniform(uniform_spec_state_t* state, trc_gl_uniform_t* spec,
                                   str_t var_name_base, str_t name, uint specindex) {
    uint32_t child_count;
    if (!dr_read_le(&state->dr, 4, &child_count, -1)) return false;
    
    size_t prev_index;
    for (size_t i = 0; i < child_count; i++) {
        size_t index = state->next_index++;
        if (index >= state->spec_count) return false;
        
        str_t base;
        if (spec->dtype.base == TrcUniformBaseType_Struct)
            base = str_fmt(NULL, "%s%s.", var_name_base.data, name.data);
        else
            base = str_fmt(NULL, "%s%s[%zu]", var_name_base.data, name.data, i);
        if (!read_uniform_spec(state, index, base, specindex)) {
            str_del(base);
            return false;
        }
        str_del(base);
        
        if (i == 0) spec->first_child = index;
        else state->uniforms[prev_index].next = index;
        prev_index = index;
    }
    
    return true;
}

static bool read_uniform_spec(uniform_spec_state_t* state, uint specindex,
                              str_t var_name_base, uint parent) {
    uint32_t name_length;
    str_t name = str_null();
    if (!dr_read_le(&state->dr, 4, &name_length, -1)) goto failure;
    if (!dr_read_str(&state->dr, NULL, name_length, &name));
    
    trc_gl_uniform_t* spec = &state->uniforms[specindex];
    memset(spec, 0, sizeof(trc_gl_uniform_t)); //To fix usage of unintialized data because of compression
    spec->parent = parent;
    spec->name = trc_create_data(state->ctx->trace, name.length+1, name.data, 0);
    spec->next = 0xffffffff;
    spec->dtype.dim[0] = 1;
    spec->dtype.dim[1] = 1;
    spec->first_child = 0xffffffff;
    
    uint8_t base;
    if (!dr_read(&state->dr, 1, &base)) goto failure;
    spec->dtype.base = (trc_gl_uniform_base_dtype_t)base;
    
    if ((int)spec->dtype.base <= 6) {
        if (!read_normal_uniform(state, spec)) goto failure;
    } else if ((int)spec->dtype.base <= 8) {
        if (!read_texture_uniform(state, spec)) goto failure;
    } else if ((int)spec->dtype.base==10 || (int)spec->dtype.base==11) {
        if (!read_composite_uniform(state, spec, var_name_base, name, specindex))
            goto failure;
    } else {
        goto failure;
    }
    
    if ((int)spec->dtype.base <= 8) {
        str_t full_name = str_cat(NULL, var_name_base, name);
        void* ctx = state->ctx;
        spec->real_loc = F(glGetUniformLocation)(state->real_program, full_name.data);
        str_del(full_name);
        if (spec->real_loc < 0) goto failure;
        
        size_t dtype_sizes[] = {4, 8, 4, 4, 8, 8, 1, 4, 4};
        spec->data_offset = state->storage_used;
        state->storage_used += dtype_sizes[(int)spec->dtype.base] * spec->dtype.dim[0] * spec->dtype.dim[1];
    }
    
    bool success = true;
    goto success;
    failure: success = false;
    success: ;
    
    str_del(name);
    
    return success;
}

static bool read_uniform_specs(uniform_spec_state_t* state) {
    if (!dr_read_le(&state->dr, 4, &state->spec_count, -1)) return false;
    if (!dr_read_le(&state->dr, 4, &state->root_count, -1)) return false;
    
    state->uniforms = malloc(state->spec_count*sizeof(trc_gl_uniform_t));
    state->next_index = state->root_count;
    state->storage_used = 0;
    for (size_t i = 0; i < state->root_count; i++) {
        if (!read_uniform_spec(state, i, (str_t){false, 0, ""}, 0xffffffff)) {
            free(state->uniforms);
            return false;
        }
    }
    return true;
}

static bool init_uniforms(trc_gl_program_rev_t* rev) {
    trace_extra_t* extra = trc_get_extrai(cmd, "replay/program/uniforms", 0);
    if (!extra) return false;
    
    uniform_spec_state_t state;
    state.ctx = ctx;
    state.dr = dr_new(extra->size, extra->data);
    state.real_program = rev->real;
    if (!read_uniform_specs(&state))
        return false;
    
    uint8_t* uniform_data = malloc(state.storage_used);
    //Initialize data
    for (size_t i = 0; i < state.spec_count; i++) {
        trc_gl_uniform_t u = state.uniforms[i];
        if ((int)u.dtype.base > 8) continue;
        uint8_t* data = uniform_data + u.data_offset;
        
        size_t size = u.dtype.dim[0] * u.dtype.dim[1];
        switch (u.dtype.base) {
        #define D(e, st, dt, f) case e: {\
            st vals[16];\
            F(f)(rev->real, u.real_loc, vals);\
            for (size_t j = 0; j < size; j++) ((dt*)data)[j] = vals[j];\
            break;\
        }
        D(TrcUniformBaseType_Float, float, float, glGetUniformfv)
        D(TrcUniformBaseType_Double, double, double, glGetUniformdv)
        D(TrcUniformBaseType_Uint, uint32_t, uint32_t, glGetUniformuiv)
        D(TrcUniformBaseType_Int, int32_t, int32_t, glGetUniformiv)
        D(TrcUniformBaseType_Bool, int32_t, bool, glGetUniformiv)
        D(TrcUniformBaseType_Sampler, int32_t, int32_t, glGetUniformiv)
        D(TrcUniformBaseType_Image, int32_t, int32_t, glGetUniformiv)
        //TODO
        //D(TrcUniformBaseType_Uint64, uint64_t, uint64_t, glGetUniformi64vARB)
        //D(TrcUniformBaseType_Int64, int64_t, int64_t, glGetUniformi64vARB)
        #undef D
        default: break;
        }
    }
    
    rev->root_uniform_count = state.root_count;
    rev->uniforms = trc_create_data_no_copy(ctx->trace, state.spec_count*sizeof(trc_gl_uniform_t), state.uniforms, 0);
    rev->uniform_data = trc_create_data_no_copy(ctx->trace, state.storage_used, uniform_data, 0);
    
    return true;
}

static void handle_link_program_extras(trc_gl_program_rev_t* rev) {
    if (!init_uniforms(rev)) {
        trc_data_t* empty_data = trc_create_data(ctx->trace, 0, NULL, 0);
        rev->root_uniform_count = 0;
        rev->uniforms = empty_data;
        rev->uniform_data = empty_data;
        ERROR2(, "Failed to initialize uniform storage");
    }
    rev->vertex_attribs = get_program_vertex_attribs(rev->real);
    rev->uniform_blocks = get_program_uniform_blocks(rev->real);
    if (gls_get_ver() >= 400) { //TODO: Check for the extension
        get_program_subroutines(rev->real, rev->subroutines);
        get_program_subroutine_uniforms(rev->real, rev->subroutine_uniforms);
    }
}

glCreateShaderProgramv: //GLenum p_type, GLsizei p_count, const char** p_strings
    if (p_count < 0) ERROR("Count is negative");
    GLuint real_program = real(p_type, p_count, p_strings);
    GLuint fake = trc_get_uint(&cmd->ret)[0];
    
    trc_data_t* empty_data = trc_create_data(ctx->trace, 0, NULL, 0);
    
    //Create shader object
    size_t res_sources_size = 0;
    char* res_sources = NULL;
    for (GLsizei i = 0; i < p_count; i++) {
        res_sources = realloc(res_sources, res_sources_size+strlen(p_strings[i])+1);
        memset(res_sources+res_sources_size, 0, strlen(p_strings[i])+1);
        strcpy(res_sources+res_sources_size, p_strings[i]);
        res_sources_size += strlen(p_strings[i]) + 1;
    }
    trc_gl_shader_rev_t shader;
    shader.real = 0;
    shader.sources = trc_create_data_no_copy(ctx->trace, res_sources_size, res_sources, 0);
    shader.info_log = trc_create_data(ctx->trace, 1, "", 0);
    shader.type = p_type;
    trc_obj_t* shader_obj = trc_create_obj(ctx->trace, false, TrcShader, &shader);
    
    //Create program object
    trc_gl_program_rev_t rev;
    rev.real = real_program;
    
    GLint len;
    F(glGetProgramiv)(rev.real, GL_INFO_LOG_LENGTH, &len);
    rev.info_log = trc_create_data(ctx->trace, len+1, NULL, 0);
    char* info_log = trc_map_data(rev.info_log, TRC_MAP_REPLACE);
    F(glGetProgramInfoLog)(rev.real, len+1, NULL, info_log);
    trc_unmap_data(info_log);
    
    GLint status;
    F(glGetProgramiv)(rev.real, GL_LINK_STATUS, &status);
    rev.link_status = status ? 1 : 0;
    rev.validation_status = -1;
    
    if (rev.link_status == 0) trc_add_error(cmd, "Failed to link program");
    else handle_link_program_extras(&rev);
    
    rev.shaders = empty_data;
    rev.linked = rev.link_status==1 ? trc_create_data(ctx->trace, sizeof(trc_obj_t*), &shader_obj, 0) : empty_data;
    rev.binary_retrievable_hint = -1;
    rev.link_revision = ctx->trace->inspection.cur_revision;
    rev.separable = true;
    
    trc_create_named_obj(ctx->ns, TrcProgram, fake, &rev);

glLinkProgram: //GLuint p_program
    trc_obj_t* program = trc_lookup_name(ctx->ns, TrcProgram, p_program, -1);
    if (!program) ERROR("Invalid program name");
    
    //TODO: Also test if it is part of the current program pipeline?
    if (get_current_tf()->active_not_paused && program==gls_get_bound_program())
        ERROR("The bound program cannot be modified while transform feedback is active and unpaused");
    
    trc_gl_program_rev_t rev = *(const trc_gl_program_rev_t*)trc_obj_get_rev(program, -1);
    
    real(rev.real);
    
    GLint len;
    F(glGetProgramiv)(rev.real, GL_INFO_LOG_LENGTH, &len);
    rev.info_log = trc_create_data(ctx->trace, len+1, NULL, 0);
    char* info_log = trc_map_data(rev.info_log, TRC_MAP_REPLACE);
    F(glGetProgramInfoLog)(rev.real, len+1, NULL, info_log);
    trc_unmap_data(info_log);
    
    GLint status;
    F(glGetProgramiv)(rev.real, GL_LINK_STATUS, &status);
    rev.link_status = status ? 1 : 0;
    
    if (rev.link_status == 0) {
        set_program(&rev);
        ERROR("Failed to link program");
    }
    
    handle_link_program_extras(&rev);
    
    size_t linked_count = rev.shaders->size / sizeof(trc_obj_ref_t);
    trc_obj_t** linked = calloc(linked_count, sizeof(trc_obj_t*));
    trc_obj_ref_t* shaders = trc_map_data(rev.shaders, TRC_MAP_READ);
    for (size_t i = 0; i < linked_count; i++)
        linked[i] = shaders[i].obj;
    trc_unmap_data(shaders);
    rev.linked = trc_create_data_no_copy(ctx->trace, linked_count*sizeof(trc_obj_t*), linked, 0);
    
    rev.link_revision = ctx->trace->inspection.cur_revision;
    
    set_program(&rev);

//TODO: Don't rely on the GL implementation to do the validation
glValidateProgram: //GLuint p_program
    if (!p_program_rev) ERROR("Invalid program name");
    
    real(p_program_rev->real);
    
    trc_gl_program_rev_t rev = *p_program_rev;
    
    GLint len;
    F(glGetProgramiv)(p_program_rev->real, GL_INFO_LOG_LENGTH, &len);
    rev.info_log = trc_create_data(ctx->trace, len+1, NULL, 0);
    char* info_log = trc_map_data(rev.info_log, TRC_MAP_REPLACE);
    F(glGetProgramInfoLog)(p_program_rev->real, len, NULL, info_log);
    trc_unmap_data(info_log);
    
    GLint status;
    F(glGetProgramiv)(p_program_rev->real, GL_VALIDATE_STATUS, &status);
    rev.validation_status = status ? 1 : 0;
    
    p_program_rev = set_program(&rev);
    
    if (rev.validation_status == 0) ERROR("Program validation failed");

glValidateProgramPipeline: //GLuint p_pipeline
    if (!p_pipeline_rev) ERROR("Invalid program pipeline name");
    
    real(p_pipeline_rev->real);
    
    if (!p_pipeline_rev->has_object) {
        trc_gl_program_pipeline_rev_t newrev = *p_pipeline_rev;
        newrev.has_object = true;
        p_pipeline_rev = set_program_pipeline(&newrev);
    }
    
    trc_gl_program_pipeline_rev_t rev = *p_pipeline_rev;
    GLint status;
    F(glGetProgramiv)(p_pipeline_rev->real, GL_VALIDATE_STATUS, &status);
    rev.validation_status = status ? 1 : 0;
    p_pipeline_rev = set_program_pipeline(&rev);
    
    if (rev.validation_status == 0) ERROR("Program piepline validation failed");

glUseProgram: //GLuint p_program
    const trc_gl_program_rev_t* program_rev = get_program(p_program);
    if (!program_rev && p_program) ERROR("Invalid program name");
    if (get_current_tf()->active_not_paused)
        ERROR("The program binding cannot be modified while transform feedback is active and unpaused");
    
    gls_set_bound_program(program_rev?program_rev->head.obj:NULL);
    
    real(program_rev?program_rev->real:0);

glGenProgramPipelines: //GLsizei p_n, GLuint* p_pipelines
    GLuint* pipelines = replay_alloc(p_n*sizeof(GLuint));
    real(p_n, pipelines);
    gen_program_pipelines(p_n, pipelines, p_pipelines, false);

glCreateProgramPipelines: //GLsizei p_n, GLuint* p_pipelines
    GLuint* pipelines = replay_alloc(p_n*sizeof(GLuint));
    real(p_n, pipelines);
    gen_program_pipelines(p_n, pipelines, p_pipelines, true);

glDeleteProgramPipelines: //GLsizei p_n, const GLuint* p_pipelines
    GLuint* pipelines = replay_alloc(p_n*sizeof(GLuint));
    for (size_t i = 0; i < p_n; ++i) {
        if (!(pipelines[i] = get_real_program_pipeline(p_pipelines[i])) && p_pipelines[i]) {
            trc_add_warning(cmd, "Invalid program pipeline name");
        } else {
            trc_obj_t* obj = get_program_pipeline(p_pipelines[i])->head.obj;
            if (gls_get_bound_pipeline() == obj)
                gls_set_bound_pipeline(NULL);
            delete_obj(p_pipelines[i], TrcProgramPipeline);
        }
    }
    real(p_n, pipelines);

//TODO: Do transform feedback checks in glBindProgramPipeline
glBindProgramPipeline: //GLuint p_pipeline
    if (p_pipeline && !p_pipeline_rev) ERROR("Invalid program pipeline name");
    if (get_current_tf()->active_not_paused)
        ERROR("The program pipeline binding cannot be modified while transform feedback is active and unpaused");
    if (p_pipeline_rev && !p_pipeline_rev->has_object) {
        trc_gl_program_pipeline_rev_t newrev = *p_pipeline_rev;
        newrev.has_object = true;
        p_pipeline_rev = set_program_pipeline(&newrev);
    }
    gls_set_bound_pipeline(p_pipeline?p_pipeline_rev->head.obj:NULL);
    real(p_pipeline?p_pipeline_rev->real:0);

glUseProgramStages: //GLuint p_pipeline, GLbitfield p_stages, GLuint p_program
    if (!p_pipeline_rev) ERROR("Invalid program pipeline name");
    if (p_program && !p_program_rev) ERROR("Invalid program name");
    if (get_current_tf()->active_not_paused &&
        p_pipeline_rev->head.obj==gls_get_bound_pipeline()) {
        ERROR("The bound program pipeline object cannot be modified while transform feedback is active and unpaused");
    }
    real(p_pipeline_rev->real, p_stages, p_program?p_program_rev->real:0);
    if (p_pipeline_rev && !p_pipeline_rev->has_object) {
        trc_gl_program_pipeline_rev_t newrev = *p_pipeline_rev;
        newrev.has_object = true;
        p_pipeline_rev = set_program_pipeline(&newrev);
    }
    trc_gl_program_pipeline_rev_t newrev = *p_pipeline_rev;
    if (p_stages & GL_VERTEX_SHADER_BIT)
        trc_set_obj_ref(&newrev.vertex_program, p_program?p_program_rev->head.obj:NULL);
    if (p_stages & GL_FRAGMENT_SHADER_BIT)
        trc_set_obj_ref(&newrev.fragment_program, p_program?p_program_rev->head.obj:NULL);
    if (p_stages & GL_GEOMETRY_SHADER_BIT)
        trc_set_obj_ref(&newrev.geometry_program, p_program?p_program_rev->head.obj:NULL);
    if (p_stages & GL_TESS_CONTROL_SHADER_BIT)
        trc_set_obj_ref(&newrev.tess_control_program, p_program?p_program_rev->head.obj:NULL);
    if (p_stages & GL_TESS_EVALUATION_SHADER_BIT)
        trc_set_obj_ref(&newrev.tess_eval_program, p_program?p_program_rev->head.obj:NULL);
    if (p_stages & GL_COMPUTE_SHADER_BIT)
        trc_set_obj_ref(&newrev.compute_program, p_program?p_program_rev->head.obj:NULL);
    set_program_pipeline(&newrev);

glActiveShaderProgram: //GLuint p_pipeline, GLuint p_program
    if (!p_pipeline_rev) ERROR("Invalid program pipeline name");
    if (!p_pipeline_rev->has_object) ERROR("Program pipeline name has no object");
    if (!p_program_rev) ERROR("Invalid program name");
    if (get_current_tf()->active_not_paused &&
        p_pipeline_rev->head.obj==gls_get_bound_pipeline()) {
        ERROR("The bound program pipeline object cannot be modified while transform feedback is active and unpaused");
    }
    real(p_pipeline_rev->real, p_program_rev->real);
    trc_gl_program_pipeline_rev_t newrev = *p_pipeline_rev;
    trc_set_obj_ref(&newrev.active_program, p_program_rev->head.obj);
    set_program_pipeline(&newrev);

glGetProgramPipelineiv: //GLuint p_pipeline, GLenum p_pname, GLint* p_params
    if (!p_pipeline_rev) ERROR2(, "Invalid program pipeline name");
    if (!p_pipeline_rev->has_object) ERROR2(, "Program pipeline name has no object");

glGetAttribLocation: //GLuint p_program, const GLchar* p_name
    if (!p_program_rev) ERROR("Invalid program name");
    real(p_program_rev->real, p_name);

glGetUniformLocation: //GLuint p_program, const GLchar* p_name
    if (!p_program_rev) ERROR("Invalid program name");
    real(p_program_rev->real, p_name);

glGetShaderiv: //GLuint p_shader, GLenum p_pname, GLint* p_params
    if (!p_shader_rev) ERROR("Invalid shader name");

glGetShaderInfoLog: //GLuint p_shader, GLsizei p_bufSize, GLsizei* p_length, GLchar* p_infoLog
    if (!p_shader_rev) ERROR("Invalid shader name");

glGetShaderSource: //GLuint p_shader, GLsizei p_bufSize, GLsizei* p_length, GLchar* p_source
    if (!p_shader_rev) ERROR("Invalid shader name");

glGetProgramInfoLog: //GLuint p_program, GLsizei p_bufSize, GLsizei* p_length, GLchar* p_infoLog
    if (!p_program_rev) ERROR("Invalid program name");

glGetProgramiv: //GLuint p_program, GLenum p_pname, GLint* p_params
    if (!p_program_rev) ERROR("Invalid program name");

glGetAttachedShaders: //GLuint p_program, GLsizei p_maxCount, GLsizei* p_count, GLuint* p_shaders
    if (!p_program_rev) ERROR("Invalid program name");
    if (p_maxCount < 0) ERROR("maxCount must be greater than or equal to zero");

glGetActiveUniform: //GLuint p_program, GLuint p_index, GLsizei p_bufSize, GLsizei* p_length, GLint* p_size, GLenum* p_type, GLchar* p_name
    if (!p_program_rev) ERROR("Invalid program name");

glGetActiveAttrib: //GLuint p_program, GLuint p_index, GLsizei p_bufSize, GLsizei* p_length, GLint* p_size, GLenum* p_type, GLchar* p_name
    if (!p_program_rev) ERROR("Invalid program name");

glGetFragDataIndex: //GLuint p_program, const GLchar* p_name
    if (!p_program_rev) ERROR("Invalid program name");

glGetFragDataLocation: //GLuint p_program, const GLchar* p_name
    if (!p_program_rev) ERROR("Invalid program name");

glGetUniformBlockIndex: //GLuint p_program, const GLchar* p_uniformBlockName
    if (!p_program_rev) ERROR("Invalid program name");

glGetUniformIndices: //GLuint p_program, GLsizei p_uniformCount, const GLchar  *const* p_uniformNames, GLuint* p_uniformIndices
    if (!p_program_rev) ERROR("Invalid program name");

glGetUniformSubroutineuiv: //GLenum p_shadertype, GLint p_location, GLuint* p_params
    ;

glGetSubroutineIndex: //GLuint p_program, GLenum p_shadertype, const GLchar* p_name
    if (!p_program_rev) ERROR("Invalid program name");

glGetActiveSubroutineName: //GLuint p_program, GLenum p_shadertype, GLuint p_index, GLsizei p_bufsize, GLsizei* p_length, GLchar* p_name
    if (!p_program_rev) ERROR("Invalid program name");
    //TODO: More validation

glGetProgramStageiv: //GLuint p_program, GLenum p_shadertype, GLenum p_pname, GLint* p_values
    if (!p_program_rev) ERROR("Invalid program name");

glGetActiveSubroutineUniformName: //GLuint p_program, GLenum p_shadertype, GLuint p_index, GLsizei p_bufsize, GLsizei* p_length, GLchar* p_name
    if (!p_program_rev) ERROR("Invalid program name");
    //TODO: More validation

glGetSubroutineUniformLocation: //GLuint p_program, GLenum p_shadertype, const GLchar* p_name
    if (!p_program_rev) ERROR("Invalid program name");

glGetActiveUniformBlockiv: //GLuint p_program, GLuint p_uniformBlockIndex, GLenum p_pname, GLint* p_params
    if (!p_program_rev) ERROR("Invalid program name");
    
    if (p_pname == GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES) {
        GLint count;
        real(p_program_rev->real, p_uniformBlockIndex, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &count);
        
        GLint* vals = malloc(sizeof(GLint)*count);
        real(p_program_rev->real, p_uniformBlockIndex, p_pname, vals);
        free(vals);
    } else {
        GLint v;
        real(p_program_rev->real, p_uniformBlockIndex, p_pname, &v);
    }

glGetActiveUniformBlockName: //GLuint p_program, GLuint p_uniformBlockIndex, GLsizei p_bufSize, GLsizei* p_length, GLchar* p_uniformBlockName
    if (!p_program_rev) ERROR("Invalid program name");
    GLchar buf[64];
    real(p_program_rev->real, p_uniformBlockIndex, 64, NULL, buf);

glGetActiveUniformName: //GLuint p_program, GLuint p_uniformIndex, GLsizei p_bufSize, GLsizei* p_length, GLchar* p_uniformName
    if (!p_program_rev) ERROR("Invalid program name");
    GLchar buf[64];
    real(p_program_rev->real, p_uniformIndex, 64, NULL, buf);

glGetActiveUniformsiv: //GLuint p_program, GLsizei p_uniformCount, const GLuint* p_uniformIndices, GLenum p_pname, GLint* p_params
    if (!p_program_rev) ERROR("Invalid program name");
    GLint* params = replay_alloc(p_uniformCount*sizeof(GLint));
    real(p_program_rev->real, p_uniformCount, p_uniformIndices, p_pname, params);

glIsProgram: //GLuint p_program
    ;

glIsShader: //GLuint p_shader
    ;

glIsProgramPipeline: //GLuint p_pipeline
    ;
