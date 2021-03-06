/*#version 150
void main() {
    gl_Position.xy = vec2[](
        vec2(-1.0, -1.0), vec2(3.0, -1.0), vec2(-1.0, 3.0))[gl_VertexID];
    gl_Position.zw = vec2(0.0, 1.0);
}



#version 150
out vec4 result;
sampler2DMS src;
uniform int sample;
void main() {
    result = texelFetch(src, ivec2(gl_FragCoord.xy), sample);
}



#version 150
out vec4 result;
sampler2DMSArray src;
uniform int sample;
uniform int layer;
void main() {
    result = texelFetch(src, ivec3(gl_FragCoord.xy, layer), sample);
}*/

#include "libtrace/replay/textures.h"
#include "libtrace/replay/utils.h"

static bool is_compressed_format(uint internal_format) {
    switch (internal_format) {
    case GL_COMPRESSED_RED:
    case GL_COMPRESSED_RG:
    case GL_COMPRESSED_RGB:
    case GL_COMPRESSED_RGBA:
    case GL_COMPRESSED_SRGB:
    case GL_COMPRESSED_SRGB_ALPHA:
    case GL_COMPRESSED_RED_RGTC1:
    case GL_COMPRESSED_SIGNED_RED_RGTC1:
    case GL_COMPRESSED_RG_RGTC2:
    case GL_COMPRESSED_SIGNED_RG_RGTC2:
    case GL_COMPRESSED_RGBA_BPTC_UNORM:
    case GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM:
    case GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT:
    case GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT:
        return true;
    default:
        return false;
    }
}

static const internal_format_info_t internal_formats[] = {
    {GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, FmtDType_Unspecified, TrcImageFormat_Red_F32},
    {GL_DEPTH_STENCIL, GL_DEPTH_STENCIL, FmtDType_Unspecified, TrcImageFormat_F32_U24_U8},
    {GL_RED, GL_RED, FmtDType_Unspecified, TrcImageFormat_Red_F32},
    {GL_RG, GL_RG, FmtDType_Unspecified, TrcImageFormat_RedGreen_F32},
    {GL_RGB, GL_RGB, FmtDType_Unspecified, TrcImageFormat_RGB_F32},
    {GL_RGBA, GL_RGBA, FmtDType_Unspecified, TrcImageFormat_RGBA_F32},
    
    {GL_DEPTH_COMPONENT16, GL_DEPTH_COMPONENT, FmtDType_U16_Norm, TrcImageFormat_Red_F32},
    {GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, FmtDType_U24_Norm, TrcImageFormat_Red_F32},
    {GL_DEPTH_COMPONENT32, GL_DEPTH_COMPONENT, FmtDType_U32_Norm, TrcImageFormat_Red_F32},
    {GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT, FmtDType_F32, TrcImageFormat_Red_F32},
    
    {GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL, FmtDType_D24_S8, TrcImageFormat_F32_U24_U8},
    {GL_DEPTH32F_STENCIL8, GL_DEPTH_STENCIL, FmtDType_D32F_S8, TrcImageFormat_F32_U24_U8},
    
    {GL_STENCIL_INDEX1, GL_STENCIL_INDEX, FmtDType_S1, TrcImageFormat_Red_U32},
    {GL_STENCIL_INDEX4, GL_STENCIL_INDEX, FmtDType_S4, TrcImageFormat_Red_U32},
    {GL_STENCIL_INDEX8, GL_STENCIL_INDEX, FmtDType_S8, TrcImageFormat_Red_U32},
    {GL_STENCIL_INDEX16, GL_STENCIL_INDEX, FmtDType_S16, TrcImageFormat_Red_U32},
    
    {GL_R8, GL_RED, FmtDType_U8_Norm, TrcImageFormat_Red_F32},
    {GL_R8_SNORM, GL_RED, FmtDType_I8_Norm, TrcImageFormat_Red_F32},
    {GL_R16, GL_RED, FmtDType_U16_Norm, TrcImageFormat_Red_F32},
    {GL_R16_SNORM, GL_RED, FmtDType_I16_Norm, TrcImageFormat_Red_F32},
    
    {GL_RG8, GL_RG, FmtDType_U8_Norm, TrcImageFormat_RedGreen_F32},
    {GL_RG8_SNORM, GL_RG, FmtDType_I8_Norm, TrcImageFormat_RedGreen_F32},
    {GL_RG16, GL_RG, FmtDType_U16_Norm, TrcImageFormat_RedGreen_F32},
    {GL_RG16_SNORM, GL_RG, FmtDType_I16_Norm, TrcImageFormat_RedGreen_F32},
    
    {GL_R3_G3_B2, GL_RGB, FmtDType_U3_U3_U2_Norm, TrcImageFormat_RGB_F32},
    {GL_RGB4, GL_RGB, FmtDType_U4_Norm, TrcImageFormat_RGB_F32},
    {GL_RGB5, GL_RGB, FmtDType_U5_Norm, TrcImageFormat_RGB_F32},
    {GL_RGB8, GL_RGB, FmtDType_U8_Norm, TrcImageFormat_RGB_F32},
    {GL_RGB8_SNORM, GL_RGB, FmtDType_I8_Norm, TrcImageFormat_RGB_F32},
    {GL_RGB10, GL_RGB, FmtDType_U10_Norm, TrcImageFormat_RGB_F32},
    {GL_RGB12, GL_RGB, FmtDType_U12_Norm, TrcImageFormat_RGB_F32},
    {GL_RGB16_SNORM, GL_RGB, FmtDType_I16_Norm, TrcImageFormat_RGB_F32},
    {GL_RGB565, GL_RGB, FmtDType_U5_U6_U5_Norm, TrcImageFormat_RGB_F32},
    
    {GL_RGBA2, GL_RGBA, FmtDType_U2_Norm, TrcImageFormat_RGBA_F32},
    {GL_RGBA4, GL_RGBA, FmtDType_U4_Norm, TrcImageFormat_RGBA_F32},
    {GL_RGB5_A1, GL_RGBA, FmtDType_U5_U5_U5_U1_Norm, TrcImageFormat_RGBA_F32},
    {GL_RGBA8, GL_RGBA, FmtDType_U8_Norm, TrcImageFormat_RGBA_F32},
    {GL_RGBA8_SNORM, GL_RGBA, FmtDType_I8_Norm, TrcImageFormat_RGBA_F32},
    {GL_RGB10_A2, GL_RGBA, FmtDType_U10_U10_U10_U2_Norm, TrcImageFormat_RGBA_F32},
    {GL_RGB10_A2UI, GL_RGBA, FmtDType_U10_U10_U10_U2, TrcImageFormat_RGBA_U32},
    {GL_RGBA12, GL_RGBA, FmtDType_U12_Norm, TrcImageFormat_RGBA_F32},
    {GL_RGBA16, GL_RGBA, FmtDType_U16_Norm, TrcImageFormat_RGBA_F32},
    
    {GL_SRGB8, GL_RGB, FmtDType_U8_Srgb, TrcImageFormat_SRGB_U8},
    {GL_SRGB8_ALPHA8, GL_RGBA, FmtDType_U8_Srgb, TrcImageFormat_SRGBA_U8},
    
    {GL_R16F, GL_RED, FmtDType_F16, TrcImageFormat_Red_F32},
    {GL_RG16F, GL_RG, FmtDType_F16, TrcImageFormat_RedGreen_F32},
    {GL_RGB16F, GL_RGB, FmtDType_F16, TrcImageFormat_RGB_F32},
    {GL_RGBA16F, GL_RGBA, FmtDType_F16, TrcImageFormat_RGBA_F32},
    
    {GL_R32F, GL_RED, FmtDType_F32, TrcImageFormat_Red_F32},
    {GL_RG32F, GL_RG, FmtDType_F32, TrcImageFormat_RedGreen_F32},
    {GL_RGB32F, GL_RGB, FmtDType_F32, TrcImageFormat_RGB_F32},
    {GL_RGBA32F, GL_RGBA, FmtDType_F32, TrcImageFormat_RGBA_F32},
    
    {GL_R11F_G11F_B10F, GL_RGBA, FmtDType_F11_F11_F10, TrcImageFormat_RGB_F32},
    {GL_RGB9_E5, GL_RGB, FmtDType_9E5F, TrcImageFormat_RGB_F32},
    
    {GL_R8I, GL_RED, FmtDType_I8, TrcImageFormat_Red_I32},
    {GL_R8UI, GL_RED, FmtDType_U8, TrcImageFormat_Red_U32},
    {GL_R16I, GL_RED, FmtDType_I16, TrcImageFormat_Red_I32},
    {GL_R16UI, GL_RED, FmtDType_U16, TrcImageFormat_Red_U32},
    {GL_R32I, GL_RED, FmtDType_I32, TrcImageFormat_Red_I32},
    {GL_R32UI, GL_RED, FmtDType_U32, TrcImageFormat_Red_U32},
    
    {GL_RG8I, GL_RG, FmtDType_I8, TrcImageFormat_RedGreen_I32},
    {GL_RG8UI, GL_RG, FmtDType_U8, TrcImageFormat_RedGreen_U32},
    {GL_RG16I, GL_RG, FmtDType_I16, TrcImageFormat_RedGreen_I32},
    {GL_RG16UI, GL_RG, FmtDType_U16, TrcImageFormat_RedGreen_U32},
    {GL_RG32I, GL_RG, FmtDType_I32, TrcImageFormat_RedGreen_I32},
    {GL_RG32UI, GL_RG, FmtDType_U32, TrcImageFormat_RedGreen_U32},
    
    {GL_RGB8I, GL_RGB, FmtDType_I8, TrcImageFormat_RGB_I32},
    {GL_RGB8UI, GL_RGB, FmtDType_U8, TrcImageFormat_RGB_U32},
    {GL_RGB16I, GL_RGB, FmtDType_I16, TrcImageFormat_RGB_I32},
    {GL_RGB16UI, GL_RGB, FmtDType_U16, TrcImageFormat_RGB_U32},
    {GL_RGB32I, GL_RGB, FmtDType_I32, TrcImageFormat_RGB_I32},
    {GL_RGB32UI, GL_RGB, FmtDType_U32, TrcImageFormat_RGB_U32},
    
    {GL_RGBA8I, GL_RGBA, FmtDType_I8, TrcImageFormat_RGBA_I32},
    {GL_RGBA8UI, GL_RGBA, FmtDType_U8, TrcImageFormat_RGBA_U32},
    {GL_RGBA16I, GL_RGBA, FmtDType_I16, TrcImageFormat_RGBA_I32},
    {GL_RGBA16UI, GL_RGBA, FmtDType_U16, TrcImageFormat_RGBA_U32},
    {GL_RGBA32I, GL_RGBA, FmtDType_I32, TrcImageFormat_RGBA_I32},
    {GL_RGBA32UI, GL_RGBA, FmtDType_U32, TrcImageFormat_RGBA_U32},
    
    {GL_COMPRESSED_RED, GL_RED, FmtDType_Compressed, TrcImageFormat_Red_F32},
    {GL_COMPRESSED_RG, GL_RG, FmtDType_Compressed, TrcImageFormat_RedGreen_F32},
    {GL_COMPRESSED_RGB, GL_RGB, FmtDType_Compressed, TrcImageFormat_RGB_F32},
    {GL_COMPRESSED_RGBA, GL_RGBA, FmtDType_Compressed, TrcImageFormat_RGBA_F32},
    {GL_COMPRESSED_SRGB, GL_RGB, FmtDType_Compressed, TrcImageFormat_SRGB_U8},
    {GL_COMPRESSED_SRGB_ALPHA, GL_RGBA, FmtDType_Compressed, TrcImageFormat_SRGBA_U8},
    
    {GL_COMPRESSED_RED_RGTC1, GL_RED, FmtDType_Compressed, TrcImageFormat_Red_F32},
    {GL_COMPRESSED_SIGNED_RED_RGTC1, GL_RED, FmtDType_Compressed, TrcImageFormat_Red_F32},
    {GL_COMPRESSED_RG_RGTC2, GL_RG, FmtDType_Compressed, TrcImageFormat_RedGreen_F32},
    {GL_COMPRESSED_SIGNED_RG_RGTC2, GL_RG, FmtDType_Compressed, TrcImageFormat_RedGreen_F32},
    
    {GL_COMPRESSED_RGBA_BPTC_UNORM, GL_RGBA, FmtDType_Compressed, TrcImageFormat_RGBA_F32},
    {GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM, GL_RGBA, FmtDType_Compressed, TrcImageFormat_SRGBA_U8},
    {GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT, GL_RGB, FmtDType_Compressed, TrcImageFormat_RGB_F32},
    {GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT, GL_RGB, FmtDType_Compressed, TrcImageFormat_RGB_F32},
    
    {GL_COMPRESSED_RGB8_ETC2, GL_RGB, FmtDType_Compressed, TrcImageFormat_RGB_F32},
    {GL_COMPRESSED_SRGB8_ETC2, GL_RGB, FmtDType_Compressed, TrcImageFormat_SRGB_U8},
    {GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2, GL_RGBA, FmtDType_Compressed, TrcImageFormat_RGBA_F32},
    {GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2, GL_RGBA, FmtDType_Compressed, TrcImageFormat_SRGBA_U8},
    {GL_COMPRESSED_RGBA8_ETC2_EAC, GL_RGBA, FmtDType_Compressed, TrcImageFormat_RGBA_F32},
    {GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC, GL_RGBA, FmtDType_Compressed, TrcImageFormat_SRGBA_U8},
    {GL_COMPRESSED_R11_EAC, GL_RED, FmtDType_Compressed, TrcImageFormat_Red_F32},
    {GL_COMPRESSED_SIGNED_R11_EAC, GL_RED, FmtDType_Compressed, TrcImageFormat_Red_F32},
    {GL_COMPRESSED_RG11_EAC, GL_RG, FmtDType_Compressed, TrcImageFormat_RedGreen_F32},
    {GL_COMPRESSED_SIGNED_RG11_EAC, GL_RG, FmtDType_Compressed, TrcImageFormat_RedGreen_F32}};

