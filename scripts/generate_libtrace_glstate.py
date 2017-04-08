#Generates src/libtrace/libtrace_glstate.h
import sys

src = '''
uint drawable_width
uint drawable_height

map buffer bound_buffer
    GL_ARRAY_BUFFER
    GL_ATOMIC_COUNTER_BUFFER
    GL_COPY_READ_BUFFER
    GL_COPY_WRITE_BUFFER
    GL_DISPATCH_INDIRECT_BUFFER
    GL_DRAW_INDIRECT_BUFFER
    GL_ELEMENT_ARRAY_BUFFER
    GL_PIXEL_PACK_BUFFER
    GL_PIXEL_UNPACK_BUFFER
    GL_QUERY_BUFFER
    GL_SHADER_STORAGE_BUFFER
    GL_TEXTURE_BUFFER
    GL_TRANSFORM_FEEDBACK_BUFFER
    GL_UNIFORM_BUFFER

program bound_program

vao bound_vao

renderbuffer bound_renderbuffer

framebuffer read_framebuffer
framebuffer draw_framebuffer

map array query bound_queries
    GL_SAMPLES_PASSED
    GL_ANY_SAMPLES_PASSED
    GL_ANY_SAMPLES_PASSED_CONSERVATIVE
    GL_PRIMITIVES_GENERATED
    GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN
    GL_TIME_ELAPSED

bool made_current_before

uint active_texture_unit
map array texture bound_textures
    GL_TEXTURE_1D
    GL_TEXTURE_2D
    GL_TEXTURE_3D
    GL_TEXTURE_1D_ARRAY
    GL_TEXTURE_2D_ARRAY
    GL_TEXTURE_RECTANGLE
    GL_TEXTURE_CUBE_MAP
    GL_TEXTURE_CUBE_MAP_ARRAY
    GL_TEXTURE_BUFFER
    GL_TEXTURE_2D_MULTISAMPLE
    GL_TEXTURE_2D_MULTISAMPLE_ARRAY

array uint8 front_color_buffer
array uint8 back_color_buffer
array uint8 back_depth_buffer
array uint8 back_stencil_buffer

//TODO: GL_CLIP_DISTANCE
map bool enabled
    GL_BLEND
    GL_COLOR_LOGIC_OP
    GL_CULL_FACE
    GL_DEBUG_OUTPUT
    GL_DEBUG_OUTPUT_SYNCHRONOUS
    GL_DEPTH_CLAMP
    GL_DEPTH_TEST
    GL_DITHER
    GL_FRAMEBUFFER_SRGB
    GL_LINE_SMOOTH
    GL_MULTISAMPLE
    GL_POLYGON_OFFSET_FILL
    GL_POLYGON_OFFSET_LINE
    GL_POLYGON_OFFSET_POINT
    GL_POLYGON_SMOOTH
    GL_PRIMITIVE_RESTART
    GL_PRIMITIVE_RESTART_FIXED_INDEX
    GL_RASTERIZER_DISCARD
    GL_SAMPLE_ALPHA_TO_COVERAGE
    GL_SAMPLE_ALPHA_TO_ONE
    GL_SAMPLE_COVERAGE
    GL_SAMPLE_SHADING
    GL_SAMPLE_MASK
    GL_SCISSOR_TEST
    GL_STENCIL_TEST
    GL_TEXTURE_CUBE_MAP_SEAMLESS
    GL_PROGRAM_POINT_SIZE

map array bool state_bool
    GL_DEPTH_WRITEMASK
    GL_COLOR_WRITEMASK
    GL_PACK_SWAP_BYTES
    GL_PACK_LSB_FIRST
    GL_UNPACK_SWAP_BYTES
    GL_UNPACK_LSB_FIRST
    GL_SAMPLE_COVERAGE_INVERT
//    GL_DOUBLEBUFFER
//    GL_STEREO
//    GL_VERTEX_ARRAY
//    GL_PRIMITIVE_RESTART_FOR_PATCHES_SUPPORTED
//    GL_TRANSFORM_FEEDBACK_PAUSED
//    GL_TRANSFORM_FEEDBACK_ACTIVE
//    GL_QUADS_FOLLOW_PROVOKING_VERTEX_CONVENTION

map array int state_int
    GL_PACK_ROW_LENGTH
    GL_PACK_IMAGE_HEIGHT
    GL_PACK_SKIP_ROWS
    GL_PACK_SKIP_PIXELS
    GL_PACK_SKIP_IMAGES
    GL_PACK_ALIGNMENT
    GL_UNPACK_ROW_LENGTH
    GL_UNPACK_IMAGE_HEIGHT
    GL_UNPACK_SKIP_ROWS
    GL_UNPACK_SKIP_PIXELS
    GL_UNPACK_SKIP_IMAGES
    GL_UNPACK_ALIGNMENT
    GL_STENCIL_CLEAR_VALUE
    GL_STENCIL_VALUE_MASK
    GL_STENCIL_REF
    GL_STENCIL_BACK_VALUE_MASK
    GL_STENCIL_BACK_REF
//    GL_STENCIL_WRITEMASK
    GL_VIEWPORT
    GL_SCISSOR_BOX
//    GL_MAX_CLIP_DISTANCES
//    GL_MAX_TEXTURE_SIZE
//    GL_SUBPIXEL_BITS
//    GL_MAX_3D_TEXTURE_SIZE
//    GL_SAMPLE_ALPHA_TO_COVERAGE
//    GL_SAMPLE_ALPHA_TO_ONE
//    GL_SAMPLE_COVERAGE
//    GL_SAMPLE_BUFFERS
//    GL_SAMPLES
//    GL_SAMPLE_COVERAGE_INVERT
//    GL_MAX_ELEMENTS_VERTICES
//    GL_MAX_ELEMENTS_INDICES
//    GL_MAJOR_VERSION
//    GL_MINOR_VERSION
//    GL_NUM_EXTENSIONS
//    GL_CONTEXT_FLAGS
//    GL_DEBUG_NEXT_LOGGED_MESSAGE_LENGTH
//    GL_ACTIVE_PROGRAM
//    GL_PROGRAM_PIPELINE_BINDING
//    GL_MAX_VIEWPORTS
//    GL_VIEWPORT_SUBPIXEL_BITS
//    GL_MAX_COMPUTE_SHARED_MEMORY_SIZE
//    GL_MAX_COMPUTE_UNIFORM_COMPONENTS
//    GL_MAX_COMPUTE_ATOMIC_COUNTER_BUFFERS
//    GL_MAX_COMPUTE_ATOMIC_COUNTERS
//    GL_MAX_COMBINED_COMPUTE_UNIFORM_COMPONENTS
//    GL_MAX_DEBUG_GROUP_STACK_DEPTH
//    GL_DEBUG_GROUP_STACK_DEPTH
//    GL_MAX_UNIFORM_LOCATIONS
//    GL_MAX_VERTEX_ATTRIB_RELATIVE_OFFSET
//    GL_MAX_VERTEX_ATTRIB_BINDINGS
//    GL_MAX_LABEL_LENGTH
//    GL_MAX_CULL_DISTANCES
//    GL_MAX_COMBINED_CLIP_AND_CULL_DISTANCES
//    GL_MAX_RENDERBUFFER_SIZE
//    GL_MAX_RECTANGLE_TEXTURE_SIZE
//    GL_MAX_CUBE_MAP_TEXTURE_SIZE
//    GL_VERTEX_ARRAY_BINDING
//    GL_NUM_COMPRESSED_TEXTURE_FORMATS
//    GL_NUM_PROGRAM_BINARY_FORMATS
//    GL_MAX_DRAW_BUFFERS
//    GL_MAX_VERTEX_ATTRIBS
//    GL_MAX_TESS_CONTROL_INPUT_COMPONENTS
//    GL_MAX_TESS_EVALUATION_INPUT_COMPONENTS
//    GL_MAX_TEXTURE_IMAGE_UNITS
//    GL_ARRAY_BUFFER_BINDING
//    GL_ELEMENT_ARRAY_BUFFER_BINDING
//    GL_PIXEL_PACK_BUFFER_BINDING
//    GL_PIXEL_UNPACK_BUFFER_BINDING
//    GL_MAX_DUAL_SOURCE_DRAW_BUFFERS
//    GL_MAX_ARRAY_TEXTURE_LAYERS
//    GL_MAX_VERTEX_UNIFORM_BLOCKS
//    GL_MAX_GEOMETRY_UNIFORM_BLOCKS
//    GL_MAX_FRAGMENT_UNIFORM_BLOCKS
//    GL_MAX_COMBINED_UNIFORM_BLOCKS
//    GL_MAX_UNIFORM_BUFFER_BINDINGS
//    GL_MAX_UNIFORM_BLOCK_SIZE
//    GL_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS
//    GL_MAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS
//    GL_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS
//    GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT
//    GL_MAX_FRAGMENT_UNIFORM_COMPONENTS
//    GL_MAX_VERTEX_UNIFORM_COMPONENTS
//    GL_MAX_VARYING_COMPONENTS
//    GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS
//    GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS
//    GL_CURRENT_PROGRAM
//    GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS
//    GL_MAX_TEXTURE_BUFFER_SIZE
//    GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_COMPONENTS
//    GL_MAX_TRANSFORM_FEEDBACK_INTERLEAVED_COMPONENTS
//    GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS
//    GL_DRAW_FRAMEBUFFER_BINDING
//    GL_RENDERBUFFER_BINDING
//    GL_READ_FRAMEBUFFER_BINDING
//    GL_MAX_COLOR_ATTACHMENTS
//    GL_MAX_SAMPLES
//    GL_MAX_ELEMENT_INDEX
//    GL_MAX_GEOMETRY_UNIFORM_COMPONENTS
//    GL_MAX_GEOMETRY_OUTPUT_VERTICES
//    GL_MAX_GEOMETRY_TOTAL_OUTPUT_COMPONENTS
//    GL_MAX_SUBROUTINES
//    GL_MAX_SUBROUTINE_UNIFORM_LOCATIONS
//    GL_NUM_SHADER_BINARY_FORMATS
//    GL_MAX_VERTEX_UNIFORM_VECTORS
//    GL_MAX_VARYING_VECTORS
//    GL_MAX_FRAGMENT_UNIFORM_VECTORS
//    GL_MAX_COMBINED_TESS_CONTROL_UNIFORM_COMPONENTS
//    GL_MAX_COMBINED_TESS_EVALUATION_UNIFORM_COMPONENTS
//    GL_TRANSFORM_FEEDBACK_BINDING
//    GL_SAMPLE_MASK_VALUE
//    GL_MAX_SAMPLE_MASK_WORDS
//    GL_MAX_GEOMETRY_SHADER_INVOCATIONS
//    GL_FRAGMENT_INTERPOLATION_OFFSET_BITS
//    GL_MIN_PROGRAM_TEXTURE_GATHER_OFFSET
//    GL_MAX_PROGRAM_TEXTURE_GATHER_OFFSET
//    GL_MAX_TRANSFORM_FEEDBACK_BUFFERS
//    GL_MAX_VERTEX_STREAMS
//    GL_PATCH_VERTICES
//    GL_MAX_PATCH_VERTICES
//    GL_MAX_TESS_GEN_LEVEL
//    GL_MAX_TESS_CONTROL_UNIFORM_COMPONENTS
//    GL_MAX_TESS_EVALUATION_UNIFORM_COMPONENTS
//    GL_MAX_TESS_CONTROL_TEXTURE_IMAGE_UNITS
//    GL_MAX_TESS_EVALUATION_TEXTURE_IMAGE_UNITS
//    GL_MAX_TESS_CONTROL_OUTPUT_COMPONENTS
//    GL_MAX_TESS_PATCH_COMPONENTS
//    GL_MAX_TESS_CONTROL_TOTAL_OUTPUT_COMPONENTS
//    GL_MAX_TESS_EVALUATION_OUTPUT_COMPONENTS
//    GL_MAX_TESS_CONTROL_UNIFORM_BLOCKS
//    GL_MAX_TESS_EVALUATION_UNIFORM_BLOCKS
//    GL_COPY_READ_BUFFER
//    GL_COPY_WRITE_BUFFER
//    GL_MAX_IMAGE_UNITS
//    GL_MAX_COMBINED_IMAGE_UNITS_AND_FRAGMENT_OUTPUTS
//    GL_DRAW_INDIRECT_BUFFER_BINDING
//    GL_PRIMITIVE_RESTART_INDEX
//    GL_MAX_IMAGE_SAMPLES
//    GL_MIN_MAP_BUFFER_ALIGNMENT
//    GL_MAX_VERTEX_IMAGE_UNIFORMS
//    GL_MAX_TESS_CONTROL_IMAGE_UNIFORMS
//    GL_MAX_TESS_EVALUATION_IMAGE_UNIFORMS
//    GL_MAX_GEOMETRY_IMAGE_UNIFORMS
//    GL_MAX_FRAGMENT_IMAGE_UNIFORMS
//    GL_MAX_COMBINED_IMAGE_UNIFORMS
//    GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS
//    GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS
//    GL_DISPATCH_INDIRECT_BUFFER_BINDING
//    GL_MAX_COLOR_TEXTURE_SAMPLES
//    GL_MAX_DEPTH_TEXTURE_SAMPLES
//    GL_MAX_INTEGER_SAMPLES
//    GL_MAX_VERTEX_OUTPUT_COMPONENTS
//    GL_MAX_GEOMETRY_INPUT_COMPONENTS
//    GL_MAX_GEOMETRY_OUTPUT_COMPONENTS
//    GL_MAX_FRAGMENT_INPUT_COMPONENTS
//    GL_CONTEXT_PROFILE_MASK
//    GL_UNPACK_COMPRESSED_BLOCK_WIDTH
//    GL_UNPACK_COMPRESSED_BLOCK_HEIGHT
//    GL_UNPACK_COMPRESSED_BLOCK_DEPTH
//    GL_UNPACK_COMPRESSED_BLOCK_SIZE
//    GL_PACK_COMPRESSED_BLOCK_WIDTH
//    GL_PACK_COMPRESSED_BLOCK_HEIGHT
//    GL_PACK_COMPRESSED_BLOCK_DEPTH
//    GL_PACK_COMPRESSED_BLOCK_SIZE
//    GL_MAX_DEBUG_MESSAGE_LENGTH
//    GL_MAX_DEBUG_LOGGED_MESSAGES
//    GL_DEBUG_LOGGED_MESSAGES
//    GL_QUERY_BUFFER_BINDING
//    GL_TEXTURE_BUFFER_OFFSET_ALIGNMENT
//    GL_MAX_COMPUTE_UNIFORM_BLOCKS
//    GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS
//    GL_MAX_COMPUTE_IMAGE_UNIFORMS
//    GL_MAX_COMPUTE_WORK_GROUP_COUNT
//    GL_MAX_COMPUTE_WORK_GROUP_SIZE
//    GL_MAX_VERTEX_ATOMIC_COUNTER_BUFFERS
//    GL_MAX_TESS_CONTROL_ATOMIC_COUNTER_BUFFERS
//    GL_MAX_TESS_EVALUATION_ATOMIC_COUNTER_BUFFERS
//    GL_MAX_GEOMETRY_ATOMIC_COUNTER_BUFFERS
//    GL_MAX_FRAGMENT_ATOMIC_COUNTER_BUFFERS
//    GL_MAX_COMBINED_ATOMIC_COUNTER_BUFFERS
//    GL_MAX_VERTEX_ATOMIC_COUNTERS
//    GL_MAX_TESS_CONTROL_ATOMIC_COUNTERS
//    GL_MAX_TESS_EVALUATION_ATOMIC_COUNTERS
//    GL_MAX_GEOMETRY_ATOMIC_COUNTERS
//    GL_MAX_FRAGMENT_ATOMIC_COUNTERS
//    GL_MAX_COMBINED_ATOMIC_COUNTERS
//    GL_MAX_ATOMIC_COUNTER_BUFFER_SIZE
//    GL_MAX_ATOMIC_COUNTER_BUFFER_BINDINGS

//TODO: GL_DRAW_BUFFER*
map array enum state_enum
//    GL_POLYGON_MODE
//    GL_CULL_FACE_MODE
//    GL_FRONT_FACE
//    GL_DEPTH_FUNC
    GL_STENCIL_FUNC
    GL_STENCIL_FAIL
    GL_STENCIL_PASS_DEPTH_FAIL
    GL_STENCIL_PASS_DEPTH_PASS
    GL_STENCIL_BACK_FUNC
    GL_STENCIL_BACK_FAIL
    GL_STENCIL_BACK_PASS_DEPTH_FAIL
    GL_STENCIL_BACK_PASS_DEPTH_PASS
    GL_BLEND_DST_RGB
    GL_BLEND_DST_ALPHA
    GL_BLEND_SRC_RGB
    GL_BLEND_SRC_ALPHA
    GL_BLEND_EQUATION_RGB
    GL_BLEND_EQUATION_ALPHA
//    GL_LOGIC_OP_MODE
//    GL_DRAW_BUFFER
//    GL_READ_BUFFER
//    GL_LINE_SMOOTH_HINT
//    GL_POLYGON_SMOOTH_HINT
//    GL_LAYER_PROVOKING_VERTEX
//    GL_VIEWPORT_INDEX_PROVOKING_VERTEX
//    GL_CONTEXT_RELEASE_BEHAVIOR
//    GL_CONTEXT_RELEASE_BEHAVIOR_FLUSH
//    GL_ACTIVE_TEXTURE
//    GL_TEXTURE_COMPRESSION_HINT
//    GL_COMPRESSED_TEXTURE_FORMATS
//    GL_PROGRAM_BINARY_FORMATS
//    GL_CLAMP_READ_COLOR
//    GL_IMPLEMENTATION_COLOR_READ_TYPE
//    GL_IMPLEMENTATION_COLOR_READ_FORMAT
//    GL_POINT_SPRITE_COORD_ORIGIN
//    GL_PROVOKING_VERTEX
//    GL_CLIP_ORIGIN
//    GL_CLIP_DEPTH_MODE

map array float state_float
    GL_DEPTH_CLEAR_VALUE
    GL_COLOR_CLEAR_VALUE
    GL_DEPTH_RANGE
    GL_POINT_SIZE
//    GL_POINT_SIZE_RANGE
//    GL_POINT_SIZE_GRANULARITY
    GL_LINE_WIDTH
//    GL_LINE_WIDTH_RANGE
//    GL_LINE_WIDTH_GRANULARITY
    GL_POLYGON_OFFSET_UNITS
    GL_POLYGON_OFFSET_FACTOR
    GL_SAMPLE_COVERAGE_VALUE
    GL_BLEND_COLOR
//    GL_MAX_VIEWPORT_DIMS
//    GL_POINT_FADE_THRESHOLD_SIZE
//    GL_VIEWPORT_BOUNDS_RANGE
//    GL_ALIASED_LINE_WIDTH_RANGE
//    GL_MAX_TEXTURE_LOD_BIAS
//    GL_MIN_PROGRAM_TEXEL_OFFSET
//    GL_MAX_PROGRAM_TEXEL_OFFSET
//    GL_MIN_SAMPLE_SHADING_VALUE
//    GL_MIN_FRAGMENT_INTERPOLATION_OFFSET
//    GL_MAX_FRAGMENT_INTERPOLATION_OFFSET
//    GL_PATCH_DEFAULT_INNER_LEVEL
//    GL_PATCH_DEFAULT_OUTER_LEVEL

map array char state_ascii_string
//    GL_VENDOR
//    GL_RENDERER
//    GL_VERSION
//    GL_SHADING_LANGUAGE_VERSION

//Used for draw calls
uint draw_vao
'''

