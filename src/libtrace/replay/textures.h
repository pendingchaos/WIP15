#ifndef TEXTURES_H
#define TEXTURES_H
typedef enum internal_format_data_type_t {
    FmtDType_U8_Norm,
    FmtDType_I8_Norm,
    FmtDType_U16_Norm,
    FmtDType_I16_Norm,
    FmtDType_U24_Norm,
    FmtDType_U32_Norm,
    FmtDType_U8,
    FmtDType_I8,
    FmtDType_U16,
    FmtDType_I16,
    FmtDType_U32,
    FmtDType_I32,
    FmtDType_F16,
    FmtDType_F32,
    FmtDType_D24_S8,
    FmtDType_D32F_S8,
    FmtDType_S1,
    FmtDType_S4,
    FmtDType_S8,
    FmtDType_S16,
    FmtDType_F11_F11_F10,
    FmtDType_9E5F,
    FmtDType_U3_U3_U2_Norm,
    FmtDType_U2_Norm,
    FmtDType_U4_Norm,
    FmtDType_U5_Norm,
    FmtDType_U10_Norm,
    FmtDType_U12_Norm,
    FmtDType_U5_U6_U5_Norm,
    FmtDType_U5_U5_U5_U1_Norm,
    FmtDType_U10_U10_U10_U2,
    FmtDType_U10_U10_U10_U2_Norm,
    FmtDType_U8_Srgb,
    FmtDType_Unspecified,
    FmtDType_Compressed
} internal_format_data_type_t;

typedef struct internal_format_info_t {
    GLenum internal_format;
    //GL_DEPTH_COMPONENT, GL_DEPTH_STENCIL, GL_RED, GL_RG, GL_RGB,
    //GL_RGBA or GL_STENCIL_INDEX
    GLenum base;
    internal_format_data_type_t dtype;
    trc_image_format_t trc_image_format;
} internal_format_info_t;

bool sample_param_double(trc_gl_sample_params_t* params, GLenum param, uint32_t count, const double* val);
//sampler_obj may be NULL
const char* validate_texture_completeness(trc_obj_t* tex_obj, trc_obj_t* sampler_obj);
const trc_gl_texture_rev_t* get_bound_tex(uint target);
void update_tex_image(const trc_gl_texture_rev_t* tex, uint level, uint face);
#endif