internal_format_info_t get_internal_format_info(GLenum internal_format) {
    for (size_t i = 0; i < sizeof(internal_formats)/sizeof(internal_formats[0]); i++) {
        if (internal_formats[i].internal_format == internal_format)
            return internal_formats[i];
    }
    assert(false);
}

int get_internal_format_size(GLenum internal_format) {
    internal_format_info_t info = get_internal_format_info(internal_format);
    size_t count = 0;
    switch (info.base) {
    case GL_DEPTH_COMPONENT:
    case GL_STENCIL_INDEX:
    case GL_RED: count = 1; break;
    case GL_DEPTH_STENCIL:
    case GL_RG: count = 2; break;
    case GL_RGB: count = 3; break;
    case GL_RGBA: count = 4; break;
    default: return -1;
    }
    
    switch (info.dtype) {
    case FmtDType_U8_Norm: return count;
    case FmtDType_I8_Norm: return count;
    case FmtDType_U16_Norm: return count * 2;
    case FmtDType_I16_Norm: return count * 2;
    case FmtDType_U24_Norm: return count * 3;
    case FmtDType_U32_Norm: return count * 4;
    case FmtDType_U8: return count;
    case FmtDType_I8: return count;
    case FmtDType_U16: return count * 2;
    case FmtDType_I16: return count * 2;
    case FmtDType_U32: return count * 4;
    case FmtDType_I32: return count * 4;
    case FmtDType_F16: return count * 2;
    case FmtDType_F32: return count * 4;
    case FmtDType_D24_S8: return 4;
    //TODO: These formats?
    case FmtDType_D32F_S8: return 5;
    case FmtDType_S1: return ceil_div(count, 8);
    case FmtDType_S4: return ceil_div(count*4, 8);
    case FmtDType_S8: return ceil_div(count*8, 8);
    case FmtDType_S16: return ceil_div(count*16, 8);
    //End These formats TODO
    case FmtDType_F11_F11_F10: return 4;
    case FmtDType_9E5F: return 4;
    case FmtDType_U3_U3_U2_Norm: return 1;
    case FmtDType_U2_Norm: return ceil_div(count*2, 8);
    case FmtDType_U4_Norm: return ceil_div(count*4, 8);
    case FmtDType_U5_Norm: return ceil_div(count*5, 8);
    case FmtDType_U10_Norm: return ceil_div(count*10, 8);
    case FmtDType_U12_Norm: return ceil_div(count*12, 8);
    case FmtDType_U5_U6_U5_Norm: return 2;
    case FmtDType_U5_U5_U5_U1_Norm: return 2;
    case FmtDType_U10_U10_U10_U2: return 4;
    case FmtDType_U10_U10_U10_U2_Norm: return 4;
    case FmtDType_U8_Srgb: return 3;
    case FmtDType_Unspecified:
    case FmtDType_Compressed: return -1;
    }
    return -1;
}

const char* validate_format_combination(GLenum format, GLenum type, GLenum internal) {
    //TODO: Is this complete?
    if (!not_one_of(type, GL_UNSIGNED_BYTE_3_3_2, GL_UNSIGNED_BYTE_2_3_3_REV, GL_UNSIGNED_SHORT_5_6_5, GL_UNSIGNED_SHORT_5_6_5_REV, -1) && format!=GL_RGB)
        return "Invalid format + type combination";
    if (!not_one_of(type, GL_UNSIGNED_SHORT_4_4_4_4, GL_UNSIGNED_SHORT_4_4_4_4_REV, GL_UNSIGNED_SHORT_5_5_5_1, GL_UNSIGNED_SHORT_1_5_5_5_REV, GL_UNSIGNED_INT_8_8_8_8,
        GL_UNSIGNED_INT_8_8_8_8_REV, GL_UNSIGNED_INT_10_10_10_2, GL_UNSIGNED_INT_2_10_10_10_REV, -1) && format!=GL_RGBA && format!=GL_BGRA)
        return "Invalid format + type combination";
    if (format==GL_DEPTH_COMPONENT && not_one_of(internal, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT16, GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT32, -1))
        return "Invalid format + internal format combination";
    if (format!=GL_DEPTH_COMPONENT && !not_one_of(internal, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT16, GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT32, -1))
        return "Invalid format + internal format combination";
    if (format==GL_STENCIL_INDEX && internal!=GL_STENCIL_INDEX)
        return "Invalid format + internal format combination";
    if (format!=GL_STENCIL_INDEX && internal==GL_STENCIL_INDEX)
        return "Invalid format + internal format combination";
    return NULL;
}

static void gen_textures(size_t count, const GLuint* real, const GLuint* fake, bool create, GLenum target) {
    trc_gl_texture_rev_t rev;
    rev.has_object = create;
    rev.type = create ? target : 0;
    rev.depth_stencil_mode = GL_DEPTH_COMPONENT;
    rev.base_level = 0;
    rev.sample_params.border_color[0] = 0;
    rev.sample_params.border_color[1] = 0;
    rev.sample_params.border_color[2] = 0;
    rev.sample_params.border_color[3] = 0;
    rev.sample_params.compare_func = GL_LEQUAL;
    rev.sample_params.compare_mode = GL_NONE;
    rev.sample_params.lod_bias = 0;
    rev.sample_params.min_filter = rev.type==GL_TEXTURE_RECTANGLE ? GL_LINEAR : GL_NEAREST_MIPMAP_LINEAR;
    rev.sample_params.mag_filter = GL_LINEAR;
    rev.sample_params.min_lod = -1000;
    rev.sample_params.max_lod = 1000;
    rev.max_level = 1000;
    rev.swizzle[0] = GL_RED;
    rev.swizzle[1] = GL_GREEN;
    rev.swizzle[2] = GL_BLUE;
    rev.swizzle[3] = GL_ALPHA;
    GLenum wrap_mode = rev.type==GL_TEXTURE_RECTANGLE ? GL_CLAMP_TO_EDGE : GL_REPEAT;
    rev.sample_params.wrap_s = wrap_mode;
    rev.sample_params.wrap_t = wrap_mode;
    rev.sample_params.wrap_r = wrap_mode;
    rev.sample_params.max_anisotropy = 1.0;
    rev.images = NULL;
    rev.immutable = false;
    rev.fixed_sample_locations = false;
    rev.samples = 0;
    rev.complete_status = -1;
    rev.incompleteness_reason = NULL;
    for (size_t i = 0; i < count; ++i) {
        rev.real = real[i];
        trc_create_named_obj(ctx->ns, TrcTexture, fake[i], &rev);
    }
}

static void set_texture_image(const trc_gl_texture_rev_t* rev, uint level, uint face,
                              uint internal_format, uint width, uint height, uint depth,
                              trc_image_format_t format, trc_chunked_data_t data) {
    trc_gl_texture_image_t img;
    memset(&img, 0, sizeof(img)); //Fill in padding to fix use of uninitialized memory errors because of compression
    img.face = face;
    img.level = level;
    img.internal_format = internal_format;
    img.compressed_internal_format = is_compressed_format(internal_format);
    img.width = width;
    img.height = height;
    img.depth = depth;
    img.data_format = format;
    img.data = data;
    
    size_t img_count = rev->images->size / sizeof(trc_gl_texture_image_t);
    trc_gl_texture_image_t* newimages = malloc((img_count+1)*sizeof(trc_gl_texture_image_t));
    
    trc_gl_texture_image_t* images = trc_map_data(rev->images, TRC_MAP_READ);
    bool replaced = false;
    for (size_t i = 0; i < img_count; i++) {
        if (images[i].face==img.face && images[i].level==img.level) {
            newimages[i] = img;
            replaced = true;
        } else {
            newimages[i] = images[i];
        }
    }
    trc_unmap_data(images);
    
    trc_gl_texture_rev_t newrev = *rev;
    if (!replaced) newimages[img_count++] = img;
    
    size_t size = img_count * sizeof(trc_gl_texture_image_t);
    newrev.images = trc_create_data_no_copy(ctx->trace, size, newimages, 0);
    
    newrev.complete_status = -1;
    
    set_texture(&newrev);
}