class Property(object):
    def __init__(self, map, array, base, name, map_keys=[]):
        self.map = map
        self.array = array
        self.base = base
        self.name = name
        self.map_keys = map_keys
        self.get_func_sig = ''
        self.set_func_sig = ''
        self.size_func_sig = ''
        self.init_func_sig = ''
        self.init1_func_sig = ''
        self.insert_func_sig = ''
        self.erase_func_sig = ''
        self.c_type = {'uint': 'uint',
                       'char': 'char',
                       'enum': 'uint',
                       'int': 'int',
                       'int64': 'int64_t',
                       'bool': 'bool',
                       'uint8': 'uint8_t',
                       'float': 'float',
                       'texture': 'uint',
                       'buffer': 'uint',
                       'program': 'uint',
                       'vao': 'uint',
                       'query': 'uint',
                       'renderbuffer': 'uint',
                       'framebuffer': 'uint'}[base]
        self.some_value = {'uint': '0',
                           'char': '0',
                           'enum': '0',
                           'int': '0',
                           'int64': '0',
                           'bool': 'false',
                           'uint8': '0',
                           'float': '0.0f',
                           'texture': '0',
                           'buffer': '0',
                           'program': '0',
                           'vao': '0',
                           'query': '0',
                           'renderbuffer': '0',
                           'framebuffer': '0'}[base]