static bool tex_image(bool dsa, GLuint tex_or_target, GLint level, GLint internal_format,
                      GLint border, GLenum format, GLenum type, bool sub, int dim, ...) {
    int size[3];
    int offset[3];
    va_list list;
    va_start(list, dim);
    for (int i = 0; i < dim; i++) size[i] = va_arg(list, int);
    for (int i = 0; (i<dim) && sub; i++) offset[i] = va_arg(list, int);
    va_end(list);
    
    const trc_gl_texture_rev_t* tex_rev;
    if (dsa) tex_rev = get_texture(tex_or_target);
    else tex_rev = get_bound_tex(tex_or_target);
    if (!tex_rev) ERROR2(false, dsa?"Invalid texture name":"No texture bound to target");
    if (!tex_rev->has_object) ERROR2(false, "Texture name has no object");
    
    if (tex_rev->immutable) ERROR2(false, "Texture object is an immutable-format texture");
    
    int max_size = gls_get_state_int(GL_MAX_TEXTURE_SIZE, 0);
    if (level<0 || level>ceil_log2(max_size)) ERROR2(false, "Invalid level");
    for (int i = 0; i < dim; i++)
        if (size[i]<0 || size[i]>max_size) ERROR2(false, "Invalid %s", (const char*[]){"width", "height", "depth/layers"}[i]);
    //TODO: Test against GL_MAX_ARRAY_TEXTURE_LAYERS
    if (sub) {
        size_t img_count = tex_rev->images->size / sizeof(trc_gl_texture_image_t);
        trc_gl_texture_image_t* images = trc_map_data(tex_rev->images, TRC_MAP_READ);
        trc_gl_texture_image_t* image = NULL;
        for (size_t i = 0; i < img_count; i++) {
            if (images[i].level == level) {
                image = &images[i];
                break;
            }
        }
        if (image == NULL) {
            trc_unmap_data(images);
            ERROR2(false, "No such mipmap");
        }
        internal_format = image->internal_format; //Used for format validation later
        if (dim>=1 && (offset[0]<0 || offset[0]+size[0] > image->width)) ERROR2(false, "Invalid x range");
        if (dim>=2 && (offset[1]<0 || offset[1]+size[1] > image->height)) ERROR2(false, "Invalid x range");
        if (dim>=3 && (offset[2]<0 || offset[2]+size[2] > image->depth)) ERROR2(false, "Invalid x range");
        trc_unmap_data(images);
    }
    if (border != 0) ERROR2(false, "Border must be 0");
    
    const char* err = validate_format_combination(format, type, internal_format);
    if (err) ERROR2(false, err);
    
    trc_obj_t* pu_buf = gls_get_bound_buffer(GL_PIXEL_UNPACK_BUFFER);
    const trc_gl_buffer_rev_t* pu_buf_rev = trc_obj_get_rev(pu_buf, -1);
    if (pu_buf_rev && pu_buf_rev->mapped) ERROR2(false, "GL_PIXEL_UNPACK_BUFFER is mapped");
    //TODO: More validation for GL_PIXEL_UNPACK_BUFFER
    
    return true;
}

static bool tex_image_ms(bool dsa, GLuint tex_or_target, GLsizei samples, GLenum internal_format,
                         GLsizei width, GLsizei height, GLboolean fixed_sample_locations,
                         bool layered, GLsizei layers) {
    const trc_gl_texture_rev_t* tex_rev;
    if (dsa) tex_rev = get_texture(tex_or_target);
    else tex_rev = get_bound_tex(tex_or_target);
    if (!tex_rev) ERROR2(false, dsa?"Invalid texture name":"No texture bound to target");
    if (!tex_rev->has_object) ERROR2(false, "Texture name has no object");
    
    int max_size = gls_get_state_int(GL_MAX_TEXTURE_SIZE, 0);
    if (width<0 || width>max_size) ERROR2(false, "Invalid width");
    if (height<0 || height>max_size) ERROR2(false, "Invalid width");
    //TODO: Test against GL_MAX_ARRAY_TEXTURE_LAYERS
    
    //TODO: Test sample count against limits
    
    trc_gl_texture_rev_t newrev = *tex_rev;
    newrev.fixed_sample_locations = fixed_sample_locations;
    newrev.samples = samples;
    set_texture(&newrev);
    
    return true;
}

const trc_gl_texture_rev_t* get_bound_tex(uint target) {
    uint unit = gls_get_active_texture_unit(ctx->trace);
    switch (target) {
    case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
    case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:
    case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
    case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:
    case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:
    case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z: target = GL_TEXTURE_CUBE_MAP; break;
    }
    return trc_obj_get_rev(gls_get_bound_textures(target, unit), -1);
}

static void save_init_packing_config(GLint temp[9]) {
    F(glGetIntegerv)(GL_PACK_SWAP_BYTES, &temp[0]);
    F(glGetIntegerv)(GL_PACK_LSB_FIRST, &temp[1]);
    F(glGetIntegerv)(GL_PACK_ROW_LENGTH, &temp[2]);
    F(glGetIntegerv)(GL_PACK_IMAGE_HEIGHT, &temp[3]);
    F(glGetIntegerv)(GL_PACK_SKIP_ROWS, &temp[4]);
    F(glGetIntegerv)(GL_PACK_SKIP_PIXELS, &temp[5]);
    F(glGetIntegerv)(GL_PACK_SKIP_IMAGES, &temp[6]);
    F(glGetIntegerv)(GL_PACK_ALIGNMENT, &temp[7]);
    F(glGetIntegerv)(GL_PIXEL_PACK_BUFFER_BINDING, &temp[8]);
    F(glPixelStorei)(GL_PACK_SWAP_BYTES, 0);
    F(glPixelStorei)(GL_PACK_LSB_FIRST, 0);
    F(glPixelStorei)(GL_PACK_ROW_LENGTH, 0);
    F(glPixelStorei)(GL_PACK_IMAGE_HEIGHT, 0);
    F(glPixelStorei)(GL_PACK_SKIP_ROWS, 0);
    F(glPixelStorei)(GL_PACK_SKIP_PIXELS, 0);
    F(glPixelStorei)(GL_PACK_SKIP_IMAGES, 0);
    F(glPixelStorei)(GL_PACK_ALIGNMENT, 1);
    F(glBindBuffer)(GL_PIXEL_PACK_BUFFER, 0);
}

static void restore_packing_config(GLint temp[9]) {
    F(glPixelStorei)(GL_PACK_SWAP_BYTES, temp[0]);
    F(glPixelStorei)(GL_PACK_LSB_FIRST, temp[1]);
    F(glPixelStorei)(GL_PACK_ROW_LENGTH, temp[2]);
    F(glPixelStorei)(GL_PACK_IMAGE_HEIGHT, temp[3]);
    F(glPixelStorei)(GL_PACK_SKIP_ROWS, temp[4]);
    F(glPixelStorei)(GL_PACK_SKIP_PIXELS, temp[5]);
    F(glPixelStorei)(GL_PACK_SKIP_IMAGES, temp[6]);
    F(glPixelStorei)(GL_PACK_ALIGNMENT, temp[7]);
    F(glBindBuffer)(GL_PIXEL_PACK_BUFFER, temp[8]);
}

static void get_ms_tex_data(GLuint src_tex, uint level, uint width, uint height, uint depth, uint ifmt, uint format, uint type, void* data) {
    GLint prev_tex, prev_read_fb, prev_draw_fb;
    F(glGetIntegerv)(depth?GL_TEXTURE_BINDING_2D_ARRAY:GL_TEXTURE_BINDING_2D, &prev_tex);
    F(glGetIntegerv)(GL_READ_FRAMEBUFFER_BINDING, &prev_read_fb);
    F(glGetIntegerv)(GL_DRAW_FRAMEBUFFER_BINDING, &prev_draw_fb);
    
    GLuint target = depth ? GL_TEXTURE_2D_ARRAY : GL_TEXTURE_2D;
    
    GLuint dest_tex;
    F(glGenTextures)(1, &dest_tex);
    F(glBindTexture)(target, dest_tex);
    if (depth)
        F(glTexImage3D)(GL_TEXTURE_2D_ARRAY, level, ifmt, width, height, depth, 0, format, type, NULL);
    else
        F(glTexImage2D)(GL_TEXTURE_2D, level, ifmt, width, height, 0, format, type, NULL);
    
    GLuint dest_fb, src_fb;
    F(glGenFramebuffers)(1, &dest_fb);
    F(glGenFramebuffers)(1, &src_fb);
    F(glBindFramebuffer)(GL_DRAW_FRAMEBUFFER, dest_fb);
    F(glBindFramebuffer)(GL_READ_FRAMEBUFFER, src_fb);
    
    GLuint attach = GL_COLOR_ATTACHMENT0;
    switch (get_internal_format_info(ifmt).base) {
    case GL_DEPTH_COMPONENT: attach = GL_DEPTH_ATTACHMENT; break;
    case GL_DEPTH_STENCIL: attach = GL_DEPTH_STENCIL_ATTACHMENT; break;
    case GL_STENCIL_INDEX: attach = GL_STENCIL_ATTACHMENT; break;
    }
    
    GLbitfield mask = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT;
    for (uint i = 0; i < depth; i++) {
        F(glFramebufferTextureLayer)(GL_DRAW_FRAMEBUFFER, attach, dest_tex, level, i);
        F(glFramebufferTextureLayer)(GL_READ_FRAMEBUFFER, attach, src_tex, level, i);
        F(glBlitFramebuffer)(0, 0, width, height, 0, 0, width, height, mask, GL_NEAREST);
    }
    if (!depth) {
        F(glFramebufferTexture)(GL_DRAW_FRAMEBUFFER, attach, dest_tex, level);
        F(glFramebufferTexture)(GL_READ_FRAMEBUFFER, attach, src_tex, level);
        F(glBlitFramebuffer)(0, 0, width, height, 0, 0, width, height, mask, GL_NEAREST);
    }
    
    F(glGetTexImage)(target, level, format, type, data);
    
    F(glDeleteFramebuffers)(1, &src_fb);
    F(glDeleteFramebuffers)(1, &dest_fb);
    F(glDeleteTextures)(1, &dest_tex);
    
    F(glBindFramebuffer)(GL_DRAW_FRAMEBUFFER, prev_draw_fb);
    F(glBindFramebuffer)(GL_READ_FRAMEBUFFER, prev_read_fb);
    F(glBindTexture)(target, prev_tex);
}