lines = src.split('\n')
lines = [l for l in lines if not l.startswith('//')]
properties = []
while len(lines):
    line = lines.pop(0)
    if line == '': continue
    types = line.split(' ')[:-1]
    name = line.split(' ')[-1]
    map = 'map' in types
    if map: types.remove('map')
    array = 'array' in types
    if array: types.remove('array')
    base = types[0]
    map_keys = []
    if map:
        while len(lines) and lines[0].startswith('    '):
            map_keys.append(lines.pop(0)[4:])
    properties.append(Property(map, array, base, name, map_keys))

for prop in properties:
    sig = prop.c_type + ' trc_gl_state_get_%s(trace_t* trace' % prop.name
    if prop.map:
        sig += ', uint key'
    if prop.array:
        sig += ', size_t index'
    sig += ')'
    prop.get_func_sig = sig
    
    sig = 'void trc_gl_state_set_%s(trace_t* trace, ' % prop.name
    if prop.map:
        sig += 'uint key, '
    if prop.array:
        sig += 'size_t index, '
    sig += prop.c_type + ' val)'
    prop.set_func_sig = sig
    
    if prop.array:
        sig = 'size_t trc_gl_state_get_%s_size(trace_t* trace' % prop.name
        if prop.map:
            sig += ', uint key'
        sig += ')'
        prop.size_func_sig = sig
        
        sig = 'void trc_gl_state_%s_init(trace_t* trace' % prop.name
        if prop.map:
            sig += ', uint key'
        sig += ', size_t count, const %s* data)' % prop.c_type
        prop.init_func_sig = sig
        
        sig = 'void trc_gl_state_%s_init1(trace_t* trace' % prop.name
        if prop.map:
            sig += ', uint key'
        sig += ', %s value)' % prop.c_type
        prop.init1_func_sig = sig
        
        sig = 'void trc_gl_state_%s_insert(trace_t* trace' % prop.name
        if prop.map:
            sig += ', uint key'
        sig += ', size_t before, %s value)' % prop.c_type
        prop.insert_func_sig = sig
        
        sig = 'void trc_gl_state_%s_erase(trace_t* trace' % prop.name
        if prop.map:
            sig += ', uint key'
        sig += ', size_t index)'
        prop.erase_func_sig = sig

output = open('../src/libtrace/libtrace_glstate.h', 'w')
sys.stdout = output

print '//Generated by genereate_libtrace_glstate.py'
print '//Do not edit. Edit generate_libtrace_glstate.py instead'
print '#ifdef WIP15_STATE_GEN_DECL'
print 'typedef struct trc_gl_context_rev_t {'
print '    uint64_t revision;'
print '    void* real;'
print '    '
for prop in properties:
    c_type = prop.c_type
    if prop.array:
        c_type = 'trc_data_t*'
    if prop.map:
        for key in prop.map_keys:
            print '    %s %s_%s;' % (c_type, prop.name, key)
    else:
        print '    %s %s;' % (c_type, prop.name)
print '} trc_gl_context_rev_t;'
print '#endif'
print

print '#ifdef WIP15_STATE_GEN_FUNC_DECL'
for prop in properties:
    print prop.get_func_sig + ';'
    print prop.set_func_sig + ';'
    if prop.array:
        print prop.size_func_sig + ';'
        print prop.init_func_sig + ';'
        print prop.init1_func_sig + ';'
        print prop.insert_func_sig + ';'
print '#endif'
print

print '#ifdef WIP15_STATE_GEN_IMPL'
print '#include <GL/gl.h>'
print '#include <assert.h>'
print
for prop in properties:
    print prop.get_func_sig + ' {'
    if prop.map:
        if prop.array:
            print '    trc_data_t* arr = NULL;'
        print '    switch (key) {'
        for key in prop.map_keys:
            if prop.array:
                print '    case %s: arr = trc_get_gl_context(trace, 0)->%s_%s; break;' % (key, prop.name, key)
            else:
                print '    case %s: return trc_get_gl_context(trace, 0)->%s_%s;' % (key, prop.name, key)
        print '    default: assert(false); return %s;' % (prop.some_value)
        print '    }'
    elif prop.array:
        print '    trc_data_t* arr = trc_get_gl_context(trace, 0)->%s;' % prop.name
    else:
        print '    return trc_get_gl_context(trace, 0)->%s;' % prop.name
    if prop.array:
        print '    %s* data = trc_lock_data(arr, true, false);' % prop.c_type
        print '    %s res = data[index];' % (prop.c_type)
        print '    trc_unlock_data(arr);'
        print '    return res;'
    print '}'
    print