void update_tex_image(const trc_gl_texture_rev_t* tex, uint level, uint face) {
    GLenum prevget;
    switch (tex->type) {
    case GL_TEXTURE_1D: prevget = GL_TEXTURE_BINDING_1D; break;
    case GL_TEXTURE_2D: prevget = GL_TEXTURE_BINDING_2D; break;
    case GL_TEXTURE_3D: prevget = GL_TEXTURE_BINDING_3D; break;
    case GL_TEXTURE_1D_ARRAY: prevget = GL_TEXTURE_BINDING_1D_ARRAY; break;
    case GL_TEXTURE_2D_ARRAY: prevget = GL_TEXTURE_BINDING_2D_ARRAY; break;
    case GL_TEXTURE_RECTANGLE: prevget = GL_TEXTURE_BINDING_RECTANGLE; break;
    case GL_TEXTURE_CUBE_MAP: prevget = GL_TEXTURE_BINDING_CUBE_MAP; break;
    case GL_TEXTURE_CUBE_MAP_ARRAY: prevget = GL_TEXTURE_BINDING_CUBE_MAP; break;
    case GL_TEXTURE_BUFFER: return; //TODO: Error
    case GL_TEXTURE_2D_MULTISAMPLE: prevget = GL_TEXTURE_BINDING_2D_MULTISAMPLE; break;
    case GL_TEXTURE_2D_MULTISAMPLE_ARRAY: prevget = GL_TEXTURE_BINDING_2D_MULTISAMPLE_ARRAY; break;
    }
    GLint prev;
    F(glGetIntegerv)(prevget, &prev);
    F(glBindTexture)(tex->type, tex->real);
    
    uint target = tex->type;
    if (target==GL_TEXTURE_CUBE_MAP) target = GL_TEXTURE_CUBE_MAP_POSITIVE_X + face;
    
    GLint width, height, depth, internal_format;
    F(glGetTexLevelParameteriv)(target, level, GL_TEXTURE_WIDTH, &width);
    F(glGetTexLevelParameteriv)(target, level, GL_TEXTURE_HEIGHT, &height);
    F(glGetTexLevelParameteriv)(target, level, GL_TEXTURE_DEPTH, &depth);
    F(glGetTexLevelParameteriv)(target, level, GL_TEXTURE_INTERNAL_FORMAT, &internal_format);
    if (!width) width = 1;
    if (!height) height = 1;
    if (!depth) depth = 1;
    
    if (!internal_format) ERROR2(, "Internal error - internal format is zero");
    
    trc_image_format_t image_format = get_internal_format_info(internal_format).trc_image_format;
    
    GLenum format, type;
    size_t pixel_size;
    switch (image_format) {
    case TrcImageFormat_Red_U32: pixel_size = 4; format = GL_RED; type = GL_UNSIGNED_INT; break;
    case TrcImageFormat_RedGreen_U32: pixel_size = 8; format = GL_RG; type = GL_UNSIGNED_INT; break;
    case TrcImageFormat_RGB_U32: pixel_size = 12; format = GL_RGB; type = GL_UNSIGNED_INT; break;
    case TrcImageFormat_RGBA_U32: pixel_size = 16; format = GL_RGBA; type = GL_UNSIGNED_INT; break;
    case TrcImageFormat_Red_I32: pixel_size = 4; format = GL_RED; type = GL_INT; break;
    case TrcImageFormat_RedGreen_I32: pixel_size = 8; format = GL_RG; type = GL_INT; break;
    case TrcImageFormat_RGB_I32: pixel_size = 12; format = GL_RGB; type = GL_INT; break;
    case TrcImageFormat_RGBA_I32: pixel_size = 16; format = GL_RGBA; type = GL_INT; break;
    case TrcImageFormat_Red_F32: pixel_size = 4; format = GL_RED; type = GL_FLOAT; break;
    case TrcImageFormat_RedGreen_F32: pixel_size = 8; format = GL_RG; type = GL_FLOAT; break;
    case TrcImageFormat_RGB_F32: pixel_size = 12; format = GL_RGB; type = GL_FLOAT; break;
    case TrcImageFormat_RGBA_F32: pixel_size = 16; format = GL_RGBA; type = GL_FLOAT; break;
    case TrcImageFormat_SRGB_U8: pixel_size = 3; format = GL_RGB; type = GL_UNSIGNED_BYTE; break;
    case TrcImageFormat_SRGBA_U8: pixel_size = 4; format = GL_RGBA; type = GL_UNSIGNED_BYTE; break;
    case TrcImageFormat_F32_U24_U8: pixel_size = 8; format = GL_DEPTH_STENCIL; type = GL_FLOAT_32_UNSIGNED_INT_24_8_REV; break;
    case TrcImageFormat_RGBA_U8: pixel_size = 4; format = GL_RGBA; type = GL_UNSIGNED_BYTE; break;
    }
    switch (internal_format) {
    case GL_DEPTH_COMPONENT: format = GL_DEPTH_COMPONENT; break;
    case GL_DEPTH_COMPONENT16: format = GL_DEPTH_COMPONENT; break;
    case GL_DEPTH_COMPONENT24: format = GL_DEPTH_COMPONENT; break;
    case GL_DEPTH_COMPONENT32: format = GL_DEPTH_COMPONENT; break;
    case GL_DEPTH_COMPONENT32F: format = GL_DEPTH_COMPONENT; break;
    case GL_STENCIL_INDEX: format = GL_STENCIL_INDEX; break;
    case GL_STENCIL_INDEX1: format = GL_STENCIL_INDEX; break;
    case GL_STENCIL_INDEX4: format = GL_STENCIL_INDEX; break;
    case GL_STENCIL_INDEX8: format = GL_STENCIL_INDEX; break;
    case GL_STENCIL_INDEX16: format = GL_STENCIL_INDEX; break;
    default: break;
    }
    
    size_t data_size = width * height * depth * pixel_size;
    void* data_buf = malloc(data_size);
    
    GLint temp[9];
    save_init_packing_config(temp);
    if (tex->type == GL_TEXTURE_2D_MULTISAMPLE) {
        get_ms_tex_data(tex->real, level, width, height, 0, internal_format, format, type, data_buf);
    } else if (tex->type == GL_TEXTURE_2D_MULTISAMPLE_ARRAY) {
        get_ms_tex_data(tex->real, level, width, height, depth, internal_format, format, type, data_buf);
    } else {
        F(glGetTexImage)(target, level, format, type, data_buf);
    }
    restore_packing_config(temp);
    
    F(glBindTexture)(tex->type, prev);
    
    trc_chunked_data_t data = trc_create_chunked_data(ctx->trace, data_size, data_buf);
    free(data_buf);
    
    set_texture_image(tex, level, face, internal_format, width, height, depth, image_format, data);
}

static uint get_face_from_target(GLenum target) {
    if (target>=GL_TEXTURE_CUBE_MAP_POSITIVE_X && target<=GL_TEXTURE_CUBE_MAP_NEGATIVE_Z)
        return target - GL_TEXTURE_CUBE_MAP_POSITIVE_X;
    return 0;
}

static void update_bound_tex_image(uint target, uint level) {
    const trc_gl_texture_rev_t* rev = get_bound_tex(target);
    if (!rev) ERROR2(, "No texture bound to target");
    update_tex_image(rev, level, get_face_from_target(target));
}

static bool tex_buffer(GLuint tex_or_target, bool dsa, GLenum internalformat,
                       GLuint buffer, GLintptr offset, GLsizeiptr size) {
    const trc_gl_texture_rev_t* rev = NULL;
    if (!dsa) rev = get_bound_tex(tex_or_target);
    else rev = get_texture(tex_or_target);
    if (!rev) ERROR2(false, dsa?"Invalid texture name":"No texture bound to target");
    if (!rev->has_object) ERROR2(false, "Texture name has no object");
    
    const trc_gl_buffer_rev_t* buffer_rev = buffer ? get_buffer(buffer) : NULL;
    if (!buffer_rev && buffer) ERROR2(false, "Invalid buffer name");
    if (buffer && !buffer_rev->has_object) ERROR2(false, "Buffer name has no object");
    if (offset<0 || size<=0 || offset+size>buffer_rev->data.size) ERROR2(false, "Invalid range");
    //TODO: Check alignment
    
    trc_gl_texture_rev_t newrev = *rev;
    newrev.buffer.internal_format = internalformat;
    newrev.buffer.buffer = buffer;
    newrev.buffer.offset = offset;
    //TODO: Write -1 when size<0? (to handle when the buffer's size changes)
    newrev.buffer.size = buffer ? (size<0?buffer_rev->data.size:size) : 0;
    newrev.complete_status = -1;
    set_texture(&newrev);
    
    return true;
}

bool sample_param_double(trc_gl_sample_params_t* params, GLenum param, uint32_t count, const double* val) {
    switch (param) {
    case GL_TEXTURE_MIN_FILTER:
    case GL_TEXTURE_MAG_FILTER:
    case GL_TEXTURE_MIN_LOD:
    case GL_TEXTURE_MAX_LOD:
    case GL_TEXTURE_WRAP_S:
    case GL_TEXTURE_WRAP_T:
    case GL_TEXTURE_WRAP_R:
    case GL_TEXTURE_COMPARE_MODE:
    case GL_TEXTURE_COMPARE_FUNC:
    case GL_TEXTURE_LOD_BIAS:
    case GL_TEXTURE_MAX_ANISOTROPY:
        if (count != 1)
            ERROR2(true, "Expected 1 value. Got %u.", count);
        break;
    case GL_TEXTURE_BORDER_COLOR:
        if (count != 4)
            ERROR2(true, "Expected 4 values. Got %u.", count);
        break;
    }
    
    switch (param) {
    case GL_TEXTURE_MIN_FILTER:
        if (val[0]!=GL_LINEAR && val[0]!=GL_NEAREST && val[0]!=GL_NEAREST_MIPMAP_NEAREST &&
            val[0]!=GL_LINEAR_MIPMAP_NEAREST && val[0]!=GL_NEAREST_MIPMAP_LINEAR &&
            val[0]!=GL_LINEAR_MIPMAP_LINEAR)
            ERROR2(true, "Invalid minification filter");
        break;
    case GL_TEXTURE_MAG_FILTER:
        if (val[0]!=GL_LINEAR && val[0]!=GL_NEAREST)
            ERROR2(true, "Invalid magnification filter");
        break;
    case GL_TEXTURE_WRAP_S:
    case GL_TEXTURE_WRAP_T:
    case GL_TEXTURE_WRAP_R:
        if (val[0]!=GL_CLAMP_TO_EDGE && val[0]!=GL_CLAMP_TO_BORDER && val[0]!=GL_MIRRORED_REPEAT &&
            val[0]!=GL_REPEAT && val[0]!=GL_MIRROR_CLAMP_TO_EDGE && val[0]!=GL_CLAMP_TO_EDGE)
            ERROR2(true, "Invalid wrap mode");
        if (val[0]==GL_MIRROR_CLAMP_TO_EDGE && gls_get_ver()<440)
            ERROR2(true, "GL_MIRROR_CLAMP_TO_EDGE is not supported until OpenGL 4.4");
        break;
    case GL_TEXTURE_COMPARE_MODE:
        if (val[0]!=GL_COMPARE_REF_TO_TEXTURE && val[0]!=GL_NONE)
            ERROR2(true, "Invalid compare mode");
        break;
    case GL_TEXTURE_COMPARE_FUNC:
        if (val[0]!=GL_LEQUAL && val[0]!=GL_GEQUAL && val[0]!=GL_LESS && val[0]!=GL_GREATER &&
            val[0]!=GL_EQUAL && val[0]!=GL_NOTEQUAL && val[0]!=GL_ALWAYS && val[0]!=GL_NEVER)
            ERROR2(true, "Invalid compare function");
        break;
    case GL_TEXTURE_MAX_ANISOTROPY:
        if (val[0] < 1.0) ERROR2(true, "Invalid max anisotropy");
        break;
    }
    
    switch (param) {
    case GL_TEXTURE_MIN_FILTER: params->min_filter = val[0]; break;
    case GL_TEXTURE_MAG_FILTER: params->mag_filter = val[0]; break;
    case GL_TEXTURE_MIN_LOD: params->min_lod = val[0]; break;
    case GL_TEXTURE_MAX_LOD: params->max_lod = val[0]; break;
    case GL_TEXTURE_WRAP_S: params->wrap_s = val[0]; break;
    case GL_TEXTURE_WRAP_T: params->wrap_t = val[0]; break;
    case GL_TEXTURE_WRAP_R: params->wrap_r = val[0]; break;
    case GL_TEXTURE_BORDER_COLOR:
        for (uint i = 0; i < 4; i++) params->border_color[i] = val[i];
        break;
    case GL_TEXTURE_COMPARE_MODE: params->compare_mode = val[0]; break;
    case GL_TEXTURE_COMPARE_FUNC: params->compare_func = val[0]; break;
    case GL_TEXTURE_LOD_BIAS: params->lod_bias = val[0]; break;
    case GL_TEXTURE_MAX_ANISOTROPY: params->max_anisotropy = val[0]; break;
    }
    
    return false;
}

static const char* sample_params_valid_with_tex(const trc_gl_sample_params_t* params, const trc_gl_texture_rev_t* tex) {
    if (tex->type == GL_TEXTURE_RECTANGLE) {
        if (params->min_filter!=GL_NEAREST && params->min_filter!=GL_LINEAR)
            return "Mipmapping is not supported for rectangle textures";
        
        switch (params->wrap_s) {
        case GL_MIRROR_CLAMP_TO_EDGE:
        case GL_MIRRORED_REPEAT:
        case GL_REPEAT:
            return "Wrap S mode is not supported for rectangle textures";
        }
        
        switch (params->wrap_t) {
        case GL_MIRROR_CLAMP_TO_EDGE:
        case GL_MIRRORED_REPEAT:
        case GL_REPEAT:
            return "Wrap T mode is not supported for rectangle textures";
        }
    }
    
    return NULL;
}

static bool texture_param_double(bool dsa, GLuint tex_or_target, GLenum param,
                                 uint32_t count, const double* val) {
    const trc_gl_texture_rev_t* rev = NULL;
    if (dsa) rev = get_texture(tex_or_target);
    else rev = get_bound_tex(tex_or_target);
    if (!rev) ERROR2(true, dsa?"Invalid texture name":"No texture bound to target");
    if (!rev->has_object) ERROR2(true, "Texture name has no object");
    trc_gl_texture_rev_t newrev = *rev;
    
    bool rectangle = newrev.type==GL_TEXTURE_RECTANGLE;
    bool multisampled = newrev.type==GL_TEXTURE_2D_MULTISAMPLE || newrev.type==GL_TEXTURE_2D_MULTISAMPLE_ARRAY;
    
    switch (param) {
    case GL_TEXTURE_MIN_FILTER:
    case GL_TEXTURE_MAG_FILTER:
    case GL_TEXTURE_MIN_LOD:
    case GL_TEXTURE_MAX_LOD:
    case GL_TEXTURE_WRAP_S:
    case GL_TEXTURE_WRAP_T:
    case GL_TEXTURE_WRAP_R:
    case GL_TEXTURE_COMPARE_MODE:
    case GL_TEXTURE_COMPARE_FUNC:
    case GL_TEXTURE_BORDER_COLOR:
    case GL_TEXTURE_LOD_BIAS:
    case GL_TEXTURE_MAX_ANISOTROPY: {
        if (multisampled)
            ERROR2(true, "Sampler parameters cannot be set for multisampled textures");
        const char* err = sample_params_valid_with_tex(&newrev.sample_params, &newrev);
        if (err) ERROR2(true, err);
        sample_param_double(&newrev.sample_params, param, count, val);
        set_texture(&newrev);
        return false;
    }
    case GL_DEPTH_STENCIL_TEXTURE_MODE:
    case GL_TEXTURE_BASE_LEVEL:
    case GL_TEXTURE_MAX_LEVEL:
    case GL_TEXTURE_SWIZZLE_R:
    case GL_TEXTURE_SWIZZLE_G:
    case GL_TEXTURE_SWIZZLE_B:
    case GL_TEXTURE_SWIZZLE_A: {
        if (count != 1) ERROR2(true, "Expected 1 value. Got %u", count);
        break;
    }
    case GL_TEXTURE_SWIZZLE_RGBA: {
        if (count != 4) ERROR2(true, "Expected 4 values. Got %u", count);
        break;
    }
    }
    
    switch (param) {
    case GL_TEXTURE_BASE_LEVEL:
        if ((multisampled||rectangle) && val[0]!=0.0)
            ERROR2(true, "Parameter value must be zero due to the texture's type");
    case GL_TEXTURE_MAX_LEVEL:
        if (val[0] < 0.0) ERROR2(true, "Parameter value must be nonnegative");
        break;
    case GL_DEPTH_STENCIL_TEXTURE_MODE:
        if (val[0]!=GL_DEPTH_COMPONENT && val[0]!=GL_STENCIL_INDEX)
            ERROR2(true, "Invalid depth stencil texture mode");
        break;
    case GL_TEXTURE_SWIZZLE_R:
    case GL_TEXTURE_SWIZZLE_G:
    case GL_TEXTURE_SWIZZLE_B:
    case GL_TEXTURE_SWIZZLE_A:
        if (not_one_of(val[0], GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA, GL_ONE, GL_ZERO, -1))
            ERROR2(true, "Invalid swizzle");
        break;
    case GL_TEXTURE_SWIZZLE_RGBA:
        for (uint i = 0; i < 4; i++) {
            if (not_one_of(val[i], GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA, GL_ONE, GL_ZERO, -1))
                ERROR2(true, "Invalid swizzle");
        }
        break;
    }
    
    switch (param) {
    case GL_DEPTH_STENCIL_TEXTURE_MODE:
        newrev.depth_stencil_mode = val[0];
        break;
    case GL_TEXTURE_BASE_LEVEL: newrev.base_level = val[0]; break;
    case GL_TEXTURE_MAX_LEVEL: newrev.max_level = val[0]; break;
    case GL_TEXTURE_SWIZZLE_R: newrev.swizzle[0] = val[0]; break;
    case GL_TEXTURE_SWIZZLE_G: newrev.swizzle[1] = val[0]; break;
    case GL_TEXTURE_SWIZZLE_B: newrev.swizzle[2] = val[0]; break;
    case GL_TEXTURE_SWIZZLE_A: newrev.swizzle[3] = val[0]; break;
    case GL_TEXTURE_SWIZZLE_RGBA:
        for (uint i = 0; i < 4; i++) newrev.swizzle[i] = val[i];
        break;
    }
    
    newrev.complete_status = -1;
    set_texture(&newrev);
    
    return false;
}

//Ignoring stencil
static bool is_internal_format_integral(GLenum format) {
    internal_format_info_t info = get_internal_format_info(format);
    switch (info.dtype) {
    case FmtDType_U8:
    case FmtDType_I8:
    case FmtDType_U16:
    case FmtDType_I16:
    case FmtDType_U32:
    case FmtDType_I32:
    case FmtDType_U10_U10_U10_U2: return true;
    default: return false;
    }
}

static const trc_gl_texture_image_t* find_image(size_t num_images,
                                                const trc_gl_texture_image_t* images,
                                                uint face, uint level) {
    for (size_t i = 0; i < num_images; i++) {
        if (images[i].face==face && images[i].level==level)
            return &images[i];
    }
    return NULL;
}

static const char* validate_mipmap_completeness(const trc_gl_texture_rev_t* tex, size_t num_images,
                                                const trc_gl_texture_image_t* images) {
    switch (tex->type) {
    case GL_TEXTURE_RECTANGLE:
    case GL_TEXTURE_BUFFER:
    case GL_TEXTURE_2D_MULTISAMPLE:
    case GL_TEXTURE_2D_MULTISAMPLE_ARRAY:
        return NULL;
    }
    
    if (tex->base_level > tex->max_level)
        return "The texture's base level is greater than it's maximum level";
    
    uint faces = tex->type == GL_TEXTURE_CUBE_MAP ? 6 : 1;
    for (size_t face = 0; face < faces; face++) {
        const trc_gl_texture_image_t* base_img = find_image(num_images, images, face, tex->base_level);
        if (!base_img) return "The texture has a missing base image";
        uint dims[3] = {base_img->width, base_img->height, base_img->depth};
        
        uint max_size = dims[0];
        if (dims[1] > max_size) max_size = dims[1];
        if (dims[2] > max_size) max_size = dims[2];
        uint p = tex->base_level + floor_log2(max_size);
        uint q = p<tex->max_level ? p : tex->max_level;
        
        uint internal_format = 0;
        for (uint i = tex->base_level; i<=q; i++) {
            const trc_gl_texture_image_t* img = find_image(num_images, images, face, i);
            if (!img) return "The texture is missing at least one image";
            
            if (internal_format == 0)
                internal_format = img->internal_format;
            if (internal_format != img->internal_format)
                return "The texture's images has mismatching internal formats";
            
            uint actual[3] = {img->width, img->height, img->depth};
            for (size_t j = 0; j < 3; j++) {
                uint expected = dims[j] / (1<<i);
                if (expected == 0) expected = 1;
                if (expected != actual[j])
                    return "The texture's images are incorrectly sized for mipmapping";
            }
        }
    }
    
    return NULL;
}

static const char* validate_cube_completeness(const trc_gl_texture_rev_t* tex, size_t num_images,
                                              const trc_gl_texture_image_t* images) {
    if (tex->type == GL_TEXTURE_CUBE_MAP) {
        bool found_image = false;
        uint width = 0;
        uint height = 0;
        uint internal_format = 0;
        for (size_t i = 0; i < 6; i++) {
            const trc_gl_texture_image_t* img = find_image(num_images, images, i, tex->base_level);
            if (!img) return "The texture has a missing base image";
            if (img->width != img->height)
                return "The texture has a non-square cubemap face";
            if (found_image) {
                if (img->width!=width || img->height!=height)
                    return "The texture has inconsistently sized cubemap faces";
                if (img->internal_format!=internal_format)
                    return "The texture has inconsistent internal formats of the cubemap faces";
            }
            width = img->width;
            height = img->height;
            internal_format = img->internal_format;
            found_image = true;
        }
    }
    
    return NULL;
}

static const char* _validate_texture_completeness(trc_obj_t* tex_obj, trc_obj_t* sampler_obj) {
    const trc_gl_texture_rev_t* tex = trc_obj_get_rev(tex_obj, -1);
    trc_gl_sample_params_t sample_params = tex->sample_params;
    if (sampler_obj) {
        const trc_gl_sampler_rev_t* sampler = trc_obj_get_rev(sampler_obj, -1);
        sample_params = sampler->params;
        const char* err = sample_params_valid_with_tex(&sampler->params, tex);
        if (err) return err;
    }
    
    size_t num_images = tex->images->size / sizeof(trc_gl_texture_image_t);
    const trc_gl_texture_image_t* images = trc_map_data(tex->images, TRC_MAP_READ);
    
    uint faces = tex->type == GL_TEXTURE_CUBE_MAP ? 6 : 1;
    for (size_t face = 0; face < faces; face++) {
        const trc_gl_texture_image_t* base_img = find_image(num_images, images, face, tex->base_level);
        if (!base_img) {
            trc_unmap_data(images);
            return "The texture has a missing base image";
        }
    }
    
    if (sample_params.min_filter!=GL_NEAREST && sample_params.min_filter!=GL_LINEAR) {
        const char* err = validate_mipmap_completeness(tex, num_images, images);
        if (err) {
            trc_unmap_data(images);
            return err;
        }
    }
    
    bool min_linear = sample_params.min_filter != GL_NEAREST_MIPMAP_NEAREST;
    min_linear = min_linear && sample_params.min_filter!=GL_NEAREST;
    if (min_linear || sample_params.mag_filter!=GL_NEAREST) {
        const trc_gl_texture_image_t* img = find_image(num_images, images, 0, tex->base_level);
        const char* err = NULL;
        if (is_internal_format_integral(img->internal_format))
            err = "The filtering parameters are incompatible with the integral internal format";
        internal_format_info_t info = get_internal_format_info(img->internal_format);
        if (info.base==GL_STENCIL_INDEX)
            err = "The filtering parameters are incompatible with the stencil index internal format";
        if (info.base==GL_DEPTH_STENCIL && tex->depth_stencil_mode==GL_STENCIL_INDEX)
            err = "The filtering parameters are incompatible with the depth stencil internal format and GL_STENCIL_INDEX depth stencil mode";
        if (err) {
            trc_unmap_data(images);
            return err;
        }
    }
    
    for (size_t i = 0; i < 6; i++) {
        const trc_gl_texture_image_t* img = find_image(num_images, images, i, tex->base_level);
        if (!img) continue;
        if (img->width==0 || img->height==0 || img->depth==0) {
            trc_unmap_data(images);
            return "One of the base images has a dimension of zero";
        }
    }
    
    const char* err = validate_cube_completeness(tex, num_images, images);
    if (err) {
        trc_unmap_data(images);
        return err;
    }
    
    trc_unmap_data(images);
    
    return NULL;
}