for prop in properties:
    print prop.set_func_sig + ' {'
    print '    trc_gl_context_rev_t state = *trc_get_gl_context(trace, 0);'
    if prop.map:
        if prop.array:
            print '    trc_data_t** arr = NULL;'
        print '    switch (key) {'
        for key in prop.map_keys:
            if prop.array:
                print '    case %s: arr = &state.%s_%s; break;' % (key, prop.name, key)
            else:
                print '    case %s: state.%s_%s = val; break;' % (key, prop.name, key)
        print '    default: assert(false);'
        print '    }'
    elif prop.array:
        print '    trc_data_t** arr = &state.%s;' % prop.name
    else:
        print '    state.%s = val;' % prop.name
    if prop.array:
        print '    void* olddata = trc_lock_data(*arr, true, false);'
        print '    trc_data_t* newdata = trc_create_inspection_data(trace, (*arr)->uncompressed_size, olddata);'
        print '    trc_unlock_data(*arr);'
        print '    %s* data = trc_lock_data(newdata, false, true);' % prop.c_type
        print '    data[index] = val;'
        print '    trc_unlock_data(newdata);'
        print '    *arr = newdata;'
    print '    trc_set_gl_context(trace, 0, &state);'
    print '}'
    print

for prop in properties:
    if not prop.array: continue
    
    print prop.size_func_sig + ' {'
    if prop.map:
        print '    switch (key) {'
        for key in prop.map_keys:
            print '    case %s: return trc_get_gl_context(trace, 0)->%s_%s->uncompressed_size / sizeof(%s);' % (key, prop.name, key, prop.c_type)
        print '    default: assert(false); return 0;'
        print '    }'
    else:
        print '    return trc_get_gl_context(trace, 0)->%s->uncompressed_size / sizeof(%s);' % (prop.name, prop.c_type)
    print '}'
    print
    
    print prop.init_func_sig + ' {'
    print '    trc_gl_context_rev_t state = *trc_get_gl_context(trace, 0);'
    if prop.map:
        print '    trc_data_t** arr = NULL;'
        print '    switch (key) {'
        for key in prop.map_keys:
            print '    case %s: arr = &state.%s_%s; break;' % (key, prop.name, key)
        print '    }'
    else:
        print '    trc_data_t** arr = &state.%s;' % prop.name;
    print '    *arr = trc_create_inspection_data(trace, count*sizeof(%s), data);' % prop.c_type
    print '    trc_set_gl_context(trace, 0, &state);'
    print '}'
    print
    
    print prop.init1_func_sig + ' {'
    if prop.map:
        print '    trc_gl_state_%s_init(trace, key, 1, &value);' % prop.name
    else:
        print '    trc_gl_state_%s_init(trace, 1, &value);' % prop.name
    print '}'
    print
    
    print prop.insert_func_sig + ' {'
    print '    trc_gl_context_rev_t state = *trc_get_gl_context(trace, 0);'
    if prop.map:
        print '    trc_data_t** arr = NULL;'
        print '    switch (key) {'
        for key in prop.map_keys:
            print '    case %s: arr = &state.%s_%s; break;' % (key, prop.name, key)
        print '    }'
    else:
        print '    trc_data_t** arr = &state.%s;' % prop.name;
    print '    size_t count = (*arr)->uncompressed_size / sizeof(%s);' % prop.c_type
    print '    trc_data_t* newarr = trc_create_inspection_data(trace, (count+1)*sizeof(%s), NULL);' % prop.c_type
    print '    %s* olddata = trc_lock_data(*arr, true, false);' % prop.c_type
    print '    %s* newdata = trc_lock_data(newarr, false, true);' % prop.c_type
    print '    memcpy(newdata, olddata, before*sizeof(%s));' % prop.c_type
    print '    newdata[before] = value;'
    print '    memcpy(newdata+before+1, olddata+before, (count-before)*sizeof(%s));' % prop.c_type
    print '    trc_unlock_data(newarr);'
    print '    trc_unlock_data(*arr);'
    print '    *arr = newarr;'
    print '    trc_set_gl_context(trace, 0, &state);'
    print '}'
    print
    
    print prop.erase_func_sig + ' {'
    print '    trc_gl_context_rev_t state = *trc_get_gl_context(trace, 0);'
    if prop.map:
        print '    trc_data_t** arr = NULL;'
        print '    switch (key) {'
        for key in prop.map_keys:
            print '    case %s: arr = &state.%s_%s; break;' % (key, prop.name, key)
        print '    }'
    else:
        print '    trc_data_t** arr = &state.%s;' % prop.name;
    print '    size_t count = (*arr)->uncompressed_size / sizeof(%s);' % prop.c_type
    print '    trc_data_t* newarr = trc_create_inspection_data(trace, (count-1)*sizeof(%s), NULL);' % prop.c_type
    print '    %s* olddata = trc_lock_data(*arr, true, false);' % prop.c_type
    print '    %s* newdata = trc_lock_data(newarr, false, true);' % prop.c_type
    print '    memcpy(newdata, olddata, index*sizeof(%s));' % prop.c_type
    print '    memcpy(newdata+index, olddata+index+1, (count-index-1)*sizeof(%s));' % prop.c_type
    print '    trc_unlock_data(newarr);'
    print '    trc_unlock_data(*arr);'
    print '    *arr = newarr;'
    print '    trc_set_gl_context(trace, 0, &state);'
    print '}'
    print
print '#endif'

output.close()