//TODO: Improve caching
const char* validate_texture_completeness(trc_obj_t* tex_obj, trc_obj_t* sampler_obj) {
    const trc_gl_texture_rev_t* rev = trc_obj_get_rev(tex_obj, -1);
    if (rev->complete_status!=-1 && !sampler_obj)
        return rev->complete_status ? NULL : rev->incompleteness_reason;
    
    const char* res = _validate_texture_completeness(tex_obj, sampler_obj);
    
    if (!sampler_obj) {
        trc_gl_texture_rev_t newrev = *rev;
        newrev.complete_status = res ? 0 : 1;
        newrev.incompleteness_reason = res;
        set_texture(&newrev);
    }
    
    return res;
}

static bool is_proxy_target(GLenum target) {
    switch (target) {
    case GL_PROXY_TEXTURE_1D:
    case GL_PROXY_TEXTURE_2D:
    case GL_PROXY_TEXTURE_3D:
    case GL_PROXY_TEXTURE_1D_ARRAY:
    case GL_PROXY_TEXTURE_2D_ARRAY:
    case GL_PROXY_TEXTURE_RECTANGLE:
    case GL_PROXY_TEXTURE_2D_MULTISAMPLE:
    case GL_PROXY_TEXTURE_2D_MULTISAMPLE_ARRAY:
    case GL_PROXY_TEXTURE_CUBE_MAP:
    case GL_PROXY_TEXTURE_CUBE_MAP_ARRAY:
        return true;
    default:
        return false;
    }
}

static bool is_compressed_image(const trc_gl_texture_rev_t* tex, uint level) {
    if (tex->type == GL_TEXTURE_BUFFER)
        return level==0 && is_compressed_format(tex->buffer.internal_format);
    
    trc_gl_texture_image_t* images = trc_map_data(tex->images, TRC_MAP_READ);
    for (size_t i = 0; i < tex->images->size/sizeof(images[0]); i++) {
        if (images[i].level == level) {
            bool res = images[i].compressed_internal_format;
            trc_unmap_data(images);
            return res;
        }
    }
    trc_unmap_data(images);
    return false;
}

static void get_tex_level_parameter(GLenum target, GLint level, GLenum pname,
                                    bool dsa, const trc_gl_texture_rev_t* tex) {
    if (!tex) ERROR2(, dsa?"Invalid texture name":"No texture bound to target");
    if (!tex->has_object) ERROR2(, "Texture name has no object");
    
    if (level < 0) ERROR2(, "The specified level is negative\n");
    if (level > ceil_log2(gls_get_state_int(GL_MAX_TEXTURE_SIZE, 0)))
        ERROR2(, "The specified level is greater than ceil(log2(GL_MAX_TEXTURE_SIZE))");
    if (target==GL_TEXTURE_BUFFER && level!=0)
        ERROR2(, "The level must be zero when the target is GL_TEXTURE_BUFFER");
    
    if (pname==GL_TEXTURE_COMPRESSED_IMAGE_SIZE && is_compressed_image(tex, level))
        ERROR2(, "GL_TEXTURE_COMPRESSED_IMAGE_SIZE cannot be quered with uncompressed textures");
    if (target!=0 && pname==GL_TEXTURE_COMPRESSED_IMAGE_SIZE && is_proxy_target(target))
        ERROR2(, "GL_TEXTURE_COMPRESSED_IMAGE_SIZE cannot be quered with proxy targets");
}

static void get_tex_parameter(GLenum pname, bool dsa, const trc_gl_texture_rev_t* tex) {
    if (!tex) ERROR2(, dsa?"Invalid texture name":"No texture bound to target");
    if (!tex->has_object) ERROR2(, "Texture name has no object");
}

glGenTextures: //GLsizei p_n, GLuint* p_textures
    if (p_n < 0) ERROR("Invalid texture name count");
    GLuint* textures = replay_alloc(p_n*sizeof(GLuint));
    real(p_n, textures);
    gen_textures(p_n, textures, p_textures, false, 0);

glCreateTextures: //GLenum p_target, GLsizei p_n, GLuint* p_textures
    if (p_n < 0) ERROR("Invalid texture name count");
    GLuint* textures = replay_alloc(p_n*sizeof(GLuint));
    real(p_target, p_n, textures);
    gen_textures(p_n, textures, p_textures, true, p_target);

glDeleteTextures: //GLsizei p_n, const GLuint* p_textures
    GLuint* textures = replay_alloc(p_n*sizeof(GLuint));
    for (size_t i = 0; i < p_n; ++i)
        if (!(textures[i] = get_real_texture(p_textures[i])) && p_textures[i]) {
            trc_add_warning(cmd, "Invalid texture name");
        } else {
            trc_obj_t* obj = get_texture(p_textures[i])->head.obj;
            
            //Reset targets
            GLenum targets[11] = {
                GL_TEXTURE_1D, GL_TEXTURE_2D, GL_TEXTURE_3D, GL_TEXTURE_1D_ARRAY,
                GL_TEXTURE_2D_ARRAY, GL_TEXTURE_RECTANGLE, GL_TEXTURE_CUBE_MAP,
                GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_BUFFER, GL_TEXTURE_2D_MULTISAMPLE,
                GL_TEXTURE_2D_MULTISAMPLE_ARRAY};
            for (size_t j = 0; j < 11; j++) {
                for (size_t k = 0; k < gls_get_bound_textures_size(targets[j]); k++) {
                    if (gls_get_bound_textures(targets[j], k) == obj)
                        gls_set_bound_textures(targets[j], k, NULL);
                }
            }
            
            //TODO: Remove from framebuffers?
            
            delete_obj(p_textures[i], TrcTexture);
        }
    real(p_n, textures);

glActiveTexture: //GLenum p_texture
    if (p_texture<GL_TEXTURE0 || p_texture-GL_TEXTURE0>=gls_get_state_int(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, 0))
        ERROR("No such texture unit");
    gls_set_active_texture_unit(p_texture-GL_TEXTURE0);
    real(p_texture);

glBindTexture: //GLenum p_target, GLuint p_texture
    const trc_gl_texture_rev_t* rev = get_texture(p_texture);
    if (!rev && p_texture) ERROR("Invalid texture name");
    real(p_target, p_texture?rev->real:0);
    if (rev && !rev->has_object) {
        trc_gl_texture_rev_t newrev = *rev;
        newrev.has_object = true;
        newrev.type = p_target;
        newrev.images = trc_create_data(ctx->trace, 0, NULL, 0);
        newrev.sample_params.min_filter = p_target==GL_TEXTURE_RECTANGLE ? GL_LINEAR : GL_NEAREST_MIPMAP_LINEAR;
        GLenum wrap_mode = p_target==GL_TEXTURE_RECTANGLE ? GL_CLAMP_TO_EDGE : GL_REPEAT;
        newrev.sample_params.wrap_s = wrap_mode;
        newrev.sample_params.wrap_t = wrap_mode;
        newrev.sample_params.wrap_r = wrap_mode;
        set_texture(&newrev);
    } else if (rev && rev->type!=p_target) {
        ERROR("Invalid target for texture object");
    }
    uint unit = gls_get_active_texture_unit(ctx->trace);
    gls_set_bound_textures(p_target, unit, rev?rev->head.obj:NULL);

glBindTextureUnit: //GLuint p_unit, GLuint p_texture
    if (p_unit>=gls_get_state_int(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, 0))
        ERROR("Invalid unit");
    if (!p_texture_rev) ERROR("Invalid texture name");
    if (!p_texture_rev->has_object) ERROR("Texture name has no object");
    real(p_unit, p_texture_rev->real);
    if (p_texture) {
        gls_set_bound_textures(p_texture_rev->type, p_unit, p_texture_rev->head.obj);
    } else {
        GLenum targets[] = {GL_TEXTURE_1D, GL_TEXTURE_2D, GL_TEXTURE_3D,
                            GL_TEXTURE_1D_ARRAY, GL_TEXTURE_2D_ARRAY,
                            GL_TEXTURE_RECTANGLE, GL_TEXTURE_BUFFER,
                            GL_TEXTURE_CUBE_MAP, GL_TEXTURE_CUBE_MAP_ARRAY,
                            GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_2D_MULTISAMPLE_ARRAY};
        for (size_t i = 0; i < sizeof(targets)/sizeof(targets[0]); i++)
            gls_set_bound_textures(targets[i], p_unit, NULL);
    }

glBindTextures: //GLuint p_first, GLsizei p_count, const GLuint* p_textures
    if (p_first+p_count>gls_get_state_int(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, 0) || p_count<0)
        ERROR("Invalid range");
    GLuint* real_tex = replay_alloc(p_count*sizeof(GLuint));
    //TODO: This probably creates a new trc_data_t for each texture
    for (size_t i = p_first; i < p_first+p_count; i++) {
        const trc_gl_texture_rev_t* rev = get_texture(p_textures[i]);
        if (!rev) ERROR("Invalid texture name at index %zu", i);
        if (!rev->has_object) ERROR("Texture name at index %zu has no object", i);
        real_tex[i] = rev->real;
        
        if (p_textures[i]) {
            gls_set_bound_textures(rev->type, i, trc_lookup_name(ctx->ns, TrcTexture, p_textures[i], -1));
        } else {
            GLenum targets[] = {GL_TEXTURE_1D, GL_TEXTURE_2D, GL_TEXTURE_3D,
                                GL_TEXTURE_1D_ARRAY, GL_TEXTURE_2D_ARRAY,
                                GL_TEXTURE_RECTANGLE, GL_TEXTURE_BUFFER,
                                GL_TEXTURE_CUBE_MAP, GL_TEXTURE_CUBE_MAP_ARRAY,
                                GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_2D_MULTISAMPLE_ARRAY};
            for (size_t j = 0; j < sizeof(targets)/sizeof(targets[0]); j++)
                gls_set_bound_textures(targets[j], i, NULL);
        }
    }
    real(p_first, p_count, real_tex);

//TODO: Support malsized data in gl*Image*D
glTexImage1D: //GLenum p_target, GLint p_level, GLint p_internalformat, GLsizei p_width, GLint p_border, GLenum p_format, GLenum p_type, const void* p_pixels
    if (tex_image(false, p_target, p_level, p_internalformat, p_border, p_format, p_type, false, 1, p_width)) {
        real(p_target, p_level, p_internalformat, p_width, p_border, p_format, p_type, p_pixels);
        update_bound_tex_image(p_target, p_level);
    }

glCompressedTexImage1D: //GLenum p_target, GLint p_level, GLenum p_internalformat, GLsizei p_width, GLint p_border, GLsizei p_imageSize, const void* p_data
    real(p_target, p_level, p_internalformat, p_width, p_border, p_imageSize, p_data);
    update_bound_tex_image(p_target, p_level);

glTexSubImage1D: //GLenum p_target, GLint p_level, GLint p_xoffset, GLsizei p_width, GLenum p_format, GLenum p_type, const void* p_pixels
    if (tex_image(false, p_target, p_level, 0, 0, p_format, p_type, true, 1, p_width, p_xoffset)) {
        real(p_target, p_level, p_xoffset, p_width, p_format, p_type, p_pixels);
        update_bound_tex_image(p_target, p_level);
    }

glTextureSubImage1D: //GLuint p_texture, GLint p_level, GLint p_xoffset, GLsizei p_width, GLenum p_format, GLenum p_type, const void* p_pixels
    if (tex_image(true, p_texture, p_level, 0, 0, p_format, p_type, true, 1, p_width, p_xoffset)) {
        real(p_texture_rev->real, p_level, p_xoffset, p_width, p_format, p_type, p_pixels);
        update_tex_image(p_texture_rev, p_level, 0);
    }

glCompressedTexSubImage1D: //GLenum p_target, GLint p_level, GLint p_xoffset, GLsizei p_width, GLenum p_format, GLsizei p_imageSize, const void* p_data
    real(p_target, p_level, p_xoffset, p_width, p_format, p_imageSize, p_data);
    update_bound_tex_image(p_target, p_level);

glTexImage2D: //GLenum p_target, GLint p_level, GLint p_internalformat, GLsizei p_width, GLsizei p_height, GLint p_border, GLenum p_format, GLenum p_type, const void* p_pixels
    if (tex_image(false, p_target, p_level, p_internalformat, p_border, p_format, p_type, false, 2, p_width, p_height)) {
        real(p_target, p_level, p_internalformat, p_width, p_height, p_border, p_format, p_type, p_pixels);
        update_bound_tex_image(p_target, p_level);
    }

glCompressedTexImage2D: //GLenum p_target, GLint p_level, GLenum p_internalformat, GLsizei p_width, GLsizei p_height, GLint p_border, GLsizei p_imageSize, const void* p_data
    real(p_target, p_level, p_internalformat, p_width, p_height, p_border, p_imageSize, p_data);
    update_bound_tex_image(p_target, p_level);

glTexSubImage2D: //GLenum p_target, GLint p_level, GLint p_xoffset, GLint p_yoffset, GLsizei p_width, GLsizei p_height, GLenum p_format, GLenum p_type, const void* p_pixels
    if (tex_image(false, p_target, p_level, 0, 0, p_format, p_type, true, 2, p_width, p_height, p_xoffset, p_yoffset)) {
        real(p_target, p_level, p_xoffset, p_yoffset, p_width, p_height, p_format, p_type, p_pixels);
        update_bound_tex_image(p_target, p_level);
    }

glTextureSubImage2D: //GLuint p_texture, GLint p_level, GLint p_xoffset, GLint p_yoffset, GLsizei p_width, GLsizei p_height, GLenum p_format, GLenum p_type, const void* p_pixels
    if (tex_image(true, p_texture, p_level, 0, 0, p_format, p_type, true, 2, p_width, p_height, p_xoffset, p_yoffset)) {
        real(p_texture_rev->real, p_level, p_xoffset, p_yoffset, p_width, p_height, p_format, p_type, p_pixels);
        update_tex_image(p_texture_rev, p_level, 0);
    }

glCompressedTexSubImage2D: //GLenum p_target, GLint p_level, GLint p_xoffset, GLint p_yoffset, GLsizei p_width, GLsizei p_height, GLenum p_format, GLsizei p_imageSize, const void* p_data
    real(p_target, p_level, p_xoffset, p_yoffset, p_width, p_height, p_format, p_imageSize, p_data);
    update_bound_tex_image(p_target, p_level);

glTexImage3D: //GLenum p_target, GLint p_level, GLint p_internalformat, GLsizei p_width, GLsizei p_height, GLsizei p_depth, GLint p_border, GLenum p_format, GLenum p_type, const void* p_pixels
    //TODO: Array textures?
    if (tex_image(false, p_target, p_level, p_internalformat, p_border, p_format, p_type, false, 3, p_width, p_height, p_depth)) {
        real(p_target, p_level, p_internalformat, p_width, p_height, p_depth, p_border, p_format, p_type, p_pixels);
        update_bound_tex_image(p_target, p_level);
    }

glCompressedTexImage3D: //GLenum p_target, GLint p_level, GLenum p_internalformat, GLsizei p_width, GLsizei p_height, GLsizei p_depth, GLint p_border, GLsizei p_imageSize, const void* p_data
    //TODO: Array textures
    real(p_target, p_level, p_internalformat, p_width, p_height, p_depth, p_border, p_imageSize, p_data);
    update_bound_tex_image(p_target, p_level);

glTexSubImage3D: //GLenum p_target, GLint p_level, GLint p_xoffset, GLint p_yoffset, GLint p_zoffset, GLsizei p_width, GLsizei p_height, GLsizei p_depth, GLenum p_format, GLenum p_type, const void* p_pixels
    if (tex_image(false, p_target, p_level, 0, 0, p_format, p_type, true, 3, p_width, p_height, p_depth, p_xoffset, p_yoffset, p_zoffset)) {
        real(p_target, p_level, p_xoffset, p_yoffset, p_zoffset, p_width, p_height, p_depth, p_format, p_type, p_pixels);
        update_bound_tex_image(p_target, p_level);
    }

glCompressedTexSubImage3D: //GLenum p_target, GLint p_level, GLint p_xoffset, GLint p_yoffset, GLint p_zoffset, GLsizei p_width, GLsizei p_height, GLsizei p_depth, GLenum p_format, GLsizei p_imageSize, const void* p_data
    real(p_target, p_level, p_xoffset, p_yoffset, p_zoffset, p_width, p_height, p_depth, p_format, p_imageSize, p_data);
    update_bound_tex_image(p_target, p_level);

glTexImage2DMultisample: //GLenum p_target, GLsizei p_samples, GLenum p_internalformat, GLsizei p_width, GLsizei p_height, GLboolean p_fixedsamplelocations
    if (tex_image_ms(false, p_target, p_samples, p_internalformat, p_width, p_height, p_fixedsamplelocations, false, 0)) {
        real(p_target, p_samples, p_internalformat, p_width, p_height, p_fixedsamplelocations);
        update_bound_tex_image(p_target, 0);
    }

glTexImage3DMultisample: //GLenum p_target, GLsizei p_samples, GLenum p_internalformat, GLsizei p_width, GLsizei p_height, GLsizei p_depth, GLboolean p_fixedsamplelocations
    if (tex_image_ms(false, p_target, p_samples, p_internalformat, p_width, p_height, p_fixedsamplelocations, true, p_depth)) {
        real(p_target, p_samples, p_internalformat, p_width, p_height, p_depth, p_fixedsamplelocations);
        update_bound_tex_image(p_target, 0);
    }

glCopyTexImage1D: //GLenum p_target, GLint p_level, GLenum p_internalformat, GLint p_x, GLint p_y, GLsizei p_width, GLint p_border
    real(p_target, p_level, p_internalformat, p_x, p_y, p_width, p_border);
    update_bound_tex_image(p_target, p_level);

glCopyTexImage2D: //GLenum p_target, GLint p_level, GLenum p_internalformat, GLint p_x, GLint p_y, GLsizei p_width, GLsizei p_height, GLint p_border
    real(p_target, p_level, p_internalformat, p_x, p_y, p_width, p_height, p_border);
    update_bound_tex_image(p_target, p_level);

glCopyTexSubImage1D: //GLenum p_target, GLint p_level, GLint p_xoffset, GLint p_x, GLint p_y, GLsizei p_width
    real(p_target, p_level, p_xoffset, p_x, p_y, p_width);
    update_bound_tex_image(p_target, p_level);

glCopyTexSubImage2D: //GLenum p_target, GLint p_level, GLint p_xoffset, GLint p_yoffset, GLint p_x, GLint p_y, GLsizei p_width, GLsizei p_height
    real(p_target, p_level, p_xoffset, p_yoffset, p_x, p_y, p_width, p_height);
    update_bound_tex_image(p_target, p_level);

glCopyTexSubImage3D: //GLenum p_target, GLint p_level, GLint p_xoffset, GLint p_yoffset, GLint p_zoffset, GLint p_x, GLint p_y, GLsizei p_width, GLsizei p_height
    real(p_target, p_level, p_xoffset, p_yoffset, p_zoffset, p_x, p_y, p_width, p_height);
    update_bound_tex_image(p_target, p_level);

glTexBuffer: //GLenum p_target, GLenum p_internalformat, GLuint p_buffer
    if (tex_buffer(p_target, false, p_internalformat, p_buffer, 0, -1))
        real(p_target, p_internalformat, p_buffer?p_buffer_rev->real:0);

glTexBufferRange: //GLenum p_target, GLenum p_internalformat, GLuint p_buffer, GLintptr p_offset, GLsizeiptr p_size
    if (tex_buffer(p_target, false, p_internalformat, p_buffer, p_offset, p_size))
        real(p_target, p_internalformat, p_buffer?p_buffer_rev->real:0, p_offset, p_size);

glTextureBuffer: //GLuint p_texture, GLenum p_internalformat, GLuint p_buffer
    if (tex_buffer(p_texture, true, p_internalformat, p_buffer, 0, -1))
        real(p_texture_rev->real, p_internalformat, p_buffer?p_buffer_rev->real:0);

glTextureBufferRange: //GLuint p_texture, GLenum p_internalformat, GLuint p_buffer, GLintptr p_offset, GLsizeiptr p_size
    if (tex_buffer(p_texture, true, p_internalformat, p_buffer, p_offset, p_size))
        real(p_texture_rev->real, p_internalformat, p_buffer?p_buffer_rev->real:0, p_offset, p_size);

//TODO: Get the images during tracing?
static void generate_texture_mipmaps(bool dsa, GLenum target, const trc_gl_texture_rev_t* tex) {
    size_t num_images = tex->images->size / sizeof(trc_gl_texture_image_t);
    const trc_gl_texture_image_t* images = trc_map_data(tex->images, TRC_MAP_READ);
    
    bool cubemap = tex->type == GL_TEXTURE_CUBE_MAP_ARRAY;
    cubemap = cubemap || tex->type == GL_TEXTURE_CUBE_MAP;
    
    if (cubemap) {
        const char* err = validate_cube_completeness(tex, num_images, images);
        if (err) {
            trc_unmap_data(images);
            ERROR2(, "Texture is not cube complete: %s", err);
        }
    }
    
    const trc_gl_texture_image_t* base_ptr = find_image(num_images, images, 0, tex->base_level);
    if (!base_ptr) ERROR2(, "Texture has no base image");
    trc_gl_texture_image_t base = *base_ptr;
    
    if (!dsa) F(glGenerateMipmap)(target);
    else F(glGenerateTextureMipmap)(tex->real);
    
    trc_unmap_data(images);
    
    uint max_size = base.width;
    if (base.height > max_size) max_size = base.height;
    if (base.depth > max_size) max_size = base.depth;
    uint p = tex->base_level + floor_log2(max_size);
    uint q = p<tex->max_level ? p : tex->max_level;
    
    for (uint i = tex->base_level; i<=q; i++) {
        for (uint j = 0; j < (cubemap?6:1); j++) {
            update_tex_image(tex, i, j);
            tex = trc_obj_get_rev(tex->head.obj, -1);
        }
    }
}

glGenerateMipmap: //GLenum p_target
    uint unit = gls_get_active_texture_unit(ctx->trace);
    trc_obj_t* tex = gls_get_bound_textures(p_target, unit);
    if (!tex) ERROR("No texture bound to target");
    generate_texture_mipmaps(false, p_target, trc_obj_get_rev(tex, -1));

glGenerateTextureMipmap: //GLuint p_texture
    if (!p_texture_rev) ERROR("Invalid texture name");
    if (!p_texture_rev->has_object) ERROR("Texture name has no object");
    generate_texture_mipmaps(true, 0, p_texture_rev);

static const trc_gl_texture_rev_t* tex_storage(bool dsa, uint tex_or_target, GLsizei levels, GLsizei width, GLsizei height, GLsizei depth) {
    const trc_gl_texture_rev_t* tex_rev;
    if (dsa) tex_rev = get_texture(tex_or_target);
    else tex_rev = get_bound_tex(tex_or_target);
    if (!tex_rev) ERROR2(NULL, dsa?"Invalid texture name":"No texture bound to target");
    if (!tex_rev->has_object) ERROR2(NULL, "Texture name has no object");
    trc_obj_t* tex = tex_rev->head.obj;
    
    const trc_gl_texture_rev_t* rev = trc_obj_get_rev(tex, -1);
    GLenum target = rev->type;
    
    if (width < 1) ERROR2(NULL, "The width is less than one");
    if (height < 1) ERROR2(NULL, "The height is less than one");
    if (depth < 1) ERROR2(NULL, "The depth is less than one");
    if (levels < 1) ERROR2(NULL, "The level count is less than one");
    switch (target) {
    case GL_TEXTURE_1D:
    case GL_TEXTURE_1D_ARRAY:
        if (levels > floor_log2(width)+1)
            ERROR2(NULL, "The level count is greater than floor(log2(width))+1");
        break;
    case GL_TEXTURE_2D:
    case GL_TEXTURE_2D_ARRAY:
    case GL_TEXTURE_CUBE_MAP:
    case GL_TEXTURE_CUBE_MAP_ARRAY:
    case GL_TEXTURE_RECTANGLE:
        if (levels > floor_log2(width>height?width:height)+1)
            ERROR2(NULL, "The level count is greater than floor(log2(max(width, height)))+1");
        break;
    case GL_TEXTURE_3D: {
        int max_size = width > height ? width : height;
        max_size = depth > max_size ? depth : max_size;
        if (levels > floor_log2(max_size)+1)
            ERROR2(NULL, "The level count is greater than floor(log2(max(width, height, depth)))+1");
        break;
    }
    }
    
    for (uint i = 0; i < levels; i++) {
        size_t faces = target == GL_TEXTURE_CUBE_MAP ? 6 : 1;
        faces = target == GL_TEXTURE_CUBE_MAP_ARRAY ? 6 : faces;
        for (uint j = 0; j < faces; j++)
            update_tex_image(rev, i, j);
    }
    
    return trc_obj_get_rev(tex, -1);
}

glTexStorage1D: //GLenum p_target, GLsizei p_levels, GLenum p_internalformat, GLsizei p_width
    if (tex_storage(false, p_target, p_levels, p_width, 1, 1))
        real(p_target, p_levels, p_internalformat, p_width);

glTexStorage2D: //GLenum p_target, GLsizei p_levels, GLenum p_internalformat, GLsizei p_width, GLsizei p_height
    if (tex_storage(false, p_target, p_levels, p_width, p_height, 1))
        real(p_target, p_levels, p_internalformat, p_width, p_height);

glTexStorage3D: //GLenum p_target, GLsizei p_levels, GLenum p_internalformat, GLsizei p_width, GLsizei p_height, GLsizei p_depth
    if (tex_storage(false, p_target, p_levels, p_width, p_height, p_depth))
        real(p_target, p_levels, p_internalformat, p_width, p_height, p_depth);

glTextureStorage1D: //GLuint p_texture, GLsizei p_levels, GLenum p_internalformat, GLsizei p_width
    const trc_gl_texture_rev_t* rev = tex_storage(true, p_texture, p_levels, p_width, 1, 1);
    if (rev) real(rev->real, p_levels, p_internalformat, p_width);

glTextureStorage2D: //GLuint p_texture, GLsizei p_levels, GLenum p_internalformat, GLsizei p_width, GLsizei p_height
    const trc_gl_texture_rev_t* rev = tex_storage(true, p_texture, p_levels, p_width, p_height, 1);
    if (rev) real(rev->real, p_levels, p_internalformat, p_width, p_height);

glTextureStorage3D: //GLuint p_texture, GLsizei p_levels, GLenum p_internalformat, GLsizei p_width, GLsizei p_height, GLsizei p_depth
    const trc_gl_texture_rev_t* rev = tex_storage(true, p_texture, p_levels, p_width, p_height, p_depth);
    if (rev) real(rev->real, p_levels, p_internalformat, p_width, p_height, p_depth);

glTexParameterf: //GLenum p_target, GLenum p_pname, GLfloat p_param
    GLdouble double_param = p_param;
    if (!texture_param_double(false, p_target, p_pname, 1, &double_param))
        real(p_target, p_pname, p_param);

glTexParameteri: //GLenum p_target, GLenum p_pname, GLint p_param
    GLdouble double_param = p_param;
    if (!texture_param_double(false, p_target, p_pname, 1, &double_param))
        real(p_target, p_pname, p_param);

glTexParameterfv: //GLenum p_target, GLenum p_pname, const GLfloat* p_params
    const double* paramsd = trc_get_double(arg_params);
    if (!texture_param_double(false, p_target, p_pname, arg_params->count, paramsd))
        real(p_target, p_pname, p_params);

glTexParameteriv: //GLenum p_target, GLenum p_pname, const GLint* p_params
    const int64_t* params64 = trc_get_int(arg_params);
    double* double_params = replay_alloc(arg_params->count*sizeof(double));
    if (p_pname == GL_TEXTURE_BORDER_COLOR)
        conv_from_signed_norm_array_i64(arg_params->count, double_params, params64, 32);
    else
        for (size_t i = 0; i < arg_params->count; i++) double_params[i] = params64[i];
    if (!texture_param_double(false, p_target, p_pname, arg_params->count, double_params))
        real(p_target, p_pname, p_params);

glTexParameterIiv: //GLenum p_target, GLenum p_pname, const GLint* p_params
    const int64_t* params64 = trc_get_int(arg_params);
    double* double_params = replay_alloc(arg_params->count*sizeof(double));
    for (size_t i = 0; i < arg_params->count; i++) double_params[i] = params64[i];
    if (!texture_param_double(false, p_target, p_pname, arg_params->count, double_params))
        real(p_target, p_pname, p_params);

glTexParameterIuiv: //GLenum p_target, GLenum p_pname, const GLuint* p_params
    const uint64_t* params64 = trc_get_uint(arg_params);
    double* double_params = replay_alloc(arg_params->count*sizeof(double));
    for (size_t i = 0; i < arg_params->count; i++) double_params[i] = params64[i];
    if (!texture_param_double(false, p_target, p_pname, arg_params->count, double_params))
        real(p_target, p_pname, p_params);

glTextureParameterf: //GLuint p_texture, GLenum p_pname, GLfloat p_param
    GLdouble double_param = p_param;
    if (!texture_param_double(true, p_texture, p_pname, 1, &double_param))
        real(p_texture_rev->real, p_pname, p_param);

glTextureParameteri: //GLuint p_texture, GLenum p_pname, GLint p_param
    GLdouble double_param = p_param;
    if (!texture_param_double(true, p_texture, p_pname, 1, &double_param))
        real(p_texture_rev->real, p_pname, p_param);

glTextureParameterfv: //GLuint p_texture, GLenum p_pname, const GLfloat* p_param
    const double* paramsd = trc_get_double(arg_param);
    if (!texture_param_double(true, p_texture, p_pname, arg_param->count, paramsd))
        real(p_texture_rev->real, p_pname, p_param);

glTextureParameteriv: //GLuint p_texture, GLenum p_pname, const GLint* p_param
    double* double_params = replay_alloc(arg_param->count*sizeof(double));
    if (p_pname == GL_TEXTURE_BORDER_COLOR)
        conv_from_signed_norm_array_i32(arg_param->count, double_params, p_param, 32);
    else
        for (size_t i = 0; i < arg_param->count; i++) double_params[i] = p_param[i];
    if (!texture_param_double(true, p_texture, p_pname, arg_param->count, double_params))
        real(p_texture_rev->real, p_pname, p_param);

glTextureParameterIiv: //GLuint p_texture, GLenum p_pname, const GLint* p_params
    double* double_params = replay_alloc(arg_params->count*sizeof(double));
    for (size_t i = 0; i < arg_params->count; i++) double_params[i] = p_params[i];
    if (!texture_param_double(true, p_texture, p_pname, arg_params->count, double_params))
        real(p_texture_rev->real, p_pname, p_params);

glTextureParameterIuiv: //GLuint p_texture, GLenum p_pname, const GLuint* p_params
    double* double_params = replay_alloc(arg_params->count*sizeof(double));
    for (size_t i = 0; i < arg_params->count; i++) double_params[i] = p_params[i];
    if (!texture_param_double(true, p_texture, p_pname, arg_params->count, double_params))
        real(p_texture_rev->real, p_pname, p_params);

glGetTexLevelParameterfv: //GLenum p_target, GLint p_level, GLenum p_pname, GLfloat* p_params
    get_tex_level_parameter(p_target, p_level, p_pname, false, get_bound_tex(p_target));

glGetTexLevelParameteriv: //GLenum p_target, GLint p_level, GLenum p_pname, GLint* p_params
    get_tex_level_parameter(p_target, p_level, p_pname, false, get_bound_tex(p_target));

glGetTexParameterfv: //GLenum p_target, GLenum p_pname, GLfloat* p_params
    get_tex_parameter(p_pname, false, get_bound_tex(p_target));

glGetTexParameteriv: //GLenum p_target, GLenum p_pname, GLint* p_params
    get_tex_parameter(p_pname, false, get_bound_tex(p_target));

glGetTexParameterIiv: //GLenum p_target, GLenum p_pname, GLint* p_params
    get_tex_parameter(p_pname, false, get_bound_tex(p_target));

glGetTexParameterIuiv: //GLenum p_target, GLenum p_pname, GLuint* p_params
    get_tex_parameter(p_pname, false, get_bound_tex(p_target));

glGetTextureLevelParameterfv: //GLuint p_texture, GLint p_level, GLenum p_pname, GLfloat* p_params
    get_tex_level_parameter(0, p_level, p_pname, true, p_texture_rev);

glGetTextureLevelParameteriv: //GLuint p_texture, GLint p_level, GLenum p_pname, GLint* p_params
    get_tex_level_parameter(0, p_level, p_pname, true, p_texture_rev);

glGetTextureParameterfv: //GLuint p_texture, GLenum p_pname, GLfloat* p_params
    get_tex_parameter(p_pname, true, p_texture_rev);

glGetTextureParameteriv: //GLuint p_texture, GLenum p_pname, GLint* p_params
    get_tex_parameter(p_pname, true, p_texture_rev);

glGetTextureParameterIiv: //GLuint p_texture, GLenum p_pname, GLint* p_params
    get_tex_parameter(p_pname, true, p_texture_rev);

glGetTextureParameterIuiv: //GLuint p_texture, GLenum p_pname, GLuint* p_params
    get_tex_parameter(p_pname, true, p_texture_rev);

glIsTexture: //GLuint p_texture
    ;
