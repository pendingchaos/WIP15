enable_entries = {"StateEnableEntry_AlphaTest": "GL_ALPHA_TEST",
                  "StateEnableEntry_AutoNormal": "GL_AUTO_NORMAL",
                  "StateEnableEntry_Blend": "GL_BLEND",
                  "StateEnableEntry_ColorArray": "GL_COLOR_ARRAY",
                  "StateEnableEntry_ColorLogicOp": "GL_COLOR_LOGIC_OP",
                  "StateEnableEntry_ColorMaterial": "GL_COLOR_MATERIAL",
                  "StateEnableEntry_ColorSum": "GL_COLOR_SUM",
                  "StateEnableEntry_ColorTable": "GL_COLOR_TABLE",
                  "StateEnableEntry_Convolution1D": "GL_CONVOLUTION_1D",
                  "StateEnableEntry_Convolution2D": "GL_CONVOLUTION_2D",
                  "StateEnableEntry_CullFace": "GL_CULL_FACE",
                  "StateEnableEntry_DepthTest": "GL_DEPTH_TEST",
                  "StateEnableEntry_Dither": "GL_DITHER",
                  "StateEnableEntry_EdgeFlagArray": "GL_EDGE_FLAG_ARRAY",
                  "StateEnableEntry_Fog": "GL_FOG",
                  "StateEnableEntry_FogCoordArray": "GL_FOG_COORD_ARRAY",
                  "StateEnableEntry_Histogram": "GL_HISTOGRAM",
                  "StateEnableEntry_IndexArray": "GL_INDEX_ARRAY",
                  "StateEnableEntry_IndexLogicOp": "GL_INDEX_LOGIC_OP",
                  "StateEnableEntry_Lighting": "GL_LIGHTING",
                  "StateEnableEntry_LineSmooth": "GL_LINE_SMOOTH",
                  "StateEnableEntry_LineStipple": "GL_LINE_STIPPLE",
                  "StateEnableEntry_Map1Color4": "GL_MAP1_COLOR_4",
                  "StateEnableEntry_Map1Index": "GL_MAP1_INDEX",
                  "StateEnableEntry_Map1Normal": "GL_MAP1_NORMAL",
                  "StateEnableEntry_Map1TexCoord1": "GL_MAP1_TEXTURE_COORD_1",
                  "StateEnableEntry_Map1TexCoord2": "GL_MAP1_TEXTURE_COORD_2",
                  "StateEnableEntry_Map1TexCoord3": "GL_MAP1_TEXTURE_COORD_3",
                  "StateEnableEntry_Map1TexCoord4": "GL_MAP1_TEXTURE_COORD_4",
                  "StateEnableEntry_Map2Color4": "GL_MAP2_COLOR_4",
                  "StateEnableEntry_Map2Index": "GL_MAP2_INDEX",
                  "StateEnableEntry_Map2Normal": "GL_MAP2_NORMAL",
                  "StateEnableEntry_Map2TexCoord1": "GL_MAP2_TEXTURE_COORD_1",
                  "StateEnableEntry_Map2TexCoord2": "GL_MAP2_TEXTURE_COORD_2",
                  "StateEnableEntry_Map2TexCoord3": "GL_MAP2_TEXTURE_COORD_3",
                  "StateEnableEntry_Map2TexCoord4": "GL_MAP2_TEXTURE_COORD_4",
                  "StateEnableEntry_Map2Vertex3": "GL_MAP2_VERTEX_3",
                  "StateEnableEntry_Map2Vertex4": "GL_MAP2_VERTEX_4",
                  "StateEnableEntry_MinMax": "GL_MINMAX",
                  "StateEnableEntry_Multisample": "GL_MULTISAMPLE",
                  "StateEnableEntry_NormalArray": "GL_NORMAL_ARRAY",
                  "StateEnableEntry_Normalize": "GL_NORMALIZE",
                  "StateEnableEntry_PointSmooth": "GL_POINT_SMOOTH",
                  "StateEnableEntry_PointSprite": "GL_POINT_SPRITE",
                  "StateEnableEntry_PolygonSmooth": "GL_POLYGON_SMOOTH",
                  "StateEnableEntry_PolygonOffsetFill": "GL_POLYGON_OFFSET_FILL",
                  "StateEnableEntry_PolygonOffsetLine": "GL_POLYGON_OFFSET_LINE",
                  "StateEnableEntry_PolygonOffsetPoint": "GL_POLYGON_OFFSET_POINT",
                  "StateEnableEntry_PolygonStipple": "GL_POLYGON_STIPPLE",
                  "StateEnableEntry_PostColorMatrixColorTable": "GL_POST_COLOR_MATRIX_COLOR_TABLE",
                  "StateEnableEntry_PostConvolutionColorTable": "GL_POST_CONVOLUTION_COLOR_TABLE",
                  "StateEnableEntry_RescaleNormal": "GL_RESCALE_NORMAL",
                  "StateEnableEntry_SampleAlphaToCoverage": "GL_SAMPLE_ALPHA_TO_COVERAGE",
                  "StateEnableEntry_SampleAlphaToOne": "GL_SAMPLE_ALPHA_TO_ONE",
                  "StateEnableEntry_SampleCoverage": "GL_SAMPLE_COVERAGE",
                  "StateEnableEntry_ScissorTest": "GL_SCISSOR_TEST",
                  "StateEnableEntry_SecondaryColorArray": "GL_SECONDARY_COLOR_ARRAY",
                  "StateEnableEntry_Separable2D": "GL_SEPARABLE_2D",
                  "StateEnableEntry_StencilTest": "GL_STENCIL_TEST",
                  "StateEnableEntry_Texture1D": "GL_TEXTURE_1D",
                  "StateEnableEntry_Texture2D": "GL_TEXTURE_2D",
                  "StateEnableEntry_Texture3D": "GL_TEXTURE_3D",
                  "StateEnableEntry_TextureCoordArray": "GL_TEXTURE_COORD_ARRAY",
                  "StateEnableEntry_TextureCubeMap": "GL_TEXTURE_CUBE_MAP",
                  "StateEnableEntry_TextureGenQ": "GL_TEXTURE_GEN_Q",
                  "StateEnableEntry_TextureGenR": "GL_TEXTURE_GEN_R",
                  "StateEnableEntry_TextureGenS": "GL_TEXTURE_GEN_S",
                  "StateEnableEntry_TextureGenT": "GL_TEXTURE_GEN_T",
                  "StateEnableEntry_VertexArray": "GL_VERTEX_ARRAY",
                  "StateEnableEntry_VertexProgramPointSize": "GL_VERTEX_PROGRAM_POINT_SIZE",
                  "StateEnableEntry_VertexProgramTwoSide": "GL_VERTEX_PROGRAM_TWO_SIDE"}

# Creates the gets using data based off apitrace's (https://github.com/apitrace/apitrace/blob/master/specs/glparams.py)
"""
X = "X"
B = "B"
I = "I"
I64 = "I64"
E = "E"
F = "F"
D = "D"
P = "P"
S = "S"
H = "H"

parameters = [
    # (functions, type, count, name) # value
    ("",	X,	1,	"GL_ZERO"),	# 0x0000
    ("",	X,	1,	"GL_ONE"),	# 0x0001

    # These are enumerated separately in GLenum_mode
    #("",	X,	1,	"GL_POINTS"),	# 0x0000
    #("",	X,	1,	"GL_LINES"),	# 0x0001
    #("",	X,	1,	"GL_LINE_LOOP"),	# 0x0002
    #("",	X,	1,	"GL_LINE_STRIP"),	# 0x0003
    #("",	X,	1,	"GL_TRIANGLES"),	# 0x0004
    #("",	X,	1,	"GL_TRIANGLE_STRIP"),	# 0x0005
    #("",	X,	1,	"GL_TRIANGLE_FAN"),	# 0x0006
    #("",	X,	1,	"GL_QUADS"),	# 0x0007
    #("",	X,	1,	"GL_QUAD_STRIP"),	# 0x0008
    #("",	X,	1,	"GL_POLYGON"),	# 0x0009
    #("",	X,	1,	"GL_LINES_ADJACENCY"),	# 0x000A
    #("",	X,	1,	"GL_LINE_STRIP_ADJACENCY"),	# 0x000B
    #("",	X,	1,	"GL_TRIANGLES_ADJACENCY"),	# 0x000C
    #("",	X,	1,	"GL_TRIANGLE_STRIP_ADJACENCY"),	# 0x000D
    #("",	X,	1,	"GL_PATCHES"),	# 0x000E

    ("",	X,	1,	"GL_RESTART_PATH_NV"),	# 0x00F0
    ("",	X,	1,	"GL_DUP_FIRST_CUBIC_CURVE_TO_NV"),	# 0x00F2
    ("",	X,	1,	"GL_DUP_LAST_CUBIC_CURVE_TO_NV"),	# 0x00F4
    ("",	X,	1,	"GL_RECT_NV"),	# 0x00F6
    ("",	X,	1,	"GL_RELATIVE_RECT_NV"),	# 0x00F7
    ("",	X,	1,	"GL_CIRCULAR_CCW_ARC_TO_NV"),	# 0x00F8
    ("",	X,	1,	"GL_CIRCULAR_CW_ARC_TO_NV"),	# 0x00FA
    ("",	X,	1,	"GL_CIRCULAR_TANGENT_ARC_TO_NV"),	# 0x00FC
    ("",	X,	1,	"GL_ARC_TO_NV"),	# 0x00FE
    ("",	X,	1,	"GL_RELATIVE_ARC_TO_NV"),	# 0x00FF

    ("",	X,	1,	"GL_ACCUM"),	# 0x0100
    ("",	X,	1,	"GL_LOAD"),	# 0x0101
    ("",	X,	1,	"GL_RETURN"),	# 0x0102
    ("",	X,	1,	"GL_MULT"),	# 0x0103
    ("",	X,	1,	"GL_ADD"),	# 0x0104
    ("",	X,	1,	"GL_NEVER"),	# 0x0200
    ("",	X,	1,	"GL_LESS"),	# 0x0201
    ("",	X,	1,	"GL_EQUAL"),	# 0x0202
    ("",	X,	1,	"GL_LEQUAL"),	# 0x0203
    ("",	X,	1,	"GL_GREATER"),	# 0x0204
    ("",	X,	1,	"GL_NOTEQUAL"),	# 0x0205
    ("",	X,	1,	"GL_GEQUAL"),	# 0x0206
    ("",	X,	1,	"GL_ALWAYS"),	# 0x0207
    ("",	X,	1,	"GL_SRC_COLOR"),	# 0x0300
    ("",	X,	1,	"GL_ONE_MINUS_SRC_COLOR"),	# 0x0301
    ("",	X,	1,	"GL_SRC_ALPHA"),	# 0x0302
    ("",	X,	1,	"GL_ONE_MINUS_SRC_ALPHA"),	# 0x0303
    ("",	X,	1,	"GL_DST_ALPHA"),	# 0x0304
    ("",	X,	1,	"GL_ONE_MINUS_DST_ALPHA"),	# 0x0305
    ("",	X,	1,	"GL_DST_COLOR"),	# 0x0306
    ("",	X,	1,	"GL_ONE_MINUS_DST_COLOR"),	# 0x0307
    ("",	X,	1,	"GL_SRC_ALPHA_SATURATE"),	# 0x0308
    ("",	X,	1,	"GL_FRONT_LEFT"),	# 0x0400
    ("",	X,	1,	"GL_FRONT_RIGHT"),	# 0x0401
    ("",	X,	1,	"GL_BACK_LEFT"),	# 0x0402
    ("",	X,	1,	"GL_BACK_RIGHT"),	# 0x0403
    ("",	X,	1,	"GL_FRONT"),	# 0x0404
    ("",	X,	1,	"GL_BACK"),	# 0x0405
    ("",	X,	1,	"GL_LEFT"),	# 0x0406
    ("",	X,	1,	"GL_RIGHT"),	# 0x0407
    ("",	X,	1,	"GL_FRONT_AND_BACK"),	# 0x0408
    ("",	X,	1,	"GL_AUX0"),	# 0x0409
    ("",	X,	1,	"GL_AUX1"),	# 0x040A
    ("",	X,	1,	"GL_AUX2"),	# 0x040B
    ("",	X,	1,	"GL_AUX3"),	# 0x040C
    ("",	X,	1,	"GL_INVALID_ENUM"),	# 0x0500
    ("",	X,	1,	"GL_INVALID_VALUE"),	# 0x0501
    ("",	X,	1,	"GL_INVALID_OPERATION"),	# 0x0502
    ("",	X,	1,	"GL_STACK_OVERFLOW"),	# 0x0503
    ("",	X,	1,	"GL_STACK_UNDERFLOW"),	# 0x0504
    ("",	X,	1,	"GL_OUT_OF_MEMORY"),	# 0x0505
    ("",	X,	1,	"GL_INVALID_FRAMEBUFFER_OPERATION"),	# 0x0506
    ("",	X,	1,	"GL_CONTEXT_LOST"),	# 0x0507
    ("",	X,	1,	"GL_2D"),	# 0x0600
    ("",	X,	1,	"GL_3D"),	# 0x0601
    ("",	X,	1,	"GL_3D_COLOR"),	# 0x0602
    ("",	X,	1,	"GL_3D_COLOR_TEXTURE"),	# 0x0603
    ("",	X,	1,	"GL_4D_COLOR_TEXTURE"),	# 0x0604
    ("",	X,	1,	"GL_PASS_THROUGH_TOKEN"),	# 0x0700
    ("",	X,	1,	"GL_POINT_TOKEN"),	# 0x0701
    ("",	X,	1,	"GL_LINE_TOKEN"),	# 0x0702
    ("",	X,	1,	"GL_POLYGON_TOKEN"),	# 0x0703
    ("",	X,	1,	"GL_BITMAP_TOKEN"),	# 0x0704
    ("",	X,	1,	"GL_DRAW_PIXEL_TOKEN"),	# 0x0705
    ("",	X,	1,	"GL_COPY_PIXEL_TOKEN"),	# 0x0706
    ("",	X,	1,	"GL_LINE_RESET_TOKEN"),	# 0x0707
    ("",	X,	1,	"GL_EXP"),	# 0x0800
    ("",	X,	1,	"GL_EXP2"),	# 0x0801
    ("",	X,	1,	"GL_CW"),	# 0x0900
    ("",	X,	1,	"GL_CCW"),	# 0x0901
    ("",	X,	1,	"GL_COEFF"),	# 0x0A00
    ("",	X,	1,	"GL_ORDER"),	# 0x0A01
    ("",	X,	1,	"GL_DOMAIN"),	# 0x0A02
    ("glGet",	F,	4,	"GL_CURRENT_COLOR"),	# 0x0B00
    ("glGet",	F,	1,	"GL_CURRENT_INDEX"),	# 0x0B01
    ("glGet",	F,	3,	"GL_CURRENT_NORMAL"),	# 0x0B02
    ("glGet",	F,	4,	"GL_CURRENT_TEXTURE_COORDS"),	# 0x0B03
    ("glGet",	F,	4,	"GL_CURRENT_RASTER_COLOR"),	# 0x0B04
    ("glGet",	F,	1,	"GL_CURRENT_RASTER_INDEX"),	# 0x0B05
    ("glGet",	F,	4,	"GL_CURRENT_RASTER_TEXTURE_COORDS"),	# 0x0B06
    ("glGet",	F,	4,	"GL_CURRENT_RASTER_POSITION"),	# 0x0B07
    ("glGet",	I,	1,	"GL_CURRENT_RASTER_POSITION_VALID"),	# 0x0B08
    ("glGet",	F,	1,	"GL_CURRENT_RASTER_DISTANCE"),	# 0x0B09
    ("glGet",	B,	1,	"GL_POINT_SMOOTH"),	# 0x0B10
    ("glGet",	F,	1,	"GL_POINT_SIZE"),	# 0x0B11
    ("glGet",	F,	2,	"GL_POINT_SIZE_RANGE"),	# 0x0B12
    ("glGet",	F,	1,	"GL_POINT_SIZE_GRANULARITY"),	# 0x0B13
    ("glGet",	B,	1,	"GL_LINE_SMOOTH"),	# 0x0B20
    ("glGet",	F,	1,	"GL_LINE_WIDTH"),	# 0x0B21
    ("glGet",	F,	2,	"GL_LINE_WIDTH_RANGE"),	# 0x0B22
    ("glGet",	F,	1,	"GL_LINE_WIDTH_GRANULARITY"),	# 0x0B23
    ("glGet",	B,	1,	"GL_LINE_STIPPLE"),	# 0x0B24
    ("glGet",	I,	1,	"GL_LINE_STIPPLE_PATTERN"),	# 0x0B25
    ("glGet",	I,	1,	"GL_LINE_STIPPLE_REPEAT"),	# 0x0B26
    ("glGet",	E,	1,	"GL_LIST_MODE"),	# 0x0B30
    ("glGet",	I,	1,	"GL_MAX_LIST_NESTING"),	# 0x0B31
    ("glGet",	I,	1,	"GL_LIST_BASE"),	# 0x0B32
    ("glGet",	I,	1,	"GL_LIST_INDEX"),	# 0x0B33
    ("glGet",	E,	2,	"GL_POLYGON_MODE"),	# 0x0B40
    ("glGet",	B,	1,	"GL_POLYGON_SMOOTH"),	# 0x0B41
    ("glGet",	B,	1,	"GL_POLYGON_STIPPLE"),	# 0x0B42
    ("glGet",	B,	1,	"GL_EDGE_FLAG"),	# 0x0B43
    ("glGet",	B,	1,	"GL_CULL_FACE"),	# 0x0B44
    ("glGet",	E,	1,	"GL_CULL_FACE_MODE"),	# 0x0B45
    ("glGet",	E,	1,	"GL_FRONT_FACE"),	# 0x0B46
    ("glGet",	B,	1,	"GL_LIGHTING"),	# 0x0B50
    ("glGet",	B,	1,	"GL_LIGHT_MODEL_LOCAL_VIEWER"),	# 0x0B51
    ("glGet",	B,	1,	"GL_LIGHT_MODEL_TWO_SIDE"),	# 0x0B52
    ("glGet",	F,	4,	"GL_LIGHT_MODEL_AMBIENT"),	# 0x0B53
    ("glGet",	E,	1,	"GL_SHADE_MODEL"),	# 0x0B54
    ("glGet",	E,	1,	"GL_COLOR_MATERIAL_FACE"),	# 0x0B55
    ("glGet",	E,	1,	"GL_COLOR_MATERIAL_PARAMETER"),	# 0x0B56
    ("glGet",	B,	1,	"GL_COLOR_MATERIAL"),	# 0x0B57
    ("glGet",	B,	1,	"GL_FOG"),	# 0x0B60
    ("glGet",	I,	1,	"GL_FOG_INDEX"),	# 0x0B61
    ("glGet",	F,	1,	"GL_FOG_DENSITY"),	# 0x0B62
    ("glGet",	F,	1,	"GL_FOG_START"),	# 0x0B63
    ("glGet",	F,	1,	"GL_FOG_END"),	# 0x0B64
    ("glGet",	E,	1,	"GL_FOG_MODE"),	# 0x0B65
    ("glGet",	F,	4,	"GL_FOG_COLOR"),	# 0x0B66
    ("glGet",	F,	2,	"GL_DEPTH_RANGE"),	# 0x0B70
    ("glGet",	B,	1,	"GL_DEPTH_TEST"),	# 0x0B71
    ("glGet",	B,	1,	"GL_DEPTH_WRITEMASK"),	# 0x0B72
    ("glGet",	F,	1,	"GL_DEPTH_CLEAR_VALUE"),	# 0x0B73
    ("glGet",	E,	1,	"GL_DEPTH_FUNC"),	# 0x0B74
    ("glGet",	F,	4,	"GL_ACCUM_CLEAR_VALUE"),	# 0x0B80
    ("glGet",	B,	1,	"GL_STENCIL_TEST"),	# 0x0B90
    ("glGet",	I,	1,	"GL_STENCIL_CLEAR_VALUE"),	# 0x0B91
    ("glGet",	E,	1,	"GL_STENCIL_FUNC"),	# 0x0B92
    ("glGet",	I,	1,	"GL_STENCIL_VALUE_MASK"),	# 0x0B93
    ("glGet",	E,	1,	"GL_STENCIL_FAIL"),	# 0x0B94
    ("glGet",	E,	1,	"GL_STENCIL_PASS_DEPTH_FAIL"),	# 0x0B95
    ("glGet",	E,	1,	"GL_STENCIL_PASS_DEPTH_PASS"),	# 0x0B96
    ("glGet",	I,	1,	"GL_STENCIL_REF"),	# 0x0B97
    ("glGet",	I,	1,	"GL_STENCIL_WRITEMASK"),	# 0x0B98
    ("glGet",	E,	1,	"GL_MATRIX_MODE"),	# 0x0BA0
    ("glGet",	B,	1,	"GL_NORMALIZE"),	# 0x0BA1
    ("glGet",	I,	4,	"GL_VIEWPORT"),	# 0x0BA2
    ("glGet",	I,	1,	"GL_MODELVIEW_STACK_DEPTH"),	# 0x0BA3
    ("glGet",	I,	1,	"GL_PROJECTION_STACK_DEPTH"),	# 0x0BA4
    ("glGet",	I,	1,	"GL_TEXTURE_STACK_DEPTH"),	# 0x0BA5
    ("glGet",	F,	16,	"GL_MODELVIEW_MATRIX"),	# 0x0BA6
    ("glGet",	F,	16,	"GL_PROJECTION_MATRIX"),	# 0x0BA7
    ("glGet",	F,	16,	"GL_TEXTURE_MATRIX"),	# 0x0BA8
    ("glGet",	I,	1,	"GL_ATTRIB_STACK_DEPTH"),	# 0x0BB0
    ("glGet",	I,	1,	"GL_CLIENT_ATTRIB_STACK_DEPTH"),	# 0x0BB1
    ("glGet",	B,	1,	"GL_ALPHA_TEST"),	# 0x0BC0
    ("glGet",	E,	1,	"GL_ALPHA_TEST_FUNC"),	# 0x0BC1
    ("glGet",	F,	1,	"GL_ALPHA_TEST_REF"),	# 0x0BC2
    ("glGet",	B,	1,	"GL_DITHER"),	# 0x0BD0
    ("glGet",	E,	1,	"GL_BLEND_DST"),	# 0x0BE0
    ("glGet",	E,	1,	"GL_BLEND_SRC"),	# 0x0BE1
    ("glGet",	B,	1,	"GL_BLEND"),	# 0x0BE2
    ("glGet",	E,	1,	"GL_LOGIC_OP_MODE"),	# 0x0BF0
    ("glGet",	B,	1,	"GL_INDEX_LOGIC_OP"),	# 0x0BF1
    ("glGet",	B,	1,	"GL_COLOR_LOGIC_OP"),	# 0x0BF2
    ("glGet",	I,	1,	"GL_AUX_BUFFERS"),	# 0x0C00
    ("glGet",	E,	1,	"GL_DRAW_BUFFER"),	# 0x0C01
    ("glGet",	E,	1,	"GL_READ_BUFFER"),	# 0x0C02
    ("glGet",	I,	4,	"GL_SCISSOR_BOX"),	# 0x0C10
    ("glGet",	B,	1,	"GL_SCISSOR_TEST"),	# 0x0C11
    ("glGet",	I,	1,	"GL_INDEX_CLEAR_VALUE"),	# 0x0C20
    ("glGet",	I,	1,	"GL_INDEX_WRITEMASK"),	# 0x0C21
    ("glGet",	F,	4,	"GL_COLOR_CLEAR_VALUE"),	# 0x0C22
    ("glGet",	B,	4,	"GL_COLOR_WRITEMASK"),	# 0x0C23
    ("glGet",	B,	1,	"GL_INDEX_MODE"),	# 0x0C30
    ("glGet",	B,	1,	"GL_RGBA_MODE"),	# 0x0C31
    ("glGet",	B,	1,	"GL_DOUBLEBUFFER"),	# 0x0C32
    ("glGet",	B,	1,	"GL_STEREO"),	# 0x0C33
    ("glGet",	E,	1,	"GL_RENDER_MODE"),	# 0x0C40
    ("glGet",	E,	1,	"GL_PERSPECTIVE_CORRECTION_HINT"),	# 0x0C50
    ("glGet",	E,	1,	"GL_POINT_SMOOTH_HINT"),	# 0x0C51
    ("glGet",	E,	1,	"GL_LINE_SMOOTH_HINT"),	# 0x0C52
    ("glGet",	E,	1,	"GL_POLYGON_SMOOTH_HINT"),	# 0x0C53
    ("glGet",	E,	1,	"GL_FOG_HINT"),	# 0x0C54
    ("glGet",	B,	1,	"GL_TEXTURE_GEN_S"),	# 0x0C60
    ("glGet",	B,	1,	"GL_TEXTURE_GEN_T"),	# 0x0C61
    ("glGet",	B,	1,	"GL_TEXTURE_GEN_R"),	# 0x0C62
    ("glGet",	B,	1,	"GL_TEXTURE_GEN_Q"),	# 0x0C63
    ("",	X,	1,	"GL_PIXEL_MAP_I_TO_I"),	# 0x0C70
    ("",	X,	1,	"GL_PIXEL_MAP_S_TO_S"),	# 0x0C71
    ("",	X,	1,	"GL_PIXEL_MAP_I_TO_R"),	# 0x0C72
    ("",	X,	1,	"GL_PIXEL_MAP_I_TO_G"),	# 0x0C73
    ("",	X,	1,	"GL_PIXEL_MAP_I_TO_B"),	# 0x0C74
    ("",	X,	1,	"GL_PIXEL_MAP_I_TO_A"),	# 0x0C75
    ("",	X,	1,	"GL_PIXEL_MAP_R_TO_R"),	# 0x0C76
    ("",	X,	1,	"GL_PIXEL_MAP_G_TO_G"),	# 0x0C77
    ("",	X,	1,	"GL_PIXEL_MAP_B_TO_B"),	# 0x0C78
    ("",	X,	1,	"GL_PIXEL_MAP_A_TO_A"),	# 0x0C79
    ("glGet",	I,	1,	"GL_PIXEL_MAP_I_TO_I_SIZE"),	# 0x0CB0
    ("glGet",	I,	1,	"GL_PIXEL_MAP_S_TO_S_SIZE"),	# 0x0CB1
    ("glGet",	I,	1,	"GL_PIXEL_MAP_I_TO_R_SIZE"),	# 0x0CB2
    ("glGet",	I,	1,	"GL_PIXEL_MAP_I_TO_G_SIZE"),	# 0x0CB3
    ("glGet",	I,	1,	"GL_PIXEL_MAP_I_TO_B_SIZE"),	# 0x0CB4
    ("glGet",	I,	1,	"GL_PIXEL_MAP_I_TO_A_SIZE"),	# 0x0CB5
    ("glGet",	I,	1,	"GL_PIXEL_MAP_R_TO_R_SIZE"),	# 0x0CB6
    ("glGet",	I,	1,	"GL_PIXEL_MAP_G_TO_G_SIZE"),	# 0x0CB7
    ("glGet",	I,	1,	"GL_PIXEL_MAP_B_TO_B_SIZE"),	# 0x0CB8
    ("glGet",	I,	1,	"GL_PIXEL_MAP_A_TO_A_SIZE"),	# 0x0CB9
    ("glGet",	B,	1,	"GL_UNPACK_SWAP_BYTES"),	# 0x0CF0
    ("glGet",	B,	1,	"GL_UNPACK_LSB_FIRST"),	# 0x0CF1
    ("glGet",	I,	1,	"GL_UNPACK_ROW_LENGTH"),	# 0x0CF2
    ("glGet",	I,	1,	"GL_UNPACK_SKIP_ROWS"),	# 0x0CF3
    ("glGet",	I,	1,	"GL_UNPACK_SKIP_PIXELS"),	# 0x0CF4
    ("glGet",	I,	1,	"GL_UNPACK_ALIGNMENT"),	# 0x0CF5
    ("glGet",	B,	1,	"GL_PACK_SWAP_BYTES"),	# 0x0D00
    ("glGet",	B,	1,	"GL_PACK_LSB_FIRST"),	# 0x0D01
    ("glGet",	I,	1,	"GL_PACK_ROW_LENGTH"),	# 0x0D02
    ("glGet",	I,	1,	"GL_PACK_SKIP_ROWS"),	# 0x0D03
    ("glGet",	I,	1,	"GL_PACK_SKIP_PIXELS"),	# 0x0D04
    ("glGet",	I,	1,	"GL_PACK_ALIGNMENT"),	# 0x0D05
    ("glGet",	B,	1,	"GL_MAP_COLOR"),	# 0x0D10
    ("glGet",	B,	1,	"GL_MAP_STENCIL"),	# 0x0D11
    ("glGet",	I,	1,	"GL_INDEX_SHIFT"),	# 0x0D12
    ("glGet",	I,	1,	"GL_INDEX_OFFSET"),	# 0x0D13
    ("glGet",	F,	1,	"GL_RED_SCALE"),	# 0x0D14
    ("glGet",	F,	1,	"GL_RED_BIAS"),	# 0x0D15
    ("glGet",	F,	1,	"GL_ZOOM_X"),	# 0x0D16
    ("glGet",	F,	1,	"GL_ZOOM_Y"),	# 0x0D17
    ("glGet",	F,	1,	"GL_GREEN_SCALE"),	# 0x0D18
    ("glGet",	F,	1,	"GL_GREEN_BIAS"),	# 0x0D19
    ("glGet",	F,	1,	"GL_BLUE_SCALE"),	# 0x0D1A
    ("glGet",	F,	1,	"GL_BLUE_BIAS"),	# 0x0D1B
    ("glGet,glGetTexEnv",	F,	1,	"GL_ALPHA_SCALE"),	# 0x0D1C
    ("glGet",	F,	1,	"GL_ALPHA_BIAS"),	# 0x0D1D
    ("glGet",	F,	1,	"GL_DEPTH_SCALE"),	# 0x0D1E
    ("glGet",	F,	1,	"GL_DEPTH_BIAS"),	# 0x0D1F
    ("glGet",	I,	1,	"GL_MAX_EVAL_ORDER"),	# 0x0D30
    ("glGet",	I,	1,	"GL_MAX_LIGHTS"),	# 0x0D31
    ("glGet",	I,	1,	"GL_MAX_CLIP_DISTANCES"),	# 0x0D32
    ("glGet",	I,	1,	"GL_MAX_TEXTURE_SIZE"),	# 0x0D33
    ("glGet",	I,	1,	"GL_MAX_PIXEL_MAP_TABLE"),	# 0x0D34
    ("glGet",	I,	1,	"GL_MAX_ATTRIB_STACK_DEPTH"),	# 0x0D35
    ("glGet",	I,	1,	"GL_MAX_MODELVIEW_STACK_DEPTH"),	# 0x0D36
    ("glGet",	I,	1,	"GL_MAX_NAME_STACK_DEPTH"),	# 0x0D37
    ("glGet",	I,	1,	"GL_MAX_PROJECTION_STACK_DEPTH"),	# 0x0D38
    ("glGet",	I,	1,	"GL_MAX_TEXTURE_STACK_DEPTH"),	# 0x0D39
    ("glGet",	F,	2,	"GL_MAX_VIEWPORT_DIMS"),	# 0x0D3A
    ("glGet",	I,	1,	"GL_MAX_CLIENT_ATTRIB_STACK_DEPTH"),	# 0x0D3B
    ("glGet",	I,	1,	"GL_SUBPIXEL_BITS"),	# 0x0D50
    ("glGet",	I,	1,	"GL_INDEX_BITS"),	# 0x0D51
    ("glGet",	I,	1,	"GL_RED_BITS"),	# 0x0D52
    ("glGet",	I,	1,	"GL_GREEN_BITS"),	# 0x0D53
    ("glGet",	I,	1,	"GL_BLUE_BITS"),	# 0x0D54
    ("glGet",	I,	1,	"GL_ALPHA_BITS"),	# 0x0D55
    ("glGet",	I,	1,	"GL_DEPTH_BITS"),	# 0x0D56
    ("glGet",	I,	1,	"GL_STENCIL_BITS"),	# 0x0D57
    ("glGet",	I,	1,	"GL_ACCUM_RED_BITS"),	# 0x0D58
    ("glGet",	I,	1,	"GL_ACCUM_GREEN_BITS"),	# 0x0D59
    ("glGet",	I,	1,	"GL_ACCUM_BLUE_BITS"),	# 0x0D5A
    ("glGet",	I,	1,	"GL_ACCUM_ALPHA_BITS"),	# 0x0D5B
    ("glGet",	I,	1,	"GL_NAME_STACK_DEPTH"),	# 0x0D70
    ("glGet",	B,	1,	"GL_AUTO_NORMAL"),	# 0x0D80
    ("glGet",	B,	1,	"GL_MAP1_COLOR_4"),	# 0x0D90
    ("glGet",	B,	1,	"GL_MAP1_INDEX"),	# 0x0D91
    ("glGet",	B,	1,	"GL_MAP1_NORMAL"),	# 0x0D92
    ("glGet",	B,	1,	"GL_MAP1_TEXTURE_COORD_1"),	# 0x0D93
    ("glGet",	B,	1,	"GL_MAP1_TEXTURE_COORD_2"),	# 0x0D94
    ("glGet",	B,	1,	"GL_MAP1_TEXTURE_COORD_3"),	# 0x0D95
    ("glGet",	B,	1,	"GL_MAP1_TEXTURE_COORD_4"),	# 0x0D96
    ("glGet",	B,	1,	"GL_MAP1_VERTEX_3"),	# 0x0D97
    ("glGet",	B,	1,	"GL_MAP1_VERTEX_4"),	# 0x0D98
    ("glGet",	B,	1,	"GL_MAP2_COLOR_4"),	# 0x0DB0
    ("glGet",	B,	1,	"GL_MAP2_INDEX"),	# 0x0DB1
    ("glGet",	B,	1,	"GL_MAP2_NORMAL"),	# 0x0DB2
    ("glGet",	B,	1,	"GL_MAP2_TEXTURE_COORD_1"),	# 0x0DB3
    ("glGet",	B,	1,	"GL_MAP2_TEXTURE_COORD_2"),	# 0x0DB4
    ("glGet",	B,	1,	"GL_MAP2_TEXTURE_COORD_3"),	# 0x0DB5
    ("glGet",	B,	1,	"GL_MAP2_TEXTURE_COORD_4"),	# 0x0DB6
    ("glGet",	B,	1,	"GL_MAP2_VERTEX_3"),	# 0x0DB7
    ("glGet",	B,	1,	"GL_MAP2_VERTEX_4"),	# 0x0DB8
    ("glGet",	F,	2,	"GL_MAP1_GRID_DOMAIN"),	# 0x0DD0
    ("glGet",	I,	1,	"GL_MAP1_GRID_SEGMENTS"),	# 0x0DD1
    ("glGet",	F,	4,	"GL_MAP2_GRID_DOMAIN"),	# 0x0DD2
    ("glGet",	I,	2,	"GL_MAP2_GRID_SEGMENTS"),	# 0x0DD3
    ("",	B,	1,	"GL_TEXTURE_1D"),	# 0x0DE0
    ("",	B,	1,	"GL_TEXTURE_2D"),	# 0x0DE1
    ("glGet",	P,	1,	"GL_FEEDBACK_BUFFER_POINTER"),	# 0x0DF0
    ("glGet",	I,	1,	"GL_FEEDBACK_BUFFER_SIZE"),	# 0x0DF1
    ("glGet",	E,	1,	"GL_FEEDBACK_BUFFER_TYPE"),	# 0x0DF2
    ("glGet",	P,	1,	"GL_SELECTION_BUFFER_POINTER"),	# 0x0DF3
    ("glGet",	I,	1,	"GL_SELECTION_BUFFER_SIZE"),	# 0x0DF4
    ("glGetTexLevelParameter",	I,	1,	"GL_TEXTURE_WIDTH"),	# 0x1000
    ("glGetTexLevelParameter",	I,	1,	"GL_TEXTURE_HEIGHT"),	# 0x1001
    ("glGetTexLevelParameter",	E,	1,	"GL_TEXTURE_INTERNAL_FORMAT"),	# 0x1003
    ("glGetTexParameter,glGetSamplerParameter",	F,	4,	"GL_TEXTURE_BORDER_COLOR"),	# 0x1004
    ("glGetTexLevelParameter",	I,	1,	"GL_TEXTURE_BORDER"),	# 0x1005
    ("",	X,	1,	"GL_DONT_CARE"),	# 0x1100
    ("",	X,	1,	"GL_FASTEST"),	# 0x1101
    ("",	X,	1,	"GL_NICEST"),	# 0x1102
    ("glGetLight,glGetMaterial",	F,	4,	"GL_AMBIENT"),	# 0x1200
    ("glGetLight,glGetMaterial",	F,	4,	"GL_DIFFUSE"),	# 0x1201
    ("glGetLight,glGetMaterial",	F,	4,	"GL_SPECULAR"),	# 0x1202
    ("glGetLight",	F,	4,	"GL_POSITION"),	# 0x1203
    ("glGetLight",	F,	3,	"GL_SPOT_DIRECTION"),	# 0x1204
    ("glGetLight",	F,	1,	"GL_SPOT_EXPONENT"),	# 0x1205
    ("glGetLight",	F,	1,	"GL_SPOT_CUTOFF"),	# 0x1206
    ("glGetLight",	F,	1,	"GL_CONSTANT_ATTENUATION"),	# 0x1207
    ("glGetLight",	F,	1,	"GL_LINEAR_ATTENUATION"),	# 0x1208
    ("glGetLight",	F,	1,	"GL_QUADRATIC_ATTENUATION"),	# 0x1209
    ("",	X,	1,	"GL_COMPILE"),	# 0x1300
    ("",	X,	1,	"GL_COMPILE_AND_EXECUTE"),	# 0x1301
    ("",	X,	1,	"GL_BYTE"),	# 0x1400
    ("",	X,	1,	"GL_UNSIGNED_BYTE"),	# 0x1401
    ("",	X,	1,	"GL_SHORT"),	# 0x1402
    ("",	X,	1,	"GL_UNSIGNED_SHORT"),	# 0x1403
    ("",	X,	1,	"GL_INT"),	# 0x1404
    ("",	X,	1,	"GL_UNSIGNED_INT"),	# 0x1405
    ("",	X,	1,	"GL_FLOAT"),	# 0x1406
    ("",	X,	1,	"GL_2_BYTES"),	# 0x1407
    ("",	X,	1,	"GL_3_BYTES"),	# 0x1408
    ("",	X,	1,	"GL_4_BYTES"),	# 0x1409
    ("",	X,	1,	"GL_DOUBLE"),	# 0x140A
    ("",	X,	1,	"GL_HALF_FLOAT"),	# 0x140B
    ("",	X,	1,	"GL_FIXED"),	# 0x140C
    ("",	X,	1,	"GL_INT64_NV"),	# 0x140E
    ("",	X,	1,	"GL_UNSIGNED_INT64_ARB"),	# 0x140F
    ("",	X,	1,	"GL_CLEAR"),	# 0x1500
    ("",	X,	1,	"GL_AND"),	# 0x1501
    ("",	X,	1,	"GL_AND_REVERSE"),	# 0x1502
    ("",	X,	1,	"GL_COPY"),	# 0x1503
    ("",	X,	1,	"GL_AND_INVERTED"),	# 0x1504
    ("",	X,	1,	"GL_NOOP"),	# 0x1505
    ("",	X,	1,	"GL_XOR"),	# 0x1506
    ("",	X,	1,	"GL_OR"),	# 0x1507
    ("",	X,	1,	"GL_NOR"),	# 0x1508
    ("",	X,	1,	"GL_EQUIV"),	# 0x1509
    ("",	X,	1,	"GL_INVERT"),	# 0x150A
    ("",	X,	1,	"GL_OR_REVERSE"),	# 0x150B
    ("",	X,	1,	"GL_COPY_INVERTED"),	# 0x150C
    ("",	X,	1,	"GL_OR_INVERTED"),	# 0x150D
    ("",	X,	1,	"GL_NAND"),	# 0x150E
    ("",	X,	1,	"GL_SET"),	# 0x150F
    ("glGetMaterial",	F,	4,	"GL_EMISSION"),	# 0x1600
    ("glGetMaterial",	F,	1,	"GL_SHININESS"),	# 0x1601
    ("",	F,	4,	"GL_AMBIENT_AND_DIFFUSE"),	# 0x1602
    ("glGetMaterial",	F,	3,	"GL_COLOR_INDEXES"),	# 0x1603
    ("",	X,	1,	"GL_MODELVIEW"),	# 0x1700
    ("",	X,	1,	"GL_PROJECTION"),	# 0x1701
    ("",	X,	1,	"GL_TEXTURE"),	# 0x1702
    ("",	X,	1,	"GL_COLOR"),	# 0x1800
    ("",	X,	1,	"GL_DEPTH"),	# 0x1801
    ("",	X,	1,	"GL_STENCIL"),	# 0x1802
    ("",	X,	1,	"GL_COLOR_INDEX"),	# 0x1900
    ("",	X,	1,	"GL_STENCIL_INDEX"),	# 0x1901
    ("",	X,	1,	"GL_DEPTH_COMPONENT"),	# 0x1902
    ("",	X,	1,	"GL_RED"),	# 0x1903
    ("",	X,	1,	"GL_GREEN"),	# 0x1904
    ("",	X,	1,	"GL_BLUE"),	# 0x1905
    ("",	X,	1,	"GL_ALPHA"),	# 0x1906
    ("",	X,	1,	"GL_RGB"),	# 0x1907
    ("",	X,	1,	"GL_RGBA"),	# 0x1908
    ("",	X,	1,	"GL_LUMINANCE"),	# 0x1909
    ("",	X,	1,	"GL_LUMINANCE_ALPHA"),	# 0x190A
    ("",	X,	1,	"GL_BITMAP"),	# 0x1A00
    ("",	X,	1,	"GL_POINT"),	# 0x1B00
    ("",	X,	1,	"GL_LINE"),	# 0x1B01
    ("",	X,	1,	"GL_FILL"),	# 0x1B02
    ("",	X,	1,	"GL_RENDER"),	# 0x1C00
    ("",	X,	1,	"GL_FEEDBACK"),	# 0x1C01
    ("",	X,	1,	"GL_SELECT"),	# 0x1C02
    ("",	X,	1,	"GL_FLAT"),	# 0x1D00
    ("",	X,	1,	"GL_SMOOTH"),	# 0x1D01
    ("",	X,	1,	"GL_KEEP"),	# 0x1E00
    ("",	X,	1,	"GL_REPLACE"),	# 0x1E01
    ("",	X,	1,	"GL_INCR"),	# 0x1E02
    ("",	X,	1,	"GL_DECR"),	# 0x1E03
    ("glGet",	S,	1,	"GL_VENDOR"),	# 0x1F00
    ("glGet",	S,	1,	"GL_RENDERER"),	# 0x1F01
    ("glGet",	S,	1,	"GL_VERSION"),	# 0x1F02
    ("glGet",	S,	1,	"GL_EXTENSIONS"),	# 0x1F03
    ("",	X,	1,	"GL_S"),	# 0x2000
    ("",	X,	1,	"GL_T"),	# 0x2001
    ("",	X,	1,	"GL_R"),	# 0x2002
    ("",	X,	1,	"GL_Q"),	# 0x2003
    ("",	X,	1,	"GL_MODULATE"),	# 0x2100
    ("",	X,	1,	"GL_DECAL"),	# 0x2101
    ("glGetTexEnv",	E,	1,	"GL_TEXTURE_ENV_MODE"),	# 0x2200
    ("glGetTexEnv",	F,	4,	"GL_TEXTURE_ENV_COLOR"),	# 0x2201
    ("",	X,	1,	"GL_TEXTURE_ENV"),	# 0x2300
    ("",	X,	1,	"GL_EYE_LINEAR"),	# 0x2400
    ("",	X,	1,	"GL_OBJECT_LINEAR"),	# 0x2401
    ("",	X,	1,	"GL_SPHERE_MAP"),	# 0x2402
    ("glGetTexGen",	E,	1,	"GL_TEXTURE_GEN_MODE"),	# 0x2500
    ("glGetTexGen",	F,	4,	"GL_OBJECT_PLANE"),	# 0x2501
    ("glGetTexGen",	F,	4,	"GL_EYE_PLANE"),	# 0x2502
    ("",	X,	1,	"GL_NEAREST"),	# 0x2600
    ("",	X,	1,	"GL_LINEAR"),	# 0x2601
    ("",	X,	1,	"GL_NEAREST_MIPMAP_NEAREST"),	# 0x2700
    ("",	X,	1,	"GL_LINEAR_MIPMAP_NEAREST"),	# 0x2701
    ("",	X,	1,	"GL_NEAREST_MIPMAP_LINEAR"),	# 0x2702
    ("",	X,	1,	"GL_LINEAR_MIPMAP_LINEAR"),	# 0x2703
    ("glGetTexParameter,glGetSamplerParameter",	E,	1,	"GL_TEXTURE_MAG_FILTER"),	# 0x2800
    ("glGetTexParameter,glGetSamplerParameter",	E,	1,	"GL_TEXTURE_MIN_FILTER"),	# 0x2801
    ("glGetTexParameter,glGetSamplerParameter",	E,	1,	"GL_TEXTURE_WRAP_S"),	# 0x2802
    ("glGetTexParameter,glGetSamplerParameter",	E,	1,	"GL_TEXTURE_WRAP_T"),	# 0x2803
    ("",	X,	1,	"GL_CLAMP"),	# 0x2900
    ("",	X,	1,	"GL_REPEAT"),	# 0x2901
    ("glGet",	F,	1,	"GL_POLYGON_OFFSET_UNITS"),	# 0x2A00
    ("glGet",	B,	1,	"GL_POLYGON_OFFSET_POINT"),	# 0x2A01
    ("glGet",	B,	1,	"GL_POLYGON_OFFSET_LINE"),	# 0x2A02
    ("",	X,	1,	"GL_R3_G3_B2"),	# 0x2A10
    ("",	X,	1,	"GL_V2F"),	# 0x2A20
    ("",	X,	1,	"GL_V3F"),	# 0x2A21
    ("",	X,	1,	"GL_C4UB_V2F"),	# 0x2A22
    ("",	X,	1,	"GL_C4UB_V3F"),	# 0x2A23
    ("",	X,	1,	"GL_C3F_V3F"),	# 0x2A24
    ("",	X,	1,	"GL_N3F_V3F"),	# 0x2A25
    ("",	X,	1,	"GL_C4F_N3F_V3F"),	# 0x2A26
    ("",	X,	1,	"GL_T2F_V3F"),	# 0x2A27
    ("",	X,	1,	"GL_T4F_V4F"),	# 0x2A28
    ("",	X,	1,	"GL_T2F_C4UB_V3F"),	# 0x2A29
    ("",	X,	1,	"GL_T2F_C3F_V3F"),	# 0x2A2A
    ("",	X,	1,	"GL_T2F_N3F_V3F"),	# 0x2A2B
    ("",	X,	1,	"GL_T2F_C4F_N3F_V3F"),	# 0x2A2C
    ("",	X,	1,	"GL_T4F_C4F_N3F_V4F"),	# 0x2A2D
    ("glGet",	B,	1,	"GL_CLIP_DISTANCE0"),	# 0x3000
    ("glGet",	B,	1,	"GL_CLIP_DISTANCE1"),	# 0x3001
    ("glGet",	B,	1,	"GL_CLIP_DISTANCE2"),	# 0x3002
    ("glGet",	B,	1,	"GL_CLIP_DISTANCE3"),	# 0x3003
    ("glGet",	B,	1,	"GL_CLIP_DISTANCE4"),	# 0x3004
    ("glGet",	B,	1,	"GL_CLIP_DISTANCE5"),	# 0x3005
    ("glGet",	B,	1,	"GL_CLIP_DISTANCE6"),	# 0x3006
    ("glGet",	B,	1,	"GL_CLIP_DISTANCE7"),	# 0x3007
    ("_glGet",	B,	1,	"GL_LIGHT0"),	# 0x4000
    ("_glGet",	B,	1,	"GL_LIGHT1"),	# 0x4001
    ("_glGet",	B,	1,	"GL_LIGHT2"),	# 0x4002
    ("_glGet",	B,	1,	"GL_LIGHT3"),	# 0x4003
    ("_glGet",	B,	1,	"GL_LIGHT4"),	# 0x4004
    ("_glGet",	B,	1,	"GL_LIGHT5"),	# 0x4005
    ("_glGet",	B,	1,	"GL_LIGHT6"),	# 0x4006
    ("_glGet",	B,	1,	"GL_LIGHT7"),	# 0x4007
    ("",	X,	1,	"GL_ABGR_EXT"),	# 0x8000
    ("",	X,	1,	"GL_CONSTANT_COLOR"),	# 0x8001
    ("",	X,	1,	"GL_ONE_MINUS_CONSTANT_COLOR"),	# 0x8002
    ("",	X,	1,	"GL_CONSTANT_ALPHA"),	# 0x8003
    ("",	X,	1,	"GL_ONE_MINUS_CONSTANT_ALPHA"),	# 0x8004
    ("glGet",	F,	4,	"GL_BLEND_COLOR"),	# 0x8005
    ("",	X,	1,	"GL_FUNC_ADD"),	# 0x8006
    ("",	X,	1,	"GL_MIN"),	# 0x8007
    ("",	X,	1,	"GL_MAX"),	# 0x8008
    ("glGet",	E,	1,	"GL_BLEND_EQUATION"),	# 0x8009
    ("",	X,	1,	"GL_FUNC_SUBTRACT"),	# 0x800A
    ("",	X,	1,	"GL_FUNC_REVERSE_SUBTRACT"),	# 0x800B
    ("",	X,	1,	"GL_CMYK_EXT"),	# 0x800C
    ("",	X,	1,	"GL_CMYKA_EXT"),	# 0x800D
    ("glGet",	I,	1,	"GL_PACK_CMYK_HINT_EXT"),	# 0x800E
    ("glGet",	I,	1,	"GL_UNPACK_CMYK_HINT_EXT"),	# 0x800F
    ("glGet",	B,	1,	"GL_CONVOLUTION_1D"),	# 0x8010
    ("glGet",	B,	1,	"GL_CONVOLUTION_2D"),	# 0x8011
    ("glGet",	B,	1,	"GL_SEPARABLE_2D"),	# 0x8012
    ("glGetConvolutionParameter",	E,	1,	"GL_CONVOLUTION_BORDER_MODE"),	# 0x8013
    ("glGetConvolutionParameter",	F,	4,	"GL_CONVOLUTION_FILTER_SCALE"),	# 0x8014
    ("glGetConvolutionParameter",	F,	4,	"GL_CONVOLUTION_FILTER_BIAS"),	# 0x8015
    ("",	X,	1,	"GL_REDUCE"),	# 0x8016
    ("glGetConvolutionParameter",	E,	1,	"GL_CONVOLUTION_FORMAT"),	# 0x8017
    ("glGetConvolutionParameter",	I,	1,	"GL_CONVOLUTION_WIDTH"),	# 0x8018
    ("glGetConvolutionParameter",	I,	1,	"GL_CONVOLUTION_HEIGHT"),	# 0x8019
    ("glGetConvolutionParameter",	I,	1,	"GL_MAX_CONVOLUTION_WIDTH"),	# 0x801A
    ("glGetConvolutionParameter",	I,	1,	"GL_MAX_CONVOLUTION_HEIGHT"),	# 0x801B
    ("glGet",	F,	1,	"GL_POST_CONVOLUTION_RED_SCALE"),	# 0x801C
    ("glGet",	F,	1,	"GL_POST_CONVOLUTION_GREEN_SCALE"),	# 0x801D
    ("glGet",	F,	1,	"GL_POST_CONVOLUTION_BLUE_SCALE"),	# 0x801E
    ("glGet",	F,	1,	"GL_POST_CONVOLUTION_ALPHA_SCALE"),	# 0x801F
    ("glGet",	F,	1,	"GL_POST_CONVOLUTION_RED_BIAS"),	# 0x8020
    ("glGet",	F,	1,	"GL_POST_CONVOLUTION_GREEN_BIAS"),	# 0x8021
    ("glGet",	F,	1,	"GL_POST_CONVOLUTION_BLUE_BIAS"),	# 0x8022
    ("glGet",	F,	1,	"GL_POST_CONVOLUTION_ALPHA_BIAS"),	# 0x8023
    ("glGet",	B,	1,	"GL_HISTOGRAM"),	# 0x8024
    ("",	X,	1,	"GL_PROXY_HISTOGRAM"),	# 0x8025
    ("glGetHistogramParameter",	I,	1,	"GL_HISTOGRAM_WIDTH"),	# 0x8026
    ("glGetHistogramParameter",	E,	1,	"GL_HISTOGRAM_FORMAT"),	# 0x8027
    ("glGetHistogramParameter",	I,	1,	"GL_HISTOGRAM_RED_SIZE"),	# 0x8028
    ("glGetHistogramParameter",	I,	1,	"GL_HISTOGRAM_GREEN_SIZE"),	# 0x8029
    ("glGetHistogramParameter",	I,	1,	"GL_HISTOGRAM_BLUE_SIZE"),	# 0x802A
    ("glGetHistogramParameter",	I,	1,	"GL_HISTOGRAM_ALPHA_SIZE"),	# 0x802B
    ("glGetHistogramParameter",	I,	1,	"GL_HISTOGRAM_LUMINANCE_SIZE"),	# 0x802C
    ("glGetHistogramParameter",	B,	1,	"GL_HISTOGRAM_SINK"),	# 0x802D
    ("glGet",	B,	1,	"GL_MINMAX"),	# 0x802E
    ("glGetMinMaxParameter",	E,	1,	"GL_MINMAX_FORMAT"),	# 0x802F
    ("glGetMinMaxParameter",	B,	1,	"GL_MINMAX_SINK"),	# 0x8030
    ("",	X,	1,	"GL_TABLE_TOO_LARGE"),	# 0x8031
    ("",	X,	1,	"GL_UNSIGNED_BYTE_3_3_2"),	# 0x8032
    ("",	X,	1,	"GL_UNSIGNED_SHORT_4_4_4_4"),	# 0x8033
    ("",	X,	1,	"GL_UNSIGNED_SHORT_5_5_5_1"),	# 0x8034
    ("",	X,	1,	"GL_UNSIGNED_INT_8_8_8_8"),	# 0x8035
    ("",	X,	1,	"GL_UNSIGNED_INT_10_10_10_2"),	# 0x8036
    ("glGet",	B,	1,	"GL_POLYGON_OFFSET_FILL"),	# 0x8037
    ("glGet",	F,	1,	"GL_POLYGON_OFFSET_FACTOR"),	# 0x8038
    ("glGet",	F,	1,	"GL_POLYGON_OFFSET_BIAS_EXT"),	# 0x8039
    ("glGet",	B,	1,	"GL_RESCALE_NORMAL"),	# 0x803A
    ("",	X,	1,	"GL_ALPHA4"),	# 0x803B
    ("",	X,	1,	"GL_ALPHA8"),	# 0x803C
    ("",	X,	1,	"GL_ALPHA12"),	# 0x803D
    ("",	X,	1,	"GL_ALPHA16"),	# 0x803E
    ("",	X,	1,	"GL_LUMINANCE4"),	# 0x803F
    ("",	X,	1,	"GL_LUMINANCE8"),	# 0x8040
    ("",	X,	1,	"GL_LUMINANCE12"),	# 0x8041
    ("",	X,	1,	"GL_LUMINANCE16"),	# 0x8042
    ("",	X,	1,	"GL_LUMINANCE4_ALPHA4"),	# 0x8043
    ("",	X,	1,	"GL_LUMINANCE6_ALPHA2"),	# 0x8044
    ("",	X,	1,	"GL_LUMINANCE8_ALPHA8"),	# 0x8045
    ("",	X,	1,	"GL_LUMINANCE12_ALPHA4"),	# 0x8046
    ("",	X,	1,	"GL_LUMINANCE12_ALPHA12"),	# 0x8047
    ("",	X,	1,	"GL_LUMINANCE16_ALPHA16"),	# 0x8048
    ("",	X,	1,	"GL_INTENSITY"),	# 0x8049
    ("",	X,	1,	"GL_INTENSITY4"),	# 0x804A
    ("",	X,	1,	"GL_INTENSITY8"),	# 0x804B
    ("",	X,	1,	"GL_INTENSITY12"),	# 0x804C
    ("",	X,	1,	"GL_INTENSITY16"),	# 0x804D
    ("",	X,	1,	"GL_RGB2_EXT"),	# 0x804E
    ("",	X,	1,	"GL_RGB4"),	# 0x804F
    ("",	X,	1,	"GL_RGB5"),	# 0x8050
    ("",	X,	1,	"GL_RGB8"),	# 0x8051
    ("",	X,	1,	"GL_RGB10"),	# 0x8052
    ("",	X,	1,	"GL_RGB12"),	# 0x8053
    ("",	X,	1,	"GL_RGB16"),	# 0x8054
    ("",	X,	1,	"GL_RGBA2"),	# 0x8055
    ("",	X,	1,	"GL_RGBA4"),	# 0x8056
    ("",	X,	1,	"GL_RGB5_A1"),	# 0x8057
    ("",	X,	1,	"GL_RGBA8"),	# 0x8058
    ("",	X,	1,	"GL_RGB10_A2"),	# 0x8059
    ("",	X,	1,	"GL_RGBA12"),	# 0x805A
    ("",	X,	1,	"GL_RGBA16"),	# 0x805B
    ("glGetTexLevelParameter",	I,	1,	"GL_TEXTURE_RED_SIZE"),	# 0x805C
    ("glGetTexLevelParameter",	I,	1,	"GL_TEXTURE_GREEN_SIZE"),	# 0x805D
    ("glGetTexLevelParameter",	I,	1,	"GL_TEXTURE_BLUE_SIZE"),	# 0x805E
    ("glGetTexLevelParameter",	I,	1,	"GL_TEXTURE_ALPHA_SIZE"),	# 0x805F
    ("glGetTexLevelParameter",	I,	1,	"GL_TEXTURE_LUMINANCE_SIZE"),	# 0x8060
    ("glGetTexLevelParameter",	I,	1,	"GL_TEXTURE_INTENSITY_SIZE"),	# 0x8061
    ("",	X,	1,	"GL_REPLACE_EXT"),	# 0x8062
    ("",	X,	1,	"GL_PROXY_TEXTURE_1D"),	# 0x8063
    ("",	X,	1,	"GL_PROXY_TEXTURE_2D"),	# 0x8064
    ("",	X,	1,	"GL_TEXTURE_TOO_LARGE_EXT"),	# 0x8065
    ("glGetTexParameter",	F,	1,	"GL_TEXTURE_PRIORITY"),	# 0x8066
    ("glGetTexParameter",	B,	1,	"GL_TEXTURE_RESIDENT"),	# 0x8067
    ("glGet_texture",	I,	1,	"GL_TEXTURE_BINDING_1D"),	# 0x8068
    ("glGet_texture",	I,	1,	"GL_TEXTURE_BINDING_2D"),	# 0x8069
    ("glGet_texture",	I,	1,	"GL_TEXTURE_BINDING_3D"),	# 0x806A
    ("glGet",	I,	1,	"GL_PACK_SKIP_IMAGES"),	# 0x806B
    ("glGet",	F,	1,	"GL_PACK_IMAGE_HEIGHT"),	# 0x806C
    ("glGet",	I,	1,	"GL_UNPACK_SKIP_IMAGES"),	# 0x806D
    ("glGet",	F,	1,	"GL_UNPACK_IMAGE_HEIGHT"),	# 0x806E
    ("",	B,	1,	"GL_TEXTURE_3D"),	# 0x806F
    ("",	X,	1,	"GL_PROXY_TEXTURE_3D"),	# 0x8070
    ("glGetTexLevelParameter",	I,	1,	"GL_TEXTURE_DEPTH"),	# 0x8071
    ("glGetTexParameter,glGetSamplerParameter",	E,	1,	"GL_TEXTURE_WRAP_R"),	# 0x8072
    ("glGet",	I,	1,	"GL_MAX_3D_TEXTURE_SIZE"),	# 0x8073
    ("glGet",	B,	1,	"GL_VERTEX_ARRAY"),	# 0x8074
    ("glGet",	B,	1,	"GL_NORMAL_ARRAY"),	# 0x8075
    ("glGet",	B,	1,	"GL_COLOR_ARRAY"),	# 0x8076
    ("glGet",	B,	1,	"GL_INDEX_ARRAY"),	# 0x8077
    ("glGet",	B,	1,	"GL_TEXTURE_COORD_ARRAY"),	# 0x8078
    ("glGet",	B,	1,	"GL_EDGE_FLAG_ARRAY"),	# 0x8079
    ("glGet",	I,	1,	"GL_VERTEX_ARRAY_SIZE"),	# 0x807A
    ("glGet",	E,	1,	"GL_VERTEX_ARRAY_TYPE"),	# 0x807B
    ("glGet",	I,	1,	"GL_VERTEX_ARRAY_STRIDE"),	# 0x807C
    ("glGet",	I,	1,	"GL_VERTEX_ARRAY_COUNT_EXT"),	# 0x807D
    ("glGet",	E,	1,	"GL_NORMAL_ARRAY_TYPE"),	# 0x807E
    ("glGet",	I,	1,	"GL_NORMAL_ARRAY_STRIDE"),	# 0x807F
    ("glGet",	I,	1,	"GL_NORMAL_ARRAY_COUNT_EXT"),	# 0x8080
    ("glGet",	I,	1,	"GL_COLOR_ARRAY_SIZE"),	# 0x8081
    ("glGet",	E,	1,	"GL_COLOR_ARRAY_TYPE"),	# 0x8082
    ("glGet",	I,	1,	"GL_COLOR_ARRAY_STRIDE"),	# 0x8083
    ("glGet",	I,	1,	"GL_COLOR_ARRAY_COUNT_EXT"),	# 0x8084
    ("glGet",	E,	1,	"GL_INDEX_ARRAY_TYPE"),	# 0x8085
    ("glGet",	I,	1,	"GL_INDEX_ARRAY_STRIDE"),	# 0x8086
    ("glGet",	I,	1,	"GL_INDEX_ARRAY_COUNT_EXT"),	# 0x8087
    ("glGet",	I,	1,	"GL_TEXTURE_COORD_ARRAY_SIZE"),	# 0x8088
    ("glGet",	E,	1,	"GL_TEXTURE_COORD_ARRAY_TYPE"),	# 0x8089
    ("glGet",	I,	1,	"GL_TEXTURE_COORD_ARRAY_STRIDE"),	# 0x808A
    ("glGet",	I,	1,	"GL_TEXTURE_COORD_ARRAY_COUNT_EXT"),	# 0x808B
    ("glGet",	I,	1,	"GL_EDGE_FLAG_ARRAY_STRIDE"),	# 0x808C
    ("glGet",	I,	1,	"GL_EDGE_FLAG_ARRAY_COUNT_EXT"),	# 0x808D
    ("glGet",	P,	1,	"GL_VERTEX_ARRAY_POINTER"),	# 0x808E
    ("glGet",	P,	1,	"GL_NORMAL_ARRAY_POINTER"),	# 0x808F
    ("glGet",	P,	1,	"GL_COLOR_ARRAY_POINTER"),	# 0x8090
    ("glGet",	P,	1,	"GL_INDEX_ARRAY_POINTER"),	# 0x8091
    ("glGet",	P,	1,	"GL_TEXTURE_COORD_ARRAY_POINTER"),	# 0x8092
    ("glGet",	P,	1,	"GL_EDGE_FLAG_ARRAY_POINTER"),	# 0x8093
    ("glGet",	I,	1,	"GL_INTERLACE_SGIX"),	# 0x8094
    ("",	X,	1,	"GL_DETAIL_TEXTURE_2D_SGIS"),	# 0x8095
    ("glGet",	I,	1,	"GL_DETAIL_TEXTURE_2D_BINDING_SGIS"),	# 0x8096
    ("",	X,	1,	"GL_LINEAR_DETAIL_SGIS"),	# 0x8097
    ("",	X,	1,	"GL_LINEAR_DETAIL_ALPHA_SGIS"),	# 0x8098
    ("",	X,	1,	"GL_LINEAR_DETAIL_COLOR_SGIS"),	# 0x8099
    ("",	X,	1,	"GL_DETAIL_TEXTURE_LEVEL_SGIS"),	# 0x809A
    ("",	X,	1,	"GL_DETAIL_TEXTURE_MODE_SGIS"),	# 0x809B
    ("",	X,	1,	"GL_DETAIL_TEXTURE_FUNC_POINTS_SGIS"),	# 0x809C
    ("glGet",	I,	1,	"GL_MULTISAMPLE"),	# 0x809D
    ("glGet",	I,	1,	"GL_SAMPLE_ALPHA_TO_COVERAGE"),	# 0x809E
    ("glGet",	I,	1,	"GL_SAMPLE_ALPHA_TO_ONE"),	# 0x809F
    ("glGet",	I,	1,	"GL_SAMPLE_COVERAGE"),	# 0x80A0
    ("",	X,	1,	"GL_1PASS_EXT"),	# 0x80A1
    ("",	X,	1,	"GL_2PASS_0_EXT"),	# 0x80A2
    ("",	X,	1,	"GL_2PASS_1_EXT"),	# 0x80A3
    ("",	X,	1,	"GL_4PASS_0_EXT"),	# 0x80A4
    ("",	X,	1,	"GL_4PASS_1_EXT"),	# 0x80A5
    ("",	X,	1,	"GL_4PASS_2_EXT"),	# 0x80A6
    ("",	X,	1,	"GL_4PASS_3_EXT"),	# 0x80A7
    ("glGet",	I,	1,	"GL_SAMPLE_BUFFERS"),	# 0x80A8
    ("glGet",	I,	1,	"GL_SAMPLES"),	# 0x80A9
    ("glGet",	F,	1,	"GL_SAMPLE_COVERAGE_VALUE"),	# 0x80AA
    ("glGet",	I,	1,	"GL_SAMPLE_COVERAGE_INVERT"),	# 0x80AB
    ("glGet",	I,	1,	"GL_SAMPLE_PATTERN_EXT"),	# 0x80AC
    ("",	X,	1,	"GL_LINEAR_SHARPEN_SGIS"),	# 0x80AD
    ("",	X,	1,	"GL_LINEAR_SHARPEN_ALPHA_SGIS"),	# 0x80AE
    ("",	X,	1,	"GL_LINEAR_SHARPEN_COLOR_SGIS"),	# 0x80AF
    ("",	X,	1,	"GL_SHARPEN_TEXTURE_FUNC_POINTS_SGIS"),	# 0x80B0
    ("glGet",	F,	16,	"GL_COLOR_MATRIX"),	# 0x80B1
    ("glGet",	I,	1,	"GL_COLOR_MATRIX_STACK_DEPTH"),	# 0x80B2
    ("glGet",	I,	1,	"GL_MAX_COLOR_MATRIX_STACK_DEPTH"),	# 0x80B3
    ("glGet",	F,	1,	"GL_POST_COLOR_MATRIX_RED_SCALE"),	# 0x80B4
    ("glGet",	F,	1,	"GL_POST_COLOR_MATRIX_GREEN_SCALE"),	# 0x80B5
    ("glGet",	F,	1,	"GL_POST_COLOR_MATRIX_BLUE_SCALE"),	# 0x80B6
    ("glGet",	F,	1,	"GL_POST_COLOR_MATRIX_ALPHA_SCALE"),	# 0x80B7
    ("glGet",	F,	1,	"GL_POST_COLOR_MATRIX_RED_BIAS"),	# 0x80B8
    ("glGet",	F,	1,	"GL_POST_COLOR_MATRIX_GREEN_BIAS"),	# 0x80B9
    ("glGet",	F,	1,	"GL_POST_COLOR_MATRIX_BLUE_BIAS"),	# 0x80BA
    ("glGet",	F,	1,	"GL_POST_COLOR_MATRIX_ALPHA_BIAS"),	# 0x80BB
    ("glGet",	I,	1,	"GL_TEXTURE_COLOR_TABLE_SGI"),	# 0x80BC
    ("",	X,	1,	"GL_PROXY_TEXTURE_COLOR_TABLE_SGI"),	# 0x80BD
    ("",	X,	1,	"GL_TEXTURE_ENV_BIAS_SGIX"),	# 0x80BE
    ("glGetTexParameter",	F,	1,	"GL_TEXTURE_COMPARE_FAIL_VALUE_ARB"),	# 0x80BF
    ("glGet",	E,	1,	"GL_BLEND_DST_RGB"),	# 0x80C8
    ("glGet",	E,	1,	"GL_BLEND_SRC_RGB"),	# 0x80C9
    ("glGet",	E,	1,	"GL_BLEND_DST_ALPHA"),	# 0x80CA
    ("glGet",	E,	1,	"GL_BLEND_SRC_ALPHA"),	# 0x80CB
    ("",	X,	1,	"GL_422_EXT"),	# 0x80CC
    ("",	X,	1,	"GL_422_REV_EXT"),	# 0x80CD
    ("",	X,	1,	"GL_422_AVERAGE_EXT"),	# 0x80CE
    ("",	X,	1,	"GL_422_REV_AVERAGE_EXT"),	# 0x80CF
    ("glGet",	B,	1,	"GL_COLOR_TABLE"),	# 0x80D0
    ("glGet",	I,	1,	"GL_POST_CONVOLUTION_COLOR_TABLE"),	# 0x80D1
    ("glGet",	I,	1,	"GL_POST_COLOR_MATRIX_COLOR_TABLE"),	# 0x80D2
    ("",	X,	1,	"GL_PROXY_COLOR_TABLE"),	# 0x80D3
    ("",	X,	1,	"GL_PROXY_POST_CONVOLUTION_COLOR_TABLE"),	# 0x80D4
    ("",	X,	1,	"GL_PROXY_POST_COLOR_MATRIX_COLOR_TABLE"),	# 0x80D5
    ("glGetColorTableParameter",	F,	4,	"GL_COLOR_TABLE_SCALE"),	# 0x80D6
    ("glGetColorTableParameter",	F,	4,	"GL_COLOR_TABLE_BIAS"),	# 0x80D7
    ("glGetColorTableParameter",	E,	1,	"GL_COLOR_TABLE_FORMAT"),	# 0x80D8
    ("glGetColorTableParameter",	I,	1,	"GL_COLOR_TABLE_WIDTH"),	# 0x80D9
    ("glGetColorTableParameter",	I,	1,	"GL_COLOR_TABLE_RED_SIZE"),	# 0x80DA
    ("glGetColorTableParameter",	I,	1,	"GL_COLOR_TABLE_GREEN_SIZE"),	# 0x80DB
    ("glGetColorTableParameter",	I,	1,	"GL_COLOR_TABLE_BLUE_SIZE"),	# 0x80DC
    ("glGetColorTableParameter",	I,	1,	"GL_COLOR_TABLE_ALPHA_SIZE"),	# 0x80DD
    ("glGetColorTableParameter",	I,	1,	"GL_COLOR_TABLE_LUMINANCE_SIZE"),	# 0x80DE
    ("glGetColorTableParameter",	I,	1,	"GL_COLOR_TABLE_INTENSITY_SIZE"),	# 0x80DF
    ("",	X,	1,	"GL_BGR"),	# 0x80E0
    ("",	X,	1,	"GL_BGRA"),	# 0x80E1
    ("",	X,	1,	"GL_COLOR_INDEX1_EXT"),	# 0x80E2
    ("",	X,	1,	"GL_COLOR_INDEX2_EXT"),	# 0x80E3
    ("",	X,	1,	"GL_COLOR_INDEX4_EXT"),	# 0x80E4
    ("",	X,	1,	"GL_COLOR_INDEX8_EXT"),	# 0x80E5
    ("",	X,	1,	"GL_COLOR_INDEX12_EXT"),	# 0x80E6
    ("",	X,	1,	"GL_COLOR_INDEX16_EXT"),	# 0x80E7
    ("glGet",	I,	1,	"GL_MAX_ELEMENTS_VERTICES"),	# 0x80E8
    ("glGet",	I,	1,	"GL_MAX_ELEMENTS_INDICES"),	# 0x80E9
    ("",	X,	1,	"GL_PHONG_WIN"),	# 0x80EA
    ("",	X,	1,	"GL_PHONG_HINT_WIN"),	# 0x80EB
    ("",	X,	1,	"GL_FOG_SPECULAR_TEXTURE_WIN"),	# 0x80EC
    ("_glGetTexLevelParameter",	I,	1,	"GL_TEXTURE_INDEX_SIZE_EXT"),	# 0x80ED
    ("",	X,	1,	"GL_PARAMETER_BUFFER_ARB"),	# 0x80EE
    ("",	X,	1,	"GL_PARAMETER_BUFFER_BINDING_ARB"),	# 0x80EF
    ("glGet",	E,	1,	"GL_CLIP_VOLUME_CLIPPING_HINT_EXT"),	# 0x80F0
    ("",	X,	1,	"GL_DUAL_ALPHA4_SGIS"),	# 0x8110
    ("",	X,	1,	"GL_DUAL_ALPHA8_SGIS"),	# 0x8111
    ("",	X,	1,	"GL_DUAL_ALPHA12_SGIS"),	# 0x8112
    ("",	X,	1,	"GL_DUAL_ALPHA16_SGIS"),	# 0x8113
    ("",	X,	1,	"GL_DUAL_LUMINANCE4_SGIS"),	# 0x8114
    ("",	X,	1,	"GL_DUAL_LUMINANCE8_SGIS"),	# 0x8115
    ("",	X,	1,	"GL_DUAL_LUMINANCE12_SGIS"),	# 0x8116
    ("",	X,	1,	"GL_DUAL_LUMINANCE16_SGIS"),	# 0x8117
    ("",	X,	1,	"GL_DUAL_INTENSITY4_SGIS"),	# 0x8118
    ("",	X,	1,	"GL_DUAL_INTENSITY8_SGIS"),	# 0x8119
    ("",	X,	1,	"GL_DUAL_INTENSITY12_SGIS"),	# 0x811A
    ("",	X,	1,	"GL_DUAL_INTENSITY16_SGIS"),	# 0x811B
    ("",	X,	1,	"GL_DUAL_LUMINANCE_ALPHA4_SGIS"),	# 0x811C
    ("",	X,	1,	"GL_DUAL_LUMINANCE_ALPHA8_SGIS"),	# 0x811D
    ("",	X,	1,	"GL_QUAD_ALPHA4_SGIS"),	# 0x811E
    ("",	X,	1,	"GL_QUAD_ALPHA8_SGIS"),	# 0x811F
    ("",	X,	1,	"GL_QUAD_LUMINANCE4_SGIS"),	# 0x8120
    ("",	X,	1,	"GL_QUAD_LUMINANCE8_SGIS"),	# 0x8121
    ("",	X,	1,	"GL_QUAD_INTENSITY4_SGIS"),	# 0x8122
    ("",	X,	1,	"GL_QUAD_INTENSITY8_SGIS"),	# 0x8123
    ("",	X,	1,	"GL_DUAL_TEXTURE_SELECT_SGIS"),	# 0x8124
    ("",	X,	1,	"GL_QUAD_TEXTURE_SELECT_SGIS"),	# 0x8125
    ("glGet",	F,	1,	"GL_POINT_SIZE_MIN"),	# 0x8126
    ("glGet",	F,	1,	"GL_POINT_SIZE_MAX"),	# 0x8127
    ("glGet",	F,	1,	"GL_POINT_FADE_THRESHOLD_SIZE"),	# 0x8128
    ("glGet",	F,	3,	"GL_POINT_DISTANCE_ATTENUATION"),	# 0x8129
    ("",	X,	1,	"GL_FOG_FUNC_SGIS"),	# 0x812A
    ("glGet",	I,	1,	"GL_FOG_FUNC_POINTS_SGIS"),	# 0x812B
    ("glGet",	I,	1,	"GL_MAX_FOG_FUNC_POINTS_SGIS"),	# 0x812C
    ("",	X,	1,	"GL_CLAMP_TO_BORDER"),	# 0x812D
    ("",	X,	1,	"GL_TEXTURE_MULTI_BUFFER_HINT_SGIX"),	# 0x812E
    ("",	X,	1,	"GL_CLAMP_TO_EDGE"),	# 0x812F
    ("glGet",	I,	1,	"GL_PACK_SKIP_VOLUMES_SGIS"),	# 0x8130
    ("glGet",	I,	1,	"GL_PACK_IMAGE_DEPTH_SGIS"),	# 0x8131
    ("glGet",	I,	1,	"GL_UNPACK_SKIP_VOLUMES_SGIS"),	# 0x8132
    ("glGet",	I,	1,	"GL_UNPACK_IMAGE_DEPTH_SGIS"),	# 0x8133
    ("glGet",	I,	1,	"GL_TEXTURE_4D_SGIS"),	# 0x8134
    ("",	X,	1,	"GL_PROXY_TEXTURE_4D_SGIS"),	# 0x8135
    ("",	X,	1,	"GL_TEXTURE_4DSIZE_SGIS"),	# 0x8136
    ("",	X,	1,	"GL_TEXTURE_WRAP_Q_SGIS"),	# 0x8137
    ("glGet",	I,	1,	"GL_MAX_4D_TEXTURE_SIZE_SGIS"),	# 0x8138
    ("glGet",	I,	1,	"GL_PIXEL_TEX_GEN_SGIX"),	# 0x8139
    ("glGetTexParameter,glGetSamplerParameter",	F,	1,	"GL_TEXTURE_MIN_LOD"),	# 0x813A
    ("glGetTexParameter,glGetSamplerParameter",	F,	1,	"GL_TEXTURE_MAX_LOD"),	# 0x813B
    ("glGetTexParameter",	F,	1,	"GL_TEXTURE_BASE_LEVEL"),	# 0x813C
    ("glGetTexParameter",	F,	1,	"GL_TEXTURE_MAX_LEVEL"),	# 0x813D
    ("glGet",	I,	1,	"GL_PIXEL_TILE_BEST_ALIGNMENT_SGIX"),	# 0x813E
    ("glGet",	I,	1,	"GL_PIXEL_TILE_CACHE_INCREMENT_SGIX"),	# 0x813F
    ("glGet",	I,	1,	"GL_PIXEL_TILE_WIDTH_SGIX"),	# 0x8140
    ("glGet",	I,	1,	"GL_PIXEL_TILE_HEIGHT_SGIX"),	# 0x8141
    ("glGet",	I,	1,	"GL_PIXEL_TILE_GRID_WIDTH_SGIX"),	# 0x8142
    ("glGet",	I,	1,	"GL_PIXEL_TILE_GRID_HEIGHT_SGIX"),	# 0x8143
    ("glGet",	I,	1,	"GL_PIXEL_TILE_GRID_DEPTH_SGIX"),	# 0x8144
    ("glGet",	I,	1,	"GL_PIXEL_TILE_CACHE_SIZE_SGIX"),	# 0x8145
    ("",	X,	1,	"GL_FILTER4_SGIS"),	# 0x8146
    ("",	X,	1,	"GL_TEXTURE_FILTER4_SIZE_SGIS"),	# 0x8147
    ("glGet",	I,	1,	"GL_SPRITE_SGIX"),	# 0x8148
    ("glGet",	I,	1,	"GL_SPRITE_MODE_SGIX"),	# 0x8149
    ("glGet",	F,	3,	"GL_SPRITE_AXIS_SGIX"),	# 0x814A
    ("glGet",	F,	3,	"GL_SPRITE_TRANSLATION_SGIX"),	# 0x814B
    ("",	X,	1,	"GL_SPRITE_AXIAL_SGIX"),	# 0x814C
    ("",	X,	1,	"GL_SPRITE_OBJECT_ALIGNED_SGIX"),	# 0x814D
    ("",	X,	1,	"GL_SPRITE_EYE_ALIGNED_SGIX"),	# 0x814E
    ("glGet",	I,	1,	"GL_TEXTURE_4D_BINDING_SGIS"),	# 0x814F
    ("",	X,	1,	"GL_IGNORE_BORDER_HP"),	# 0x8150
    ("",	X,	1,	"GL_CONSTANT_BORDER"),	# 0x8151
    ("",	X,	1,	"GL_REPLICATE_BORDER"),	# 0x8153
    ("glGetConvolutionParameter",	F,	4,	"GL_CONVOLUTION_BORDER_COLOR"),	# 0x8154
    ("",	X,	1,	"GL_IMAGE_SCALE_X_HP"),	# 0x8155
    ("",	X,	1,	"GL_IMAGE_SCALE_Y_HP"),	# 0x8156
    ("",	X,	1,	"GL_IMAGE_TRANSLATE_X_HP"),	# 0x8157
    ("",	X,	1,	"GL_IMAGE_TRANSLATE_Y_HP"),	# 0x8158
    ("",	X,	1,	"GL_IMAGE_ROTATE_ANGLE_HP"),	# 0x8159
    ("",	X,	1,	"GL_IMAGE_ROTATE_ORIGIN_X_HP"),	# 0x815A
    ("",	X,	1,	"GL_IMAGE_ROTATE_ORIGIN_Y_HP"),	# 0x815B
    ("",	X,	1,	"GL_IMAGE_MAG_FILTER_HP"),	# 0x815C
    ("",	X,	1,	"GL_IMAGE_MIN_FILTER_HP"),	# 0x815D
    ("",	X,	1,	"GL_IMAGE_CUBIC_WEIGHT_HP"),	# 0x815E
    ("",	X,	1,	"GL_CUBIC_HP"),	# 0x815F
    ("",	X,	1,	"GL_AVERAGE_HP"),	# 0x8160
    ("",	X,	1,	"GL_IMAGE_TRANSFORM_2D_HP"),	# 0x8161
    ("",	X,	1,	"GL_POST_IMAGE_TRANSFORM_COLOR_TABLE_HP"),	# 0x8162
    ("",	X,	1,	"GL_PROXY_POST_IMAGE_TRANSFORM_COLOR_TABLE_HP"),	# 0x8163
    ("_glGet",	B,	1,	"GL_OCCLUSION_TEST_HP"),	# 0x8165
    ("_glGet",	B,	1,	"GL_OCCLUSION_TEST_RESULT_HP"),	# 0x8166
    ("",	X,	1,	"GL_TEXTURE_LIGHTING_MODE_HP"),	# 0x8167
    ("",	X,	1,	"GL_TEXTURE_POST_SPECULAR_HP"),	# 0x8168
    ("",	X,	1,	"GL_TEXTURE_PRE_SPECULAR_HP"),	# 0x8169
    ("",	X,	1,	"GL_LINEAR_CLIPMAP_LINEAR_SGIX"),	# 0x8170
    ("glGetTexParameter",	I,	2,	"GL_TEXTURE_CLIPMAP_CENTER_SGIX"),	# 0x8171
    ("glGetTexParameter",	F,	1,	"GL_TEXTURE_CLIPMAP_FRAME_SGIX"),	# 0x8172
    ("glGetTexParameter",	I,	2,	"GL_TEXTURE_CLIPMAP_OFFSET_SGIX"),	# 0x8173
    ("glGetTexParameter",	I,	3,	"GL_TEXTURE_CLIPMAP_VIRTUAL_DEPTH_SGIX"),	# 0x8174
    ("",	X,	1,	"GL_TEXTURE_CLIPMAP_LOD_OFFSET_SGIX"),	# 0x8175
    ("",	I,	1,	"GL_TEXTURE_CLIPMAP_DEPTH_SGIX"),	# 0x8176
    ("glGet",	I,	1,	"GL_MAX_CLIPMAP_DEPTH_SGIX"),	# 0x8177
    ("glGet",	I,	1,	"GL_MAX_CLIPMAP_VIRTUAL_DEPTH_SGIX"),	# 0x8178
    ("glGetTexParameter",	F,	1,	"GL_POST_TEXTURE_FILTER_BIAS_SGIX"),	# 0x8179
    ("glGetTexParameter",	F,	1,	"GL_POST_TEXTURE_FILTER_SCALE_SGIX"),	# 0x817A
    ("glGet",	F,	2,	"GL_POST_TEXTURE_FILTER_BIAS_RANGE_SGIX"),	# 0x817B
    ("glGet",	F,	2,	"GL_POST_TEXTURE_FILTER_SCALE_RANGE_SGIX"),	# 0x817C
    ("glGet",	I,	1,	"GL_REFERENCE_PLANE_SGIX"),	# 0x817D
    ("glGet",	F,	4,	"GL_REFERENCE_PLANE_EQUATION_SGIX"),	# 0x817E
    ("glGet",	I,	1,	"GL_IR_INSTRUMENT1_SGIX"),	# 0x817F
    ("",	X,	1,	"GL_INSTRUMENT_BUFFER_POINTER_SGIX"),	# 0x8180
    ("glGet",	I,	1,	"GL_INSTRUMENT_MEASUREMENTS_SGIX"),	# 0x8181
    ("",	X,	1,	"GL_LIST_PRIORITY_SGIX"),	# 0x8182
    ("glGet",	I,	1,	"GL_CALLIGRAPHIC_FRAGMENT_SGIX"),	# 0x8183
    ("",	X,	1,	"GL_PIXEL_TEX_GEN_Q_CEILING_SGIX"),	# 0x8184
    ("",	X,	1,	"GL_PIXEL_TEX_GEN_Q_ROUND_SGIX"),	# 0x8185
    ("",	X,	1,	"GL_PIXEL_TEX_GEN_Q_FLOOR_SGIX"),	# 0x8186
    ("",	X,	1,	"GL_PIXEL_TEX_GEN_ALPHA_REPLACE_SGIX"),	# 0x8187
    ("",	X,	1,	"GL_PIXEL_TEX_GEN_ALPHA_NO_REPLACE_SGIX"),	# 0x8188
    ("",	X,	1,	"GL_PIXEL_TEX_GEN_ALPHA_LS_SGIX"),	# 0x8189
    ("",	X,	1,	"GL_PIXEL_TEX_GEN_ALPHA_MS_SGIX"),	# 0x818A
    ("glGet",	I,	1,	"GL_FRAMEZOOM_SGIX"),	# 0x818B
    ("glGet",	I,	1,	"GL_FRAMEZOOM_FACTOR_SGIX"),	# 0x818C
    ("glGet",	I,	1,	"GL_MAX_FRAMEZOOM_FACTOR_SGIX"),	# 0x818D
    ("glGetTexParameter",	F,	1,	"GL_TEXTURE_LOD_BIAS_S_SGIX"),	# 0x818E
    ("glGetTexParameter",	F,	1,	"GL_TEXTURE_LOD_BIAS_T_SGIX"),	# 0x818F
    ("glGetTexParameter",	F,	1,	"GL_TEXTURE_LOD_BIAS_R_SGIX"),	# 0x8190
    ("glGetTexParameter",	B,	1,	"GL_GENERATE_MIPMAP"),	# 0x8191
    ("glGet",	E,	1,	"GL_GENERATE_MIPMAP_HINT"),	# 0x8192
    ("",	X,	1,	"GL_GEOMETRY_DEFORMATION_SGIX"),	# 0x8194
    ("",	X,	1,	"GL_TEXTURE_DEFORMATION_SGIX"),	# 0x8195
    ("glGet",	I,	1,	"GL_DEFORMATIONS_MASK_SGIX"),	# 0x8196
    ("",	X,	1,	"GL_MAX_DEFORMATION_ORDER_SGIX"),	# 0x8197
    ("glGet",	I,	1,	"GL_FOG_OFFSET_SGIX"),	# 0x8198
    ("glGet",	F,	4,	"GL_FOG_OFFSET_VALUE_SGIX"),	# 0x8199
    ("glGetTexParameter",	B,	1,	"GL_TEXTURE_COMPARE_SGIX"),	# 0x819A
    ("glGetTexParameter",	E,	1,	"GL_TEXTURE_COMPARE_OPERATOR_SGIX"),	# 0x819B
    ("",	X,	1,	"GL_TEXTURE_LEQUAL_R_SGIX"),	# 0x819C
    ("",	X,	1,	"GL_TEXTURE_GEQUAL_R_SGIX"),	# 0x819D
    ("",	X,	1,	"GL_DEPTH_COMPONENT16"),	# 0x81A5
    ("",	X,	1,	"GL_DEPTH_COMPONENT24"),	# 0x81A6
    ("",	X,	1,	"GL_DEPTH_COMPONENT32"),	# 0x81A7
    ("glGet",	I,	1,	"GL_ARRAY_ELEMENT_LOCK_FIRST_EXT"),	# 0x81A8
    ("glGet",	I,	1,	"GL_ARRAY_ELEMENT_LOCK_COUNT_EXT"),	# 0x81A9
    ("glGet",	B,	1,	"GL_CULL_VERTEX_EXT"),	# 0x81AA
    ("glGet",	F,	4,	"GL_CULL_VERTEX_EYE_POSITION_EXT"),	# 0x81AB
    ("glGet",	F,	4,	"GL_CULL_VERTEX_OBJECT_POSITION_EXT"),	# 0x81AC
    ("",	X,	1,	"GL_IUI_V2F_EXT"),	# 0x81AD
    ("",	X,	1,	"GL_IUI_V3F_EXT"),	# 0x81AE
    ("",	X,	1,	"GL_IUI_N3F_V2F_EXT"),	# 0x81AF
    ("",	X,	1,	"GL_IUI_N3F_V3F_EXT"),	# 0x81B0
    ("",	X,	1,	"GL_T2F_IUI_V2F_EXT"),	# 0x81B1
    ("",	X,	1,	"GL_T2F_IUI_V3F_EXT"),	# 0x81B2
    ("",	X,	1,	"GL_T2F_IUI_N3F_V2F_EXT"),	# 0x81B3
    ("",	X,	1,	"GL_T2F_IUI_N3F_V3F_EXT"),	# 0x81B4
    ("",	X,	1,	"GL_INDEX_TEST_EXT"),	# 0x81B5
    ("",	X,	1,	"GL_INDEX_TEST_FUNC_EXT"),	# 0x81B6
    ("",	X,	1,	"GL_INDEX_TEST_REF_EXT"),	# 0x81B7
    ("",	X,	1,	"GL_INDEX_MATERIAL_EXT"),	# 0x81B8
    ("",	X,	1,	"GL_INDEX_MATERIAL_PARAMETER_EXT"),	# 0x81B9
    ("",	X,	1,	"GL_INDEX_MATERIAL_FACE_EXT"),	# 0x81BA
    ("",	X,	1,	"GL_YCRCB_422_SGIX"),	# 0x81BB
    ("",	X,	1,	"GL_YCRCB_444_SGIX"),	# 0x81BC
    ("",	X,	1,	"GL_WRAP_BORDER_SUN"),	# 0x81D4
    ("",	X,	1,	"GL_UNPACK_CONSTANT_DATA_SUNX"),	# 0x81D5
    ("",	X,	1,	"GL_TEXTURE_CONSTANT_DATA_SUNX"),	# 0x81D6
    ("",	X,	1,	"GL_TRIANGLE_LIST_SUN"),	# 0x81D7
    ("",	X,	1,	"GL_REPLACEMENT_CODE_SUN"),	# 0x81D8
    ("",	X,	1,	"GL_GLOBAL_ALPHA_SUN"),	# 0x81D9
    ("",	X,	1,	"GL_GLOBAL_ALPHA_FACTOR_SUN"),	# 0x81DA
    ("glGet",	B,	4,	"GL_TEXTURE_COLOR_WRITEMASK_SGIS"),	# 0x81EF
    ("",	X,	1,	"GL_EYE_DISTANCE_TO_POINT_SGIS"),	# 0x81F0
    ("",	X,	1,	"GL_OBJECT_DISTANCE_TO_POINT_SGIS"),	# 0x81F1
    ("",	X,	1,	"GL_EYE_DISTANCE_TO_LINE_SGIS"),	# 0x81F2
    ("",	X,	1,	"GL_OBJECT_DISTANCE_TO_LINE_SGIS"),	# 0x81F3
    ("",	X,	1,	"GL_EYE_POINT_SGIS"),	# 0x81F4
    ("",	X,	1,	"GL_OBJECT_POINT_SGIS"),	# 0x81F5
    ("",	X,	1,	"GL_EYE_LINE_SGIS"),	# 0x81F6
    ("",	X,	1,	"GL_OBJECT_LINE_SGIS"),	# 0x81F7
    ("glGet",	E,	1,	"GL_LIGHT_MODEL_COLOR_CONTROL"),	# 0x81F8
    ("",	X,	1,	"GL_SINGLE_COLOR"),	# 0x81F9
    ("",	X,	1,	"GL_SEPARATE_SPECULAR_COLOR"),	# 0x81FA
    ("",	X,	1,	"GL_SHARED_TEXTURE_PALETTE_EXT"),	# 0x81FB
    ("",	X,	1,	"GL_FOG_SCALE_SGIX"),	# 0x81FC
    ("",	X,	1,	"GL_FOG_SCALE_VALUE_SGIX"),	# 0x81FD
    ("",	X,	1,	"GL_TEXT_FRAGMENT_SHADER_ATI"),	# 0x8200
    ("glGetFramebufferAttachmentParameter",	E,	1,	"GL_FRAMEBUFFER_ATTACHMENT_COLOR_ENCODING"),	# 0x8210
    ("glGetFramebufferAttachmentParameter",	E,	1,	"GL_FRAMEBUFFER_ATTACHMENT_COMPONENT_TYPE"),	# 0x8211
    ("glGetFramebufferAttachmentParameter",	I,	1,	"GL_FRAMEBUFFER_ATTACHMENT_RED_SIZE"),	# 0x8212
    ("glGetFramebufferAttachmentParameter",	I,	1,	"GL_FRAMEBUFFER_ATTACHMENT_GREEN_SIZE"),	# 0x8213
    ("glGetFramebufferAttachmentParameter",	I,	1,	"GL_FRAMEBUFFER_ATTACHMENT_BLUE_SIZE"),	# 0x8214
    ("glGetFramebufferAttachmentParameter",	I,	1,	"GL_FRAMEBUFFER_ATTACHMENT_ALPHA_SIZE"),	# 0x8215
    ("glGetFramebufferAttachmentParameter",	I,	1,	"GL_FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE"),	# 0x8216
    ("glGetFramebufferAttachmentParameter",	I,	1,	"GL_FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE"),	# 0x8217
    ("",	X,	1,	"GL_FRAMEBUFFER_DEFAULT"),	# 0x8218
    ("",	X,	1,	"GL_FRAMEBUFFER_UNDEFINED"),	# 0x8219
    ("",	X,	1,	"GL_DEPTH_STENCIL_ATTACHMENT"),	# 0x821A
    ("glGet",	I,	1,	"GL_MAJOR_VERSION"),	# 0x821B
    ("glGet",	I,	1,	"GL_MINOR_VERSION"),	# 0x821C
    ("glGet",	I,	1,	"GL_NUM_EXTENSIONS"),	# 0x821D
    ("glGet",	I,	1,	"GL_CONTEXT_FLAGS"),	# 0x821E
    ("glGetBufferParameter",	B,	1,	"GL_BUFFER_IMMUTABLE_STORAGE"),	# 0x821F
    ("glGetBufferParameter",	I,	1,	"GL_BUFFER_STORAGE_FLAGS"),	# 0x8220
    ("glGet",	B,	1,	"GL_PRIMITIVE_RESTART_FOR_PATCHES_SUPPORTED"),	# 0x8221
    ("",	X,	1,	"GL_INDEX"),	# 0x8222
    ("",	X,	1,	"GL_COMPRESSED_RED"),	# 0x8225
    ("",	X,	1,	"GL_COMPRESSED_RG"),	# 0x8226
    ("",	X,	1,	"GL_RG"),	# 0x8227
    ("",	X,	1,	"GL_RG_INTEGER"),	# 0x8228
    ("",	X,	1,	"GL_R8"),	# 0x8229
    ("",	X,	1,	"GL_R16"),	# 0x822A
    ("",	X,	1,	"GL_RG8"),	# 0x822B
    ("",	X,	1,	"GL_RG16"),	# 0x822C
    ("",	X,	1,	"GL_R16F"),	# 0x822D
    ("",	X,	1,	"GL_R32F"),	# 0x822E
    ("",	X,	1,	"GL_RG16F"),	# 0x822F
    ("",	X,	1,	"GL_RG32F"),	# 0x8230
    ("",	X,	1,	"GL_R8I"),	# 0x8231
    ("",	X,	1,	"GL_R8UI"),	# 0x8232
    ("",	X,	1,	"GL_R16I"),	# 0x8233
    ("",	X,	1,	"GL_R16UI"),	# 0x8234
    ("",	X,	1,	"GL_R32I"),	# 0x8235
    ("",	X,	1,	"GL_R32UI"),	# 0x8236
    ("",	X,	1,	"GL_RG8I"),	# 0x8237
    ("",	X,	1,	"GL_RG8UI"),	# 0x8238
    ("",	X,	1,	"GL_RG16I"),	# 0x8239
    ("",	X,	1,	"GL_RG16UI"),	# 0x823A
    ("",	X,	1,	"GL_RG32I"),	# 0x823B
    ("",	X,	1,	"GL_RG32UI"),	# 0x823C
    ("",	X,	1,	"GL_SYNC_CL_EVENT_ARB"),	# 0x8240
    ("",	X,	1,	"GL_SYNC_CL_EVENT_COMPLETE_ARB"),	# 0x8241
    ("glGet",	B,	1,	"GL_DEBUG_OUTPUT_SYNCHRONOUS"),	# 0x8242
    ("glGet",	I,	1,	"GL_DEBUG_NEXT_LOGGED_MESSAGE_LENGTH"),	# 0x8243
    ("_glGet",	P,	1,	"GL_DEBUG_CALLBACK_FUNCTION"),	# 0x8244
    ("_glGet",	P,	1,	"GL_DEBUG_CALLBACK_USER_PARAM"),	# 0x8245
    ("",	X,	1,	"GL_DEBUG_SOURCE_API"),	# 0x8246
    ("",	X,	1,	"GL_DEBUG_SOURCE_WINDOW_SYSTEM"),	# 0x8247
    ("",	X,	1,	"GL_DEBUG_SOURCE_SHADER_COMPILER"),	# 0x8248
    ("",	X,	1,	"GL_DEBUG_SOURCE_THIRD_PARTY"),	# 0x8249
    ("",	X,	1,	"GL_DEBUG_SOURCE_APPLICATION"),	# 0x824A
    ("",	X,	1,	"GL_DEBUG_SOURCE_OTHER"),	# 0x824B
    ("",	X,	1,	"GL_DEBUG_TYPE_ERROR"),	# 0x824C
    ("",	X,	1,	"GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR"),	# 0x824D
    ("",	X,	1,	"GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR"),	# 0x824E
    ("",	X,	1,	"GL_DEBUG_TYPE_PORTABILITY"),	# 0x824F
    ("",	X,	1,	"GL_DEBUG_TYPE_PERFORMANCE"),	# 0x8250
    ("",	X,	1,	"GL_DEBUG_TYPE_OTHER"),	# 0x8251
    ("",	X,	1,	"GL_LOSE_CONTEXT_ON_RESET_ARB"),	# 0x8252
    ("",	X,	1,	"GL_GUILTY_CONTEXT_RESET_ARB"),	# 0x8253
    ("",	X,	1,	"GL_INNOCENT_CONTEXT_RESET_ARB"),	# 0x8254
    ("",	X,	1,	"GL_UNKNOWN_CONTEXT_RESET_ARB"),	# 0x8255
    ("glGet",	E,	1,	"GL_RESET_NOTIFICATION_STRATEGY_ARB"),	# 0x8256
    ("glGetProgram",	B,	1,	"GL_PROGRAM_BINARY_RETRIEVABLE_HINT"),	# 0x8257
    ("",	X,	1,	"GL_PROGRAM_SEPARABLE"),	# 0x8258
    ("glGet",	I,	1,	"GL_ACTIVE_PROGRAM"),	# 0x8259
    ("glGet",	I,	1,	"GL_PROGRAM_PIPELINE_BINDING"),	# 0x825A
    ("glGet",	I,	1,	"GL_MAX_VIEWPORTS"),	# 0x825B
    ("glGet",	I,	1,	"GL_VIEWPORT_SUBPIXEL_BITS"),	# 0x825C
    ("glGet",	F,	2,	"GL_VIEWPORT_BOUNDS_RANGE"),	# 0x825D
    ("glGet",	E,	1,	"GL_LAYER_PROVOKING_VERTEX"),	# 0x825E
    ("glGet",	E,	1,	"GL_VIEWPORT_INDEX_PROVOKING_VERTEX"),	# 0x825F
    ("",	X,	1,	"GL_UNDEFINED_VERTEX"),	# 0x8260
    ("",	X,	1,	"GL_NO_RESET_NOTIFICATION_ARB"),	# 0x8261
    ("glGet",	I,	1,	"GL_MAX_COMPUTE_SHARED_MEMORY_SIZE"),	# 0x8262
    ("glGet",	I,	1,	"GL_MAX_COMPUTE_UNIFORM_COMPONENTS"),	# 0x8263
    ("glGet",	I,	1,	"GL_MAX_COMPUTE_ATOMIC_COUNTER_BUFFERS"),	# 0x8264
    ("glGet",	I,	1,	"GL_MAX_COMPUTE_ATOMIC_COUNTERS"),	# 0x8265
    ("glGet",	I,	1,	"GL_MAX_COMBINED_COMPUTE_UNIFORM_COMPONENTS"),	# 0x8266
    ("",	X,	1,	"GL_COMPUTE_WORK_GROUP_SIZE"),	# 0x8267
    ("",	X,	1,	"GL_DEBUG_TYPE_MARKER"),	# 0x8268
    ("",	X,	1,	"GL_DEBUG_TYPE_PUSH_GROUP"),	# 0x8269
    ("",	X,	1,	"GL_DEBUG_TYPE_POP_GROUP"),	# 0x826A
    ("",	X,	1,	"GL_DEBUG_SEVERITY_NOTIFICATION"),	# 0x826B
    ("glGet",	I,	1,	"GL_MAX_DEBUG_GROUP_STACK_DEPTH"),	# 0x826C
    ("glGet",	I,	1,	"GL_DEBUG_GROUP_STACK_DEPTH"),	# 0x826D
    ("glGet",	I,	1,	"GL_MAX_UNIFORM_LOCATIONS"),	# 0x826E
    ("",	B,	1,	"GL_INTERNALFORMAT_SUPPORTED"),	# 0x826F
    ("",	E,	1,	"GL_INTERNALFORMAT_PREFERRED"),	# 0x8270
    ("",	I,	1,	"GL_INTERNALFORMAT_RED_SIZE"),	# 0x8271
    ("",	I,	1,	"GL_INTERNALFORMAT_GREEN_SIZE"),	# 0x8272
    ("",	I,	1,	"GL_INTERNALFORMAT_BLUE_SIZE"),	# 0x8273
    ("",	I,	1,	"GL_INTERNALFORMAT_ALPHA_SIZE"),	# 0x8274
    ("",	I,	1,	"GL_INTERNALFORMAT_DEPTH_SIZE"),	# 0x8275
    ("",	I,	1,	"GL_INTERNALFORMAT_STENCIL_SIZE"),	# 0x8276
    ("",	I,	1,	"GL_INTERNALFORMAT_SHARED_SIZE"),	# 0x8277
    ("",	E,	1,	"GL_INTERNALFORMAT_RED_TYPE"),	# 0x8278
    ("",	E,	1,	"GL_INTERNALFORMAT_GREEN_TYPE"),	# 0x8279
    ("",	E,	1,	"GL_INTERNALFORMAT_BLUE_TYPE"),	# 0x827A
    ("",	E,	1,	"GL_INTERNALFORMAT_ALPHA_TYPE"),	# 0x827B
    ("",	E,	1,	"GL_INTERNALFORMAT_DEPTH_TYPE"),	# 0x827C
    ("",	E,	1,	"GL_INTERNALFORMAT_STENCIL_TYPE"),	# 0x827D
    ("",	I,	1,	"GL_MAX_WIDTH"),	# 0x827E
    ("",	I,	1,	"GL_MAX_HEIGHT"),	# 0x827F
    ("",	I,	1,	"GL_MAX_DEPTH"),	# 0x8280
    ("",	I,	1,	"GL_MAX_LAYERS"),	# 0x8281
    ("",	I,	1,	"GL_MAX_COMBINED_DIMENSIONS"),	# 0x8282
    ("",	B,	1,	"GL_COLOR_COMPONENTS"),	# 0x8283
    ("",	B,	1,	"GL_DEPTH_COMPONENTS"),	# 0x8284
    ("",	B,	1,	"GL_STENCIL_COMPONENTS"),	# 0x8285
    ("",	B,	1,	"GL_COLOR_RENDERABLE"),	# 0x8286
    ("",	B,	1,	"GL_DEPTH_RENDERABLE"),	# 0x8287
    ("",	B,	1,	"GL_STENCIL_RENDERABLE"),	# 0x8288
    ("",	E,	1,	"GL_FRAMEBUFFER_RENDERABLE"),	# 0x8289
    ("",	E,	1,	"GL_FRAMEBUFFER_RENDERABLE_LAYERED"),	# 0x828A
    ("",	E,	1,	"GL_FRAMEBUFFER_BLEND"),	# 0x828B
    ("",	E,	1,	"GL_READ_PIXELS"),	# 0x828C
    ("",	E,	1,	"GL_READ_PIXELS_FORMAT"),	# 0x828D
    ("",	E,	1,	"GL_READ_PIXELS_TYPE"),	# 0x828E
    ("",	E,	1,	"GL_TEXTURE_IMAGE_FORMAT"),	# 0x828F
    ("",	E,	1,	"GL_TEXTURE_IMAGE_TYPE"),	# 0x8290
    ("",	E,	1,	"GL_GET_TEXTURE_IMAGE_FORMAT"),	# 0x8291
    ("",	E,	1,	"GL_GET_TEXTURE_IMAGE_TYPE"),	# 0x8292
    ("",	B,	1,	"GL_MIPMAP"),	# 0x8293
    ("",	E,	1,	"GL_MANUAL_GENERATE_MIPMAP"),	# 0x8294
    ("",	E,	1,	"GL_AUTO_GENERATE_MIPMAP"),	# 0x8295
    ("",	E,	1,	"GL_COLOR_ENCODING"),	# 0x8296
    ("",	E,	1,	"GL_SRGB_READ"),	# 0x8297
    ("",	E,	1,	"GL_SRGB_WRITE"),	# 0x8298
    ("",	E,	1,	"GL_SRGB_DECODE_ARB"),	# 0x8299
    ("",	E,	1,	"GL_FILTER"),	# 0x829A
    ("",	E,	1,	"GL_VERTEX_TEXTURE"),	# 0x829B
    ("",	E,	1,	"GL_TESS_CONTROL_TEXTURE"),	# 0x829C
    ("",	E,	1,	"GL_TESS_EVALUATION_TEXTURE"),	# 0x829D
    ("",	E,	1,	"GL_GEOMETRY_TEXTURE"),	# 0x829E
    ("",	E,	1,	"GL_FRAGMENT_TEXTURE"),	# 0x829F
    ("",	E,	1,	"GL_COMPUTE_TEXTURE"),	# 0x82A0
    ("",	E,	1,	"GL_TEXTURE_SHADOW"),	# 0x82A1
    ("",	E,	1,	"GL_TEXTURE_GATHER"),	# 0x82A2
    ("",	E,	1,	"GL_TEXTURE_GATHER_SHADOW"),	# 0x82A3
    ("",	E,	1,	"GL_SHADER_IMAGE_LOAD"),	# 0x82A4
    ("",	E,	1,	"GL_SHADER_IMAGE_STORE"),	# 0x82A5
    ("",	E,	1,	"GL_SHADER_IMAGE_ATOMIC"),	# 0x82A6
    ("",	I,	1,	"GL_IMAGE_TEXEL_SIZE"),	# 0x82A7
    ("",	E,	1,	"GL_IMAGE_COMPATIBILITY_CLASS"),	# 0x82A8
    ("",	E,	1,	"GL_IMAGE_PIXEL_FORMAT"),	# 0x82A9
    ("",	E,	1,	"GL_IMAGE_PIXEL_TYPE"),	# 0x82AA
    ("",	E,	1,	"GL_SIMULTANEOUS_TEXTURE_AND_DEPTH_TEST"),	# 0x82AC
    ("",	E,	1,	"GL_SIMULTANEOUS_TEXTURE_AND_STENCIL_TEST"),	# 0x82AD
    ("",	E,	1,	"GL_SIMULTANEOUS_TEXTURE_AND_DEPTH_WRITE"),	# 0x82AE
    ("",	E,	1,	"GL_SIMULTANEOUS_TEXTURE_AND_STENCIL_WRITE"),	# 0x82AF
    ("",	I,	1,	"GL_TEXTURE_COMPRESSED_BLOCK_WIDTH"),	# 0x82B1
    ("",	I,	1,	"GL_TEXTURE_COMPRESSED_BLOCK_HEIGHT"),	# 0x82B2
    ("",	I,	1,	"GL_TEXTURE_COMPRESSED_BLOCK_SIZE"),	# 0x82B3
    ("",	E,	1,	"GL_CLEAR_BUFFER"),	# 0x82B4
    ("",	E,	1,	"GL_TEXTURE_VIEW"),	# 0x82B5
    ("",	E,	1,	"GL_VIEW_COMPATIBILITY_CLASS"),	# 0x82B6
    ("",	X,	1,	"GL_FULL_SUPPORT"),	# 0x82B7
    ("",	X,	1,	"GL_CAVEAT_SUPPORT"),	# 0x82B8
    ("",	X,	1,	"GL_IMAGE_CLASS_4_X_32"),	# 0x82B9
    ("",	X,	1,	"GL_IMAGE_CLASS_2_X_32"),	# 0x82BA
    ("",	X,	1,	"GL_IMAGE_CLASS_1_X_32"),	# 0x82BB
    ("",	X,	1,	"GL_IMAGE_CLASS_4_X_16"),	# 0x82BC
    ("",	X,	1,	"GL_IMAGE_CLASS_2_X_16"),	# 0x82BD
    ("",	X,	1,	"GL_IMAGE_CLASS_1_X_16"),	# 0x82BE
    ("",	X,	1,	"GL_IMAGE_CLASS_4_X_8"),	# 0x82BF
    ("",	X,	1,	"GL_IMAGE_CLASS_2_X_8"),	# 0x82C0
    ("",	X,	1,	"GL_IMAGE_CLASS_1_X_8"),	# 0x82C1
    ("",	X,	1,	"GL_IMAGE_CLASS_11_11_10"),	# 0x82C2
    ("",	X,	1,	"GL_IMAGE_CLASS_10_10_10_2"),	# 0x82C3
    ("",	X,	1,	"GL_VIEW_CLASS_128_BITS"),	# 0x82C4
    ("",	X,	1,	"GL_VIEW_CLASS_96_BITS"),	# 0x82C5
    ("",	X,	1,	"GL_VIEW_CLASS_64_BITS"),	# 0x82C6
    ("",	X,	1,	"GL_VIEW_CLASS_48_BITS"),	# 0x82C7
    ("",	X,	1,	"GL_VIEW_CLASS_32_BITS"),	# 0x82C8
    ("",	X,	1,	"GL_VIEW_CLASS_24_BITS"),	# 0x82C9
    ("",	X,	1,	"GL_VIEW_CLASS_16_BITS"),	# 0x82CA
    ("",	X,	1,	"GL_VIEW_CLASS_8_BITS"),	# 0x82CB
    ("",	X,	1,	"GL_VIEW_CLASS_S3TC_DXT1_RGB"),	# 0x82CC
    ("",	X,	1,	"GL_VIEW_CLASS_S3TC_DXT1_RGBA"),	# 0x82CD
    ("",	X,	1,	"GL_VIEW_CLASS_S3TC_DXT3_RGBA"),	# 0x82CE
    ("",	X,	1,	"GL_VIEW_CLASS_S3TC_DXT5_RGBA"),	# 0x82CF
    ("",	X,	1,	"GL_VIEW_CLASS_RGTC1_RED"),	# 0x82D0
    ("",	X,	1,	"GL_VIEW_CLASS_RGTC2_RG"),	# 0x82D1
    ("",	X,	1,	"GL_VIEW_CLASS_BPTC_UNORM"),	# 0x82D2
    ("",	X,	1,	"GL_VIEW_CLASS_BPTC_FLOAT"),	# 0x82D3
    ("glGetVertexAttrib",	I,	1,	"GL_VERTEX_ATTRIB_BINDING"),	# 0x82D4
    ("glGetVertexAttrib",	I,	1,	"GL_VERTEX_ATTRIB_RELATIVE_OFFSET"),	# 0x82D5
    ("",	I,	1,	"GL_VERTEX_BINDING_DIVISOR"),	# 0x82D6
    ("",	I,	1,	"GL_VERTEX_BINDING_OFFSET"),	# 0x82D7
    ("",	I,	1,	"GL_VERTEX_BINDING_STRIDE"),	# 0x82D8
    ("glGet",	I,	1,	"GL_MAX_VERTEX_ATTRIB_RELATIVE_OFFSET"),	# 0x82D9
    ("glGet",	I,	1,	"GL_MAX_VERTEX_ATTRIB_BINDINGS"),	# 0x82DA
    ("glGetTexParameter",	I,	1,	"GL_TEXTURE_VIEW_MIN_LEVEL"),	# 0x82DB
    ("glGetTexParameter",	I,	1,	"GL_TEXTURE_VIEW_NUM_LEVELS"),	# 0x82DC
    ("glGetTexParameter",	I,	1,	"GL_TEXTURE_VIEW_MIN_LAYER"),	# 0x82DD
    ("glGetTexParameter",	I,	1,	"GL_TEXTURE_VIEW_NUM_LAYERS"),	# 0x82DE
    ("glGetTexParameter",	I,	1,	"GL_TEXTURE_IMMUTABLE_LEVELS"),	# 0x82DF
    ("",	X,	1,	"GL_BUFFER"),	# 0x82E0
    ("",	X,	1,	"GL_SHADER"),	# 0x82E1
    ("",	X,	1,	"GL_PROGRAM"),	# 0x82E2
    ("",	X,	1,	"GL_QUERY"),	# 0x82E3
    ("",	X,	1,	"GL_PROGRAM_PIPELINE"),	# 0x82E4
    ("",	X,	1,	"GL_MAX_VERTEX_ATTRIB_STRIDE"),	# 0x82E5
    ("",	X,	1,	"GL_SAMPLER"),	# 0x82E6
    ("",	X,	1,	"GL_DISPLAY_LIST"),	# 0x82E7
    ("glGet",	I,	1,	"GL_MAX_LABEL_LENGTH"),	# 0x82E8
    ("",	X,	1,	"GL_NUM_SHADING_LANGUAGE_VERSIONS"),	# 0x82E9
    ("",	X,	1,	"GL_QUERY_TARGET"),	# 0x82EA
    #("",	X,	1,	"GL_TEXTURE_BINDING"),	# 0x82EB
    ("",	X,	1,	"GL_TRANSFORM_FEEDBACK_OVERFLOW_ARB"),	# 0x82EC
    ("",	X,	1,	"GL_TRANSFORM_FEEDBACK_STREAM_OVERFLOW_ARB"),	# 0x82ED
    ("",	X,	1,	"GL_VERTICES_SUBMITTED_ARB"),	# 0x82EE
    ("",	X,	1,	"GL_PRIMITIVES_SUBMITTED_ARB"),	# 0x82EF
    ("",	X,	1,	"GL_VERTEX_SHADER_INVOCATIONS_ARB"),	# 0x82F0
    ("",	X,	1,	"GL_TESS_CONTROL_SHADER_PATCHES_ARB"),	# 0x82F1
    ("",	X,	1,	"GL_TESS_EVALUATION_SHADER_INVOCATIONS_ARB"),	# 0x82F2
    ("",	X,	1,	"GL_GEOMETRY_SHADER_PRIMITIVES_EMITTED_ARB"),	# 0x82F3
    ("",	X,	1,	"GL_FRAGMENT_SHADER_INVOCATIONS_ARB"),	# 0x82F4
    ("",	X,	1,	"GL_COMPUTE_SHADER_INVOCATIONS_ARB"),	# 0x82F5
    ("",	X,	1,	"GL_CLIPPING_INPUT_PRIMITIVES_ARB"),	# 0x82F6
    ("",	X,	1,	"GL_CLIPPING_OUTPUT_PRIMITIVES_ARB"),	# 0x82F7
    ("",	X,	1,	"GL_SPARSE_BUFFER_PAGE_SIZE_ARB"),	# 0x82F8
    ("glGet",	I,	1,	"GL_MAX_CULL_DISTANCES"),	# 0x82F9
    ("glGet",	I,	1,	"GL_MAX_COMBINED_CLIP_AND_CULL_DISTANCES"),	# 0x82FA
    ("glGet",	E,	1,	"GL_CONTEXT_RELEASE_BEHAVIOR"),	# 0x82FB
    ("glGet",	E,	1,	"GL_CONTEXT_RELEASE_BEHAVIOR_FLUSH"),	# 0x82FC
    ("",	X,	1,	"GL_DEPTH_PASS_INSTRUMENT_SGIX"),	# 0x8310
    ("",	X,	1,	"GL_DEPTH_PASS_INSTRUMENT_COUNTERS_SGIX"),	# 0x8311
    ("",	X,	1,	"GL_DEPTH_PASS_INSTRUMENT_MAX_SGIX"),	# 0x8312
    #("",	X,	1,	"GL_FRAGMENTS_INSTRUMENT_SGIX"),	# 0x8313
    #("",	X,	1,	"GL_FRAGMENTS_INSTRUMENT_COUNTERS_SGIX"),	# 0x8314
    #("",	X,	1,	"GL_FRAGMENTS_INSTRUMENT_MAX_SGIX"),	# 0x8315
    ("glGet",	I,	1,	"GL_CONVOLUTION_HINT_SGIX"),	# 0x8316
    ("",	X,	1,	"GL_YCRCB_SGIX"),	# 0x8318
    ("",	X,	1,	"GL_YCRCBA_SGIX"),	# 0x8319
    #("",	X,	1,	"GL_UNPACK_COMPRESSED_SIZE_SGIX"),	# 0x831A
    #("",	X,	1,	"GL_PACK_MAX_COMPRESSED_SIZE_SGIX"),	# 0x831B
    #("",	X,	1,	"GL_PACK_COMPRESSED_SIZE_SGIX"),	# 0x831C
    #("",	X,	1,	"GL_SLIM8U_SGIX"),	# 0x831D
    #("",	X,	1,	"GL_SLIM10U_SGIX"),	# 0x831E
    #("",	X,	1,	"GL_SLIM12S_SGIX"),	# 0x831F
    ("",	X,	1,	"GL_ALPHA_MIN_SGIX"),	# 0x8320
    ("",	X,	1,	"GL_ALPHA_MAX_SGIX"),	# 0x8321
    ("",	X,	1,	"GL_SCALEBIAS_HINT_SGIX"),	# 0x8322
    ("",	X,	1,	"GL_ASYNC_MARKER_SGIX"),	# 0x8329
    ("glGet",	I,	1,	"GL_PIXEL_TEX_GEN_MODE_SGIX"),	# 0x832B
    ("",	X,	1,	"GL_ASYNC_HISTOGRAM_SGIX"),	# 0x832C
    ("",	X,	1,	"GL_MAX_ASYNC_HISTOGRAM_SGIX"),	# 0x832D
    ("",	X,	1,	"GL_PIXEL_TRANSFORM_2D_EXT"),	# 0x8330
    ("",	X,	1,	"GL_PIXEL_MAG_FILTER_EXT"),	# 0x8331
    ("",	X,	1,	"GL_PIXEL_MIN_FILTER_EXT"),	# 0x8332
    ("",	X,	1,	"GL_PIXEL_CUBIC_WEIGHT_EXT"),	# 0x8333
    ("",	X,	1,	"GL_CUBIC_EXT"),	# 0x8334
    ("",	X,	1,	"GL_AVERAGE_EXT"),	# 0x8335
    ("",	X,	1,	"GL_PIXEL_TRANSFORM_2D_STACK_DEPTH_EXT"),	# 0x8336
    ("",	X,	1,	"GL_MAX_PIXEL_TRANSFORM_2D_STACK_DEPTH_EXT"),	# 0x8337
    ("",	X,	1,	"GL_PIXEL_TRANSFORM_2D_MATRIX_EXT"),	# 0x8338
    ("",	X,	1,	"GL_FRAGMENT_MATERIAL_EXT"),	# 0x8349
    ("",	X,	1,	"GL_FRAGMENT_NORMAL_EXT"),	# 0x834A
    ("",	X,	1,	"GL_FRAGMENT_COLOR_EXT"),	# 0x834C
    ("",	X,	1,	"GL_ATTENUATION_EXT"),	# 0x834D
    ("",	X,	1,	"GL_SHADOW_ATTENUATION_EXT"),	# 0x834E
    ("glGet",	I,	1,	"GL_TEXTURE_APPLICATION_MODE_EXT"),	# 0x834F
    ("glGet",	I,	1,	"GL_TEXTURE_LIGHT_EXT"),	# 0x8350
    ("glGet",	I,	1,	"GL_TEXTURE_MATERIAL_FACE_EXT"),	# 0x8351
    ("glGet",	I,	1,	"GL_TEXTURE_MATERIAL_PARAMETER_EXT"),	# 0x8352
    ("glGet",	I,	1,	"GL_PIXEL_TEXTURE_SGIS"),	# 0x8353
    ("glGet",	I,	1,	"GL_PIXEL_FRAGMENT_RGB_SOURCE_SGIS"),	# 0x8354
    ("glGet",	I,	1,	"GL_PIXEL_FRAGMENT_ALPHA_SOURCE_SGIS"),	# 0x8355
    ("glGet",	I,	1,	"GL_PIXEL_GROUP_COLOR_SGIS"),	# 0x8356
    #("",	X,	1,	"GL_LINE_QUALITY_HINT_SGIX"),	# 0x835B
    ("",	X,	1,	"GL_ASYNC_TEX_IMAGE_SGIX"),	# 0x835C
    ("",	X,	1,	"GL_ASYNC_DRAW_PIXELS_SGIX"),	# 0x835D
    ("",	X,	1,	"GL_ASYNC_READ_PIXELS_SGIX"),	# 0x835E
    ("",	X,	1,	"GL_MAX_ASYNC_TEX_IMAGE_SGIX"),	# 0x835F
    ("",	X,	1,	"GL_MAX_ASYNC_DRAW_PIXELS_SGIX"),	# 0x8360
    ("",	X,	1,	"GL_MAX_ASYNC_READ_PIXELS_SGIX"),	# 0x8361
    ("",	X,	1,	"GL_UNSIGNED_BYTE_2_3_3_REV"),	# 0x8362
    ("",	X,	1,	"GL_UNSIGNED_SHORT_5_6_5"),	# 0x8363
    ("",	X,	1,	"GL_UNSIGNED_SHORT_5_6_5_REV"),	# 0x8364
    ("",	X,	1,	"GL_UNSIGNED_SHORT_4_4_4_4_REV"),	# 0x8365
    ("",	X,	1,	"GL_UNSIGNED_SHORT_1_5_5_5_REV"),	# 0x8366
    ("",	X,	1,	"GL_UNSIGNED_INT_8_8_8_8_REV"),	# 0x8367
    ("",	X,	1,	"GL_UNSIGNED_INT_2_10_10_10_REV"),	# 0x8368
    ("glGetTexParameter",	F,	1,	"GL_TEXTURE_MAX_CLAMP_S_SGIX"),	# 0x8369
    ("glGetTexParameter",	F,	1,	"GL_TEXTURE_MAX_CLAMP_T_SGIX"),	# 0x836A
    ("glGetTexParameter",	F,	1,	"GL_TEXTURE_MAX_CLAMP_R_SGIX"),	# 0x836B
    ("",	X,	1,	"GL_MIRRORED_REPEAT"),	# 0x8370
    ("",	X,	1,	"GL_RGB_S3TC"),	# 0x83A0
    ("",	X,	1,	"GL_RGB4_S3TC"),	# 0x83A1
    ("",	X,	1,	"GL_RGBA_S3TC"),	# 0x83A2
    ("",	X,	1,	"GL_RGBA4_S3TC"),	# 0x83A3
    ("",	X,	1,	"GL_RGBA_DXT5_S3TC"),	# 0x83A4
    ("",	X,	1,	"GL_RGBA4_DXT5_S3TC"),	# 0x83A5
    ("",	X,	1,	"GL_VERTEX_PRECLIP_SGIX"),	# 0x83EE
    ("",	X,	1,	"GL_VERTEX_PRECLIP_HINT_SGIX"),	# 0x83EF
    ("",	X,	1,	"GL_COMPRESSED_RGB_S3TC_DXT1_EXT"),	# 0x83F0
    ("",	X,	1,	"GL_COMPRESSED_RGBA_S3TC_DXT1_EXT"),	# 0x83F1
    ("",	X,	1,	"GL_COMPRESSED_RGBA_S3TC_DXT3_EXT"),	# 0x83F2
    ("",	X,	1,	"GL_COMPRESSED_RGBA_S3TC_DXT5_EXT"),	# 0x83F3
    ("",	X,	1,	"GL_PARALLEL_ARRAYS_INTEL"),	# 0x83F4
    ("",	X,	1,	"GL_VERTEX_ARRAY_PARALLEL_POINTERS_INTEL"),	# 0x83F5
    ("",	X,	1,	"GL_NORMAL_ARRAY_PARALLEL_POINTERS_INTEL"),	# 0x83F6
    ("",	X,	1,	"GL_COLOR_ARRAY_PARALLEL_POINTERS_INTEL"),	# 0x83F7
    ("",	X,	1,	"GL_TEXTURE_COORD_ARRAY_PARALLEL_POINTERS_INTEL"),	# 0x83F8
    ("",	X,	1,	"GL_PERFQUERY_DONOT_FLUSH_INTEL"),	# 0x83F9
    ("",	X,	1,	"GL_PERFQUERY_FLUSH_INTEL"),	# 0x83FA
    ("",	X,	1,	"GL_PERFQUERY_WAIT_INTEL"),	# 0x83FB
    ("glGetTexParameter",	E,	1,	"GL_TEXTURE_MEMORY_LAYOUT_INTEL"),	# 0x83FF
    ("glGet",	I,	1,	"GL_FRAGMENT_LIGHTING_SGIX"),	# 0x8400
    ("glGet",	I,	1,	"GL_FRAGMENT_COLOR_MATERIAL_SGIX"),	# 0x8401
    ("glGet",	I,	1,	"GL_FRAGMENT_COLOR_MATERIAL_FACE_SGIX"),	# 0x8402
    ("glGet",	I,	1,	"GL_FRAGMENT_COLOR_MATERIAL_PARAMETER_SGIX"),	# 0x8403
    ("glGet",	I,	1,	"GL_MAX_FRAGMENT_LIGHTS_SGIX"),	# 0x8404
    ("glGet",	I,	1,	"GL_MAX_ACTIVE_LIGHTS_SGIX"),	# 0x8405
    ("glGet",	I,	1,	"GL_CURRENT_RASTER_NORMAL_SGIX"),	# 0x8406
    ("glGet",	I,	1,	"GL_LIGHT_ENV_MODE_SGIX"),	# 0x8407
    ("glGet",	I,	1,	"GL_FRAGMENT_LIGHT_MODEL_LOCAL_VIEWER_SGIX"),	# 0x8408
    ("glGet",	I,	1,	"GL_FRAGMENT_LIGHT_MODEL_TWO_SIDE_SGIX"),	# 0x8409
    ("glGet",	F,	4,	"GL_FRAGMENT_LIGHT_MODEL_AMBIENT_SGIX"),	# 0x840A
    ("glGet",	I,	1,	"GL_FRAGMENT_LIGHT_MODEL_NORMAL_INTERPOLATION_SGIX"),	# 0x840B
    ("glGet",	I,	1,	"GL_FRAGMENT_LIGHT0_SGIX"),	# 0x840C
    ("",	X,	1,	"GL_FRAGMENT_LIGHT1_SGIX"),	# 0x840D
    ("",	X,	1,	"GL_FRAGMENT_LIGHT2_SGIX"),	# 0x840E
    ("",	X,	1,	"GL_FRAGMENT_LIGHT3_SGIX"),	# 0x840F
    ("",	X,	1,	"GL_FRAGMENT_LIGHT4_SGIX"),	# 0x8410
    ("",	X,	1,	"GL_FRAGMENT_LIGHT5_SGIX"),	# 0x8411
    ("",	X,	1,	"GL_FRAGMENT_LIGHT6_SGIX"),	# 0x8412
    ("",	X,	1,	"GL_FRAGMENT_LIGHT7_SGIX"),	# 0x8413
    ("",	X,	1,	"GL_PACK_RESAMPLE_SGIX"),	# 0x842C
    ("",	X,	1,	"GL_UNPACK_RESAMPLE_SGIX"),	# 0x842D
    ("",	X,	1,	"GL_RESAMPLE_REPLICATE_SGIX"),	# 0x842E
    ("",	X,	1,	"GL_RESAMPLE_ZERO_FILL_SGIX"),	# 0x842F
    ("",	X,	1,	"GL_RESAMPLE_DECIMATE_SGIX"),	# 0x8430
    ("",	X,	1,	"GL_TANGENT_ARRAY_EXT"),	# 0x8439
    ("",	X,	1,	"GL_BINORMAL_ARRAY_EXT"),	# 0x843A
    ("",	X,	1,	"GL_CURRENT_TANGENT_EXT"),	# 0x843B
    ("",	X,	1,	"GL_CURRENT_BINORMAL_EXT"),	# 0x843C
    ("glGet",	E,	1,	"GL_TANGENT_ARRAY_TYPE_EXT"),	# 0x843E
    ("",	X,	1,	"GL_TANGENT_ARRAY_STRIDE_EXT"),	# 0x843F
    ("glGet",	E,	1,	"GL_BINORMAL_ARRAY_TYPE_EXT"),	# 0x8440
    ("",	X,	1,	"GL_BINORMAL_ARRAY_STRIDE_EXT"),	# 0x8441
    ("glGet",	P,	1,	"GL_TANGENT_ARRAY_POINTER_EXT"),	# 0x8442
    ("glGet",	P,	1,	"GL_BINORMAL_ARRAY_POINTER_EXT"),	# 0x8443
    ("",	X,	1,	"GL_MAP1_TANGENT_EXT"),	# 0x8444
    ("",	X,	1,	"GL_MAP2_TANGENT_EXT"),	# 0x8445
    ("",	X,	1,	"GL_MAP1_BINORMAL_EXT"),	# 0x8446
    ("",	X,	1,	"GL_MAP2_BINORMAL_EXT"),	# 0x8447
    ("",	X,	1,	"GL_NEAREST_CLIPMAP_NEAREST_SGIX"),	# 0x844D
    ("",	X,	1,	"GL_NEAREST_CLIPMAP_LINEAR_SGIX"),	# 0x844E
    ("",	X,	1,	"GL_LINEAR_CLIPMAP_NEAREST_SGIX"),	# 0x844F
    ("glGet",	E,	1,	"GL_FOG_COORD_SRC"),	# 0x8450
    ("",	X,	1,	"GL_FOG_COORD"),	# 0x8451
    ("",	X,	1,	"GL_FRAGMENT_DEPTH"),	# 0x8452
    ("glGet",	F,	1,	"GL_CURRENT_FOG_COORD"),	# 0x8453
    ("glGet",	E,	1,	"GL_FOG_COORD_ARRAY_TYPE"),	# 0x8454
    ("glGet",	I,	1,	"GL_FOG_COORD_ARRAY_STRIDE"),	# 0x8455
    ("",	X,	1,	"GL_FOG_COORD_ARRAY_POINTER"),	# 0x8456
    ("glGet",	B,	1,	"GL_FOG_COORD_ARRAY"),	# 0x8457
    ("glGet",	B,	1,	"GL_COLOR_SUM"),	# 0x8458
    ("glGet",	F,	4,	"GL_CURRENT_SECONDARY_COLOR"),	# 0x8459
    ("glGet",	I,	1,	"GL_SECONDARY_COLOR_ARRAY_SIZE"),	# 0x845A
    ("glGet",	E,	1,	"GL_SECONDARY_COLOR_ARRAY_TYPE"),	# 0x845B
    ("glGet",	I,	1,	"GL_SECONDARY_COLOR_ARRAY_STRIDE"),	# 0x845C
    ("",	X,	1,	"GL_SECONDARY_COLOR_ARRAY_POINTER"),	# 0x845D
    ("glGet",	B,	1,	"GL_SECONDARY_COLOR_ARRAY"),	# 0x845E
    ("",	X,	1,	"GL_CURRENT_RASTER_SECONDARY_COLOR"),	# 0x845F
    ("glGet",	F,	2,	"GL_ALIASED_POINT_SIZE_RANGE"),	# 0x846D
    ("glGet",	F,	2,	"GL_ALIASED_LINE_WIDTH_RANGE"),	# 0x846E
    ("",	X,	1,	"GL_SCREEN_COORDINATES_REND"),	# 0x8490
    ("",	X,	1,	"GL_INVERTED_SCREEN_W_REND"),	# 0x8491
    ("",	X,	1,	"GL_TEXTURE0"),	# 0x84C0
    ("",	X,	1,	"GL_TEXTURE1"),	# 0x84C1
    ("",	X,	1,	"GL_TEXTURE2"),	# 0x84C2
    ("",	X,	1,	"GL_TEXTURE3"),	# 0x84C3
    ("",	X,	1,	"GL_TEXTURE4"),	# 0x84C4
    ("",	X,	1,	"GL_TEXTURE5"),	# 0x84C5
    ("",	X,	1,	"GL_TEXTURE6"),	# 0x84C6
    ("",	X,	1,	"GL_TEXTURE7"),	# 0x84C7
    ("",	X,	1,	"GL_TEXTURE8"),	# 0x84C8
    ("",	X,	1,	"GL_TEXTURE9"),	# 0x84C9
    ("",	X,	1,	"GL_TEXTURE10"),	# 0x84CA
    ("",	X,	1,	"GL_TEXTURE11"),	# 0x84CB
    ("",	X,	1,	"GL_TEXTURE12"),	# 0x84CC
    ("",	X,	1,	"GL_TEXTURE13"),	# 0x84CD
    ("",	X,	1,	"GL_TEXTURE14"),	# 0x84CE
    ("",	X,	1,	"GL_TEXTURE15"),	# 0x84CF
    ("",	X,	1,	"GL_TEXTURE16"),	# 0x84D0
    ("",	X,	1,	"GL_TEXTURE17"),	# 0x84D1
    ("",	X,	1,	"GL_TEXTURE18"),	# 0x84D2
    ("",	X,	1,	"GL_TEXTURE19"),	# 0x84D3
    ("",	X,	1,	"GL_TEXTURE20"),	# 0x84D4
    ("",	X,	1,	"GL_TEXTURE21"),	# 0x84D5
    ("",	X,	1,	"GL_TEXTURE22"),	# 0x84D6
    ("",	X,	1,	"GL_TEXTURE23"),	# 0x84D7
    ("",	X,	1,	"GL_TEXTURE24"),	# 0x84D8
    ("",	X,	1,	"GL_TEXTURE25"),	# 0x84D9
    ("",	X,	1,	"GL_TEXTURE26"),	# 0x84DA
    ("",	X,	1,	"GL_TEXTURE27"),	# 0x84DB
    ("",	X,	1,	"GL_TEXTURE28"),	# 0x84DC
    ("",	X,	1,	"GL_TEXTURE29"),	# 0x84DD
    ("",	X,	1,	"GL_TEXTURE30"),	# 0x84DE
    ("",	X,	1,	"GL_TEXTURE31"),	# 0x84DF
    ("glGet",	E,	1,	"GL_ACTIVE_TEXTURE"),	# 0x84E0
    ("glGet",	E,	1,	"GL_CLIENT_ACTIVE_TEXTURE"),	# 0x84E1
    ("glGet",	I,	1,	"GL_MAX_TEXTURE_UNITS"),	# 0x84E2
    ("glGet",	F,	16,	"GL_TRANSPOSE_MODELVIEW_MATRIX"),	# 0x84E3
    ("glGet",	F,	16,	"GL_TRANSPOSE_PROJECTION_MATRIX"),	# 0x84E4
    ("glGet",	F,	16,	"GL_TRANSPOSE_TEXTURE_MATRIX"),	# 0x84E5
    ("glGet",	F,	16,	"GL_TRANSPOSE_COLOR_MATRIX"),	# 0x84E6
    ("",	X,	1,	"GL_SUBTRACT"),	# 0x84E7
    ("glGet",	I,	1,	"GL_MAX_RENDERBUFFER_SIZE"),	# 0x84E8
    ("",	X,	1,	"GL_COMPRESSED_ALPHA"),	# 0x84E9
    ("",	X,	1,	"GL_COMPRESSED_LUMINANCE"),	# 0x84EA
    ("",	X,	1,	"GL_COMPRESSED_LUMINANCE_ALPHA"),	# 0x84EB
    ("",	X,	1,	"GL_COMPRESSED_INTENSITY"),	# 0x84EC
    ("",	X,	1,	"GL_COMPRESSED_RGB"),	# 0x84ED
    ("",	X,	1,	"GL_COMPRESSED_RGBA"),	# 0x84EE
    ("glGet",	E,	1,	"GL_TEXTURE_COMPRESSION_HINT"),	# 0x84EF
    ("glGetActiveUniformBlock",	B,	1,	"GL_UNIFORM_BLOCK_REFERENCED_BY_TESS_CONTROL_SHADER"),	# 0x84F0
    ("glGetActiveUniformBlock",	B,	1,	"GL_UNIFORM_BLOCK_REFERENCED_BY_TESS_EVALUATION_SHADER"),	# 0x84F1
    ("",	X,	1,	"GL_ALL_COMPLETED_NV"),	# 0x84F2
    ("",	X,	1,	"GL_FENCE_STATUS_NV"),	# 0x84F3
    ("",	X,	1,	"GL_FENCE_CONDITION_NV"),	# 0x84F4
    ("",	B,	1,	"GL_TEXTURE_RECTANGLE"),	# 0x84F5
    ("glGet_texture",	I,	1,	"GL_TEXTURE_BINDING_RECTANGLE"),	# 0x84F6
    ("",	X,	1,	"GL_PROXY_TEXTURE_RECTANGLE"),	# 0x84F7
    ("glGet",	I,	1,	"GL_MAX_RECTANGLE_TEXTURE_SIZE"),	# 0x84F8
    ("",	X,	1,	"GL_DEPTH_STENCIL"),	# 0x84F9
    ("",	X,	1,	"GL_UNSIGNED_INT_24_8"),	# 0x84FA
    ("glGet",	F,	1,	"GL_MAX_TEXTURE_LOD_BIAS"),	# 0x84FD
    ("glGetTexParameter,glGetSamplerParameter",	F,	1,	"GL_TEXTURE_MAX_ANISOTROPY_EXT"),	# 0x84FE
    ("glGet",	F,	1,	"GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT"),	# 0x84FF
    ("",	X,	1,	"GL_TEXTURE_FILTER_CONTROL"),	# 0x8500
    ("glGetTexParameter,glGetSamplerParameter,glGetTexEnv",	F,	1,	"GL_TEXTURE_LOD_BIAS"),	# 0x8501
    ("",	X,	1,	"GL_MODELVIEW1_STACK_DEPTH_EXT"),	# 0x8502
    ("",	X,	1,	"GL_COMBINE4_NV"),	# 0x8503
    ("glGet",	F,	1,	"GL_MAX_SHININESS_NV"),	# 0x8504
    ("glGet",	F,	1,	"GL_MAX_SPOT_EXPONENT_NV"),	# 0x8505
    ("",	X,	1,	"GL_MODELVIEW1_MATRIX_EXT"),	# 0x8506
    ("",	X,	1,	"GL_INCR_WRAP"),	# 0x8507
    ("",	X,	1,	"GL_DECR_WRAP"),	# 0x8508
    ("",	X,	1,	"GL_VERTEX_WEIGHTING_EXT"),	# 0x8509
    ("",	X,	1,	"GL_MODELVIEW1_ARB"),	# 0x850A
    ("",	X,	1,	"GL_CURRENT_VERTEX_WEIGHT_EXT"),	# 0x850B
    ("",	X,	1,	"GL_VERTEX_WEIGHT_ARRAY_EXT"),	# 0x850C
    ("",	X,	1,	"GL_VERTEX_WEIGHT_ARRAY_SIZE_EXT"),	# 0x850D
    ("glGet",	E,	1,	"GL_VERTEX_WEIGHT_ARRAY_TYPE_EXT"),	# 0x850E
    ("",	X,	1,	"GL_VERTEX_WEIGHT_ARRAY_STRIDE_EXT"),	# 0x850F
    ("",	X,	1,	"GL_VERTEX_WEIGHT_ARRAY_POINTER_EXT"),	# 0x8510
    ("",	X,	1,	"GL_NORMAL_MAP"),	# 0x8511
    ("",	X,	1,	"GL_REFLECTION_MAP"),	# 0x8512
    ("",	B,	1,	"GL_TEXTURE_CUBE_MAP"),	# 0x8513
    ("glGet_texture",	I,	1,	"GL_TEXTURE_BINDING_CUBE_MAP"),	# 0x8514
    ("",	X,	1,	"GL_TEXTURE_CUBE_MAP_POSITIVE_X"),	# 0x8515
    ("",	X,	1,	"GL_TEXTURE_CUBE_MAP_NEGATIVE_X"),	# 0x8516
    ("",	X,	1,	"GL_TEXTURE_CUBE_MAP_POSITIVE_Y"),	# 0x8517
    ("",	X,	1,	"GL_TEXTURE_CUBE_MAP_NEGATIVE_Y"),	# 0x8518
    ("",	X,	1,	"GL_TEXTURE_CUBE_MAP_POSITIVE_Z"),	# 0x8519
    ("",	X,	1,	"GL_TEXTURE_CUBE_MAP_NEGATIVE_Z"),	# 0x851A
    ("",	X,	1,	"GL_PROXY_TEXTURE_CUBE_MAP"),	# 0x851B
    ("glGet",	I,	1,	"GL_MAX_CUBE_MAP_TEXTURE_SIZE"),	# 0x851C
    ("_glGet",	B,	1,	"GL_VERTEX_ARRAY_RANGE_NV"),	# 0x851D
    ("_glGet",	I,	1,	"GL_VERTEX_ARRAY_RANGE_LENGTH_NV"),	# 0x851E
    ("_glGet",	B,	1,	"GL_VERTEX_ARRAY_RANGE_VALID_NV"),	# 0x851F
    ("_glGet",	I,	1,	"GL_MAX_VERTEX_ARRAY_RANGE_ELEMENT_NV"),	# 0x8520
    ("_glGet",	P,	1,	"GL_VERTEX_ARRAY_RANGE_POINTER_NV"),	# 0x8521
    ("",	X,	1,	"GL_REGISTER_COMBINERS_NV"),	# 0x8522
    ("",	X,	1,	"GL_VARIABLE_A_NV"),	# 0x8523
    ("",	X,	1,	"GL_VARIABLE_B_NV"),	# 0x8524
    ("",	X,	1,	"GL_VARIABLE_C_NV"),	# 0x8525
    ("",	X,	1,	"GL_VARIABLE_D_NV"),	# 0x8526
    ("",	X,	1,	"GL_VARIABLE_E_NV"),	# 0x8527
    ("",	X,	1,	"GL_VARIABLE_F_NV"),	# 0x8528
    ("",	X,	1,	"GL_VARIABLE_G_NV"),	# 0x8529
    ("glGet",	F,	4,	"GL_CONSTANT_COLOR0_NV"),	# 0x852A
    ("glGet",	F,	4,	"GL_CONSTANT_COLOR1_NV"),	# 0x852B
    ("",	X,	1,	"GL_PRIMARY_COLOR_NV"),	# 0x852C
    ("",	X,	1,	"GL_SECONDARY_COLOR_NV"),	# 0x852D
    ("",	X,	1,	"GL_SPARE0_NV"),	# 0x852E
    ("",	X,	1,	"GL_SPARE1_NV"),	# 0x852F
    ("",	X,	1,	"GL_DISCARD_NV"),	# 0x8530
    ("",	X,	1,	"GL_E_TIMES_F_NV"),	# 0x8531
    ("",	X,	1,	"GL_SPARE0_PLUS_SECONDARY_COLOR_NV"),	# 0x8532
    ("",	X,	1,	"GL_VERTEX_ARRAY_RANGE_WITHOUT_FLUSH_NV"),	# 0x8533
    ("glGet",	E,	1,	"GL_MULTISAMPLE_FILTER_HINT_NV"),	# 0x8534
    ("",	X,	1,	"GL_PER_STAGE_CONSTANTS_NV"),	# 0x8535
    ("",	X,	1,	"GL_UNSIGNED_IDENTITY_NV"),	# 0x8536
    ("",	X,	1,	"GL_UNSIGNED_INVERT_NV"),	# 0x8537
    ("",	X,	1,	"GL_EXPAND_NORMAL_NV"),	# 0x8538
    ("",	X,	1,	"GL_EXPAND_NEGATE_NV"),	# 0x8539
    ("",	X,	1,	"GL_HALF_BIAS_NORMAL_NV"),	# 0x853A
    ("",	X,	1,	"GL_HALF_BIAS_NEGATE_NV"),	# 0x853B
    ("",	X,	1,	"GL_SIGNED_IDENTITY_NV"),	# 0x853C
    ("",	X,	1,	"GL_SIGNED_NEGATE_NV"),	# 0x853D
    ("",	X,	1,	"GL_SCALE_BY_TWO_NV"),	# 0x853E
    ("",	X,	1,	"GL_SCALE_BY_FOUR_NV"),	# 0x853F
    ("",	X,	1,	"GL_SCALE_BY_ONE_HALF_NV"),	# 0x8540
    ("",	X,	1,	"GL_BIAS_BY_NEGATIVE_ONE_HALF_NV"),	# 0x8541
    ("",	X,	1,	"GL_COMBINER_INPUT_NV"),	# 0x8542
    ("",	X,	1,	"GL_COMBINER_MAPPING_NV"),	# 0x8543
    ("",	X,	1,	"GL_COMBINER_COMPONENT_USAGE_NV"),	# 0x8544
    ("",	X,	1,	"GL_COMBINER_AB_DOT_PRODUCT_NV"),	# 0x8545
    ("",	X,	1,	"GL_COMBINER_CD_DOT_PRODUCT_NV"),	# 0x8546
    ("",	X,	1,	"GL_COMBINER_MUX_SUM_NV"),	# 0x8547
    ("",	X,	1,	"GL_COMBINER_SCALE_NV"),	# 0x8548
    ("",	X,	1,	"GL_COMBINER_BIAS_NV"),	# 0x8549
    ("",	X,	1,	"GL_COMBINER_AB_OUTPUT_NV"),	# 0x854A
    ("",	X,	1,	"GL_COMBINER_CD_OUTPUT_NV"),	# 0x854B
    ("",	X,	1,	"GL_COMBINER_SUM_OUTPUT_NV"),	# 0x854C
    ("glGet",	I,	1,	"GL_MAX_GENERAL_COMBINERS_NV"),	# 0x854D
    ("glGet",	I,	1,	"GL_NUM_GENERAL_COMBINERS_NV"),	# 0x854E
    ("glGet",	B,	1,	"GL_COLOR_SUM_CLAMP_NV"),	# 0x854F
    ("",	X,	1,	"GL_COMBINER0_NV"),	# 0x8550
    ("",	X,	1,	"GL_COMBINER1_NV"),	# 0x8551
    ("",	X,	1,	"GL_COMBINER2_NV"),	# 0x8552
    ("",	X,	1,	"GL_COMBINER3_NV"),	# 0x8553
    ("",	X,	1,	"GL_COMBINER4_NV"),	# 0x8554
    ("",	X,	1,	"GL_COMBINER5_NV"),	# 0x8555
    ("",	X,	1,	"GL_COMBINER6_NV"),	# 0x8556
    ("",	X,	1,	"GL_COMBINER7_NV"),	# 0x8557
    ("glGet",	B,	1,	"GL_PRIMITIVE_RESTART_NV"),	# 0x8558
    ("glGet",	I,	1,	"GL_PRIMITIVE_RESTART_INDEX_NV"),	# 0x8559
    ("glGet",	E,	1,	"GL_FOG_DISTANCE_MODE_NV"),	# 0x855A
    ("",	X,	1,	"GL_EYE_RADIAL_NV"),	# 0x855B
    ("",	X,	1,	"GL_EYE_PLANE_ABSOLUTE_NV"),	# 0x855C
    ("",	X,	1,	"GL_EMBOSS_LIGHT_NV"),	# 0x855D
    ("",	X,	1,	"GL_EMBOSS_CONSTANT_NV"),	# 0x855E
    ("",	X,	1,	"GL_EMBOSS_MAP_NV"),	# 0x855F
    ("",	X,	1,	"GL_RED_MIN_CLAMP_INGR"),	# 0x8560
    ("",	X,	1,	"GL_GREEN_MIN_CLAMP_INGR"),	# 0x8561
    ("",	X,	1,	"GL_BLUE_MIN_CLAMP_INGR"),	# 0x8562
    ("",	X,	1,	"GL_ALPHA_MIN_CLAMP_INGR"),	# 0x8563
    ("",	X,	1,	"GL_RED_MAX_CLAMP_INGR"),	# 0x8564
    ("",	X,	1,	"GL_GREEN_MAX_CLAMP_INGR"),	# 0x8565
    ("",	X,	1,	"GL_BLUE_MAX_CLAMP_INGR"),	# 0x8566
    ("",	X,	1,	"GL_ALPHA_MAX_CLAMP_INGR"),	# 0x8567
    ("",	X,	1,	"GL_INTERLACE_READ_INGR"),	# 0x8568
    ("",	X,	1,	"GL_COMBINE"),	# 0x8570
    ("glGetTexEnv",	E,	1,	"GL_COMBINE_RGB"),	# 0x8571
    ("glGetTexEnv",	E,	1,	"GL_COMBINE_ALPHA"),	# 0x8572
    ("glGetTexEnv",	F,	1,	"GL_RGB_SCALE"),	# 0x8573
    ("",	X,	1,	"GL_ADD_SIGNED"),	# 0x8574
    ("",	X,	1,	"GL_INTERPOLATE"),	# 0x8575
    ("",	X,	1,	"GL_CONSTANT"),	# 0x8576
    ("",	X,	1,	"GL_PRIMARY_COLOR"),	# 0x8577
    ("",	X,	1,	"GL_PREVIOUS"),	# 0x8578
    ("glGetTexEnv",	E,	1,	"GL_SRC0_RGB"),	# 0x8580
    ("glGetTexEnv",	E,	1,	"GL_SRC1_RGB"),	# 0x8581
    ("glGetTexEnv",	E,	1,	"GL_SRC2_RGB"),	# 0x8582
    ("glGetTexEnv",	E,	1,	"GL_SOURCE3_RGB_NV"),	# 0x8583
    ("glGetTexEnv",	E,	1,	"GL_SRC0_ALPHA"),	# 0x8588
    ("glGetTexEnv",	E,	1,	"GL_SRC1_ALPHA"),	# 0x8589
    ("glGetTexEnv",	E,	1,	"GL_SRC2_ALPHA"),	# 0x858A
    ("glGetTexEnv",	E,	1,	"GL_SOURCE3_ALPHA_NV"),	# 0x858B
    ("glGetTexEnv",	E,	1,	"GL_OPERAND0_RGB"),	# 0x8590
    ("glGetTexEnv",	E,	1,	"GL_OPERAND1_RGB"),	# 0x8591
    ("glGetTexEnv",	E,	1,	"GL_OPERAND2_RGB"),	# 0x8592
    ("glGetTexEnv",	E,	1,	"GL_OPERAND3_RGB_NV"),	# 0x8593
    ("glGetTexEnv",	E,	1,	"GL_OPERAND0_ALPHA"),	# 0x8598
    ("glGetTexEnv",	E,	1,	"GL_OPERAND1_ALPHA"),	# 0x8599
    ("glGetTexEnv",	E,	1,	"GL_OPERAND2_ALPHA"),	# 0x859A
    ("glGetTexEnv",	E,	1,	"GL_OPERAND3_ALPHA_NV"),	# 0x859B
    ("",	X,	1,	"GL_PACK_SUBSAMPLE_RATE_SGIX"),	# 0x85A0
    ("",	X,	1,	"GL_UNPACK_SUBSAMPLE_RATE_SGIX"),	# 0x85A1
    ("",	X,	1,	"GL_PIXEL_SUBSAMPLE_4444_SGIX"),	# 0x85A2
    ("",	X,	1,	"GL_PIXEL_SUBSAMPLE_2424_SGIX"),	# 0x85A3
    ("",	X,	1,	"GL_PIXEL_SUBSAMPLE_4242_SGIX"),	# 0x85A4
    ("",	X,	1,	"GL_PERTURB_EXT"),	# 0x85AE
    ("",	X,	1,	"GL_TEXTURE_NORMAL_EXT"),	# 0x85AF
    ("",	X,	1,	"GL_LIGHT_MODEL_SPECULAR_VECTOR_APPLE"),	# 0x85B0
    ("",	X,	1,	"GL_TRANSFORM_HINT_APPLE"),	# 0x85B1
    ("",	X,	1,	"GL_UNPACK_CLIENT_STORAGE_APPLE"),	# 0x85B2
    ("",	X,	1,	"GL_BUFFER_OBJECT_APPLE"),	# 0x85B3
    ("",	X,	1,	"GL_STORAGE_CLIENT_APPLE"),	# 0x85B4
    ("glGet",	I,	1,	"GL_VERTEX_ARRAY_BINDING"),	# 0x85B5
    ("glGetTexParameter",	I,	1,	"GL_TEXTURE_RANGE_LENGTH_APPLE"),	# 0x85B7
    ("",	P,	1,	"GL_TEXTURE_RANGE_POINTER_APPLE"),	# 0x85B8
    ("",	X,	1,	"GL_YCBCR_422_APPLE"),	# 0x85B9
    ("",	X,	1,	"GL_UNSIGNED_SHORT_8_8_APPLE"),	# 0x85BA
    ("",	X,	1,	"GL_UNSIGNED_SHORT_8_8_REV_APPLE"),	# 0x85BB
    ("glGetTexParameter",	E,	1,	"GL_TEXTURE_STORAGE_HINT_APPLE"),	# 0x85BC
    ("",	X,	1,	"GL_STORAGE_PRIVATE_APPLE"),	# 0x85BD
    ("",	X,	1,	"GL_STORAGE_CACHED_APPLE"),	# 0x85BE
    ("",	X,	1,	"GL_STORAGE_SHARED_APPLE"),	# 0x85BF
    ("",	X,	1,	"GL_REPLACEMENT_CODE_ARRAY_SUN"),	# 0x85C0
    ("glGet",	E,	1,	"GL_REPLACEMENT_CODE_ARRAY_TYPE_SUN"),	# 0x85C1
    ("",	X,	1,	"GL_REPLACEMENT_CODE_ARRAY_STRIDE_SUN"),	# 0x85C2
    ("",	X,	1,	"GL_REPLACEMENT_CODE_ARRAY_POINTER_SUN"),	# 0x85C3
    ("",	X,	1,	"GL_R1UI_V3F_SUN"),	# 0x85C4
    ("",	X,	1,	"GL_R1UI_C4UB_V3F_SUN"),	# 0x85C5
    ("",	X,	1,	"GL_R1UI_C3F_V3F_SUN"),	# 0x85C6
    ("",	X,	1,	"GL_R1UI_N3F_V3F_SUN"),	# 0x85C7
    ("",	X,	1,	"GL_R1UI_C4F_N3F_V3F_SUN"),	# 0x85C8
    ("",	X,	1,	"GL_R1UI_T2F_V3F_SUN"),	# 0x85C9
    ("",	X,	1,	"GL_R1UI_T2F_N3F_V3F_SUN"),	# 0x85CA
    ("",	X,	1,	"GL_R1UI_T2F_C4F_N3F_V3F_SUN"),	# 0x85CB
    ("",	X,	1,	"GL_SLICE_ACCUM_SUN"),	# 0x85CC
    ("",	X,	1,	"GL_QUAD_MESH_SUN"),	# 0x8614
    ("",	X,	1,	"GL_TRIANGLE_MESH_SUN"),	# 0x8615
    ("_glGet",	B,	1,	"GL_VERTEX_PROGRAM_ARB"),	# 0x8620
    ("",	X,	1,	"GL_VERTEX_STATE_PROGRAM_NV"),	# 0x8621
    ("glGetVertexAttrib",	B,	1,	"GL_VERTEX_ATTRIB_ARRAY_ENABLED"),	# 0x8622
    ("glGetVertexAttrib",	I,	1,	"GL_VERTEX_ATTRIB_ARRAY_SIZE"),	# 0x8623
    ("glGetVertexAttrib",	I,	1,	"GL_VERTEX_ATTRIB_ARRAY_STRIDE"),	# 0x8624
    ("glGetVertexAttrib",	E,	1,	"GL_VERTEX_ATTRIB_ARRAY_TYPE"),	# 0x8625
    ("glGetVertexAttrib",	D,	4,	"GL_CURRENT_VERTEX_ATTRIB"),	# 0x8626
    ("glGetProgramARB",	I,	1,	"GL_PROGRAM_LENGTH_ARB"),	# 0x8627
    ("",	S,	1,	"GL_PROGRAM_STRING_ARB"),	# 0x8628
    ("",	X,	1,	"GL_MODELVIEW_PROJECTION_NV"),	# 0x8629
    ("",	X,	1,	"GL_IDENTITY_NV"),	# 0x862A
    ("",	X,	1,	"GL_INVERSE_NV"),	# 0x862B
    ("",	X,	1,	"GL_TRANSPOSE_NV"),	# 0x862C
    ("",	X,	1,	"GL_INVERSE_TRANSPOSE_NV"),	# 0x862D
    ("glGet",	I,	1,	"GL_MAX_PROGRAM_MATRIX_STACK_DEPTH_ARB"),	# 0x862E
    ("glGet",	I,	1,	"GL_MAX_PROGRAM_MATRICES_ARB"),	# 0x862F
    ("",	X,	1,	"GL_MATRIX0_NV"),	# 0x8630
    ("",	X,	1,	"GL_MATRIX1_NV"),	# 0x8631
    ("",	X,	1,	"GL_MATRIX2_NV"),	# 0x8632
    ("",	X,	1,	"GL_MATRIX3_NV"),	# 0x8633
    ("",	X,	1,	"GL_MATRIX4_NV"),	# 0x8634
    ("",	X,	1,	"GL_MATRIX5_NV"),	# 0x8635
    ("",	X,	1,	"GL_MATRIX6_NV"),	# 0x8636
    ("",	X,	1,	"GL_MATRIX7_NV"),	# 0x8637
    ("glGet",	I,	1,	"GL_CURRENT_MATRIX_STACK_DEPTH_ARB"),	# 0x8640
    ("glGet",	F,	16,	"GL_CURRENT_MATRIX_ARB"),	# 0x8641
    ("glGet",	B,	1,	"GL_PROGRAM_POINT_SIZE"),	# 0x8642
    ("glGet",	B,	1,	"GL_VERTEX_PROGRAM_TWO_SIDE"),	# 0x8643
    ("",	X,	1,	"GL_PROGRAM_PARAMETER_NV"),	# 0x8644
    ("glGetVertexAttrib",	P,	1,	"GL_VERTEX_ATTRIB_ARRAY_POINTER"),	# 0x8645
    ("glGetProgramNV",	I,	1,	"GL_PROGRAM_TARGET_NV"),	# 0x8646
    ("glGetProgramNV",	B,	1,	"GL_PROGRAM_RESIDENT_NV"),	# 0x8647
    ("",	X,	1,	"GL_TRACK_MATRIX_NV"),	# 0x8648
    ("",	X,	1,	"GL_TRACK_MATRIX_TRANSFORM_NV"),	# 0x8649
    ("glGet",	I,	1,	"GL_VERTEX_PROGRAM_BINDING_NV"),	# 0x864A
    ("glGet",	I,	1,	"GL_PROGRAM_ERROR_POSITION_ARB"),	# 0x864B
    ("",	X,	1,	"GL_OFFSET_TEXTURE_RECTANGLE_NV"),	# 0x864C
    ("",	X,	1,	"GL_OFFSET_TEXTURE_RECTANGLE_SCALE_NV"),	# 0x864D
    ("",	X,	1,	"GL_DOT_PRODUCT_TEXTURE_RECTANGLE_NV"),	# 0x864E
    ("glGet",	B,	1,	"GL_DEPTH_CLAMP"),	# 0x864F
    ("_glGet",	B,	1,	"GL_VERTEX_ATTRIB_ARRAY0_NV"),	# 0x8650
    ("_glGet",	B,	1,	"GL_VERTEX_ATTRIB_ARRAY1_NV"),	# 0x8651
    ("_glGet",	B,	1,	"GL_VERTEX_ATTRIB_ARRAY2_NV"),	# 0x8652
    ("_glGet",	B,	1,	"GL_VERTEX_ATTRIB_ARRAY3_NV"),	# 0x8653
    ("_glGet",	B,	1,	"GL_VERTEX_ATTRIB_ARRAY4_NV"),	# 0x8654
    ("_glGet",	B,	1,	"GL_VERTEX_ATTRIB_ARRAY5_NV"),	# 0x8655
    ("_glGet",	B,	1,	"GL_VERTEX_ATTRIB_ARRAY6_NV"),	# 0x8656
    ("_glGet",	B,	1,	"GL_VERTEX_ATTRIB_ARRAY7_NV"),	# 0x8657
    ("_glGet",	B,	1,	"GL_VERTEX_ATTRIB_ARRAY8_NV"),	# 0x8658
    ("_glGet",	B,	1,	"GL_VERTEX_ATTRIB_ARRAY9_NV"),	# 0x8659
    ("_glGet",	B,	1,	"GL_VERTEX_ATTRIB_ARRAY10_NV"),	# 0x865A
    ("_glGet",	B,	1,	"GL_VERTEX_ATTRIB_ARRAY11_NV"),	# 0x865B
    ("_glGet",	B,	1,	"GL_VERTEX_ATTRIB_ARRAY12_NV"),	# 0x865C
    ("_glGet",	B,	1,	"GL_VERTEX_ATTRIB_ARRAY13_NV"),	# 0x865D
    ("_glGet",	B,	1,	"GL_VERTEX_ATTRIB_ARRAY14_NV"),	# 0x865E
    ("_glGet",	B,	1,	"GL_VERTEX_ATTRIB_ARRAY15_NV"),	# 0x865F
    ("",	X,	1,	"GL_MAP1_VERTEX_ATTRIB0_4_NV"),	# 0x8660
    ("",	X,	1,	"GL_MAP1_VERTEX_ATTRIB1_4_NV"),	# 0x8661
    ("",	X,	1,	"GL_MAP1_VERTEX_ATTRIB2_4_NV"),	# 0x8662
    ("",	X,	1,	"GL_MAP1_VERTEX_ATTRIB3_4_NV"),	# 0x8663
    ("",	X,	1,	"GL_MAP1_VERTEX_ATTRIB4_4_NV"),	# 0x8664
    ("",	X,	1,	"GL_MAP1_VERTEX_ATTRIB5_4_NV"),	# 0x8665
    ("",	X,	1,	"GL_MAP1_VERTEX_ATTRIB6_4_NV"),	# 0x8666
    ("",	X,	1,	"GL_MAP1_VERTEX_ATTRIB7_4_NV"),	# 0x8667
    ("",	X,	1,	"GL_MAP1_VERTEX_ATTRIB8_4_NV"),	# 0x8668
    ("",	X,	1,	"GL_MAP1_VERTEX_ATTRIB9_4_NV"),	# 0x8669
    ("",	X,	1,	"GL_MAP1_VERTEX_ATTRIB10_4_NV"),	# 0x866A
    ("",	X,	1,	"GL_MAP1_VERTEX_ATTRIB11_4_NV"),	# 0x866B
    ("",	X,	1,	"GL_MAP1_VERTEX_ATTRIB12_4_NV"),	# 0x866C
    ("",	X,	1,	"GL_MAP1_VERTEX_ATTRIB13_4_NV"),	# 0x866D
    ("",	X,	1,	"GL_MAP1_VERTEX_ATTRIB14_4_NV"),	# 0x866E
    ("",	X,	1,	"GL_MAP1_VERTEX_ATTRIB15_4_NV"),	# 0x866F
    ("",	X,	1,	"GL_MAP2_VERTEX_ATTRIB0_4_NV"),	# 0x8670
    ("",	X,	1,	"GL_MAP2_VERTEX_ATTRIB1_4_NV"),	# 0x8671
    ("",	X,	1,	"GL_MAP2_VERTEX_ATTRIB2_4_NV"),	# 0x8672
    ("",	X,	1,	"GL_MAP2_VERTEX_ATTRIB3_4_NV"),	# 0x8673
    ("",	X,	1,	"GL_MAP2_VERTEX_ATTRIB4_4_NV"),	# 0x8674
    ("",	X,	1,	"GL_MAP2_VERTEX_ATTRIB5_4_NV"),	# 0x8675
    ("",	X,	1,	"GL_MAP2_VERTEX_ATTRIB6_4_NV"),	# 0x8676
    ("glGetProgramARB",	I,	1,	"GL_PROGRAM_BINDING_ARB"),	# 0x8677
    ("",	X,	1,	"GL_MAP2_VERTEX_ATTRIB8_4_NV"),	# 0x8678
    ("",	X,	1,	"GL_MAP2_VERTEX_ATTRIB9_4_NV"),	# 0x8679
    ("",	X,	1,	"GL_MAP2_VERTEX_ATTRIB10_4_NV"),	# 0x867A
    ("",	X,	1,	"GL_MAP2_VERTEX_ATTRIB11_4_NV"),	# 0x867B
    ("",	X,	1,	"GL_MAP2_VERTEX_ATTRIB12_4_NV"),	# 0x867C
    ("",	X,	1,	"GL_MAP2_VERTEX_ATTRIB13_4_NV"),	# 0x867D
    ("",	X,	1,	"GL_MAP2_VERTEX_ATTRIB14_4_NV"),	# 0x867E
    ("",	X,	1,	"GL_MAP2_VERTEX_ATTRIB15_4_NV"),	# 0x867F
    ("glGetTexLevelParameter",	I,	1,	"GL_TEXTURE_COMPRESSED_IMAGE_SIZE"),	# 0x86A0
    ("glGetTexLevelParameter",	B,	1,	"GL_TEXTURE_COMPRESSED"),	# 0x86A1
    ("glGet",	I,	1,	"GL_NUM_COMPRESSED_TEXTURE_FORMATS"),	# 0x86A2
    ("glGet",	E,	'_glGetInteger(GL_NUM_COMPRESSED_TEXTURE_FORMATS)',	"GL_COMPRESSED_TEXTURE_FORMATS"),	# 0x86A3
    ("glGet",	I,	1,	"GL_MAX_VERTEX_UNITS_ARB"),	# 0x86A4
    ("glGet",	I,	1,	"GL_ACTIVE_VERTEX_UNITS_ARB"),	# 0x86A5
    ("glGet",	B,	1,	"GL_WEIGHT_SUM_UNITY_ARB"),	# 0x86A6
    ("glGet",	B,	1,	"GL_VERTEX_BLEND_ARB"),	# 0x86A7
    ("glGet",	F,	1,	"GL_CURRENT_WEIGHT_ARB"),	# 0x86A8
    ("glGet",	E,	1,	"GL_WEIGHT_ARRAY_TYPE_ARB"),	# 0x86A9
    ("glGet",	I,	1,	"GL_WEIGHT_ARRAY_STRIDE_ARB"),	# 0x86AA
    ("glGet",	I,	1,	"GL_WEIGHT_ARRAY_SIZE_ARB"),	# 0x86AB
    ("glGet",	P,	1,	"GL_WEIGHT_ARRAY_POINTER_ARB"),	# 0x86AC
    ("glGet",	B,	1,	"GL_WEIGHT_ARRAY_ARB"),	# 0x86AD
    ("",	X,	1,	"GL_DOT3_RGB"),	# 0x86AE
    ("",	X,	1,	"GL_DOT3_RGBA"),	# 0x86AF
    ("",	X,	1,	"GL_COMPRESSED_RGB_FXT1_3DFX"),	# 0x86B0
    ("",	X,	1,	"GL_COMPRESSED_RGBA_FXT1_3DFX"),	# 0x86B1
    ("",	X,	1,	"GL_MULTISAMPLE_3DFX"),	# 0x86B2
    ("",	X,	1,	"GL_SAMPLE_BUFFERS_3DFX"),	# 0x86B3
    ("",	X,	1,	"GL_SAMPLES_3DFX"),	# 0x86B4
    ("",	X,	1,	"GL_EVAL_2D_NV"),	# 0x86C0
    ("",	X,	1,	"GL_EVAL_TRIANGULAR_2D_NV"),	# 0x86C1
    ("",	X,	1,	"GL_MAP_TESSELLATION_NV"),	# 0x86C2
    ("",	X,	1,	"GL_MAP_ATTRIB_U_ORDER_NV"),	# 0x86C3
    ("",	X,	1,	"GL_MAP_ATTRIB_V_ORDER_NV"),	# 0x86C4
    ("",	X,	1,	"GL_EVAL_FRACTIONAL_TESSELLATION_NV"),	# 0x86C5
    ("",	X,	1,	"GL_EVAL_VERTEX_ATTRIB0_NV"),	# 0x86C6
    ("",	X,	1,	"GL_EVAL_VERTEX_ATTRIB1_NV"),	# 0x86C7
    ("",	X,	1,	"GL_EVAL_VERTEX_ATTRIB2_NV"),	# 0x86C8
    ("",	X,	1,	"GL_EVAL_VERTEX_ATTRIB3_NV"),	# 0x86C9
    ("",	X,	1,	"GL_EVAL_VERTEX_ATTRIB4_NV"),	# 0x86CA
    ("",	X,	1,	"GL_EVAL_VERTEX_ATTRIB5_NV"),	# 0x86CB
    ("",	X,	1,	"GL_EVAL_VERTEX_ATTRIB6_NV"),	# 0x86CC
    ("",	X,	1,	"GL_EVAL_VERTEX_ATTRIB7_NV"),	# 0x86CD
    ("",	X,	1,	"GL_EVAL_VERTEX_ATTRIB8_NV"),	# 0x86CE
    ("",	X,	1,	"GL_EVAL_VERTEX_ATTRIB9_NV"),	# 0x86CF
    ("",	X,	1,	"GL_EVAL_VERTEX_ATTRIB10_NV"),	# 0x86D0
    ("",	X,	1,	"GL_EVAL_VERTEX_ATTRIB11_NV"),	# 0x86D1
    ("",	X,	1,	"GL_EVAL_VERTEX_ATTRIB12_NV"),	# 0x86D2
    ("",	X,	1,	"GL_EVAL_VERTEX_ATTRIB13_NV"),	# 0x86D3
    ("",	X,	1,	"GL_EVAL_VERTEX_ATTRIB14_NV"),	# 0x86D4
    ("",	X,	1,	"GL_EVAL_VERTEX_ATTRIB15_NV"),	# 0x86D5
    ("",	X,	1,	"GL_MAX_MAP_TESSELLATION_NV"),	# 0x86D6
    ("",	X,	1,	"GL_MAX_RATIONAL_EVAL_ORDER_NV"),	# 0x86D7
    ("",	X,	1,	"GL_MAX_PROGRAM_PATCH_ATTRIBS_NV"),	# 0x86D8
    ("glGetTexEnv",	E,	1,	"GL_RGBA_UNSIGNED_DOT_PRODUCT_MAPPING_NV"),	# 0x86D9
    ("",	X,	1,	"GL_UNSIGNED_INT_S8_S8_8_8_NV"),	# 0x86DA
    ("",	X,	1,	"GL_UNSIGNED_INT_8_8_S8_S8_REV_NV"),	# 0x86DB
    ("",	X,	1,	"GL_DSDT_MAG_INTENSITY_NV"),	# 0x86DC
    ("",	X,	1,	"GL_SHADER_CONSISTENT_NV"),	# 0x86DD
    ("",	X,	1,	"GL_TEXTURE_SHADER_NV"),	# 0x86DE
    ("glGetTexEnv",	E,	1,	"GL_SHADER_OPERATION_NV"),	# 0x86DF
    ("glGetTexEnv",	E,	4,	"GL_CULL_MODES_NV"),	# 0x86E0
    ("glGetTexEnv",	F,	4,	"GL_OFFSET_TEXTURE_MATRIX_NV"),	# 0x86E1
    ("glGetTexEnv",	F,	1,	"GL_OFFSET_TEXTURE_SCALE_NV"),	# 0x86E2
    ("glGetTexEnv",	F,	1,	"GL_OFFSET_TEXTURE_BIAS_NV"),	# 0x86E3
    ("glGetTexEnv",	E,	1,	"GL_PREVIOUS_TEXTURE_INPUT_NV"),	# 0x86E4
    ("glGetTexEnv",	F,	3,	"GL_CONST_EYE_NV"),	# 0x86E5
    ("",	X,	1,	"GL_PASS_THROUGH_NV"),	# 0x86E6
    ("",	X,	1,	"GL_CULL_FRAGMENT_NV"),	# 0x86E7
    ("",	X,	1,	"GL_OFFSET_TEXTURE_2D_NV"),	# 0x86E8
    ("",	X,	1,	"GL_DEPENDENT_AR_TEXTURE_2D_NV"),	# 0x86E9
    ("",	X,	1,	"GL_DEPENDENT_GB_TEXTURE_2D_NV"),	# 0x86EA
    ("",	X,	1,	"GL_SURFACE_STATE_NV"),	# 0x86EB
    ("",	X,	1,	"GL_DOT_PRODUCT_NV"),	# 0x86EC
    ("",	X,	1,	"GL_DOT_PRODUCT_DEPTH_REPLACE_NV"),	# 0x86ED
    ("",	X,	1,	"GL_DOT_PRODUCT_TEXTURE_2D_NV"),	# 0x86EE
    ("",	X,	1,	"GL_DOT_PRODUCT_TEXTURE_3D_NV"),	# 0x86EF
    ("",	X,	1,	"GL_DOT_PRODUCT_TEXTURE_CUBE_MAP_NV"),	# 0x86F0
    ("",	X,	1,	"GL_DOT_PRODUCT_DIFFUSE_CUBE_MAP_NV"),	# 0x86F1
    ("",	X,	1,	"GL_DOT_PRODUCT_REFLECT_CUBE_MAP_NV"),	# 0x86F2
    ("",	X,	1,	"GL_DOT_PRODUCT_CONST_EYE_REFLECT_CUBE_MAP_NV"),	# 0x86F3
    ("",	X,	1,	"GL_HILO_NV"),	# 0x86F4
    ("",	X,	1,	"GL_DSDT_NV"),	# 0x86F5
    ("",	X,	1,	"GL_DSDT_MAG_NV"),	# 0x86F6
    ("",	X,	1,	"GL_DSDT_MAG_VIB_NV"),	# 0x86F7
    ("",	X,	1,	"GL_HILO16_NV"),	# 0x86F8
    ("",	X,	1,	"GL_SIGNED_HILO_NV"),	# 0x86F9
    ("",	X,	1,	"GL_SIGNED_HILO16_NV"),	# 0x86FA
    ("",	X,	1,	"GL_SIGNED_RGBA_NV"),	# 0x86FB
    ("",	X,	1,	"GL_SIGNED_RGBA8_NV"),	# 0x86FC
    ("",	X,	1,	"GL_SURFACE_REGISTERED_NV"),	# 0x86FD
    ("",	X,	1,	"GL_SIGNED_RGB_NV"),	# 0x86FE
    ("",	X,	1,	"GL_SIGNED_RGB8_NV"),	# 0x86FF
    ("",	X,	1,	"GL_SURFACE_MAPPED_NV"),	# 0x8700
    ("",	X,	1,	"GL_SIGNED_LUMINANCE_NV"),	# 0x8701
    ("",	X,	1,	"GL_SIGNED_LUMINANCE8_NV"),	# 0x8702
    ("",	X,	1,	"GL_SIGNED_LUMINANCE_ALPHA_NV"),	# 0x8703
    ("",	X,	1,	"GL_SIGNED_LUMINANCE8_ALPHA8_NV"),	# 0x8704
    ("",	X,	1,	"GL_SIGNED_ALPHA_NV"),	# 0x8705
    ("",	X,	1,	"GL_SIGNED_ALPHA8_NV"),	# 0x8706
    ("",	X,	1,	"GL_SIGNED_INTENSITY_NV"),	# 0x8707
    ("",	X,	1,	"GL_SIGNED_INTENSITY8_NV"),	# 0x8708
    ("",	X,	1,	"GL_DSDT8_NV"),	# 0x8709
    ("",	X,	1,	"GL_DSDT8_MAG8_NV"),	# 0x870A
    ("",	X,	1,	"GL_DSDT8_MAG8_INTENSITY8_NV"),	# 0x870B
    ("",	X,	1,	"GL_SIGNED_RGB_UNSIGNED_ALPHA_NV"),	# 0x870C
    ("",	X,	1,	"GL_SIGNED_RGB8_UNSIGNED_ALPHA8_NV"),	# 0x870D
    ("",	X,	1,	"GL_HI_SCALE_NV"),	# 0x870E
    ("",	X,	1,	"GL_LO_SCALE_NV"),	# 0x870F
    ("",	X,	1,	"GL_DS_SCALE_NV"),	# 0x8710
    ("",	X,	1,	"GL_DT_SCALE_NV"),	# 0x8711
    ("",	X,	1,	"GL_MAGNITUDE_SCALE_NV"),	# 0x8712
    ("",	X,	1,	"GL_VIBRANCE_SCALE_NV"),	# 0x8713
    ("",	X,	1,	"GL_HI_BIAS_NV"),	# 0x8714
    ("",	X,	1,	"GL_LO_BIAS_NV"),	# 0x8715
    ("",	X,	1,	"GL_DS_BIAS_NV"),	# 0x8716
    ("",	X,	1,	"GL_DT_BIAS_NV"),	# 0x8717
    ("",	X,	1,	"GL_MAGNITUDE_BIAS_NV"),	# 0x8718
    ("",	X,	1,	"GL_VIBRANCE_BIAS_NV"),	# 0x8719
    ("",	X,	1,	"GL_TEXTURE_BORDER_VALUES_NV"),	# 0x871A
    ("",	X,	1,	"GL_TEXTURE_HI_SIZE_NV"),	# 0x871B
    ("",	X,	1,	"GL_TEXTURE_LO_SIZE_NV"),	# 0x871C
    ("",	X,	1,	"GL_TEXTURE_DS_SIZE_NV"),	# 0x871D
    ("",	X,	1,	"GL_TEXTURE_DT_SIZE_NV"),	# 0x871E
    ("",	X,	1,	"GL_TEXTURE_MAG_SIZE_NV"),	# 0x871F
    ("_glGet",	F,	16,	"GL_MODELVIEW2_ARB"),	# 0x8722
    ("_glGet",	F,	16,	"GL_MODELVIEW3_ARB"),	# 0x8723
    ("_glGet",	F,	16,	"GL_MODELVIEW4_ARB"),	# 0x8724
    ("_glGet",	F,	16,	"GL_MODELVIEW5_ARB"),	# 0x8725
    ("_glGet",	F,	16,	"GL_MODELVIEW6_ARB"),	# 0x8726
    ("_glGet",	F,	16,	"GL_MODELVIEW7_ARB"),	# 0x8727
    ("_glGet",	F,	16,	"GL_MODELVIEW8_ARB"),	# 0x8728
    ("_glGet",	F,	16,	"GL_MODELVIEW9_ARB"),	# 0x8729
    ("_glGet",	F,	16,	"GL_MODELVIEW10_ARB"),	# 0x872A
    ("_glGet",	F,	16,	"GL_MODELVIEW11_ARB"),	# 0x872B
    ("_glGet",	F,	16,	"GL_MODELVIEW12_ARB"),	# 0x872C
    ("_glGet",	F,	16,	"GL_MODELVIEW13_ARB"),	# 0x872D
    ("_glGet",	F,	16,	"GL_MODELVIEW14_ARB"),	# 0x872E
    ("_glGet",	F,	16,	"GL_MODELVIEW15_ARB"),	# 0x872F
    ("_glGet",	F,	16,	"GL_MODELVIEW16_ARB"),	# 0x8730
    ("_glGet",	F,	16,	"GL_MODELVIEW17_ARB"),	# 0x8731
    ("_glGet",	F,	16,	"GL_MODELVIEW18_ARB"),	# 0x8732
    ("_glGet",	F,	16,	"GL_MODELVIEW19_ARB"),	# 0x8733
    ("_glGet",	F,	16,	"GL_MODELVIEW20_ARB"),	# 0x8734
    ("_glGet",	F,	16,	"GL_MODELVIEW21_ARB"),	# 0x8735
    ("_glGet",	F,	16,	"GL_MODELVIEW22_ARB"),	# 0x8736
    ("_glGet",	F,	16,	"GL_MODELVIEW23_ARB"),	# 0x8737
    ("_glGet",	F,	16,	"GL_MODELVIEW24_ARB"),	# 0x8738
    ("_glGet",	F,	16,	"GL_MODELVIEW25_ARB"),	# 0x8739
    ("_glGet",	F,	16,	"GL_MODELVIEW26_ARB"),	# 0x873A
    ("_glGet",	F,	16,	"GL_MODELVIEW27_ARB"),	# 0x873B
    ("_glGet",	F,	16,	"GL_MODELVIEW28_ARB"),	# 0x873C
    ("_glGet",	F,	16,	"GL_MODELVIEW29_ARB"),	# 0x873D
    ("_glGet",	F,	16,	"GL_MODELVIEW30_ARB"),	# 0x873E
    ("_glGet",	F,	16,	"GL_MODELVIEW31_ARB"),	# 0x873F
    ("",	X,	1,	"GL_DOT3_RGB_EXT"),	# 0x8740
    # XXX: GL_DOT3_RGBA_EXT == GL_PROGRAM_BINARY_LENGTH, but you can't glGet GL_DOT3_RGBA_EXT
    ("glGetProgram",	I,	1,	"GL_PROGRAM_BINARY_LENGTH"),	# 0x8741,
    ("",	X,	1,	"GL_MIRROR_CLAMP_ATI"),	# 0x8742
    ("",	X,	1,	"GL_MIRROR_CLAMP_TO_EDGE"),	# 0x8743
    ("",	X,	1,	"GL_MODULATE_ADD_ATI"),	# 0x8744
    ("",	X,	1,	"GL_MODULATE_SIGNED_ADD_ATI"),	# 0x8745
    ("",	X,	1,	"GL_MODULATE_SUBTRACT_ATI"),	# 0x8746
    ("",	X,	1,	"GL_SET_AMD"),	# 0x874A
    ("",	X,	1,	"GL_REPLACE_VALUE_AMD"),	# 0x874B
    ("",	X,	1,	"GL_STENCIL_OP_VALUE_AMD"),	# 0x874C
    ("",	X,	1,	"GL_STENCIL_BACK_OP_VALUE_AMD"),	# 0x874D
    ("glGetVertexAttrib",	B,	1,	"GL_VERTEX_ATTRIB_ARRAY_LONG"),	# 0x874E
    ("",	X,	1,	"GL_OCCLUSION_QUERY_EVENT_MASK_AMD"),	# 0x874F
    #("",	X,	1,	"GL_DEPTH_STENCIL_MESA"),	# 0x8750
    #("",	X,	1,	"GL_UNSIGNED_INT_24_8_MESA"),	# 0x8751
    #("",	X,	1,	"GL_UNSIGNED_INT_8_24_REV_MESA"),	# 0x8752
    #("",	X,	1,	"GL_UNSIGNED_SHORT_15_1_MESA"),	# 0x8753
    #("",	X,	1,	"GL_UNSIGNED_SHORT_1_15_REV_MESA"),	# 0x8754
    #("",	X,	1,	"GL_TRACE_MASK_MESA"),	# 0x8755
    #("",	X,	1,	"GL_TRACE_NAME_MESA"),	# 0x8756
    ("",	X,	1,	"GL_YCBCR_MESA"),	# 0x8757
    ("glGet",	B,	1,	"GL_PACK_INVERT_MESA"),	# 0x8758
    ("",	X,	1,	"GL_TEXTURE_1D_STACK_MESAX"),	# 0x8759
    ("",	X,	1,	"GL_TEXTURE_2D_STACK_MESAX"),	# 0x875A
    ("",	X,	1,	"GL_PROXY_TEXTURE_1D_STACK_MESAX"),	# 0x875B
    ("",	X,	1,	"GL_PROXY_TEXTURE_2D_STACK_MESAX"),	# 0x875C
    ("",	X,	1,	"GL_TEXTURE_1D_STACK_BINDING_MESAX"),	# 0x875D
    ("",	X,	1,	"GL_TEXTURE_2D_STACK_BINDING_MESAX"),	# 0x875E
    ("",	X,	1,	"GL_STATIC_ATI"),	# 0x8760
    ("",	X,	1,	"GL_DYNAMIC_ATI"),	# 0x8761
    ("",	X,	1,	"GL_PRESERVE_ATI"),	# 0x8762
    ("",	X,	1,	"GL_DISCARD_ATI"),	# 0x8763
    ("glGetBufferParameter",	I,	1,	"GL_BUFFER_SIZE"),	# 0x8764
    ("glGetBufferParameter",	E,	1,	"GL_BUFFER_USAGE"),	# 0x8765
    ("",	X,	1,	"GL_ARRAY_OBJECT_BUFFER_ATI"),	# 0x8766
    ("",	X,	1,	"GL_ARRAY_OBJECT_OFFSET_ATI"),	# 0x8767
    ("",	X,	1,	"GL_ELEMENT_ARRAY_ATI"),	# 0x8768
    ("glGet",	E,	1,	"GL_ELEMENT_ARRAY_TYPE_ATI"),	# 0x8769
    ("",	X,	1,	"GL_ELEMENT_ARRAY_POINTER_ATI"),	# 0x876A
    ("",	X,	1,	"GL_MAX_VERTEX_STREAMS_ATI"),	# 0x876B
    ("",	X,	1,	"GL_VERTEX_STREAM0_ATI"),	# 0x876C
    ("",	X,	1,	"GL_VERTEX_STREAM1_ATI"),	# 0x876D
    ("",	X,	1,	"GL_VERTEX_STREAM2_ATI"),	# 0x876E
    ("",	X,	1,	"GL_VERTEX_STREAM3_ATI"),	# 0x876F
    ("",	X,	1,	"GL_VERTEX_STREAM4_ATI"),	# 0x8770
    ("",	X,	1,	"GL_VERTEX_STREAM5_ATI"),	# 0x8771
    ("",	X,	1,	"GL_VERTEX_STREAM6_ATI"),	# 0x8772
    ("",	X,	1,	"GL_VERTEX_STREAM7_ATI"),	# 0x8773
    ("",	X,	1,	"GL_VERTEX_SOURCE_ATI"),	# 0x8774
    ("",	X,	1,	"GL_BUMP_ROT_MATRIX_ATI"),	# 0x8775
    ("",	X,	1,	"GL_BUMP_ROT_MATRIX_SIZE_ATI"),	# 0x8776
    ("",	X,	1,	"GL_BUMP_NUM_TEX_UNITS_ATI"),	# 0x8777
    ("",	X,	1,	"GL_BUMP_TEX_UNITS_ATI"),	# 0x8778
    ("",	X,	1,	"GL_DUDV_ATI"),	# 0x8779
    ("",	X,	1,	"GL_DU8DV8_ATI"),	# 0x877A
    ("",	X,	1,	"GL_BUMP_ENVMAP_ATI"),	# 0x877B
    ("glGetTexEnv",	E,	1,	"GL_BUMP_TARGET_ATI"),	# 0x877C
    ("",	X,	1,	"GL_VERTEX_SHADER_EXT"),	# 0x8780
    ("glGet",	I,	1,	"GL_VERTEX_SHADER_BINDING_EXT"),	# 0x8781
    ("",	X,	1,	"GL_OP_INDEX_EXT"),	# 0x8782
    ("",	X,	1,	"GL_OP_NEGATE_EXT"),	# 0x8783
    ("",	X,	1,	"GL_OP_DOT3_EXT"),	# 0x8784
    ("",	X,	1,	"GL_OP_DOT4_EXT"),	# 0x8785
    ("",	X,	1,	"GL_OP_MUL_EXT"),	# 0x8786
    ("",	X,	1,	"GL_OP_ADD_EXT"),	# 0x8787
    ("",	X,	1,	"GL_OP_MADD_EXT"),	# 0x8788
    ("",	X,	1,	"GL_OP_FRAC_EXT"),	# 0x8789
    ("",	X,	1,	"GL_OP_MAX_EXT"),	# 0x878A
    ("",	X,	1,	"GL_OP_MIN_EXT"),	# 0x878B
    ("",	X,	1,	"GL_OP_SET_GE_EXT"),	# 0x878C
    ("",	X,	1,	"GL_OP_SET_LT_EXT"),	# 0x878D
    ("",	X,	1,	"GL_OP_CLAMP_EXT"),	# 0x878E
    ("",	X,	1,	"GL_OP_FLOOR_EXT"),	# 0x878F
    ("",	X,	1,	"GL_OP_ROUND_EXT"),	# 0x8790
    ("",	X,	1,	"GL_OP_EXP_BASE_2_EXT"),	# 0x8791
    ("",	X,	1,	"GL_OP_LOG_BASE_2_EXT"),	# 0x8792
    ("",	X,	1,	"GL_OP_POWER_EXT"),	# 0x8793
    ("",	X,	1,	"GL_OP_RECIP_EXT"),	# 0x8794
    ("",	X,	1,	"GL_OP_RECIP_SQRT_EXT"),	# 0x8795
    ("",	X,	1,	"GL_OP_SUB_EXT"),	# 0x8796
    ("",	X,	1,	"GL_OP_CROSS_PRODUCT_EXT"),	# 0x8797
    ("",	X,	1,	"GL_OP_MULTIPLY_MATRIX_EXT"),	# 0x8798
    ("",	X,	1,	"GL_OP_MOV_EXT"),	# 0x8799
    ("",	X,	1,	"GL_OUTPUT_VERTEX_EXT"),	# 0x879A
    ("",	X,	1,	"GL_OUTPUT_COLOR0_EXT"),	# 0x879B
    ("",	X,	1,	"GL_OUTPUT_COLOR1_EXT"),	# 0x879C
    ("",	X,	1,	"GL_OUTPUT_TEXTURE_COORD0_EXT"),	# 0x879D
    ("",	X,	1,	"GL_OUTPUT_TEXTURE_COORD1_EXT"),	# 0x879E
    ("",	X,	1,	"GL_OUTPUT_TEXTURE_COORD2_EXT"),	# 0x879F
    ("",	X,	1,	"GL_OUTPUT_TEXTURE_COORD3_EXT"),	# 0x87A0
    ("",	X,	1,	"GL_OUTPUT_TEXTURE_COORD4_EXT"),	# 0x87A1
    ("",	X,	1,	"GL_OUTPUT_TEXTURE_COORD5_EXT"),	# 0x87A2
    ("",	X,	1,	"GL_OUTPUT_TEXTURE_COORD6_EXT"),	# 0x87A3
    ("",	X,	1,	"GL_OUTPUT_TEXTURE_COORD7_EXT"),	# 0x87A4
    ("",	X,	1,	"GL_OUTPUT_TEXTURE_COORD8_EXT"),	# 0x87A5
    ("",	X,	1,	"GL_OUTPUT_TEXTURE_COORD9_EXT"),	# 0x87A6
    ("",	X,	1,	"GL_OUTPUT_TEXTURE_COORD10_EXT"),	# 0x87A7
    ("",	X,	1,	"GL_OUTPUT_TEXTURE_COORD11_EXT"),	# 0x87A8
    ("",	X,	1,	"GL_OUTPUT_TEXTURE_COORD12_EXT"),	# 0x87A9
    ("",	X,	1,	"GL_OUTPUT_TEXTURE_COORD13_EXT"),	# 0x87AA
    ("",	X,	1,	"GL_OUTPUT_TEXTURE_COORD14_EXT"),	# 0x87AB
    ("",	X,	1,	"GL_OUTPUT_TEXTURE_COORD15_EXT"),	# 0x87AC
    ("",	X,	1,	"GL_OUTPUT_TEXTURE_COORD16_EXT"),	# 0x87AD
    ("",	X,	1,	"GL_OUTPUT_TEXTURE_COORD17_EXT"),	# 0x87AE
    ("",	X,	1,	"GL_OUTPUT_TEXTURE_COORD18_EXT"),	# 0x87AF
    ("",	X,	1,	"GL_OUTPUT_TEXTURE_COORD19_EXT"),	# 0x87B0
    ("",	X,	1,	"GL_OUTPUT_TEXTURE_COORD20_EXT"),	# 0x87B1
    ("",	X,	1,	"GL_OUTPUT_TEXTURE_COORD21_EXT"),	# 0x87B2
    ("",	X,	1,	"GL_OUTPUT_TEXTURE_COORD22_EXT"),	# 0x87B3
    ("",	X,	1,	"GL_OUTPUT_TEXTURE_COORD23_EXT"),	# 0x87B4
    ("",	X,	1,	"GL_OUTPUT_TEXTURE_COORD24_EXT"),	# 0x87B5
    ("",	X,	1,	"GL_OUTPUT_TEXTURE_COORD25_EXT"),	# 0x87B6
    ("",	X,	1,	"GL_OUTPUT_TEXTURE_COORD26_EXT"),	# 0x87B7
    ("",	X,	1,	"GL_OUTPUT_TEXTURE_COORD27_EXT"),	# 0x87B8
    ("",	X,	1,	"GL_OUTPUT_TEXTURE_COORD28_EXT"),	# 0x87B9
    ("",	X,	1,	"GL_OUTPUT_TEXTURE_COORD29_EXT"),	# 0x87BA
    ("",	X,	1,	"GL_OUTPUT_TEXTURE_COORD30_EXT"),	# 0x87BB
    ("",	X,	1,	"GL_OUTPUT_TEXTURE_COORD31_EXT"),	# 0x87BC
    ("",	X,	1,	"GL_OUTPUT_FOG_EXT"),	# 0x87BD
    ("",	X,	1,	"GL_SCALAR_EXT"),	# 0x87BE
    ("",	X,	1,	"GL_VECTOR_EXT"),	# 0x87BF
    ("",	X,	1,	"GL_MATRIX_EXT"),	# 0x87C0
    ("",	X,	1,	"GL_VARIANT_EXT"),	# 0x87C1
    ("",	X,	1,	"GL_INVARIANT_EXT"),	# 0x87C2
    ("",	X,	1,	"GL_LOCAL_CONSTANT_EXT"),	# 0x87C3
    ("",	X,	1,	"GL_LOCAL_EXT"),	# 0x87C4
    ("",	X,	1,	"GL_MAX_VERTEX_SHADER_INSTRUCTIONS_EXT"),	# 0x87C5
    ("",	X,	1,	"GL_MAX_VERTEX_SHADER_VARIANTS_EXT"),	# 0x87C6
    ("",	X,	1,	"GL_MAX_VERTEX_SHADER_INVARIANTS_EXT"),	# 0x87C7
    ("",	X,	1,	"GL_MAX_VERTEX_SHADER_LOCAL_CONSTANTS_EXT"),	# 0x87C8
    ("",	X,	1,	"GL_MAX_VERTEX_SHADER_LOCALS_EXT"),	# 0x87C9
    ("",	X,	1,	"GL_MAX_OPTIMIZED_VERTEX_SHADER_INSTRUCTIONS_EXT"),	# 0x87CA
    ("",	X,	1,	"GL_MAX_OPTIMIZED_VERTEX_SHADER_VARIANTS_EXT"),	# 0x87CB
    ("",	X,	1,	"GL_MAX_OPTIMIZED_VERTEX_SHADER_LOCAL_CONSTANTS_EXT"),	# 0x87CC
    ("",	X,	1,	"GL_MAX_OPTIMIZED_VERTEX_SHADER_INVARIANTS_EXT"),	# 0x87CD
    ("",	X,	1,	"GL_MAX_OPTIMIZED_VERTEX_SHADER_LOCALS_EXT"),	# 0x87CE
    ("",	X,	1,	"GL_VERTEX_SHADER_INSTRUCTIONS_EXT"),	# 0x87CF
    ("",	X,	1,	"GL_VERTEX_SHADER_VARIANTS_EXT"),	# 0x87D0
    ("",	X,	1,	"GL_VERTEX_SHADER_INVARIANTS_EXT"),	# 0x87D1
    ("",	X,	1,	"GL_VERTEX_SHADER_LOCAL_CONSTANTS_EXT"),	# 0x87D2
    ("",	X,	1,	"GL_VERTEX_SHADER_LOCALS_EXT"),	# 0x87D3
    ("",	X,	1,	"GL_VERTEX_SHADER_OPTIMIZED_EXT"),	# 0x87D4
    ("",	X,	1,	"GL_X_EXT"),	# 0x87D5
    ("",	X,	1,	"GL_Y_EXT"),	# 0x87D6
    ("",	X,	1,	"GL_Z_EXT"),	# 0x87D7
    ("",	X,	1,	"GL_W_EXT"),	# 0x87D8
    ("",	X,	1,	"GL_NEGATIVE_X_EXT"),	# 0x87D9
    ("",	X,	1,	"GL_NEGATIVE_Y_EXT"),	# 0x87DA
    ("",	X,	1,	"GL_NEGATIVE_Z_EXT"),	# 0x87DB
    ("",	X,	1,	"GL_NEGATIVE_W_EXT"),	# 0x87DC
    ("",	X,	1,	"GL_ZERO_EXT"),	# 0x87DD
    ("",	X,	1,	"GL_ONE_EXT"),	# 0x87DE
    ("",	X,	1,	"GL_NEGATIVE_ONE_EXT"),	# 0x87DF
    ("",	X,	1,	"GL_NORMALIZED_RANGE_EXT"),	# 0x87E0
    ("",	X,	1,	"GL_FULL_RANGE_EXT"),	# 0x87E1
    ("",	X,	1,	"GL_CURRENT_VERTEX_EXT"),	# 0x87E2
    ("",	X,	1,	"GL_MVP_MATRIX_EXT"),	# 0x87E3
    ("",	X,	1,	"GL_VARIANT_VALUE_EXT"),	# 0x87E4
    ("",	X,	1,	"GL_VARIANT_DATATYPE_EXT"),	# 0x87E5
    ("",	X,	1,	"GL_VARIANT_ARRAY_STRIDE_EXT"),	# 0x87E6
    ("glGet",	E,	1,	"GL_VARIANT_ARRAY_TYPE_EXT"),	# 0x87E7
    ("",	X,	1,	"GL_VARIANT_ARRAY_EXT"),	# 0x87E8
    ("",	X,	1,	"GL_VARIANT_ARRAY_POINTER_EXT"),	# 0x87E9
    ("",	X,	1,	"GL_INVARIANT_VALUE_EXT"),	# 0x87EA
    ("",	X,	1,	"GL_INVARIANT_DATATYPE_EXT"),	# 0x87EB
    ("",	X,	1,	"GL_LOCAL_CONSTANT_VALUE_EXT"),	# 0x87EC
    ("",	X,	1,	"GL_LOCAL_CONSTANT_DATATYPE_EXT"),	# 0x87ED
    ("",	X,	1,	"GL_ATC_RGBA_INTERPOLATED_ALPHA_AMD"),	# 0x87EE
    ("",	X,	1,	"GL_PN_TRIANGLES_ATI"),	# 0x87F0
    ("",	X,	1,	"GL_MAX_PN_TRIANGLES_TESSELATION_LEVEL_ATI"),	# 0x87F1
    ("",	X,	1,	"GL_PN_TRIANGLES_POINT_MODE_ATI"),	# 0x87F2
    ("",	X,	1,	"GL_PN_TRIANGLES_NORMAL_MODE_ATI"),	# 0x87F3
    ("",	X,	1,	"GL_PN_TRIANGLES_TESSELATION_LEVEL_ATI"),	# 0x87F4
    ("",	X,	1,	"GL_PN_TRIANGLES_POINT_MODE_LINEAR_ATI"),	# 0x87F5
    ("",	X,	1,	"GL_PN_TRIANGLES_POINT_MODE_CUBIC_ATI"),	# 0x87F6
    ("",	X,	1,	"GL_PN_TRIANGLES_NORMAL_MODE_LINEAR_ATI"),	# 0x87F7
    ("",	X,	1,	"GL_PN_TRIANGLES_NORMAL_MODE_QUADRATIC_ATI"),	# 0x87F8
    ("",	X,	1,	"GL_3DC_X_AMD"),	# 0x87F9
    ("",	X,	1,	"GL_3DC_XY_AMD"),	# 0x87FA
    ("glGet",	I,	1,	"GL_VBO_FREE_MEMORY_ATI"),	# 0x87FB
    ("glGet",	I,	1,	"GL_TEXTURE_FREE_MEMORY_ATI"),	# 0x87FC
    ("glGet",	I,	1,	"GL_RENDERBUFFER_FREE_MEMORY_ATI"),	# 0x87FD
    ("glGet",	I,	1,	"GL_NUM_PROGRAM_BINARY_FORMATS"),	# 0x87FE
    ("glGet",	E,	"_glGetInteger(GL_NUM_PROGRAM_BINARY_FORMATS)",	"GL_PROGRAM_BINARY_FORMATS"),	# 0x87FF
    ("glGet",	E,	1,	"GL_STENCIL_BACK_FUNC"),	# 0x8800
    ("glGet",	E,	1,	"GL_STENCIL_BACK_FAIL"),	# 0x8801
    ("glGet",	E,	1,	"GL_STENCIL_BACK_PASS_DEPTH_FAIL"),	# 0x8802
    ("glGet",	E,	1,	"GL_STENCIL_BACK_PASS_DEPTH_PASS"),	# 0x8803
    ("_glGet",	B,	1,	"GL_FRAGMENT_PROGRAM_ARB"),	# 0x8804
    ("glGetProgramARB",	I,	1,	"GL_PROGRAM_ALU_INSTRUCTIONS_ARB"),	# 0x8805
    ("glGetProgramARB",	I,	1,	"GL_PROGRAM_TEX_INSTRUCTIONS_ARB"),	# 0x8806
    ("glGetProgramARB",	I,	1,	"GL_PROGRAM_TEX_INDIRECTIONS_ARB"),	# 0x8807
    ("glGetProgramARB",	I,	1,	"GL_PROGRAM_NATIVE_ALU_INSTRUCTIONS_ARB"),	# 0x8808
    ("glGetProgramARB",	I,	1,	"GL_PROGRAM_NATIVE_TEX_INSTRUCTIONS_ARB"),	# 0x8809
    ("glGetProgramARB",	I,	1,	"GL_PROGRAM_NATIVE_TEX_INDIRECTIONS_ARB"),	# 0x880A
    ("glGetProgramARB",	I,	1,	"GL_MAX_PROGRAM_ALU_INSTRUCTIONS_ARB"),	# 0x880B
    ("glGetProgramARB",	I,	1,	"GL_MAX_PROGRAM_TEX_INSTRUCTIONS_ARB"),	# 0x880C
    ("glGetProgramARB",	I,	1,	"GL_MAX_PROGRAM_TEX_INDIRECTIONS_ARB"),	# 0x880D
    ("glGetProgramARB",	I,	1,	"GL_MAX_PROGRAM_NATIVE_ALU_INSTRUCTIONS_ARB"),	# 0x880E
    ("glGetProgramARB",	I,	1,	"GL_MAX_PROGRAM_NATIVE_TEX_INSTRUCTIONS_ARB"),	# 0x880F
    ("glGetProgramARB",	I,	1,	"GL_MAX_PROGRAM_NATIVE_TEX_INDIRECTIONS_ARB"),	# 0x8810
    ("",	X,	1,	"GL_RGBA32F"),	# 0x8814
    ("",	X,	1,	"GL_RGB32F"),	# 0x8815
    ("",	X,	1,	"GL_ALPHA32F_ARB"),	# 0x8816
    ("",	X,	1,	"GL_INTENSITY32F_ARB"),	# 0x8817
    ("",	X,	1,	"GL_LUMINANCE32F_ARB"),	# 0x8818
    ("",	X,	1,	"GL_LUMINANCE_ALPHA32F_ARB"),	# 0x8819
    ("",	X,	1,	"GL_RGBA16F"),	# 0x881A
    ("",	X,	1,	"GL_RGB16F"),	# 0x881B
    ("",	X,	1,	"GL_ALPHA16F_ARB"),	# 0x881C
    ("",	X,	1,	"GL_INTENSITY16F_ARB"),	# 0x881D
    ("",	X,	1,	"GL_LUMINANCE16F_ARB"),	# 0x881E
    ("",	X,	1,	"GL_LUMINANCE_ALPHA16F_ARB"),	# 0x881F
    ("glGet",	B,	1,	"GL_RGBA_FLOAT_MODE_ARB"),	# 0x8820
    ("glGet",	I,	1,	"GL_MAX_DRAW_BUFFERS"),	# 0x8824
    ("glGet",	E,	1,	"GL_DRAW_BUFFER0"),	# 0x8825
    ("glGet",	E,	1,	"GL_DRAW_BUFFER1"),	# 0x8826
    ("glGet",	E,	1,	"GL_DRAW_BUFFER2"),	# 0x8827
    ("glGet",	E,	1,	"GL_DRAW_BUFFER3"),	# 0x8828
    ("glGet",	E,	1,	"GL_DRAW_BUFFER4"),	# 0x8829
    ("glGet",	E,	1,	"GL_DRAW_BUFFER5"),	# 0x882A
    ("glGet",	E,	1,	"GL_DRAW_BUFFER6"),	# 0x882B
    ("glGet",	E,	1,	"GL_DRAW_BUFFER7"),	# 0x882C
    ("",	X,	1,	"GL_COLOR_CLEAR_UNCLAMPED_VALUE_ATI"),	# 0x8835
    #("",	X,	1,	"GL_COMPRESSED_LUMINANCE_ALPHA_3DC_ATI"),	# 0x8837
    ("glGet",	E,	1,	"GL_BLEND_EQUATION_ALPHA"),	# 0x883D
    ("",	X,	1,	"GL_SUBSAMPLE_DISTANCE_AMD"),	# 0x883F
    ("glGet",	B,	1,	"GL_MATRIX_PALETTE_ARB"),	# 0x8840
    ("glGet",	I,	1,	"GL_MAX_MATRIX_PALETTE_STACK_DEPTH_ARB"),	# 0x8841
    ("glGet",	I,	1,	"GL_MAX_PALETTE_MATRICES_ARB"),	# 0x8842
    ("glGet",	I,	1,	"GL_CURRENT_PALETTE_MATRIX_ARB"),	# 0x8843
    ("glGet",	B,	1,	"GL_MATRIX_INDEX_ARRAY_ARB"),	# 0x8844
    ("glGet",	I,	1,	"GL_CURRENT_MATRIX_INDEX_ARB"),	# 0x8845
    ("glGet",	I,	1,	"GL_MATRIX_INDEX_ARRAY_SIZE_ARB"),	# 0x8846
    ("glGet",	E,	1,	"GL_MATRIX_INDEX_ARRAY_TYPE_ARB"),	# 0x8847
    ("glGet",	I,	1,	"GL_MATRIX_INDEX_ARRAY_STRIDE_ARB"),	# 0x8848
    ("glGet",	P,	1,	"GL_MATRIX_INDEX_ARRAY_POINTER_ARB"),	# 0x8849
    ("glGetTexLevelParameter",	I,	1,	"GL_TEXTURE_DEPTH_SIZE"),	# 0x884A
    ("glGetTexParameter",	E,	1,	"GL_DEPTH_TEXTURE_MODE"),	# 0x884B
    ("glGetTexParameter,glGetSamplerParameter",	E,	1,	"GL_TEXTURE_COMPARE_MODE"),	# 0x884C
    ("glGetTexParameter,glGetSamplerParameter",	E,	1,	"GL_TEXTURE_COMPARE_FUNC"),	# 0x884D
    ("",	X,	1,	"GL_COMPARE_REF_TO_TEXTURE"),	# 0x884E
    ("glGet,glGetTexParameter,glGetSamplerParameter",	B,	1,	"GL_TEXTURE_CUBE_MAP_SEAMLESS"),	# 0x884F
    ("",	X,	1,	"GL_OFFSET_PROJECTIVE_TEXTURE_2D_NV"),	# 0x8850
    ("",	X,	1,	"GL_OFFSET_PROJECTIVE_TEXTURE_2D_SCALE_NV"),	# 0x8851
    ("",	X,	1,	"GL_OFFSET_PROJECTIVE_TEXTURE_RECTANGLE_NV"),	# 0x8852
    ("",	X,	1,	"GL_OFFSET_PROJECTIVE_TEXTURE_RECTANGLE_SCALE_NV"),	# 0x8853
    ("",	X,	1,	"GL_OFFSET_HILO_TEXTURE_2D_NV"),	# 0x8854
    ("",	X,	1,	"GL_OFFSET_HILO_TEXTURE_RECTANGLE_NV"),	# 0x8855
    ("",	X,	1,	"GL_OFFSET_HILO_PROJECTIVE_TEXTURE_2D_NV"),	# 0x8856
    ("",	X,	1,	"GL_OFFSET_HILO_PROJECTIVE_TEXTURE_RECTANGLE_NV"),	# 0x8857
    ("",	X,	1,	"GL_DEPENDENT_HILO_TEXTURE_2D_NV"),	# 0x8858
    ("",	X,	1,	"GL_DEPENDENT_RGB_TEXTURE_3D_NV"),	# 0x8859
    ("",	X,	1,	"GL_DEPENDENT_RGB_TEXTURE_CUBE_MAP_NV"),	# 0x885A
    ("",	X,	1,	"GL_DOT_PRODUCT_PASS_THROUGH_NV"),	# 0x885B
    ("",	X,	1,	"GL_DOT_PRODUCT_TEXTURE_1D_NV"),	# 0x885C
    ("",	X,	1,	"GL_DOT_PRODUCT_AFFINE_DEPTH_REPLACE_NV"),	# 0x885D
    ("",	X,	1,	"GL_HILO8_NV"),	# 0x885E
    ("",	X,	1,	"GL_SIGNED_HILO8_NV"),	# 0x885F
    ("",	X,	1,	"GL_FORCE_BLUE_TO_ONE_NV"),	# 0x8860
    ("glGet",	B,	1,	"GL_POINT_SPRITE"),	# 0x8861
    ("glGetTexEnv",	B,	1,	"GL_COORD_REPLACE"),	# 0x8862
    ("glGet",	E,	1,	"GL_POINT_SPRITE_R_MODE_NV"),	# 0x8863
    ("glGetQuery",	I,	1,	"GL_QUERY_COUNTER_BITS"),	# 0x8864
    ("glGetQuery",	I,	1,	"GL_CURRENT_QUERY"),	# 0x8865
    ("glGetQueryObject",	I,	1,	"GL_QUERY_RESULT"),	# 0x8866
    ("glGetQueryObject",	B,	1,	"GL_QUERY_RESULT_AVAILABLE"),	# 0x8867
    ("glGet",	I,	1,	"GL_MAX_FRAGMENT_PROGRAM_LOCAL_PARAMETERS_NV"),	# 0x8868
    ("glGet",	I,	1,	"GL_MAX_VERTEX_ATTRIBS"),	# 0x8869
    ("glGetVertexAttrib",	B,	1,	"GL_VERTEX_ATTRIB_ARRAY_NORMALIZED"),	# 0x886A
    ("glGet",	I,	1,	"GL_MAX_TESS_CONTROL_INPUT_COMPONENTS"),	# 0x886C
    ("glGet",	I,	1,	"GL_MAX_TESS_EVALUATION_INPUT_COMPONENTS"),	# 0x886D
    ("",	X,	1,	"GL_DEPTH_STENCIL_TO_RGBA_NV"),	# 0x886E
    ("",	X,	1,	"GL_DEPTH_STENCIL_TO_BGRA_NV"),	# 0x886F
    ("",	X,	1,	"GL_FRAGMENT_PROGRAM_NV"),	# 0x8870
    ("glGet",	I,	1,	"GL_MAX_TEXTURE_COORDS"),	# 0x8871
    ("glGet",	I,	1,	"GL_MAX_TEXTURE_IMAGE_UNITS"),	# 0x8872
    ("glGet",	I,	1,	"GL_FRAGMENT_PROGRAM_BINDING_NV"),	# 0x8873
    ("glGet",	S,	1,	"GL_PROGRAM_ERROR_STRING_ARB"),	# 0x8874
    ("",	X,	1,	"GL_PROGRAM_FORMAT_ASCII_ARB"),	# 0x8875
    ("glGetProgramARB",	E,	1,	"GL_PROGRAM_FORMAT_ARB"),	# 0x8876
    ("",	X,	1,	"GL_WRITE_PIXEL_DATA_RANGE_NV"),	# 0x8878
    ("",	X,	1,	"GL_READ_PIXEL_DATA_RANGE_NV"),	# 0x8879
    ("",	X,	1,	"GL_WRITE_PIXEL_DATA_RANGE_LENGTH_NV"),	# 0x887A
    ("",	X,	1,	"GL_READ_PIXEL_DATA_RANGE_LENGTH_NV"),	# 0x887B
    ("",	X,	1,	"GL_WRITE_PIXEL_DATA_RANGE_POINTER_NV"),	# 0x887C
    ("",	X,	1,	"GL_READ_PIXEL_DATA_RANGE_POINTER_NV"),	# 0x887D
    ("",	X,	1,	"GL_GEOMETRY_SHADER_INVOCATIONS"),	# 0x887F
    ("",	X,	1,	"GL_FLOAT_R_NV"),	# 0x8880
    ("",	X,	1,	"GL_FLOAT_RG_NV"),	# 0x8881
    ("",	X,	1,	"GL_FLOAT_RGB_NV"),	# 0x8882
    ("",	X,	1,	"GL_FLOAT_RGBA_NV"),	# 0x8883
    ("",	X,	1,	"GL_FLOAT_R16_NV"),	# 0x8884
    ("",	X,	1,	"GL_FLOAT_R32_NV"),	# 0x8885
    ("",	X,	1,	"GL_FLOAT_RG16_NV"),	# 0x8886
    ("",	X,	1,	"GL_FLOAT_RG32_NV"),	# 0x8887
    ("",	X,	1,	"GL_FLOAT_RGB16_NV"),	# 0x8888
    ("",	X,	1,	"GL_FLOAT_RGB32_NV"),	# 0x8889
    ("",	X,	1,	"GL_FLOAT_RGBA16_NV"),	# 0x888A
    ("",	X,	1,	"GL_FLOAT_RGBA32_NV"),	# 0x888B
    ("",	X,	1,	"GL_TEXTURE_FLOAT_COMPONENTS_NV"),	# 0x888C
    ("",	X,	1,	"GL_FLOAT_CLEAR_COLOR_VALUE_NV"),	# 0x888D
    ("",	X,	1,	"GL_FLOAT_RGBA_MODE_NV"),	# 0x888E
    ("glGetTexParameter",	E,	1,	"GL_TEXTURE_UNSIGNED_REMAP_MODE_NV"),	# 0x888F
    ("glGet",	B,	1,	"GL_DEPTH_BOUNDS_TEST_EXT"),	# 0x8890
    ("glGet",	F,	2,	"GL_DEPTH_BOUNDS_EXT"),	# 0x8891
    ("",	X,	1,	"GL_ARRAY_BUFFER"),	# 0x8892
    ("",	X,	1,	"GL_ELEMENT_ARRAY_BUFFER"),	# 0x8893
    ("glGet",	I,	1,	"GL_ARRAY_BUFFER_BINDING"),	# 0x8894
    ("glGet",	I,	1,	"GL_ELEMENT_ARRAY_BUFFER_BINDING"),	# 0x8895
    ("glGet",	I,	1,	"GL_VERTEX_ARRAY_BUFFER_BINDING"),	# 0x8896
    ("glGet",	I,	1,	"GL_NORMAL_ARRAY_BUFFER_BINDING"),	# 0x8897
    ("glGet",	I,	1,	"GL_COLOR_ARRAY_BUFFER_BINDING"),	# 0x8898
    ("glGet",	I,	1,	"GL_INDEX_ARRAY_BUFFER_BINDING"),	# 0x8899
    ("glGet",	I,	1,	"GL_TEXTURE_COORD_ARRAY_BUFFER_BINDING"),	# 0x889A
    ("glGet",	I,	1,	"GL_EDGE_FLAG_ARRAY_BUFFER_BINDING"),	# 0x889B
    ("glGet",	I,	1,	"GL_SECONDARY_COLOR_ARRAY_BUFFER_BINDING"),	# 0x889C
    ("glGet",	I,	1,	"GL_FOG_COORD_ARRAY_BUFFER_BINDING"),	# 0x889D
    ("glGet",	I,	1,	"GL_WEIGHT_ARRAY_BUFFER_BINDING"),	# 0x889E
    ("glGetVertexAttrib",	I,	1,	"GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING"),	# 0x889F
    ("glGetProgramARB",	I,	1,	"GL_PROGRAM_INSTRUCTIONS_ARB"),	# 0x88A0
    ("glGetProgramARB",	I,	1,	"GL_MAX_PROGRAM_INSTRUCTIONS_ARB"),	# 0x88A1
    ("glGetProgramARB",	I,	1,	"GL_PROGRAM_NATIVE_INSTRUCTIONS_ARB"),	# 0x88A2
    ("glGetProgramARB",	I,	1,	"GL_MAX_PROGRAM_NATIVE_INSTRUCTIONS_ARB"),	# 0x88A3
    ("glGetProgramARB",	I,	1,	"GL_PROGRAM_TEMPORARIES_ARB"),	# 0x88A4
    ("glGetProgramARB",	I,	1,	"GL_MAX_PROGRAM_TEMPORARIES_ARB"),	# 0x88A5
    ("glGetProgramARB",	I,	1,	"GL_PROGRAM_NATIVE_TEMPORARIES_ARB"),	# 0x88A6
    ("glGetProgramARB",	I,	1,	"GL_MAX_PROGRAM_NATIVE_TEMPORARIES_ARB"),	# 0x88A7
    ("glGetProgramARB",	I,	1,	"GL_PROGRAM_PARAMETERS_ARB"),	# 0x88A8
    ("glGetProgramARB",	I,	1,	"GL_MAX_PROGRAM_PARAMETERS_ARB"),	# 0x88A9
    ("glGetProgramARB",	I,	1,	"GL_PROGRAM_NATIVE_PARAMETERS_ARB"),	# 0x88AA
    ("glGetProgramARB",	I,	1,	"GL_MAX_PROGRAM_NATIVE_PARAMETERS_ARB"),	# 0x88AB
    ("glGetProgramARB",	I,	1,	"GL_PROGRAM_ATTRIBS_ARB"),	# 0x88AC
    ("glGetProgramARB",	I,	1,	"GL_MAX_PROGRAM_ATTRIBS_ARB"),	# 0x88AD
    ("glGetProgramARB",	I,	1,	"GL_PROGRAM_NATIVE_ATTRIBS_ARB"),	# 0x88AE
    ("glGetProgramARB",	I,	1,	"GL_MAX_PROGRAM_NATIVE_ATTRIBS_ARB"),	# 0x88AF
    ("glGetProgramARB",	I,	1,	"GL_PROGRAM_ADDRESS_REGISTERS_ARB"),	# 0x88B0
    ("glGetProgramARB",	I,	1,	"GL_MAX_PROGRAM_ADDRESS_REGISTERS_ARB"),	# 0x88B1
    ("glGetProgramARB",	I,	1,	"GL_PROGRAM_NATIVE_ADDRESS_REGISTERS_ARB"),	# 0x88B2
    ("glGetProgramARB",	I,	1,	"GL_MAX_PROGRAM_NATIVE_ADDRESS_REGISTERS_ARB"),	# 0x88B3
    ("glGetProgramARB",	I,	1,	"GL_MAX_PROGRAM_LOCAL_PARAMETERS_ARB"),	# 0x88B4
    ("glGetProgramARB",	I,	1,	"GL_MAX_PROGRAM_ENV_PARAMETERS_ARB"),	# 0x88B5
    ("glGetProgramARB",	B,	1,	"GL_PROGRAM_UNDER_NATIVE_LIMITS_ARB"),	# 0x88B6
    ("glGet",	F,	16,	"GL_TRANSPOSE_CURRENT_MATRIX_ARB"),	# 0x88B7
    ("",	X,	1,	"GL_READ_ONLY"),	# 0x88B8
    ("",	X,	1,	"GL_WRITE_ONLY"),	# 0x88B9
    ("",	X,	1,	"GL_READ_WRITE"),	# 0x88BA
    ("glGetBufferParameter",	E,	1,	"GL_BUFFER_ACCESS"),	# 0x88BB
    ("glGetBufferParameter",	B,	1,	"GL_BUFFER_MAPPED"),	# 0x88BC
    ("glGetBufferParameter",	P,	1,	"GL_BUFFER_MAP_POINTER"),	# 0x88BD
    ("",	X,	1,	"GL_WRITE_DISCARD_NV"),	# 0x88BE
    ("",	X,	1,	"GL_TIME_ELAPSED"),	# 0x88BF
    ("",	X,	1,	"GL_MATRIX0_ARB"),	# 0x88C0
    ("",	X,	1,	"GL_MATRIX1_ARB"),	# 0x88C1
    ("",	X,	1,	"GL_MATRIX2_ARB"),	# 0x88C2
    ("",	X,	1,	"GL_MATRIX3_ARB"),	# 0x88C3
    ("",	X,	1,	"GL_MATRIX4_ARB"),	# 0x88C4
    ("",	X,	1,	"GL_MATRIX5_ARB"),	# 0x88C5
    ("",	X,	1,	"GL_MATRIX6_ARB"),	# 0x88C6
    ("",	X,	1,	"GL_MATRIX7_ARB"),	# 0x88C7
    ("",	X,	1,	"GL_MATRIX8_ARB"),	# 0x88C8
    ("",	X,	1,	"GL_MATRIX9_ARB"),	# 0x88C9
    ("",	X,	1,	"GL_MATRIX10_ARB"),	# 0x88CA
    ("",	X,	1,	"GL_MATRIX11_ARB"),	# 0x88CB
    ("",	X,	1,	"GL_MATRIX12_ARB"),	# 0x88CC
    ("",	X,	1,	"GL_MATRIX13_ARB"),	# 0x88CD
    ("",	X,	1,	"GL_MATRIX14_ARB"),	# 0x88CE
    ("",	X,	1,	"GL_MATRIX15_ARB"),	# 0x88CF
    ("",	X,	1,	"GL_MATRIX16_ARB"),	# 0x88D0
    ("",	X,	1,	"GL_MATRIX17_ARB"),	# 0x88D1
    ("",	X,	1,	"GL_MATRIX18_ARB"),	# 0x88D2
    ("",	X,	1,	"GL_MATRIX19_ARB"),	# 0x88D3
    ("",	X,	1,	"GL_MATRIX20_ARB"),	# 0x88D4
    ("",	X,	1,	"GL_MATRIX21_ARB"),	# 0x88D5
    ("",	X,	1,	"GL_MATRIX22_ARB"),	# 0x88D6
    ("",	X,	1,	"GL_MATRIX23_ARB"),	# 0x88D7
    ("",	X,	1,	"GL_MATRIX24_ARB"),	# 0x88D8
    ("",	X,	1,	"GL_MATRIX25_ARB"),	# 0x88D9
    ("",	X,	1,	"GL_MATRIX26_ARB"),	# 0x88DA
    ("",	X,	1,	"GL_MATRIX27_ARB"),	# 0x88DB
    ("",	X,	1,	"GL_MATRIX28_ARB"),	# 0x88DC
    ("",	X,	1,	"GL_MATRIX29_ARB"),	# 0x88DD
    ("",	X,	1,	"GL_MATRIX30_ARB"),	# 0x88DE
    ("",	X,	1,	"GL_MATRIX31_ARB"),	# 0x88DF
    ("",	X,	1,	"GL_STREAM_DRAW"),	# 0x88E0
    ("",	X,	1,	"GL_STREAM_READ"),	# 0x88E1
    ("",	X,	1,	"GL_STREAM_COPY"),	# 0x88E2
    ("",	X,	1,	"GL_STATIC_DRAW"),	# 0x88E4
    ("",	X,	1,	"GL_STATIC_READ"),	# 0x88E5
    ("",	X,	1,	"GL_STATIC_COPY"),	# 0x88E6
    ("",	X,	1,	"GL_DYNAMIC_DRAW"),	# 0x88E8
    ("",	X,	1,	"GL_DYNAMIC_READ"),	# 0x88E9
    ("",	X,	1,	"GL_DYNAMIC_COPY"),	# 0x88EA
    ("",	X,	1,	"GL_PIXEL_PACK_BUFFER"),	# 0x88EB
    ("",	X,	1,	"GL_PIXEL_UNPACK_BUFFER"),	# 0x88EC
    ("glGet",	I,	1,	"GL_PIXEL_PACK_BUFFER_BINDING"),	# 0x88ED
    ("",	X,	1,	"GL_ETC1_SRGB8_NV"),	# 0x88EE
    ("glGet",	I,	1,	"GL_PIXEL_UNPACK_BUFFER_BINDING"),	# 0x88EF
    ("",	X,	1,	"GL_DEPTH24_STENCIL8"),	# 0x88F0
    ("glGetTexLevelParameter",	I,	1,	"GL_TEXTURE_STENCIL_SIZE"),	# 0x88F1
    ("",	X,	1,	"GL_STENCIL_TAG_BITS_EXT"),	# 0x88F2
    ("",	X,	1,	"GL_STENCIL_CLEAR_TAG_VALUE_EXT"),	# 0x88F3
    ("glGetProgramARB",	I,	1,	"GL_MAX_PROGRAM_EXEC_INSTRUCTIONS_NV"),	# 0x88F4
    ("glGetProgramARB",	I,	1,	"GL_MAX_PROGRAM_CALL_DEPTH_NV"),	# 0x88F5
    ("glGetProgramARB",	I,	1,	"GL_MAX_PROGRAM_IF_DEPTH_NV"),	# 0x88F6
    ("glGetProgramARB",	I,	1,	"GL_MAX_PROGRAM_LOOP_DEPTH_NV"),	# 0x88F7
    ("glGetProgramARB",	I,	1,	"GL_MAX_PROGRAM_LOOP_COUNT_NV"),	# 0x88F8
    ("",	X,	1,	"GL_SRC1_COLOR"),	# 0x88F9
    ("",	X,	1,	"GL_ONE_MINUS_SRC1_COLOR"),	# 0x88FA
    ("",	X,	1,	"GL_ONE_MINUS_SRC1_ALPHA"),	# 0x88FB
    ("glGet",	I,	1,	"GL_MAX_DUAL_SOURCE_DRAW_BUFFERS"),	# 0x88FC
    ("glGetVertexAttrib",	B,	1,	"GL_VERTEX_ATTRIB_ARRAY_INTEGER"),	# 0x88FD
    ("glGetVertexAttrib",	I,	1,	"GL_VERTEX_ATTRIB_ARRAY_DIVISOR"),	# 0x88FE
    ("glGet",	I,	1,	"GL_MAX_ARRAY_TEXTURE_LAYERS"),	# 0x88FF
    ("glGet",	F,	1,	"GL_MIN_PROGRAM_TEXEL_OFFSET"),	# 0x8904
    ("glGet",	F,	1,	"GL_MAX_PROGRAM_TEXEL_OFFSET"),	# 0x8905
    ("",	X,	1,	"GL_PROGRAM_ATTRIB_COMPONENTS_NV"),	# 0x8906
    ("",	X,	1,	"GL_PROGRAM_RESULT_COMPONENTS_NV"),	# 0x8907
    ("glGetProgramARB",	I,	1,	"GL_MAX_PROGRAM_ATTRIB_COMPONENTS_NV"),	# 0x8908
    ("glGetProgramARB",	I,	1,	"GL_MAX_PROGRAM_RESULT_COMPONENTS_NV"),	# 0x8909
    ("glGet",	B,	1,	"GL_STENCIL_TEST_TWO_SIDE_EXT"),	# 0x8910
    ("glGet",	E,	1,	"GL_ACTIVE_STENCIL_FACE_EXT"),	# 0x8911
    ("",	X,	1,	"GL_MIRROR_CLAMP_TO_BORDER_EXT"),	# 0x8912
    ("",	X,	1,	"GL_SAMPLES_PASSED"),	# 0x8914
    ("glGetProgram",	I,	1,	"GL_GEOMETRY_VERTICES_OUT"),	# 0x8916
    ("glGetProgram",	E,	1,	"GL_GEOMETRY_INPUT_TYPE"),	# 0x8917
    ("glGetProgram",	E,	1,	"GL_GEOMETRY_OUTPUT_TYPE"),	# 0x8918
    ("_glGet",	I,	1,	"GL_SAMPLER_BINDING"),	# 0x8919
    ("glGet",	E,	1,	"GL_CLAMP_VERTEX_COLOR"),	# 0x891A
    ("glGet",	E,	1,	"GL_CLAMP_FRAGMENT_COLOR"),	# 0x891B
    ("glGet",	E,	1,	"GL_CLAMP_READ_COLOR"),	# 0x891C
    ("",	X,	1,	"GL_FIXED_ONLY"),	# 0x891D
    ("",	X,	1,	"GL_TESS_CONTROL_PROGRAM_NV"),	# 0x891E
    ("",	X,	1,	"GL_TESS_EVALUATION_PROGRAM_NV"),	# 0x891F
    ("",	X,	1,	"GL_FRAGMENT_SHADER_ATI"),	# 0x8920
    ("",	X,	1,	"GL_REG_0_ATI"),	# 0x8921
    ("",	X,	1,	"GL_REG_1_ATI"),	# 0x8922
    ("",	X,	1,	"GL_REG_2_ATI"),	# 0x8923
    ("",	X,	1,	"GL_REG_3_ATI"),	# 0x8924
    ("",	X,	1,	"GL_REG_4_ATI"),	# 0x8925
    ("",	X,	1,	"GL_REG_5_ATI"),	# 0x8926
    ("",	X,	1,	"GL_REG_6_ATI"),	# 0x8927
    ("",	X,	1,	"GL_REG_7_ATI"),	# 0x8928
    ("",	X,	1,	"GL_REG_8_ATI"),	# 0x8929
    ("",	X,	1,	"GL_REG_9_ATI"),	# 0x892A
    ("",	X,	1,	"GL_REG_10_ATI"),	# 0x892B
    ("",	X,	1,	"GL_REG_11_ATI"),	# 0x892C
    ("",	X,	1,	"GL_REG_12_ATI"),	# 0x892D
    ("",	X,	1,	"GL_REG_13_ATI"),	# 0x892E
    ("",	X,	1,	"GL_REG_14_ATI"),	# 0x892F
    ("",	X,	1,	"GL_REG_15_ATI"),	# 0x8930
    ("",	X,	1,	"GL_REG_16_ATI"),	# 0x8931
    ("",	X,	1,	"GL_REG_17_ATI"),	# 0x8932
    ("",	X,	1,	"GL_REG_18_ATI"),	# 0x8933
    ("",	X,	1,	"GL_REG_19_ATI"),	# 0x8934
    ("",	X,	1,	"GL_REG_20_ATI"),	# 0x8935
    ("",	X,	1,	"GL_REG_21_ATI"),	# 0x8936
    ("",	X,	1,	"GL_REG_22_ATI"),	# 0x8937
    ("",	X,	1,	"GL_REG_23_ATI"),	# 0x8938
    ("",	X,	1,	"GL_REG_24_ATI"),	# 0x8939
    ("",	X,	1,	"GL_REG_25_ATI"),	# 0x893A
    ("",	X,	1,	"GL_REG_26_ATI"),	# 0x893B
    ("",	X,	1,	"GL_REG_27_ATI"),	# 0x893C
    ("",	X,	1,	"GL_REG_28_ATI"),	# 0x893D
    ("",	X,	1,	"GL_REG_29_ATI"),	# 0x893E
    ("",	X,	1,	"GL_REG_30_ATI"),	# 0x893F
    ("",	X,	1,	"GL_REG_31_ATI"),	# 0x8940
    ("",	X,	1,	"GL_CON_0_ATI"),	# 0x8941
    ("",	X,	1,	"GL_CON_1_ATI"),	# 0x8942
    ("",	X,	1,	"GL_CON_2_ATI"),	# 0x8943
    ("",	X,	1,	"GL_CON_3_ATI"),	# 0x8944
    ("",	X,	1,	"GL_CON_4_ATI"),	# 0x8945
    ("",	X,	1,	"GL_CON_5_ATI"),	# 0x8946
    ("",	X,	1,	"GL_CON_6_ATI"),	# 0x8947
    ("",	X,	1,	"GL_CON_7_ATI"),	# 0x8948
    ("",	X,	1,	"GL_CON_8_ATI"),	# 0x8949
    ("",	X,	1,	"GL_CON_9_ATI"),	# 0x894A
    ("",	X,	1,	"GL_CON_10_ATI"),	# 0x894B
    ("",	X,	1,	"GL_CON_11_ATI"),	# 0x894C
    ("",	X,	1,	"GL_CON_12_ATI"),	# 0x894D
    ("",	X,	1,	"GL_CON_13_ATI"),	# 0x894E
    ("",	X,	1,	"GL_CON_14_ATI"),	# 0x894F
    ("",	X,	1,	"GL_CON_15_ATI"),	# 0x8950
    ("",	X,	1,	"GL_CON_16_ATI"),	# 0x8951
    ("",	X,	1,	"GL_CON_17_ATI"),	# 0x8952
    ("",	X,	1,	"GL_CON_18_ATI"),	# 0x8953
    ("",	X,	1,	"GL_CON_19_ATI"),	# 0x8954
    ("",	X,	1,	"GL_CON_20_ATI"),	# 0x8955
    ("",	X,	1,	"GL_CON_21_ATI"),	# 0x8956
    ("",	X,	1,	"GL_CON_22_ATI"),	# 0x8957
    ("",	X,	1,	"GL_CON_23_ATI"),	# 0x8958
    ("",	X,	1,	"GL_CON_24_ATI"),	# 0x8959
    ("",	X,	1,	"GL_CON_25_ATI"),	# 0x895A
    ("",	X,	1,	"GL_CON_26_ATI"),	# 0x895B
    ("",	X,	1,	"GL_CON_27_ATI"),	# 0x895C
    ("",	X,	1,	"GL_CON_28_ATI"),	# 0x895D
    ("",	X,	1,	"GL_CON_29_ATI"),	# 0x895E
    ("",	X,	1,	"GL_CON_30_ATI"),	# 0x895F
    ("",	X,	1,	"GL_CON_31_ATI"),	# 0x8960
    ("",	X,	1,	"GL_MOV_ATI"),	# 0x8961
    ("",	X,	1,	"GL_ADD_ATI"),	# 0x8963
    ("",	X,	1,	"GL_MUL_ATI"),	# 0x8964
    ("",	X,	1,	"GL_SUB_ATI"),	# 0x8965
    ("",	X,	1,	"GL_DOT3_ATI"),	# 0x8966
    ("",	X,	1,	"GL_DOT4_ATI"),	# 0x8967
    ("",	X,	1,	"GL_MAD_ATI"),	# 0x8968
    ("",	X,	1,	"GL_LERP_ATI"),	# 0x8969
    ("",	X,	1,	"GL_CND_ATI"),	# 0x896A
    ("",	X,	1,	"GL_CND0_ATI"),	# 0x896B
    ("",	X,	1,	"GL_DOT2_ADD_ATI"),	# 0x896C
    ("",	X,	1,	"GL_SECONDARY_INTERPOLATOR_ATI"),	# 0x896D
    ("",	X,	1,	"GL_NUM_FRAGMENT_REGISTERS_ATI"),	# 0x896E
    ("",	X,	1,	"GL_NUM_FRAGMENT_CONSTANTS_ATI"),	# 0x896F
    ("",	X,	1,	"GL_NUM_PASSES_ATI"),	# 0x8970
    ("",	X,	1,	"GL_NUM_INSTRUCTIONS_PER_PASS_ATI"),	# 0x8971
    ("",	X,	1,	"GL_NUM_INSTRUCTIONS_TOTAL_ATI"),	# 0x8972
    ("",	X,	1,	"GL_NUM_INPUT_INTERPOLATOR_COMPONENTS_ATI"),	# 0x8973
    ("",	X,	1,	"GL_NUM_LOOPBACK_COMPONENTS_ATI"),	# 0x8974
    ("",	X,	1,	"GL_COLOR_ALPHA_PAIRING_ATI"),	# 0x8975
    ("",	X,	1,	"GL_SWIZZLE_STR_ATI"),	# 0x8976
    ("",	X,	1,	"GL_SWIZZLE_STQ_ATI"),	# 0x8977
    ("",	X,	1,	"GL_SWIZZLE_STR_DR_ATI"),	# 0x8978
    ("",	X,	1,	"GL_SWIZZLE_STQ_DQ_ATI"),	# 0x8979
    ("",	X,	1,	"GL_SWIZZLE_STRQ_ATI"),	# 0x897A
    ("",	X,	1,	"GL_SWIZZLE_STRQ_DQ_ATI"),	# 0x897B
    ("",	X,	1,	"GL_INTERLACE_OML"),	# 0x8980
    ("",	X,	1,	"GL_INTERLACE_READ_OML"),	# 0x8981
    ("",	X,	1,	"GL_FORMAT_SUBSAMPLE_24_24_OML"),	# 0x8982
    ("",	X,	1,	"GL_FORMAT_SUBSAMPLE_244_244_OML"),	# 0x8983
    ("",	X,	1,	"GL_PACK_RESAMPLE_OML"),	# 0x8984
    ("",	X,	1,	"GL_UNPACK_RESAMPLE_OML"),	# 0x8985
    ("",	X,	1,	"GL_RESAMPLE_REPLICATE_OML"),	# 0x8986
    ("",	X,	1,	"GL_RESAMPLE_ZERO_FILL_OML"),	# 0x8987
    ("",	X,	1,	"GL_RESAMPLE_AVERAGE_OML"),	# 0x8988
    ("",	X,	1,	"GL_RESAMPLE_DECIMATE_OML"),	# 0x8989
    ("",	X,	1,	"GL_POINT_SIZE_ARRAY_TYPE_OES"),	# 0x898A
    ("",	X,	1,	"GL_POINT_SIZE_ARRAY_STRIDE_OES"),	# 0x898B
    ("",	X,	1,	"GL_POINT_SIZE_ARRAY_POINTER_OES"),	# 0x898C
    ("",	X,	1,	"GL_MODELVIEW_MATRIX_FLOAT_AS_INT_BITS_OES"),	# 0x898D
    ("",	X,	1,	"GL_PROJECTION_MATRIX_FLOAT_AS_INT_BITS_OES"),	# 0x898E
    ("",	X,	1,	"GL_TEXTURE_MATRIX_FLOAT_AS_INT_BITS_OES"),	# 0x898F
    ("",	X,	1,	"GL_VERTEX_ATTRIB_MAP1_APPLE"),	# 0x8A00
    ("",	X,	1,	"GL_VERTEX_ATTRIB_MAP2_APPLE"),	# 0x8A01
    ("",	X,	1,	"GL_VERTEX_ATTRIB_MAP1_SIZE_APPLE"),	# 0x8A02
    ("",	X,	1,	"GL_VERTEX_ATTRIB_MAP1_COEFF_APPLE"),	# 0x8A03
    ("",	X,	1,	"GL_VERTEX_ATTRIB_MAP1_ORDER_APPLE"),	# 0x8A04
    ("",	X,	1,	"GL_VERTEX_ATTRIB_MAP1_DOMAIN_APPLE"),	# 0x8A05
    ("",	X,	1,	"GL_VERTEX_ATTRIB_MAP2_SIZE_APPLE"),	# 0x8A06
    ("",	X,	1,	"GL_VERTEX_ATTRIB_MAP2_COEFF_APPLE"),	# 0x8A07
    ("",	X,	1,	"GL_VERTEX_ATTRIB_MAP2_ORDER_APPLE"),	# 0x8A08
    ("",	X,	1,	"GL_VERTEX_ATTRIB_MAP2_DOMAIN_APPLE"),	# 0x8A09
    ("",	X,	1,	"GL_DRAW_PIXELS_APPLE"),	# 0x8A0A
    ("",	X,	1,	"GL_FENCE_APPLE"),	# 0x8A0B
    ("",	X,	1,	"GL_ELEMENT_ARRAY_APPLE"),	# 0x8A0C
    ("glGet",	E,	1,	"GL_ELEMENT_ARRAY_TYPE_APPLE"),	# 0x8A0D
    ("",	X,	1,	"GL_ELEMENT_ARRAY_POINTER_APPLE"),	# 0x8A0E
    ("",	X,	1,	"GL_COLOR_FLOAT_APPLE"),	# 0x8A0F
    ("",	X,	1,	"GL_UNIFORM_BUFFER"),	# 0x8A11
    ("",	X,	1,	"GL_BUFFER_SERIALIZED_MODIFY_APPLE"),	# 0x8A12
    ("",	X,	1,	"GL_BUFFER_FLUSHING_UNMAP_APPLE"),	# 0x8A13
    ("",	X,	1,	"GL_AUX_DEPTH_STENCIL_APPLE"),	# 0x8A14
    ("",	X,	1,	"GL_PACK_ROW_BYTES_APPLE"),	# 0x8A15
    ("",	X,	1,	"GL_UNPACK_ROW_BYTES_APPLE"),	# 0x8A16
    ("",	X,	1,	"GL_RELEASED_APPLE"),	# 0x8A19
    ("",	X,	1,	"GL_VOLATILE_APPLE"),	# 0x8A1A
    ("",	X,	1,	"GL_RETAINED_APPLE"),	# 0x8A1B
    ("",	X,	1,	"GL_UNDEFINED_APPLE"),	# 0x8A1C
    ("",	B,	1,	"GL_PURGEABLE_APPLE"),	# 0x8A1D
    ("",	X,	1,	"GL_RGB_422_APPLE"),	# 0x8A1F
    ("glGet,glGet_i",	I,	1,	"GL_UNIFORM_BUFFER_BINDING"),	# 0x8A28
    ("glGet_i",	I,	1,	"GL_UNIFORM_BUFFER_START"),	# 0x8A29
    ("glGet_i",	I,	1,	"GL_UNIFORM_BUFFER_SIZE"),	# 0x8A2A
    ("glGet",	I,	1,	"GL_MAX_VERTEX_UNIFORM_BLOCKS"),	# 0x8A2B
    ("glGet",	I,	1,	"GL_MAX_GEOMETRY_UNIFORM_BLOCKS"),	# 0x8A2C
    ("glGet",	I,	1,	"GL_MAX_FRAGMENT_UNIFORM_BLOCKS"),	# 0x8A2D
    ("glGet",	I,	1,	"GL_MAX_COMBINED_UNIFORM_BLOCKS"),	# 0x8A2E
    ("glGet",	I,	1,	"GL_MAX_UNIFORM_BUFFER_BINDINGS"),	# 0x8A2F
    ("glGet",	I,	1,	"GL_MAX_UNIFORM_BLOCK_SIZE"),	# 0x8A30
    ("glGet",	I,	1,	"GL_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS"),	# 0x8A31
    ("glGet",	I,	1,	"GL_MAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS"),	# 0x8A32
    ("glGet",	I,	1,	"GL_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS"),	# 0x8A33
    ("glGet",	I,	1,	"GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT"),	# 0x8A34
    ("glGetProgram",	I,	1,	"GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH"),	# 0x8A35
    ("glGetProgram",	I,	1,	"GL_ACTIVE_UNIFORM_BLOCKS"),	# 0x8A36
    ("glGetActiveUniforms",	E,	1,	"GL_UNIFORM_TYPE"),	# 0x8A37
    ("glGetActiveUniforms",	I,	1,	"GL_UNIFORM_SIZE"),	# 0x8A38
    ("glGetActiveUniforms",	I,	1,	"GL_UNIFORM_NAME_LENGTH"),	# 0x8A39
    ("glGetActiveUniforms",	I,	1,	"GL_UNIFORM_BLOCK_INDEX"),	# 0x8A3A
    ("glGetActiveUniforms",	I,	1,	"GL_UNIFORM_OFFSET"),	# 0x8A3B
    ("glGetActiveUniforms",	I,	1,	"GL_UNIFORM_ARRAY_STRIDE"),	# 0x8A3C
    ("glGetActiveUniforms",	I,	1,	"GL_UNIFORM_MATRIX_STRIDE"),	# 0x8A3D
    ("glGetActiveUniforms",	B,	1,	"GL_UNIFORM_IS_ROW_MAJOR"),	# 0x8A3E
    ("glGetActiveUniformBlock",	I,	1,	"GL_UNIFORM_BLOCK_BINDING"),	# 0x8A3F
    ("glGetActiveUniformBlock",	I,	1,	"GL_UNIFORM_BLOCK_DATA_SIZE"),	# 0x8A40
    ("glGetActiveUniformBlock",	I,	1,	"GL_UNIFORM_BLOCK_NAME_LENGTH"),	# 0x8A41
    ("glGetActiveUniformBlock",	I,	1,	"GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS"),	# 0x8A42
    ("glGetActiveUniformBlock",	I,	1,	"GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES"),	# 0x8A43
    ("glGetActiveUniformBlock",	B,	1,	"GL_UNIFORM_BLOCK_REFERENCED_BY_VERTEX_SHADER"),	# 0x8A44
    ("glGetActiveUniformBlock",	B,	1,	"GL_UNIFORM_BLOCK_REFERENCED_BY_GEOMETRY_SHADER"),	# 0x8A45
    ("glGetActiveUniformBlock",	B,	1,	"GL_UNIFORM_BLOCK_REFERENCED_BY_FRAGMENT_SHADER"),	# 0x8A46
    ("glGetTexParameter,glGetSamplerParameter",	E,	1,	"GL_TEXTURE_SRGB_DECODE_EXT"),	# 0x8A48
    ("",	X,	1,	"GL_DECODE_EXT"),	# 0x8A49
    ("",	X,	1,	"GL_SKIP_DECODE_EXT"),	# 0x8A4A
    ("",	X,	1,	"GL_PROGRAM_PIPELINE_OBJECT_EXT"),	# 0x8A4F
    ("",	X,	1,	"GL_RGB_RAW_422_APPLE"),	# 0x8A51
    ("",	X,	1,	"GL_FRAGMENT_SHADER_DISCARDS_SAMPLES_EXT"),	# 0x8A52
    ("",	X,	1,	"GL_SYNC_OBJECT_APPLE"),	# 0x8A53
    ("",	X,	1,	"GL_COMPRESSED_SRGB_PVRTC_2BPPV1_EXT"),	# 0x8A54
    ("",	X,	1,	"GL_COMPRESSED_SRGB_PVRTC_4BPPV1_EXT"),	# 0x8A55
    ("",	X,	1,	"GL_COMPRESSED_SRGB_ALPHA_PVRTC_2BPPV1_EXT"),	# 0x8A56
    ("",	X,	1,	"GL_COMPRESSED_SRGB_ALPHA_PVRTC_4BPPV1_EXT"),	# 0x8A57
    ("glGetProgramPipeline",	I,	1,	"GL_FRAGMENT_SHADER"),	# 0x8B30
    ("glGetProgramPipeline",	I,	1,	"GL_VERTEX_SHADER"),	# 0x8B31
    ("",	H,	1,	"GL_PROGRAM_OBJECT_ARB"),	# 0x8B40
    ("",	X,	1,	"GL_SHADER_OBJECT_ARB"),	# 0x8B48
    ("glGet",	I,	1,	"GL_MAX_FRAGMENT_UNIFORM_COMPONENTS"),	# 0x8B49
    ("glGet",	I,	1,	"GL_MAX_VERTEX_UNIFORM_COMPONENTS"),	# 0x8B4A
    ("glGet",	I,	1,	"GL_MAX_VARYING_COMPONENTS"),	# 0x8B4B
    ("glGet",	I,	1,	"GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS"),	# 0x8B4C
    ("glGet",	I,	1,	"GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS"),	# 0x8B4D
    ("",	E,	1,	"GL_OBJECT_TYPE_ARB"),	# 0x8B4E
    ("glGetShader",	E,	1,	"GL_SHADER_TYPE"),	# 0x8B4F
    ("",	X,	1,	"GL_FLOAT_VEC2"),	# 0x8B50
    ("",	X,	1,	"GL_FLOAT_VEC3"),	# 0x8B51
    ("",	X,	1,	"GL_FLOAT_VEC4"),	# 0x8B52
    ("",	X,	1,	"GL_INT_VEC2"),	# 0x8B53
    ("",	X,	1,	"GL_INT_VEC3"),	# 0x8B54
    ("",	X,	1,	"GL_INT_VEC4"),	# 0x8B55
    ("",	X,	1,	"GL_BOOL"),	# 0x8B56
    ("",	X,	1,	"GL_BOOL_VEC2"),	# 0x8B57
    ("",	X,	1,	"GL_BOOL_VEC3"),	# 0x8B58
    ("",	X,	1,	"GL_BOOL_VEC4"),	# 0x8B59
    ("",	X,	1,	"GL_FLOAT_MAT2"),	# 0x8B5A
    ("",	X,	1,	"GL_FLOAT_MAT3"),	# 0x8B5B
    ("",	X,	1,	"GL_FLOAT_MAT4"),	# 0x8B5C
    ("",	X,	1,	"GL_SAMPLER_1D"),	# 0x8B5D
    ("",	X,	1,	"GL_SAMPLER_2D"),	# 0x8B5E
    ("",	X,	1,	"GL_SAMPLER_3D"),	# 0x8B5F
    ("",	X,	1,	"GL_SAMPLER_CUBE"),	# 0x8B60
    ("",	X,	1,	"GL_SAMPLER_1D_SHADOW"),	# 0x8B61
    ("",	X,	1,	"GL_SAMPLER_2D_SHADOW"),	# 0x8B62
    ("",	X,	1,	"GL_SAMPLER_2D_RECT"),	# 0x8B63
    ("",	X,	1,	"GL_SAMPLER_2D_RECT_SHADOW"),	# 0x8B64
    ("",	X,	1,	"GL_FLOAT_MAT2x3"),	# 0x8B65
    ("",	X,	1,	"GL_FLOAT_MAT2x4"),	# 0x8B66
    ("",	X,	1,	"GL_FLOAT_MAT3x2"),	# 0x8B67
    ("",	X,	1,	"GL_FLOAT_MAT3x4"),	# 0x8B68
    ("",	X,	1,	"GL_FLOAT_MAT4x2"),	# 0x8B69
    ("",	X,	1,	"GL_FLOAT_MAT4x3"),	# 0x8B6A
    ("glGetShader,glGetProgram",	B,	1,	"GL_DELETE_STATUS"),	# 0x8B80
    ("glGetShader,glGetProgram",	B,	1,	"GL_COMPILE_STATUS"),	# 0x8B81
    ("glGetShader,glGetProgram",	B,	1,	"GL_LINK_STATUS"),	# 0x8B82
    ("glGetShader,glGetProgram",	B,	1,	"GL_VALIDATE_STATUS"),	# 0x8B83
    ("glGetShader,glGetProgram",	I,	1,	"GL_INFO_LOG_LENGTH"),	# 0x8B84
    ("glGetProgram",	I,	1,	"GL_ATTACHED_SHADERS"),	# 0x8B85
    ("glGetProgram",	I,	1,	"GL_ACTIVE_UNIFORMS"),	# 0x8B86
    ("glGetProgram",	I,	1,	"GL_ACTIVE_UNIFORM_MAX_LENGTH"),	# 0x8B87
    ("glGetShader",	I,	1,	"GL_SHADER_SOURCE_LENGTH"),	# 0x8B88
    ("glGetProgram",	I,	1,	"GL_ACTIVE_ATTRIBUTES"),	# 0x8B89
    ("glGetProgram",	I,	1,	"GL_ACTIVE_ATTRIBUTE_MAX_LENGTH"),	# 0x8B8A
    ("",	X,	1,	"GL_FRAGMENT_SHADER_DERIVATIVE_HINT"),	# 0x8B8B
    ("glGet",	S,	1,	"GL_SHADING_LANGUAGE_VERSION"),	# 0x8B8C
    ("glGet",	I,	1,	"GL_CURRENT_PROGRAM"),	# 0x8B8D
    ("",	X,	1,	"GL_PALETTE4_RGB8_OES"),	# 0x8B90
    ("",	X,	1,	"GL_PALETTE4_RGBA8_OES"),	# 0x8B91
    ("",	X,	1,	"GL_PALETTE4_R5_G6_B5_OES"),	# 0x8B92
    ("",	X,	1,	"GL_PALETTE4_RGBA4_OES"),	# 0x8B93
    ("",	X,	1,	"GL_PALETTE4_RGB5_A1_OES"),	# 0x8B94
    ("",	X,	1,	"GL_PALETTE8_RGB8_OES"),	# 0x8B95
    ("",	X,	1,	"GL_PALETTE8_RGBA8_OES"),	# 0x8B96
    ("",	X,	1,	"GL_PALETTE8_R5_G6_B5_OES"),	# 0x8B97
    ("",	X,	1,	"GL_PALETTE8_RGBA4_OES"),	# 0x8B98
    ("",	X,	1,	"GL_PALETTE8_RGB5_A1_OES"),	# 0x8B99
    ("glGet",	E,	1,	"GL_IMPLEMENTATION_COLOR_READ_TYPE"),	# 0x8B9A
    ("glGet",	E,	1,	"GL_IMPLEMENTATION_COLOR_READ_FORMAT"),	# 0x8B9B
    ("",	X,	1,	"GL_POINT_SIZE_ARRAY_OES"),	# 0x8B9C
    ("glGetTexParameter",	I,	4,	"GL_TEXTURE_CROP_RECT_OES"),	# 0x8B9D
    ("glGet",	I,	1,	"GL_MATRIX_INDEX_ARRAY_BUFFER_BINDING_OES"),	# 0x8B9E
    ("",	I,	1,	"GL_POINT_SIZE_ARRAY_BUFFER_BINDING_OES"),	# 0x8B9F
    #("",	X,	1,	"GL_FRAGMENT_PROGRAM_POSITION_MESA"),	# 0x8BB0
    #("",	X,	1,	"GL_FRAGMENT_PROGRAM_CALLBACK_MESA"),	# 0x8BB1
    #("",	X,	1,	"GL_FRAGMENT_PROGRAM_CALLBACK_FUNC_MESA"),	# 0x8BB2
    #("",	X,	1,	"GL_FRAGMENT_PROGRAM_CALLBACK_DATA_MESA"),	# 0x8BB3
    #("",	X,	1,	"GL_VERTEX_PROGRAM_POSITION_MESA"),	# 0x8BB4
    #("",	X,	1,	"GL_VERTEX_PROGRAM_CALLBACK_MESA"),	# 0x8BB5
    #("",	X,	1,	"GL_VERTEX_PROGRAM_CALLBACK_FUNC_MESA"),	# 0x8BB6
    #("",	X,	1,	"GL_VERTEX_PROGRAM_CALLBACK_DATA_MESA"),	# 0x8BB7
    ("",	X,	1,	"GL_COUNTER_TYPE_AMD"),	# 0x8BC0
    ("",	X,	1,	"GL_COUNTER_RANGE_AMD"),	# 0x8BC1
    ("",	X,	1,	"GL_UNSIGNED_INT64_AMD"),	# 0x8BC2
    ("",	X,	1,	"GL_PERCENTAGE_AMD"),	# 0x8BC3
    ("",	X,	1,	"GL_PERFMON_RESULT_AVAILABLE_AMD"),	# 0x8BC4
    ("",	X,	1,	"GL_PERFMON_RESULT_SIZE_AMD"),	# 0x8BC5
    ("",	X,	1,	"GL_PERFMON_RESULT_AMD"),	# 0x8BC6
    #("",	X,	1,	"GL_TEXTURE_WIDTH_QCOM"),	# 0x8BD2
    #("",	X,	1,	"GL_TEXTURE_HEIGHT_QCOM"),	# 0x8BD3
    #("",	X,	1,	"GL_TEXTURE_DEPTH_QCOM"),	# 0x8BD4
    #("",	X,	1,	"GL_TEXTURE_INTERNAL_FORMAT_QCOM"),	# 0x8BD5
    #("",	X,	1,	"GL_TEXTURE_FORMAT_QCOM"),	# 0x8BD6
    #("",	X,	1,	"GL_TEXTURE_TYPE_QCOM"),	# 0x8BD7
    #("",	X,	1,	"GL_TEXTURE_IMAGE_VALID_QCOM"),	# 0x8BD8
    #("",	X,	1,	"GL_TEXTURE_NUM_LEVELS_QCOM"),	# 0x8BD9
    #("",	X,	1,	"GL_TEXTURE_TARGET_QCOM"),	# 0x8BDA
    #("",	X,	1,	"GL_TEXTURE_OBJECT_VALID_QCOM"),	# 0x8BDB
    #("",	X,	1,	"GL_STATE_RESTORE"),	# 0x8BDC
    #("",	X,	1,	"GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG"),	# 0x8C00
    #("",	X,	1,	"GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG"),	# 0x8C01
    #("",	X,	1,	"GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG"),	# 0x8C02
    #("",	X,	1,	"GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG"),	# 0x8C03
    #("",	X,	1,	"GL_MODULATE_COLOR_IMG"),	# 0x8C04
    #("",	X,	1,	"GL_RECIP_ADD_SIGNED_ALPHA_IMG"),	# 0x8C05
    #("",	X,	1,	"GL_TEXTURE_ALPHA_MODULATE_IMG"),	# 0x8C06
    #("",	X,	1,	"GL_FACTOR_ALPHA_MODULATE_IMG"),	# 0x8C07
    #("",	X,	1,	"GL_FRAGMENT_ALPHA_MODULATE_IMG"),	# 0x8C08
    #("",	X,	1,	"GL_ADD_BLEND_IMG"),	# 0x8C09
    #("",	X,	1,	"GL_SGX_BINARY_IMG"),	# 0x8C0A
    ("glGetTexLevelParameter",	E,	1,	"GL_TEXTURE_RED_TYPE"),	# 0x8C10
    ("glGetTexLevelParameter",	E,	1,	"GL_TEXTURE_GREEN_TYPE"),	# 0x8C11
    ("glGetTexLevelParameter",	E,	1,	"GL_TEXTURE_BLUE_TYPE"),	# 0x8C12
    ("glGetTexLevelParameter",	E,	1,	"GL_TEXTURE_ALPHA_TYPE"),	# 0x8C13
    ("glGetTexLevelParameter",	E,	1,	"GL_TEXTURE_LUMINANCE_TYPE"),	# 0x8C14
    ("glGetTexLevelParameter",	E,	1,	"GL_TEXTURE_INTENSITY_TYPE"),	# 0x8C15
    ("glGetTexLevelParameter",	E,	1,	"GL_TEXTURE_DEPTH_TYPE"),	# 0x8C16
    ("",	X,	1,	"GL_UNSIGNED_NORMALIZED"),	# 0x8C17
    ("",	B,	1,	"GL_TEXTURE_1D_ARRAY"),	# 0x8C18
    ("",	X,	1,	"GL_PROXY_TEXTURE_1D_ARRAY"),	# 0x8C19
    ("",	B,	1,	"GL_TEXTURE_2D_ARRAY"),	# 0x8C1A
    ("",	X,	1,	"GL_PROXY_TEXTURE_2D_ARRAY"),	# 0x8C1B
    ("glGet_texture",	I,	1,	"GL_TEXTURE_BINDING_1D_ARRAY"),	# 0x8C1C
    ("glGet_texture",	I,	1,	"GL_TEXTURE_BINDING_2D_ARRAY"),	# 0x8C1D
    ("",	X,	1,	"GL_GEOMETRY_PROGRAM_NV"),	# 0x8C26
    ("glGetProgramARB",	I,	1,	"GL_MAX_PROGRAM_OUTPUT_VERTICES_NV"),	# 0x8C27
    ("glGetProgramARB",	I,	1,	"GL_MAX_PROGRAM_TOTAL_OUTPUT_COMPONENTS_NV"),	# 0x8C28
    ("glGet",	I,	1,	"GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS"),	# 0x8C29
    ("glGet_texture",	I,	1,	"GL_TEXTURE_BUFFER"),	# 0x8C2A
    ("glGet",	I,	1,	"GL_MAX_TEXTURE_BUFFER_SIZE"),	# 0x8C2B
    ("glGet_texture",	I,	1,	"GL_TEXTURE_BINDING_BUFFER"),	# 0x8C2C
    ("glGet_texture",	I,	1,	"GL_TEXTURE_BUFFER_DATA_STORE_BINDING"),	# 0x8C2D
    ("glGet_texture",	E,	1,	"GL_TEXTURE_BUFFER_FORMAT_ARB"),	# 0x8C2E
    ("",	B,	1,	"GL_ANY_SAMPLES_PASSED"),	# 0x8C2F
    ("glGet",	B,	1,	"GL_SAMPLE_SHADING"),	# 0x8C36
    ("glGet",	F,	1,	"GL_MIN_SAMPLE_SHADING_VALUE"),	# 0x8C37
    ("",	X,	1,	"GL_R11F_G11F_B10F"),	# 0x8C3A
    ("",	X,	1,	"GL_UNSIGNED_INT_10F_11F_11F_REV"),	# 0x8C3B
    ("",	X,	1,	"GL_RGBA_SIGNED_COMPONENTS_EXT"),	# 0x8C3C
    ("",	X,	1,	"GL_RGB9_E5"),	# 0x8C3D
    ("",	X,	1,	"GL_UNSIGNED_INT_5_9_9_9_REV"),	# 0x8C3E
    ("",	X,	1,	"GL_TEXTURE_SHARED_SIZE"),	# 0x8C3F
    ("",	X,	1,	"GL_SRGB"),	# 0x8C40
    ("",	X,	1,	"GL_SRGB8"),	# 0x8C41
    ("",	X,	1,	"GL_SRGB_ALPHA"),	# 0x8C42
    ("",	X,	1,	"GL_SRGB8_ALPHA8"),	# 0x8C43
    ("",	X,	1,	"GL_SLUMINANCE_ALPHA"),	# 0x8C44
    ("",	X,	1,	"GL_SLUMINANCE8_ALPHA8"),	# 0x8C45
    ("",	X,	1,	"GL_SLUMINANCE"),	# 0x8C46
    ("",	X,	1,	"GL_SLUMINANCE8"),	# 0x8C47
    ("",	X,	1,	"GL_COMPRESSED_SRGB"),	# 0x8C48
    ("",	X,	1,	"GL_COMPRESSED_SRGB_ALPHA"),	# 0x8C49
    ("",	X,	1,	"GL_COMPRESSED_SLUMINANCE"),	# 0x8C4A
    ("",	X,	1,	"GL_COMPRESSED_SLUMINANCE_ALPHA"),	# 0x8C4B
    ("",	X,	1,	"GL_COMPRESSED_SRGB_S3TC_DXT1_EXT"),	# 0x8C4C
    ("",	X,	1,	"GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT"),	# 0x8C4D
    ("",	X,	1,	"GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT"),	# 0x8C4E
    ("",	X,	1,	"GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT"),	# 0x8C4F
    ("",	X,	1,	"GL_COMPRESSED_LUMINANCE_LATC1_EXT"),	# 0x8C70
    ("",	X,	1,	"GL_COMPRESSED_SIGNED_LUMINANCE_LATC1_EXT"),	# 0x8C71
    ("",	X,	1,	"GL_COMPRESSED_LUMINANCE_ALPHA_LATC2_EXT"),	# 0x8C72
    ("",	X,	1,	"GL_COMPRESSED_SIGNED_LUMINANCE_ALPHA_LATC2_EXT"),	# 0x8C73
    ("",	X,	1,	"GL_TESS_CONTROL_PROGRAM_PARAMETER_BUFFER_NV"),	# 0x8C74
    ("",	X,	1,	"GL_TESS_EVALUATION_PROGRAM_PARAMETER_BUFFER_NV"),	# 0x8C75
    ("glGetProgram",	I,	1,	"GL_TRANSFORM_FEEDBACK_VARYING_MAX_LENGTH"),	# 0x8C76
    ("",	X,	1,	"GL_BACK_PRIMARY_COLOR_NV"),	# 0x8C77
    ("",	X,	1,	"GL_BACK_SECONDARY_COLOR_NV"),	# 0x8C78
    ("",	X,	1,	"GL_TEXTURE_COORD_NV"),	# 0x8C79
    ("",	X,	1,	"GL_CLIP_DISTANCE_NV"),	# 0x8C7A
    ("",	X,	1,	"GL_VERTEX_ID_NV"),	# 0x8C7B
    ("",	X,	1,	"GL_PRIMITIVE_ID_NV"),	# 0x8C7C
    ("",	X,	1,	"GL_GENERIC_ATTRIB_NV"),	# 0x8C7D
    ("",	X,	1,	"GL_TRANSFORM_FEEDBACK_ATTRIBS_NV"),	# 0x8C7E
    ("glGetProgram",	E,	1,	"GL_TRANSFORM_FEEDBACK_BUFFER_MODE"),	# 0x8C7F
    ("glGet",	I,	1,	"GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_COMPONENTS"),	# 0x8C80
    ("",	X,	1,	"GL_ACTIVE_VARYINGS_NV"),	# 0x8C81
    ("",	X,	1,	"GL_ACTIVE_VARYING_MAX_LENGTH_NV"),	# 0x8C82
    ("glGetProgram",	I,	1,	"GL_TRANSFORM_FEEDBACK_VARYINGS"),	# 0x8C83
    ("glGet_i",	I,	1,	"GL_TRANSFORM_FEEDBACK_BUFFER_START"),	# 0x8C84
    ("glGet_i",	I,	1,	"GL_TRANSFORM_FEEDBACK_BUFFER_SIZE"),	# 0x8C85
    ("",	X,	1,	"GL_TRANSFORM_FEEDBACK_RECORD_NV"),	# 0x8C86
    ("",	X,	1,	"GL_PRIMITIVES_GENERATED"),	# 0x8C87
    ("",	X,	1,	"GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN"),	# 0x8C88
    ("glGet",	B,	1,	"GL_RASTERIZER_DISCARD"),	# 0x8C89
    ("glGet",	I,	1,	"GL_MAX_TRANSFORM_FEEDBACK_INTERLEAVED_COMPONENTS"),	# 0x8C8A
    ("glGet",	I,	1,	"GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS"),	# 0x8C8B
    ("",	X,	1,	"GL_INTERLEAVED_ATTRIBS"),	# 0x8C8C
    ("",	X,	1,	"GL_SEPARATE_ATTRIBS"),	# 0x8C8D
    ("",	X,	1,	"GL_TRANSFORM_FEEDBACK_BUFFER"),	# 0x8C8E
    ("glGet,glGet_i",	I,	1,	"GL_TRANSFORM_FEEDBACK_BUFFER_BINDING"),	# 0x8C8F
    ("",	X,	1,	"GL_ATC_RGB_AMD"),	# 0x8C92
    ("",	X,	1,	"GL_ATC_RGBA_EXPLICIT_ALPHA_AMD"),	# 0x8C93
    ("glGet",	E,	1,	"GL_POINT_SPRITE_COORD_ORIGIN"),	# 0x8CA0
    ("",	X,	1,	"GL_LOWER_LEFT"),	# 0x8CA1
    ("",	X,	1,	"GL_UPPER_LEFT"),	# 0x8CA2
    ("",	X,	1,	"GL_STENCIL_BACK_REF"),	# 0x8CA3
    ("",	X,	1,	"GL_STENCIL_BACK_VALUE_MASK"),	# 0x8CA4
    ("",	X,	1,	"GL_STENCIL_BACK_WRITEMASK"),	# 0x8CA5
    ("glGet",	I,	1,	"GL_DRAW_FRAMEBUFFER_BINDING"),	# 0x8CA6
    ("glGet",	I,	1,	"GL_RENDERBUFFER_BINDING"),	# 0x8CA7
    ("",	I,	1,	"GL_READ_FRAMEBUFFER"),	# 0x8CA8
    ("",	I,	1,	"GL_DRAW_FRAMEBUFFER"),	# 0x8CA9
    ("glGet",	I,	1,	"GL_READ_FRAMEBUFFER_BINDING"),	# 0x8CAA
    ("glGetRenderbufferParameter",	I,	1,	"GL_RENDERBUFFER_SAMPLES"),	# 0x8CAB
    ("",	X,	1,	"GL_DEPTH_COMPONENT32F"),	# 0x8CAC
    ("",	X,	1,	"GL_DEPTH32F_STENCIL8"),	# 0x8CAD
    ("glGetFramebufferAttachmentParameter",	E,	1,	"GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE"),	# 0x8CD0
    ("glGetFramebufferAttachmentParameter",	I,	1,	"GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME"),	# 0x8CD1
    ("glGetFramebufferAttachmentParameter",	I,	1,	"GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL"),	# 0x8CD2
    ("glGetFramebufferAttachmentParameter",	E,	1,	"GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE"),	# 0x8CD3
    ("glGetFramebufferAttachmentParameter",	I,	1,	"GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LAYER"),	# 0x8CD4
    ("",	X,	1,	"GL_FRAMEBUFFER_COMPLETE"),	# 0x8CD5
    ("",	X,	1,	"GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT"),	# 0x8CD6
    ("",	X,	1,	"GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT"),	# 0x8CD7
    ("",	X,	1,	"GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT"),	# 0x8CD9
    ("",	X,	1,	"GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT"),	# 0x8CDA
    ("",	X,	1,	"GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER"),	# 0x8CDB
    ("",	X,	1,	"GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER"),	# 0x8CDC
    ("",	X,	1,	"GL_FRAMEBUFFER_UNSUPPORTED"),	# 0x8CDD
    ("glGet",	I,	1,	"GL_MAX_COLOR_ATTACHMENTS"),	# 0x8CDF
    ("",	X,	1,	"GL_COLOR_ATTACHMENT0"),	# 0x8CE0
    ("",	X,	1,	"GL_COLOR_ATTACHMENT1"),	# 0x8CE1
    ("",	X,	1,	"GL_COLOR_ATTACHMENT2"),	# 0x8CE2
    ("",	X,	1,	"GL_COLOR_ATTACHMENT3"),	# 0x8CE3
    ("",	X,	1,	"GL_COLOR_ATTACHMENT4"),	# 0x8CE4
    ("",	X,	1,	"GL_COLOR_ATTACHMENT5"),	# 0x8CE5
    ("",	X,	1,	"GL_COLOR_ATTACHMENT6"),	# 0x8CE6
    ("",	X,	1,	"GL_COLOR_ATTACHMENT7"),	# 0x8CE7
    ("",	X,	1,	"GL_COLOR_ATTACHMENT8"),	# 0x8CE8
    ("",	X,	1,	"GL_COLOR_ATTACHMENT9"),	# 0x8CE9
    ("",	X,	1,	"GL_COLOR_ATTACHMENT10"),	# 0x8CEA
    ("",	X,	1,	"GL_COLOR_ATTACHMENT11"),	# 0x8CEB
    ("",	X,	1,	"GL_COLOR_ATTACHMENT12"),	# 0x8CEC
    ("",	X,	1,	"GL_COLOR_ATTACHMENT13"),	# 0x8CED
    ("",	X,	1,	"GL_COLOR_ATTACHMENT14"),	# 0x8CEE
    ("",	X,	1,	"GL_COLOR_ATTACHMENT15"),	# 0x8CEF
    ("",	X,	1,	"GL_COLOR_ATTACHMENT16"),	# 0x8CF0
    ("",	X,	1,	"GL_COLOR_ATTACHMENT17"),	# 0x8CF1
    ("",	X,	1,	"GL_COLOR_ATTACHMENT18"),	# 0x8CF2
    ("",	X,	1,	"GL_COLOR_ATTACHMENT19"),	# 0x8CF3
    ("",	X,	1,	"GL_COLOR_ATTACHMENT20"),	# 0x8CF4
    ("",	X,	1,	"GL_COLOR_ATTACHMENT21"),	# 0x8CF5
    ("",	X,	1,	"GL_COLOR_ATTACHMENT22"),	# 0x8CF6
    ("",	X,	1,	"GL_COLOR_ATTACHMENT23"),	# 0x8CF7
    ("",	X,	1,	"GL_COLOR_ATTACHMENT24"),	# 0x8CF8
    ("",	X,	1,	"GL_COLOR_ATTACHMENT25"),	# 0x8CF9
    ("",	X,	1,	"GL_COLOR_ATTACHMENT26"),	# 0x8CFA
    ("",	X,	1,	"GL_COLOR_ATTACHMENT27"),	# 0x8CFB
    ("",	X,	1,	"GL_COLOR_ATTACHMENT28"),	# 0x8CFC
    ("",	X,	1,	"GL_COLOR_ATTACHMENT29"),	# 0x8CFD
    ("",	X,	1,	"GL_COLOR_ATTACHMENT30"),	# 0x8CFE
    ("",	X,	1,	"GL_COLOR_ATTACHMENT31"),	# 0x8CFF
    ("",	X,	1,	"GL_DEPTH_ATTACHMENT"),	# 0x8D00
    ("",	X,	1,	"GL_STENCIL_ATTACHMENT"),	# 0x8D20
    ("",	X,	1,	"GL_FRAMEBUFFER"),	# 0x8D40
    ("",	X,	1,	"GL_RENDERBUFFER"),	# 0x8D41
    ("glGetRenderbufferParameter",	I,	1,	"GL_RENDERBUFFER_WIDTH"),	# 0x8D42
    ("glGetRenderbufferParameter",	I,	1,	"GL_RENDERBUFFER_HEIGHT"),	# 0x8D43
    ("glGetRenderbufferParameter",	E,	1,	"GL_RENDERBUFFER_INTERNAL_FORMAT"),	# 0x8D44
    ("",	X,	1,	"GL_STENCIL_INDEX1"),	# 0x8D46
    ("",	X,	1,	"GL_STENCIL_INDEX4"),	# 0x8D47
    ("",	X,	1,	"GL_STENCIL_INDEX8"),	# 0x8D48
    ("",	X,	1,	"GL_STENCIL_INDEX16"),	# 0x8D49
    ("glGetRenderbufferParameter",	I,	1,	"GL_RENDERBUFFER_RED_SIZE"),	# 0x8D50
    ("glGetRenderbufferParameter",	I,	1,	"GL_RENDERBUFFER_GREEN_SIZE"),	# 0x8D51
    ("glGetRenderbufferParameter",	I,	1,	"GL_RENDERBUFFER_BLUE_SIZE"),	# 0x8D52
    ("glGetRenderbufferParameter",	I,	1,	"GL_RENDERBUFFER_ALPHA_SIZE"),	# 0x8D53
    ("glGetRenderbufferParameter",	I,	1,	"GL_RENDERBUFFER_DEPTH_SIZE"),	# 0x8D54
    ("glGetRenderbufferParameter",	I,	1,	"GL_RENDERBUFFER_STENCIL_SIZE"),	# 0x8D55
    ("",	X,	1,	"GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE"),	# 0x8D56
    ("glGet",	I,	1,	"GL_MAX_SAMPLES"),	# 0x8D57
    ("",	X,	1,	"GL_TEXTURE_GEN_STR_OES"),	# 0x8D60
    ("",	X,	1,	"GL_HALF_FLOAT_OES"),	# 0x8D61
    ("",	X,	1,	"GL_RGB565_OES"),	# 0x8D62
    ("",	X,	1,	"GL_ETC1_RGB8_OES"),	# 0x8D64
    ("",	X,	1,	"GL_TEXTURE_EXTERNAL_OES"),	# 0x8D65
    ("",	X,	1,	"GL_SAMPLER_EXTERNAL_OES"),	# 0x8D66
    ("",	X,	1,	"GL_TEXTURE_BINDING_EXTERNAL_OES"),	# 0x8D67
    ("",	X,	1,	"GL_REQUIRED_TEXTURE_IMAGE_UNITS_OES"),	# 0x8D68
    ("glGet",	B,	1,	"GL_PRIMITIVE_RESTART_FIXED_INDEX"),	# 0x8D69
    ("",	B,	1,	"GL_ANY_SAMPLES_PASSED_CONSERVATIVE"),	# 0x8D6A
    ("glGet",	I,	1,	"GL_MAX_ELEMENT_INDEX"),	# 0x8D6B
    ("",	X,	1,	"GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_SAMPLES_EXT"),	# 0x8D6C
    ("",	X,	1,	"GL_RGBA32UI"),	# 0x8D70
    ("",	X,	1,	"GL_RGB32UI"),	# 0x8D71
    ("",	X,	1,	"GL_ALPHA32UI_EXT"),	# 0x8D72
    ("",	X,	1,	"GL_INTENSITY32UI_EXT"),	# 0x8D73
    ("",	X,	1,	"GL_LUMINANCE32UI_EXT"),	# 0x8D74
    ("",	X,	1,	"GL_LUMINANCE_ALPHA32UI_EXT"),	# 0x8D75
    ("",	X,	1,	"GL_RGBA16UI"),	# 0x8D76
    ("",	X,	1,	"GL_RGB16UI"),	# 0x8D77
    ("",	X,	1,	"GL_ALPHA16UI_EXT"),	# 0x8D78
    ("",	X,	1,	"GL_INTENSITY16UI_EXT"),	# 0x8D79
    ("",	X,	1,	"GL_LUMINANCE16UI_EXT"),	# 0x8D7A
    ("",	X,	1,	"GL_LUMINANCE_ALPHA16UI_EXT"),	# 0x8D7B
    ("",	X,	1,	"GL_RGBA8UI"),	# 0x8D7C
    ("",	X,	1,	"GL_RGB8UI"),	# 0x8D7D
    ("",	X,	1,	"GL_ALPHA8UI_EXT"),	# 0x8D7E
    ("",	X,	1,	"GL_INTENSITY8UI_EXT"),	# 0x8D7F
    ("",	X,	1,	"GL_LUMINANCE8UI_EXT"),	# 0x8D80
    ("",	X,	1,	"GL_LUMINANCE_ALPHA8UI_EXT"),	# 0x8D81
    ("",	X,	1,	"GL_RGBA32I"),	# 0x8D82
    ("",	X,	1,	"GL_RGB32I"),	# 0x8D83
    ("",	X,	1,	"GL_ALPHA32I_EXT"),	# 0x8D84
    ("",	X,	1,	"GL_INTENSITY32I_EXT"),	# 0x8D85
    ("",	X,	1,	"GL_LUMINANCE32I_EXT"),	# 0x8D86
    ("",	X,	1,	"GL_LUMINANCE_ALPHA32I_EXT"),	# 0x8D87
    ("",	X,	1,	"GL_RGBA16I"),	# 0x8D88
    ("",	X,	1,	"GL_RGB16I"),	# 0x8D89
    ("",	X,	1,	"GL_ALPHA16I_EXT"),	# 0x8D8A
    ("",	X,	1,	"GL_INTENSITY16I_EXT"),	# 0x8D8B
    ("",	X,	1,	"GL_LUMINANCE16I_EXT"),	# 0x8D8C
    ("",	X,	1,	"GL_LUMINANCE_ALPHA16I_EXT"),	# 0x8D8D
    ("",	X,	1,	"GL_RGBA8I"),	# 0x8D8E
    ("",	X,	1,	"GL_RGB8I"),	# 0x8D8F
    ("",	X,	1,	"GL_ALPHA8I_EXT"),	# 0x8D90
    ("",	X,	1,	"GL_INTENSITY8I_EXT"),	# 0x8D91
    ("",	X,	1,	"GL_LUMINANCE8I_EXT"),	# 0x8D92
    ("",	X,	1,	"GL_LUMINANCE_ALPHA8I_EXT"),	# 0x8D93
    ("",	X,	1,	"GL_RED_INTEGER"),	# 0x8D94
    ("",	X,	1,	"GL_GREEN_INTEGER"),	# 0x8D95
    ("",	X,	1,	"GL_BLUE_INTEGER"),	# 0x8D96
    ("",	X,	1,	"GL_ALPHA_INTEGER"),	# 0x8D97
    ("",	X,	1,	"GL_RGB_INTEGER"),	# 0x8D98
    ("",	X,	1,	"GL_RGBA_INTEGER"),	# 0x8D99
    ("",	X,	1,	"GL_BGR_INTEGER"),	# 0x8D9A
    ("",	X,	1,	"GL_BGRA_INTEGER"),	# 0x8D9B
    ("",	X,	1,	"GL_LUMINANCE_INTEGER_EXT"),	# 0x8D9C
    ("",	X,	1,	"GL_LUMINANCE_ALPHA_INTEGER_EXT"),	# 0x8D9D
    ("glGet",	B,	1,	"GL_RGBA_INTEGER_MODE_EXT"),	# 0x8D9E
    ("",	X,	1,	"GL_INT_2_10_10_10_REV"),	# 0x8D9F
    ("glGetProgramARB",	I,	1,	"GL_MAX_PROGRAM_PARAMETER_BUFFER_BINDINGS_NV"),	# 0x8DA0
    ("glGetProgramARB",	I,	1,	"GL_MAX_PROGRAM_PARAMETER_BUFFER_SIZE_NV"),	# 0x8DA1
    ("",	X,	1,	"GL_VERTEX_PROGRAM_PARAMETER_BUFFER_NV"),	# 0x8DA2
    ("",	X,	1,	"GL_GEOMETRY_PROGRAM_PARAMETER_BUFFER_NV"),	# 0x8DA3
    ("",	X,	1,	"GL_FRAGMENT_PROGRAM_PARAMETER_BUFFER_NV"),	# 0x8DA4
    ("glGetProgramARB",	I,	1,	"GL_MAX_PROGRAM_GENERIC_ATTRIBS_NV"),	# 0x8DA5
    ("glGetProgramARB",	I,	1,	"GL_MAX_PROGRAM_GENERIC_RESULTS_NV"),	# 0x8DA6
    ("glGetFramebufferAttachmentParameter",	B,	1,	"GL_FRAMEBUFFER_ATTACHMENT_LAYERED"),	# 0x8DA7
    ("",	X,	1,	"GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS"),	# 0x8DA8
    ("",	X,	1,	"GL_FRAMEBUFFER_INCOMPLETE_LAYER_COUNT_ARB"),	# 0x8DA9
    ("",	X,	1,	"GL_LAYER_NV"),	# 0x8DAA
    ("",	X,	1,	"GL_DEPTH_COMPONENT32F_NV"),	# 0x8DAB
    ("",	X,	1,	"GL_DEPTH32F_STENCIL8_NV"),	# 0x8DAC
    ("",	X,	1,	"GL_FLOAT_32_UNSIGNED_INT_24_8_REV"),	# 0x8DAD
    ("",	X,	1,	"GL_SHADER_INCLUDE_ARB"),	# 0x8DAE
    ("",	X,	1,	"GL_DEPTH_BUFFER_FLOAT_MODE_NV"),	# 0x8DAF
    ("glGet",	B,	1,	"GL_FRAMEBUFFER_SRGB"),	# 0x8DB9
    ("glGet",	B,	1,	"GL_FRAMEBUFFER_SRGB_CAPABLE_EXT"),	# 0x8DBA
    ("",	X,	1,	"GL_COMPRESSED_RED_RGTC1"),	# 0x8DBB
    ("",	X,	1,	"GL_COMPRESSED_SIGNED_RED_RGTC1"),	# 0x8DBC
    ("",	X,	1,	"GL_COMPRESSED_RG_RGTC2"),	# 0x8DBD
    ("",	X,	1,	"GL_COMPRESSED_SIGNED_RG_RGTC2"),	# 0x8DBE
    ("",	X,	1,	"GL_SAMPLER_1D_ARRAY"),	# 0x8DC0
    ("",	X,	1,	"GL_SAMPLER_2D_ARRAY"),	# 0x8DC1
    ("",	X,	1,	"GL_SAMPLER_BUFFER"),	# 0x8DC2
    ("",	X,	1,	"GL_SAMPLER_1D_ARRAY_SHADOW"),	# 0x8DC3
    ("",	X,	1,	"GL_SAMPLER_2D_ARRAY_SHADOW"),	# 0x8DC4
    ("",	X,	1,	"GL_SAMPLER_CUBE_SHADOW"),	# 0x8DC5
    ("",	X,	1,	"GL_UNSIGNED_INT_VEC2"),	# 0x8DC6
    ("",	X,	1,	"GL_UNSIGNED_INT_VEC3"),	# 0x8DC7
    ("",	X,	1,	"GL_UNSIGNED_INT_VEC4"),	# 0x8DC8
    ("",	X,	1,	"GL_INT_SAMPLER_1D"),	# 0x8DC9
    ("",	X,	1,	"GL_INT_SAMPLER_2D"),	# 0x8DCA
    ("",	X,	1,	"GL_INT_SAMPLER_3D"),	# 0x8DCB
    ("",	X,	1,	"GL_INT_SAMPLER_CUBE"),	# 0x8DCC
    ("",	X,	1,	"GL_INT_SAMPLER_2D_RECT"),	# 0x8DCD
    ("",	X,	1,	"GL_INT_SAMPLER_1D_ARRAY"),	# 0x8DCE
    ("",	X,	1,	"GL_INT_SAMPLER_2D_ARRAY"),	# 0x8DCF
    ("",	X,	1,	"GL_INT_SAMPLER_BUFFER"),	# 0x8DD0
    ("",	X,	1,	"GL_UNSIGNED_INT_SAMPLER_1D"),	# 0x8DD1
    ("",	X,	1,	"GL_UNSIGNED_INT_SAMPLER_2D"),	# 0x8DD2
    ("",	X,	1,	"GL_UNSIGNED_INT_SAMPLER_3D"),	# 0x8DD3
    ("",	X,	1,	"GL_UNSIGNED_INT_SAMPLER_CUBE"),	# 0x8DD4
    ("",	X,	1,	"GL_UNSIGNED_INT_SAMPLER_2D_RECT"),	# 0x8DD5
    ("",	X,	1,	"GL_UNSIGNED_INT_SAMPLER_1D_ARRAY"),	# 0x8DD6
    ("",	X,	1,	"GL_UNSIGNED_INT_SAMPLER_2D_ARRAY"),	# 0x8DD7
    ("",	X,	1,	"GL_UNSIGNED_INT_SAMPLER_BUFFER"),	# 0x8DD8
    ("glGetProgramPipeline",	I,	1,	"GL_GEOMETRY_SHADER"),	# 0x8DD9
    ("glGetProgram",	I,	1,	"GL_GEOMETRY_VERTICES_OUT_ARB"),	# 0x8DDA
    ("glGetProgram",	E,	1,	"GL_GEOMETRY_INPUT_TYPE_ARB"),	# 0x8DDB
    ("glGetProgram",	E,	1,	"GL_GEOMETRY_OUTPUT_TYPE_ARB"),	# 0x8DDC
    ("glGet",	I,	1,	"GL_MAX_GEOMETRY_VARYING_COMPONENTS_ARB"),	# 0x8DDD
    ("glGet",	I,	1,	"GL_MAX_VERTEX_VARYING_COMPONENTS_ARB"),	# 0x8DDE
    ("glGet",	I,	1,	"GL_MAX_GEOMETRY_UNIFORM_COMPONENTS"),	# 0x8DDF
    ("glGet",	I,	1,	"GL_MAX_GEOMETRY_OUTPUT_VERTICES"),	# 0x8DE0
    ("glGet",	I,	1,	"GL_MAX_GEOMETRY_TOTAL_OUTPUT_COMPONENTS"),	# 0x8DE1
    ("glGet",	I,	1,	"GL_MAX_VERTEX_BINDABLE_UNIFORMS_EXT"),	# 0x8DE2
    ("glGet",	I,	1,	"GL_MAX_FRAGMENT_BINDABLE_UNIFORMS_EXT"),	# 0x8DE3
    ("glGet",	I,	1,	"GL_MAX_GEOMETRY_BINDABLE_UNIFORMS_EXT"),	# 0x8DE4
    ("",	I,	1,	"GL_ACTIVE_SUBROUTINES"),	# 0x8DE5
    ("",	I,	1,	"GL_ACTIVE_SUBROUTINE_UNIFORMS"),	# 0x8DE6
    ("glGet",	I,	1,	"GL_MAX_SUBROUTINES"),	# 0x8DE7
    ("glGet",	I,	1,	"GL_MAX_SUBROUTINE_UNIFORM_LOCATIONS"),	# 0x8DE8
    ("glGetNamedString",	I,	1,	"GL_NAMED_STRING_LENGTH_ARB"),	# 0x8DE9
    ("glGetNamedString",	E,	1,	"GL_NAMED_STRING_TYPE_ARB"),	# 0x8DEA
    ("glGet",	I,	1,	"GL_MAX_BINDABLE_UNIFORM_SIZE_EXT"),	# 0x8DED
    ("",	X,	1,	"GL_UNIFORM_BUFFER_EXT"),	# 0x8DEE
    ("glGet",	I,	1,	"GL_UNIFORM_BUFFER_BINDING_EXT"),	# 0x8DEF
    ("",	X,	1,	"GL_LOW_FLOAT"),	# 0x8DF0
    ("",	X,	1,	"GL_MEDIUM_FLOAT"),	# 0x8DF1
    ("",	X,	1,	"GL_HIGH_FLOAT"),	# 0x8DF2
    ("",	X,	1,	"GL_LOW_INT"),	# 0x8DF3
    ("",	X,	1,	"GL_MEDIUM_INT"),	# 0x8DF4
    ("",	X,	1,	"GL_HIGH_INT"),	# 0x8DF5
    ("",	X,	1,	"GL_UNSIGNED_INT_10_10_10_2_OES"),	# 0x8DF6
    ("",	X,	1,	"GL_INT_10_10_10_2_OES"),	# 0x8DF7
    ("",	X,	1,	"GL_SHADER_BINARY_FORMATS"),	# 0x8DF8
    ("glGet",	I,	1,	"GL_NUM_SHADER_BINARY_FORMATS"),	# 0x8DF9
    ("glGet",	B,	1,	"GL_SHADER_COMPILER"),	# 0x8DFA
    ("glGet",	I,	1,	"GL_MAX_VERTEX_UNIFORM_VECTORS"),	# 0x8DFB
    ("glGet",	I,	1,	"GL_MAX_VARYING_VECTORS"),	# 0x8DFC
    ("glGet",	I,	1,	"GL_MAX_FRAGMENT_UNIFORM_VECTORS"),	# 0x8DFD
    ("",	X,	1,	"GL_RENDERBUFFER_COLOR_SAMPLES_NV"),	# 0x8E10
    ("glGet",	I,	1,	"GL_MAX_MULTISAMPLE_COVERAGE_MODES_NV"),	# 0x8E11
    ("",	X,	1,	"GL_MULTISAMPLE_COVERAGE_MODES_NV"),	# 0x8E12
    ("",	X,	1,	"GL_QUERY_WAIT"),	# 0x8E13
    ("",	X,	1,	"GL_QUERY_NO_WAIT"),	# 0x8E14
    ("",	X,	1,	"GL_QUERY_BY_REGION_WAIT"),	# 0x8E15
    ("",	X,	1,	"GL_QUERY_BY_REGION_NO_WAIT"),	# 0x8E16
    ("",	X,	1,	"GL_QUERY_WAIT_INVERTED"),	# 0x8E17
    ("",	X,	1,	"GL_QUERY_NO_WAIT_INVERTED"),	# 0x8E18
    ("",	X,	1,	"GL_QUERY_BY_REGION_WAIT_INVERTED"),	# 0x8E19
    ("",	X,	1,	"GL_QUERY_BY_REGION_NO_WAIT_INVERTED"),	# 0x8E1A
    ("glGet",	I,	1,	"GL_MAX_COMBINED_TESS_CONTROL_UNIFORM_COMPONENTS"),	# 0x8E1E
    ("glGet",	I,	1,	"GL_MAX_COMBINED_TESS_EVALUATION_UNIFORM_COMPONENTS"),	# 0x8E1F
    ("",	X,	1,	"GL_COLOR_SAMPLES_NV"),	# 0x8E20
    ("",	X,	1,	"GL_TRANSFORM_FEEDBACK"),	# 0x8E22
    ("glGet",	B,	1,	"GL_TRANSFORM_FEEDBACK_BUFFER_PAUSED"),	# 0x8E23
    ("glGet",	B,	1,	"GL_TRANSFORM_FEEDBACK_BUFFER_ACTIVE"),	# 0x8E24
    ("glGet",	I,	1,	"GL_TRANSFORM_FEEDBACK_BINDING"),	# 0x8E25
    ("",	X,	1,	"GL_FRAME_NV"),	# 0x8E26
    ("",	X,	1,	"GL_FIELDS_NV"),	# 0x8E27
    ("_glGet",	I64,	1,	"GL_TIMESTAMP"),	# 0x8E28
    ("",	X,	1,	"GL_NUM_FILL_STREAMS_NV"),	# 0x8E29
    ("",	X,	1,	"GL_PRESENT_TIME_NV"),	# 0x8E2A
    ("",	X,	1,	"GL_PRESENT_DURATION_NV"),	# 0x8E2B
    ("",	X,	1,	"GL_DEPTH_COMPONENT16_NONLINEAR_NV"),	# 0x8E2C
    ("",	X,	1,	"GL_PROGRAM_MATRIX_EXT"),	# 0x8E2D
    ("",	X,	1,	"GL_TRANSPOSE_PROGRAM_MATRIX_EXT"),	# 0x8E2E
    ("",	X,	1,	"GL_PROGRAM_MATRIX_STACK_DEPTH_EXT"),	# 0x8E2F
    ("glGetTexParameter",	E,	1,	"GL_TEXTURE_SWIZZLE_R"),	# 0x8E42
    ("glGetTexParameter",	E,	1,	"GL_TEXTURE_SWIZZLE_G"),	# 0x8E43
    ("glGetTexParameter",	E,	1,	"GL_TEXTURE_SWIZZLE_B"),	# 0x8E44
    ("glGetTexParameter",	E,	1,	"GL_TEXTURE_SWIZZLE_A"),	# 0x8E45
    ("glGetTexParameter",	E,	4,	"GL_TEXTURE_SWIZZLE_RGBA"),	# 0x8E46
    ("",	I,	1,	"GL_ACTIVE_SUBROUTINE_UNIFORM_LOCATIONS"),	# 0x8E47
    ("",	I,	1,	"GL_ACTIVE_SUBROUTINE_MAX_LENGTH"),	# 0x8E48
    ("",	I,	1,	"GL_ACTIVE_SUBROUTINE_UNIFORM_MAX_LENGTH"),	# 0x8E49
    ("",	I,	1,	"GL_NUM_COMPATIBLE_SUBROUTINES"),	# 0x8E4A
    ("",	I,	1,	"GL_COMPATIBLE_SUBROUTINES"),	# 0x8E4B
    ("glGet",	B,	1,	"GL_QUADS_FOLLOW_PROVOKING_VERTEX_CONVENTION"),	# 0x8E4C
    ("",	X,	1,	"GL_FIRST_VERTEX_CONVENTION"),	# 0x8E4D
    ("",	X,	1,	"GL_LAST_VERTEX_CONVENTION"),	# 0x8E4E
    ("glGet",	E,	1,	"GL_PROVOKING_VERTEX"),	# 0x8E4F
    ("glGetMultisample",	F,	2,	"GL_SAMPLE_POSITION"),	# 0x8E50
    ("glGet",	B,	1,	"GL_SAMPLE_MASK"),	# 0x8E51
    ("glGet",	I,	1,	"GL_SAMPLE_MASK_VALUE"),	# 0x8E52
    ("",	X,	1,	"GL_TEXTURE_BINDING_RENDERBUFFER_NV"),	# 0x8E53
    ("glGet",	I,	1,	"GL_TEXTURE_RENDERBUFFER_DATA_STORE_BINDING_NV"),	# 0x8E54
    ("",	X,	1,	"GL_TEXTURE_RENDERBUFFER_NV"),	# 0x8E55
    ("",	X,	1,	"GL_SAMPLER_RENDERBUFFER_NV"),	# 0x8E56
    ("",	X,	1,	"GL_INT_SAMPLER_RENDERBUFFER_NV"),	# 0x8E57
    ("",	X,	1,	"GL_UNSIGNED_INT_SAMPLER_RENDERBUFFER_NV"),	# 0x8E58
    ("glGet",	I,	1,	"GL_MAX_SAMPLE_MASK_WORDS"),	# 0x8E59
    ("glGet",	I,	1,	"GL_MAX_GEOMETRY_SHADER_INVOCATIONS"),	# 0x8E5A
    ("glGet",	F,	1,	"GL_MIN_FRAGMENT_INTERPOLATION_OFFSET"),	# 0x8E5B
    ("glGet",	F,	1,	"GL_MAX_FRAGMENT_INTERPOLATION_OFFSET"),	# 0x8E5C
    ("glGet",	I,	1,	"GL_FRAGMENT_INTERPOLATION_OFFSET_BITS"),	# 0x8E5D
    ("glGet",	I,	1,	"GL_MIN_PROGRAM_TEXTURE_GATHER_OFFSET"),	# 0x8E5E
    ("glGet",	I,	1,	"GL_MAX_PROGRAM_TEXTURE_GATHER_OFFSET"),	# 0x8E5F
    ("glGet",	I,	1,	"GL_MAX_TRANSFORM_FEEDBACK_BUFFERS"),	# 0x8E70
    ("glGet",	I,	1,	"GL_MAX_VERTEX_STREAMS"),	# 0x8E71
    ("glGet",	I,	1,	"GL_PATCH_VERTICES"),	# 0x8E72
    ("glGet",	F,	2,	"GL_PATCH_DEFAULT_INNER_LEVEL"),	# 0x8E73
    ("glGet",	F,	4,	"GL_PATCH_DEFAULT_OUTER_LEVEL"),	# 0x8E74
    ("glGetProgram",	I,	1,	"GL_TESS_CONTROL_OUTPUT_VERTICES"),	# 0x8E75
    ("glGetProgram",	E,	1,	"GL_TESS_GEN_MODE"),	# 0x8E76
    ("glGetProgram",	E,	1,	"GL_TESS_GEN_SPACING"),	# 0x8E77
    ("glGetProgram",	E,	1,	"GL_TESS_GEN_VERTEX_ORDER"),	# 0x8E78
    ("glGetProgram",	E,	1,	"GL_TESS_GEN_POINT_MODE"),	# 0x8E79
    ("",	X,	1,	"GL_ISOLINES"),	# 0x8E7A
    ("",	X,	1,	"GL_FRACTIONAL_ODD"),	# 0x8E7B
    ("",	X,	1,	"GL_FRACTIONAL_EVEN"),	# 0x8E7C
    ("glGet",	I,	1,	"GL_MAX_PATCH_VERTICES"),	# 0x8E7D
    ("glGet",	I,	1,	"GL_MAX_TESS_GEN_LEVEL"),	# 0x8E7E
    ("glGet",	I,	1,	"GL_MAX_TESS_CONTROL_UNIFORM_COMPONENTS"),	# 0x8E7F
    ("glGet",	I,	1,	"GL_MAX_TESS_EVALUATION_UNIFORM_COMPONENTS"),	# 0x8E80
    ("glGet",	I,	1,	"GL_MAX_TESS_CONTROL_TEXTURE_IMAGE_UNITS"),	# 0x8E81
    ("glGet",	I,	1,	"GL_MAX_TESS_EVALUATION_TEXTURE_IMAGE_UNITS"),	# 0x8E82
    ("glGet",	I,	1,	"GL_MAX_TESS_CONTROL_OUTPUT_COMPONENTS"),	# 0x8E83
    ("glGet",	I,	1,	"GL_MAX_TESS_PATCH_COMPONENTS"),	# 0x8E84
    ("glGet",	I,	1,	"GL_MAX_TESS_CONTROL_TOTAL_OUTPUT_COMPONENTS"),	# 0x8E85
    ("glGet",	I,	1,	"GL_MAX_TESS_EVALUATION_OUTPUT_COMPONENTS"),	# 0x8E86
    ("glGetProgramPipeline",	I,	1,	"GL_TESS_EVALUATION_SHADER"),	# 0x8E87
    ("glGetProgramPipeline",	I,	1,	"GL_TESS_CONTROL_SHADER"),	# 0x8E88
    ("glGet",	I,	1,	"GL_MAX_TESS_CONTROL_UNIFORM_BLOCKS"),	# 0x8E89
    ("glGet",	I,	1,	"GL_MAX_TESS_EVALUATION_UNIFORM_BLOCKS"),	# 0x8E8A
    ("",	X,	1,	"GL_COMPRESSED_RGBA_BPTC_UNORM"),	# 0x8E8C
    ("",	X,	1,	"GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM"),	# 0x8E8D
    ("",	X,	1,	"GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT"),	# 0x8E8E
    ("",	X,	1,	"GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT"),	# 0x8E8F
    #("",	X,	1,	"GL_COVERAGE_COMPONENT_NV"),	# 0x8ED0
    #("",	X,	1,	"GL_COVERAGE_COMPONENT4_NV"),	# 0x8ED1
    #("",	X,	1,	"GL_COVERAGE_ATTACHMENT_NV"),	# 0x8ED2
    #("",	X,	1,	"GL_COVERAGE_BUFFERS_NV"),	# 0x8ED3
    #("",	X,	1,	"GL_COVERAGE_SAMPLES_NV"),	# 0x8ED4
    #("",	X,	1,	"GL_COVERAGE_ALL_FRAGMENTS_NV"),	# 0x8ED5
    #("",	X,	1,	"GL_COVERAGE_EDGE_FRAGMENTS_NV"),	# 0x8ED6
    #("",	X,	1,	"GL_COVERAGE_AUTOMATIC_NV"),	# 0x8ED7
    ("_glGetBufferParameter",	I64,	1,	"GL_BUFFER_GPU_ADDRESS_NV"),	# 0x8F1D
    ("",	X,	1,	"GL_VERTEX_ATTRIB_ARRAY_UNIFIED_NV"),	# 0x8F1E
    ("",	X,	1,	"GL_ELEMENT_ARRAY_UNIFIED_NV"),	# 0x8F1F
    ("",	X,	1,	"GL_VERTEX_ATTRIB_ARRAY_ADDRESS_NV"),	# 0x8F20
    ("",	X,	1,	"GL_VERTEX_ARRAY_ADDRESS_NV"),	# 0x8F21
    ("",	X,	1,	"GL_NORMAL_ARRAY_ADDRESS_NV"),	# 0x8F22
    ("",	X,	1,	"GL_COLOR_ARRAY_ADDRESS_NV"),	# 0x8F23
    ("",	X,	1,	"GL_INDEX_ARRAY_ADDRESS_NV"),	# 0x8F24
    ("",	X,	1,	"GL_TEXTURE_COORD_ARRAY_ADDRESS_NV"),	# 0x8F25
    ("",	X,	1,	"GL_EDGE_FLAG_ARRAY_ADDRESS_NV"),	# 0x8F26
    ("",	X,	1,	"GL_SECONDARY_COLOR_ARRAY_ADDRESS_NV"),	# 0x8F27
    ("",	X,	1,	"GL_FOG_COORD_ARRAY_ADDRESS_NV"),	# 0x8F28
    ("",	X,	1,	"GL_ELEMENT_ARRAY_ADDRESS_NV"),	# 0x8F29
    ("",	X,	1,	"GL_VERTEX_ATTRIB_ARRAY_LENGTH_NV"),	# 0x8F2A
    ("",	X,	1,	"GL_VERTEX_ARRAY_LENGTH_NV"),	# 0x8F2B
    ("",	X,	1,	"GL_NORMAL_ARRAY_LENGTH_NV"),	# 0x8F2C
    ("",	X,	1,	"GL_COLOR_ARRAY_LENGTH_NV"),	# 0x8F2D
    ("",	X,	1,	"GL_INDEX_ARRAY_LENGTH_NV"),	# 0x8F2E
    ("",	X,	1,	"GL_TEXTURE_COORD_ARRAY_LENGTH_NV"),	# 0x8F2F
    ("",	X,	1,	"GL_EDGE_FLAG_ARRAY_LENGTH_NV"),	# 0x8F30
    ("",	X,	1,	"GL_SECONDARY_COLOR_ARRAY_LENGTH_NV"),	# 0x8F31
    ("",	X,	1,	"GL_FOG_COORD_ARRAY_LENGTH_NV"),	# 0x8F32
    ("",	X,	1,	"GL_ELEMENT_ARRAY_LENGTH_NV"),	# 0x8F33
    ("",	X,	1,	"GL_GPU_ADDRESS_NV"),	# 0x8F34
    ("_glGet",	I64,	1,	"GL_MAX_SHADER_BUFFER_ADDRESS_NV"),	# 0x8F35
    ("glGet",	I,	1,	"GL_COPY_READ_BUFFER"),	# 0x8F36
    ("glGet",	I,	1,	"GL_COPY_WRITE_BUFFER"),	# 0x8F37
    ("glGet",	I,	1,	"GL_MAX_IMAGE_UNITS"),	# 0x8F38
    ("glGet",	I,	1,	"GL_MAX_COMBINED_IMAGE_UNITS_AND_FRAGMENT_OUTPUTS"),	# 0x8F39
    ("",	X,	1,	"GL_IMAGE_BINDING_NAME"),	# 0x8F3A
    ("",	X,	1,	"GL_IMAGE_BINDING_LEVEL"),	# 0x8F3B
    ("",	X,	1,	"GL_IMAGE_BINDING_LAYERED"),	# 0x8F3C
    ("",	X,	1,	"GL_IMAGE_BINDING_LAYER"),	# 0x8F3D
    ("",	X,	1,	"GL_IMAGE_BINDING_ACCESS"),	# 0x8F3E
    ("",	X,	1,	"GL_DRAW_INDIRECT_BUFFER"),	# 0x8F3F
    ("",	X,	1,	"GL_DRAW_INDIRECT_UNIFIED_NV"),	# 0x8F40
    ("",	X,	1,	"GL_DRAW_INDIRECT_ADDRESS_NV"),	# 0x8F41
    ("",	X,	1,	"GL_DRAW_INDIRECT_LENGTH_NV"),	# 0x8F42
    ("glGet",	I,	1,	"GL_DRAW_INDIRECT_BUFFER_BINDING"),	# 0x8F43
    ("",	X,	1,	"GL_MAX_PROGRAM_SUBROUTINE_PARAMETERS_NV"),	# 0x8F44
    ("",	X,	1,	"GL_MAX_PROGRAM_SUBROUTINE_NUM_NV"),	# 0x8F45
    ("",	X,	1,	"GL_DOUBLE_MAT2"),	# 0x8F46
    ("",	X,	1,	"GL_DOUBLE_MAT3"),	# 0x8F47
    ("",	X,	1,	"GL_DOUBLE_MAT4"),	# 0x8F48
    ("",	X,	1,	"GL_DOUBLE_MAT2x3"),	# 0x8F49
    ("",	X,	1,	"GL_DOUBLE_MAT2x4"),	# 0x8F4A
    ("",	X,	1,	"GL_DOUBLE_MAT3x2"),	# 0x8F4B
    ("",	X,	1,	"GL_DOUBLE_MAT3x4"),	# 0x8F4C
    ("",	X,	1,	"GL_DOUBLE_MAT4x2"),	# 0x8F4D
    ("",	X,	1,	"GL_DOUBLE_MAT4x3"),	# 0x8F4E
    ("",	X,	1,	"GL_VERTEX_BINDING_BUFFER"),	# 0x8F4F
    ("",	X,	1,	"GL_MALI_SHADER_BINARY_ARM"),	# 0x8F60
    ("",	X,	1,	"GL_MALI_PROGRAM_BINARY_ARM"),	# 0x8F61
    ("",	X,	1,	"GL_MAX_SHADER_PIXEL_LOCAL_STORAGE_FAST_SIZE_EXT"),	# 0x8F63
    ("",	X,	1,	"GL_SHADER_PIXEL_LOCAL_STORAGE_EXT"),	# 0x8F64
    ("",	X,	1,	"GL_FETCH_PER_SAMPLE_ARM"),	# 0x8F65
    ("",	X,	1,	"GL_FRAGMENT_SHADER_FRAMEBUFFER_FETCH_MRT_ARM"),	# 0x8F66
    ("",	X,	1,	"GL_MAX_SHADER_PIXEL_LOCAL_STORAGE_SIZE_EXT"),	# 0x8F67
    ("",	X,	1,	"GL_RED_SNORM"),	# 0x8F90
    ("",	X,	1,	"GL_RG_SNORM"),	# 0x8F91
    ("",	X,	1,	"GL_RGB_SNORM"),	# 0x8F92
    ("",	X,	1,	"GL_RGBA_SNORM"),	# 0x8F93
    ("",	X,	1,	"GL_R8_SNORM"),	# 0x8F94
    ("",	X,	1,	"GL_RG8_SNORM"),	# 0x8F95
    ("",	X,	1,	"GL_RGB8_SNORM"),	# 0x8F96
    ("",	X,	1,	"GL_RGBA8_SNORM"),	# 0x8F97
    ("",	X,	1,	"GL_R16_SNORM"),	# 0x8F98
    ("",	X,	1,	"GL_RG16_SNORM"),	# 0x8F99
    ("",	X,	1,	"GL_RGB16_SNORM"),	# 0x8F9A
    ("",	X,	1,	"GL_RGBA16_SNORM"),	# 0x8F9B
    ("",	X,	1,	"GL_SIGNED_NORMALIZED"),	# 0x8F9C
    ("glGet",	B,	1,	"GL_PRIMITIVE_RESTART"),	# 0x8F9D
    ("glGet",	I,	1,	"GL_PRIMITIVE_RESTART_INDEX"),	# 0x8F9E
    ("glGet",	I,	1,	"GL_MAX_PROGRAM_TEXTURE_GATHER_COMPONENTS_ARB"),	# 0x8F9F
    ("",	X,	1,	"GL_PERFMON_GLOBAL_MODE_QCOM"),	# 0x8FA0
    ("",	X,	1,	"GL_BINNING_CONTROL_HINT_QCOM"),	# 0x8FB0
    ("",	X,	1,	"GL_CPU_OPTIMIZED_QCOM"),	# 0x8FB1
    ("",	X,	1,	"GL_GPU_OPTIMIZED_QCOM"),	# 0x8FB2
    ("",	X,	1,	"GL_RENDER_DIRECT_TO_FRAMEBUFFER_QCOM"),	# 0x8FB3
    ("",	X,	1,	"GL_GPU_DISJOINT_EXT"),	# 0x8FBB
    ("",	X,	1,	"GL_SHADER_BINARY_VIV"),	# 0x8FC4
    ("",	X,	1,	"GL_INT8_NV"),	# 0x8FE0
    ("",	X,	1,	"GL_INT8_VEC2_NV"),	# 0x8FE1
    ("",	X,	1,	"GL_INT8_VEC3_NV"),	# 0x8FE2
    ("",	X,	1,	"GL_INT8_VEC4_NV"),	# 0x8FE3
    ("",	X,	1,	"GL_INT16_NV"),	# 0x8FE4
    ("",	X,	1,	"GL_INT16_VEC2_NV"),	# 0x8FE5
    ("",	X,	1,	"GL_INT16_VEC3_NV"),	# 0x8FE6
    ("",	X,	1,	"GL_INT16_VEC4_NV"),	# 0x8FE7
    ("",	X,	1,	"GL_INT64_VEC2_ARB"),	# 0x8FE9
    ("",	X,	1,	"GL_INT64_VEC3_ARB"),	# 0x8FEA
    ("",	X,	1,	"GL_INT64_VEC4_ARB"),	# 0x8FEB
    ("",	X,	1,	"GL_UNSIGNED_INT8_NV"),	# 0x8FEC
    ("",	X,	1,	"GL_UNSIGNED_INT8_VEC2_NV"),	# 0x8FED
    ("",	X,	1,	"GL_UNSIGNED_INT8_VEC3_NV"),	# 0x8FEE
    ("",	X,	1,	"GL_UNSIGNED_INT8_VEC4_NV"),	# 0x8FEF
    ("",	X,	1,	"GL_UNSIGNED_INT16_NV"),	# 0x8FF0
    ("",	X,	1,	"GL_UNSIGNED_INT16_VEC2_NV"),	# 0x8FF1
    ("",	X,	1,	"GL_UNSIGNED_INT16_VEC3_NV"),	# 0x8FF2
    ("",	X,	1,	"GL_UNSIGNED_INT16_VEC4_NV"),	# 0x8FF3
    ("",	X,	1,	"GL_UNSIGNED_INT64_VEC2_ARB"),	# 0x8FF5
    ("",	X,	1,	"GL_UNSIGNED_INT64_VEC3_ARB"),	# 0x8FF6
    ("",	X,	1,	"GL_UNSIGNED_INT64_VEC4_ARB"),	# 0x8FF7
    ("",	X,	1,	"GL_FLOAT16_NV"),	# 0x8FF8
    ("",	X,	1,	"GL_FLOAT16_VEC2_NV"),	# 0x8FF9
    ("",	X,	1,	"GL_FLOAT16_VEC3_NV"),	# 0x8FFA
    ("",	X,	1,	"GL_FLOAT16_VEC4_NV"),	# 0x8FFB
    ("",	X,	1,	"GL_DOUBLE_VEC2"),	# 0x8FFC
    ("",	X,	1,	"GL_DOUBLE_VEC3"),	# 0x8FFD
    ("",	X,	1,	"GL_DOUBLE_VEC4"),	# 0x8FFE
    ("",	X,	1,	"GL_SAMPLER_BUFFER_AMD"),	# 0x9001
    ("",	X,	1,	"GL_INT_SAMPLER_BUFFER_AMD"),	# 0x9002
    ("",	X,	1,	"GL_UNSIGNED_INT_SAMPLER_BUFFER_AMD"),	# 0x9003
    ("",	X,	1,	"GL_TESSELLATION_MODE_AMD"),	# 0x9004
    ("",	X,	1,	"GL_TESSELLATION_FACTOR_AMD"),	# 0x9005
    ("",	X,	1,	"GL_DISCRETE_AMD"),	# 0x9006
    ("",	X,	1,	"GL_CONTINUOUS_AMD"),	# 0x9007
    ("",	B,	1,	"GL_TEXTURE_CUBE_MAP_ARRAY"),	# 0x9009
    ("glGet_texture",	I,	1,	"GL_TEXTURE_BINDING_CUBE_MAP_ARRAY"),	# 0x900A
    ("",	X,	1,	"GL_PROXY_TEXTURE_CUBE_MAP_ARRAY"),	# 0x900B
    ("",	X,	1,	"GL_SAMPLER_CUBE_MAP_ARRAY"),	# 0x900C
    ("",	X,	1,	"GL_SAMPLER_CUBE_MAP_ARRAY_SHADOW"),	# 0x900D
    ("",	X,	1,	"GL_INT_SAMPLER_CUBE_MAP_ARRAY"),	# 0x900E
    ("",	X,	1,	"GL_UNSIGNED_INT_SAMPLER_CUBE_MAP_ARRAY"),	# 0x900F
    ("",	X,	1,	"GL_ALPHA_SNORM"),	# 0x9010
    ("",	X,	1,	"GL_LUMINANCE_SNORM"),	# 0x9011
    ("",	X,	1,	"GL_LUMINANCE_ALPHA_SNORM"),	# 0x9012
    ("",	X,	1,	"GL_INTENSITY_SNORM"),	# 0x9013
    ("",	X,	1,	"GL_ALPHA8_SNORM"),	# 0x9014
    ("",	X,	1,	"GL_LUMINANCE8_SNORM"),	# 0x9015
    ("",	X,	1,	"GL_LUMINANCE8_ALPHA8_SNORM"),	# 0x9016
    ("",	X,	1,	"GL_INTENSITY8_SNORM"),	# 0x9017
    ("",	X,	1,	"GL_ALPHA16_SNORM"),	# 0x9018
    ("",	X,	1,	"GL_LUMINANCE16_SNORM"),	# 0x9019
    ("",	X,	1,	"GL_LUMINANCE16_ALPHA16_SNORM"),	# 0x901A
    ("",	X,	1,	"GL_INTENSITY16_SNORM"),	# 0x901B
    ("",	X,	1,	"GL_FACTOR_MIN_AMD"),	# 0x901C
    ("",	X,	1,	"GL_FACTOR_MAX_AMD"),	# 0x901D
    ("",	B,	1,	"GL_DEPTH_CLAMP_NEAR_AMD"),	# 0x901E
    ("",	B,	1,	"GL_DEPTH_CLAMP_FAR_AMD"),	# 0x901F
    ("",	X,	1,	"GL_VIDEO_BUFFER_NV"),	# 0x9020
    ("glGet",	I,	1,	"GL_VIDEO_BUFFER_BINDING_NV"),	# 0x9021
    ("",	X,	1,	"GL_FIELD_UPPER_NV"),	# 0x9022
    ("",	X,	1,	"GL_FIELD_LOWER_NV"),	# 0x9023
    ("",	X,	1,	"GL_NUM_VIDEO_CAPTURE_STREAMS_NV"),	# 0x9024
    ("",	X,	1,	"GL_NEXT_VIDEO_CAPTURE_BUFFER_STATUS_NV"),	# 0x9025
    ("",	X,	1,	"GL_VIDEO_CAPTURE_TO_422_SUPPORTED_NV"),	# 0x9026
    ("",	X,	1,	"GL_LAST_VIDEO_CAPTURE_STATUS_NV"),	# 0x9027
    ("",	X,	1,	"GL_VIDEO_BUFFER_PITCH_NV"),	# 0x9028
    ("",	X,	1,	"GL_VIDEO_COLOR_CONVERSION_MATRIX_NV"),	# 0x9029
    ("",	X,	1,	"GL_VIDEO_COLOR_CONVERSION_MAX_NV"),	# 0x902A
    ("",	X,	1,	"GL_VIDEO_COLOR_CONVERSION_MIN_NV"),	# 0x902B
    ("",	X,	1,	"GL_VIDEO_COLOR_CONVERSION_OFFSET_NV"),	# 0x902C
    ("",	X,	1,	"GL_VIDEO_BUFFER_INTERNAL_FORMAT_NV"),	# 0x902D
    ("",	X,	1,	"GL_PARTIAL_SUCCESS_NV"),	# 0x902E
    ("",	X,	1,	"GL_SUCCESS_NV"),	# 0x902F
    ("",	X,	1,	"GL_FAILURE_NV"),	# 0x9030
    ("",	X,	1,	"GL_YCBYCR8_422_NV"),	# 0x9031
    ("",	X,	1,	"GL_YCBAYCR8A_4224_NV"),	# 0x9032
    ("",	X,	1,	"GL_Z6Y10Z6CB10Z6Y10Z6CR10_422_NV"),	# 0x9033
    ("",	X,	1,	"GL_Z6Y10Z6CB10Z6A10Z6Y10Z6CR10Z6A10_4224_NV"),	# 0x9034
    ("",	X,	1,	"GL_Z4Y12Z4CB12Z4Y12Z4CR12_422_NV"),	# 0x9035
    ("",	X,	1,	"GL_Z4Y12Z4CB12Z4A12Z4Y12Z4CR12Z4A12_4224_NV"),	# 0x9036
    ("",	X,	1,	"GL_Z4Y12Z4CB12Z4CR12_444_NV"),	# 0x9037
    ("",	X,	1,	"GL_VIDEO_CAPTURE_FRAME_WIDTH_NV"),	# 0x9038
    ("",	X,	1,	"GL_VIDEO_CAPTURE_FRAME_HEIGHT_NV"),	# 0x9039
    ("",	X,	1,	"GL_VIDEO_CAPTURE_FIELD_UPPER_HEIGHT_NV"),	# 0x903A
    ("",	X,	1,	"GL_VIDEO_CAPTURE_FIELD_LOWER_HEIGHT_NV"),	# 0x903B
    ("",	X,	1,	"GL_VIDEO_CAPTURE_SURFACE_ORIGIN_NV"),	# 0x903C
    ("",	X,	1,	"GL_TEXTURE_COVERAGE_SAMPLES_NV"),	# 0x9045
    ("",	X,	1,	"GL_TEXTURE_COLOR_SAMPLES_NV"),	# 0x9046
    ("glGet",	I,	1,	"GL_GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX"), # 0x9047
    ("glGet",	I,	1,	"GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX"), # 0x9048
    ("glGet",	I,	1,	"GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX"), # 0x9049
    ("glGet",	I,	1,	"GL_GPU_MEMORY_INFO_EVICTION_COUNT_NVX"), # 0x904A
    ("glGet",	I,	1,	"GL_GPU_MEMORY_INFO_EVICTED_MEMORY_NVX"), # 0x904B
    ("",	X,	1,	"GL_IMAGE_1D"),	# 0x904C
    ("",	X,	1,	"GL_IMAGE_2D"),	# 0x904D
    ("",	X,	1,	"GL_IMAGE_3D"),	# 0x904E
    ("",	X,	1,	"GL_IMAGE_2D_RECT"),	# 0x904F
    ("",	X,	1,	"GL_IMAGE_CUBE"),	# 0x9050
    ("",	X,	1,	"GL_IMAGE_BUFFER"),	# 0x9051
    ("",	X,	1,	"GL_IMAGE_1D_ARRAY"),	# 0x9052
    ("",	X,	1,	"GL_IMAGE_2D_ARRAY"),	# 0x9053
    ("",	X,	1,	"GL_IMAGE_CUBE_MAP_ARRAY"),	# 0x9054
    ("",	X,	1,	"GL_IMAGE_2D_MULTISAMPLE"),	# 0x9055
    ("",	X,	1,	"GL_IMAGE_2D_MULTISAMPLE_ARRAY"),	# 0x9056
    ("",	X,	1,	"GL_INT_IMAGE_1D"),	# 0x9057
    ("",	X,	1,	"GL_INT_IMAGE_2D"),	# 0x9058
    ("",	X,	1,	"GL_INT_IMAGE_3D"),	# 0x9059
    ("",	X,	1,	"GL_INT_IMAGE_2D_RECT"),	# 0x905A
    ("",	X,	1,	"GL_INT_IMAGE_CUBE"),	# 0x905B
    ("",	X,	1,	"GL_INT_IMAGE_BUFFER"),	# 0x905C
    ("",	X,	1,	"GL_INT_IMAGE_1D_ARRAY"),	# 0x905D
    ("",	X,	1,	"GL_INT_IMAGE_2D_ARRAY"),	# 0x905E
    ("",	X,	1,	"GL_INT_IMAGE_CUBE_MAP_ARRAY"),	# 0x905F
    ("",	X,	1,	"GL_INT_IMAGE_2D_MULTISAMPLE"),	# 0x9060
    ("",	X,	1,	"GL_INT_IMAGE_2D_MULTISAMPLE_ARRAY"),	# 0x9061
    ("",	X,	1,	"GL_UNSIGNED_INT_IMAGE_1D"),	# 0x9062
    ("",	X,	1,	"GL_UNSIGNED_INT_IMAGE_2D"),	# 0x9063
    ("",	X,	1,	"GL_UNSIGNED_INT_IMAGE_3D"),	# 0x9064
    ("",	X,	1,	"GL_UNSIGNED_INT_IMAGE_2D_RECT"),	# 0x9065
    ("",	X,	1,	"GL_UNSIGNED_INT_IMAGE_CUBE"),	# 0x9066
    ("",	X,	1,	"GL_UNSIGNED_INT_IMAGE_BUFFER"),	# 0x9067
    ("",	X,	1,	"GL_UNSIGNED_INT_IMAGE_1D_ARRAY"),	# 0x9068
    ("",	X,	1,	"GL_UNSIGNED_INT_IMAGE_2D_ARRAY"),	# 0x9069
    ("",	X,	1,	"GL_UNSIGNED_INT_IMAGE_CUBE_MAP_ARRAY"),	# 0x906A
    ("",	X,	1,	"GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE"),	# 0x906B
    ("",	X,	1,	"GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY"),	# 0x906C
    ("glGet",	I,	1,	"GL_MAX_IMAGE_SAMPLES"),	# 0x906D
    ("",	X,	1,	"GL_IMAGE_BINDING_FORMAT"),	# 0x906E
    ("",	X,	1,	"GL_RGB10_A2UI"),	# 0x906F
    ("",	X,	1,	"GL_PATH_FORMAT_SVG_NV"),	# 0x9070
    ("",	X,	1,	"GL_PATH_FORMAT_PS_NV"),	# 0x9071
    ("",	X,	1,	"GL_STANDARD_FONT_NAME_NV"),	# 0x9072
    ("",	X,	1,	"GL_SYSTEM_FONT_NAME_NV"),	# 0x9073
    ("",	X,	1,	"GL_FILE_NAME_NV"),	# 0x9074
    ("",	X,	1,	"GL_PATH_STROKE_WIDTH_NV"),	# 0x9075
    ("",	X,	1,	"GL_PATH_END_CAPS_NV"),	# 0x9076
    ("",	X,	1,	"GL_PATH_INITIAL_END_CAP_NV"),	# 0x9077
    ("",	X,	1,	"GL_PATH_TERMINAL_END_CAP_NV"),	# 0x9078
    ("",	X,	1,	"GL_PATH_JOIN_STYLE_NV"),	# 0x9079
    ("",	X,	1,	"GL_PATH_MITER_LIMIT_NV"),	# 0x907A
    ("",	X,	1,	"GL_PATH_DASH_CAPS_NV"),	# 0x907B
    ("",	X,	1,	"GL_PATH_INITIAL_DASH_CAP_NV"),	# 0x907C
    ("",	X,	1,	"GL_PATH_TERMINAL_DASH_CAP_NV"),	# 0x907D
    ("",	X,	1,	"GL_PATH_DASH_OFFSET_NV"),	# 0x907E
    ("",	X,	1,	"GL_PATH_CLIENT_LENGTH_NV"),	# 0x907F
    ("",	X,	1,	"GL_PATH_FILL_MODE_NV"),	# 0x9080
    ("",	X,	1,	"GL_PATH_FILL_MASK_NV"),	# 0x9081
    ("",	X,	1,	"GL_PATH_FILL_COVER_MODE_NV"),	# 0x9082
    ("",	X,	1,	"GL_PATH_STROKE_COVER_MODE_NV"),	# 0x9083
    ("",	X,	1,	"GL_PATH_STROKE_MASK_NV"),	# 0x9084
    ("",	X,	1,	"GL_PATH_SAMPLE_QUALITY_NV"),	# 0x9085
    ("",	X,	1,	"GL_PATH_STROKE_OVERSAMPLE_COUNT_NV"),	# 0x9087
    ("",	X,	1,	"GL_COUNT_UP_NV"),	# 0x9088
    ("",	X,	1,	"GL_COUNT_DOWN_NV"),	# 0x9089
    ("",	X,	1,	"GL_PATH_OBJECT_BOUNDING_BOX_NV"),	# 0x908A
    ("",	X,	1,	"GL_CONVEX_HULL_NV"),	# 0x908B
    ("",	X,	1,	"GL_MULTI_HULLS_NV"),	# 0x908C
    ("",	X,	1,	"GL_BOUNDING_BOX_NV"),	# 0x908D
    ("",	X,	1,	"GL_TRANSLATE_X_NV"),	# 0x908E
    ("",	X,	1,	"GL_TRANSLATE_Y_NV"),	# 0x908F
    ("",	X,	1,	"GL_TRANSLATE_2D_NV"),	# 0x9090
    ("",	X,	1,	"GL_TRANSLATE_3D_NV"),	# 0x9091
    ("",	X,	1,	"GL_AFFINE_2D_NV"),	# 0x9092
    ("",	X,	1,	"GL_PROJECTIVE_2D_NV"),	# 0x9093
    ("",	X,	1,	"GL_AFFINE_3D_NV"),	# 0x9094
    ("",	X,	1,	"GL_PROJECTIVE_3D_NV"),	# 0x9095
    ("",	X,	1,	"GL_TRANSPOSE_AFFINE_2D_NV"),	# 0x9096
    ("",	X,	1,	"GL_TRANSPOSE_PROJECTIVE_2D_NV"),	# 0x9097
    ("",	X,	1,	"GL_TRANSPOSE_AFFINE_3D_NV"),	# 0x9098
    ("",	X,	1,	"GL_TRANSPOSE_PROJECTIVE_3D_NV"),	# 0x9099
    ("",	X,	1,	"GL_UTF8_NV"),	# 0x909A
    ("",	X,	1,	"GL_UTF16_NV"),	# 0x909B
    ("",	X,	1,	"GL_BOUNDING_BOX_OF_BOUNDING_BOXES_NV"),	# 0x909C
    ("",	X,	1,	"GL_PATH_COMMAND_COUNT_NV"),	# 0x909D
    ("",	X,	1,	"GL_PATH_COORD_COUNT_NV"),	# 0x909E
    ("",	X,	1,	"GL_PATH_DASH_ARRAY_COUNT_NV"),	# 0x909F
    ("",	X,	1,	"GL_PATH_COMPUTED_LENGTH_NV"),	# 0x90A0
    ("",	X,	1,	"GL_PATH_FILL_BOUNDING_BOX_NV"),	# 0x90A1
    ("",	X,	1,	"GL_PATH_STROKE_BOUNDING_BOX_NV"),	# 0x90A2
    ("",	X,	1,	"GL_SQUARE_NV"),	# 0x90A3
    ("",	X,	1,	"GL_ROUND_NV"),	# 0x90A4
    ("",	X,	1,	"GL_TRIANGULAR_NV"),	# 0x90A5
    ("",	X,	1,	"GL_BEVEL_NV"),	# 0x90A6
    ("",	X,	1,	"GL_MITER_REVERT_NV"),	# 0x90A7
    ("",	X,	1,	"GL_MITER_TRUNCATE_NV"),	# 0x90A8
    ("",	X,	1,	"GL_SKIP_MISSING_GLYPH_NV"),	# 0x90A9
    ("",	X,	1,	"GL_USE_MISSING_GLYPH_NV"),	# 0x90AA
    ("",	X,	1,	"GL_PATH_ERROR_POSITION_NV"),	# 0x90AB
    ("",	X,	1,	"GL_PATH_FOG_GEN_MODE_NV"),	# 0x90AC
    ("",	X,	1,	"GL_ACCUM_ADJACENT_PAIRS_NV"),	# 0x90AD
    ("",	X,	1,	"GL_ADJACENT_PAIRS_NV"),	# 0x90AE
    ("",	X,	1,	"GL_FIRST_TO_REST_NV"),	# 0x90AF
    ("",	X,	1,	"GL_PATH_GEN_MODE_NV"),	# 0x90B0
    ("",	X,	1,	"GL_PATH_GEN_COEFF_NV"),	# 0x90B1
    ("",	X,	1,	"GL_PATH_GEN_COLOR_FORMAT_NV"),	# 0x90B2
    ("",	X,	1,	"GL_PATH_GEN_COMPONENTS_NV"),	# 0x90B3
    ("",	X,	1,	"GL_PATH_DASH_OFFSET_RESET_NV"),	# 0x90B4
    ("",	X,	1,	"GL_MOVE_TO_RESETS_NV"),	# 0x90B5
    ("",	X,	1,	"GL_MOVE_TO_CONTINUES_NV"),	# 0x90B6
    ("",	X,	1,	"GL_PATH_STENCIL_FUNC_NV"),	# 0x90B7
    ("",	X,	1,	"GL_PATH_STENCIL_REF_NV"),	# 0x90B8
    ("",	X,	1,	"GL_PATH_STENCIL_VALUE_MASK_NV"),	# 0x90B9
    ("",	X,	1,	"GL_SCALED_RESOLVE_FASTEST_EXT"),	# 0x90BA
    ("",	X,	1,	"GL_SCALED_RESOLVE_NICEST_EXT"),	# 0x90BB
    ("glGet",	I,	1,	"GL_MIN_MAP_BUFFER_ALIGNMENT"),	# 0x90BC
    ("",	X,	1,	"GL_PATH_STENCIL_DEPTH_OFFSET_FACTOR_NV"),	# 0x90BD
    ("",	X,	1,	"GL_PATH_STENCIL_DEPTH_OFFSET_UNITS_NV"),	# 0x90BE
    ("",	X,	1,	"GL_PATH_COVER_DEPTH_FUNC_NV"),	# 0x90BF
    ("",	X,	1,	"GL_IMAGE_FORMAT_COMPATIBILITY_TYPE"),	# 0x90C7
    ("",	X,	1,	"GL_IMAGE_FORMAT_COMPATIBILITY_BY_SIZE"),	# 0x90C8
    ("",	X,	1,	"GL_IMAGE_FORMAT_COMPATIBILITY_BY_CLASS"),	# 0x90C9
    ("glGet",	I,	1,	"GL_MAX_VERTEX_IMAGE_UNIFORMS"),	# 0x90CA
    ("glGet",	I,	1,	"GL_MAX_TESS_CONTROL_IMAGE_UNIFORMS"),	# 0x90CB
    ("glGet",	I,	1,	"GL_MAX_TESS_EVALUATION_IMAGE_UNIFORMS"),	# 0x90CC
    ("glGet",	I,	1,	"GL_MAX_GEOMETRY_IMAGE_UNIFORMS"),	# 0x90CD
    ("glGet",	I,	1,	"GL_MAX_FRAGMENT_IMAGE_UNIFORMS"),	# 0x90CE
    ("glGet",	I,	1,	"GL_MAX_COMBINED_IMAGE_UNIFORMS"),	# 0x90CF
    ("",	X,	1,	"GL_MAX_DEEP_3D_TEXTURE_WIDTH_HEIGHT_NV"),	# 0x90D0
    ("",	X,	1,	"GL_MAX_DEEP_3D_TEXTURE_DEPTH_NV"),	# 0x90D1
    ("",	I,	1,	"GL_SHADER_STORAGE_BUFFER"),	# 0x90D2
    ("glGet,glGet_i",	I,	1,	"GL_SHADER_STORAGE_BUFFER_BINDING"),	# 0x90D3
    ("glGet_i",	I,	1,	"GL_SHADER_STORAGE_BUFFER_START"),	# 0x90D4
    ("glGet_i",	I,	1,	"GL_SHADER_STORAGE_BUFFER_SIZE"),	# 0x90D5
    ("",	I,	1,	"GL_MAX_VERTEX_SHADER_STORAGE_BLOCKS"),	# 0x90D6
    ("",	I,	1,	"GL_MAX_GEOMETRY_SHADER_STORAGE_BLOCKS"),	# 0x90D7
    ("",	I,	1,	"GL_MAX_TESS_CONTROL_SHADER_STORAGE_BLOCKS"),	# 0x90D8
    ("",	I,	1,	"GL_MAX_TESS_EVALUATION_SHADER_STORAGE_BLOCKS"),	# 0x90D9
    ("",	I,	1,	"GL_MAX_FRAGMENT_SHADER_STORAGE_BLOCKS"),	# 0x90DA
    ("glGet",	I,	1,	"GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS"),	# 0x90DB
    ("",	I,	1,	"GL_MAX_COMBINED_SHADER_STORAGE_BLOCKS"),	# 0x90DC
    ("",	I,	1,	"GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS"),	# 0x90DD
    ("",	I,	1,	"GL_MAX_SHADER_STORAGE_BLOCK_SIZE"),	# 0x90DE
    ("",	X,	1,	"GL_SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT"),	# 0x90DF
    ("",	X,	1,	"GL_SYNC_X11_FENCE_EXT"),	# 0x90E1
    ("glGetTexParameter",	E,	1,	"GL_DEPTH_STENCIL_TEXTURE_MODE"),	# 0x90EA
    ("glGet",	I,	1,	"GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS"),	# 0x90EB
    ("",	X,	1,	"GL_UNIFORM_BLOCK_REFERENCED_BY_COMPUTE_SHADER"),	# 0x90EC
    ("",	X,	1,	"GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_COMPUTE_SHADER"),	# 0x90ED
    ("",	X,	1,	"GL_DISPATCH_INDIRECT_BUFFER"),	# 0x90EE
    ("glGet",	I,	1,	"GL_DISPATCH_INDIRECT_BUFFER_BINDING"),	# 0x90EF
    ("",	X,	1,	"GL_COLOR_ATTACHMENT_EXT"),	# 0x90F0
    ("",	X,	1,	"GL_MULTIVIEW_EXT"),	# 0x90F1
    ("",	X,	1,	"GL_MAX_MULTIVIEW_BUFFERS_EXT"),	# 0x90F2
    ("",	X,	1,	"GL_CONTEXT_ROBUST_ACCESS"),	# 0x90F3
    ("",	X,	1,	"GL_COMPUTE_PROGRAM_NV"),	# 0x90FB
    ("",	X,	1,	"GL_COMPUTE_PROGRAM_PARAMETER_BUFFER_NV"),	# 0x90FC
    ("",	X,	1,	"GL_TEXTURE_2D_MULTISAMPLE"),	# 0x9100
    ("",	X,	1,	"GL_PROXY_TEXTURE_2D_MULTISAMPLE"),	# 0x9101
    ("",	X,	1,	"GL_TEXTURE_2D_MULTISAMPLE_ARRAY"),	# 0x9102
    ("",	X,	1,	"GL_PROXY_TEXTURE_2D_MULTISAMPLE_ARRAY"),	# 0x9103
    ("glGet_texture",	I,	1,	"GL_TEXTURE_BINDING_2D_MULTISAMPLE"),	# 0x9104
    ("glGet_texture",	I,	1,	"GL_TEXTURE_BINDING_2D_MULTISAMPLE_ARRAY"),	# 0x9105
    ("glGetTexLevelParameter",	I,	1,	"GL_TEXTURE_SAMPLES"),	# 0x9106
    ("glGetTexLevelParameter",	B,	1,	"GL_TEXTURE_FIXED_SAMPLE_LOCATIONS"),	# 0x9107
    ("",	X,	1,	"GL_SAMPLER_2D_MULTISAMPLE"),	# 0x9108
    ("",	X,	1,	"GL_INT_SAMPLER_2D_MULTISAMPLE"),	# 0x9109
    ("",	X,	1,	"GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE"),	# 0x910A
    ("",	X,	1,	"GL_SAMPLER_2D_MULTISAMPLE_ARRAY"),	# 0x910B
    ("",	X,	1,	"GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY"),	# 0x910C
    ("",	X,	1,	"GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY"),	# 0x910D
    ("glGet",	I,	1,	"GL_MAX_COLOR_TEXTURE_SAMPLES"),	# 0x910E
    ("glGet",	I,	1,	"GL_MAX_DEPTH_TEXTURE_SAMPLES"),	# 0x910F
    ("glGet",	I,	1,	"GL_MAX_INTEGER_SAMPLES"),	# 0x9110
    ("_glGet",	I64,	1,	"GL_MAX_SERVER_WAIT_TIMEOUT"),	# 0x9111
    ("",	X,	1,	"GL_OBJECT_TYPE"),	# 0x9112
    ("",	X,	1,	"GL_SYNC_CONDITION"),	# 0x9113
    ("",	X,	1,	"GL_SYNC_STATUS"),	# 0x9114
    ("",	X,	1,	"GL_SYNC_FLAGS"),	# 0x9115
    ("",	X,	1,	"GL_SYNC_FENCE"),	# 0x9116
    ("",	X,	1,	"GL_SYNC_GPU_COMMANDS_COMPLETE"),	# 0x9117
    ("",	X,	1,	"GL_UNSIGNALED"),	# 0x9118
    ("",	X,	1,	"GL_SIGNALED"),	# 0x9119
    ("",	X,	1,	"GL_ALREADY_SIGNALED"),	# 0x911A
    ("",	X,	1,	"GL_TIMEOUT_EXPIRED"),	# 0x911B
    ("",	X,	1,	"GL_CONDITION_SATISFIED"),	# 0x911C
    ("",	X,	1,	"GL_WAIT_FAILED"),	# 0x911D
    ("glGetBufferParameter",	I,	1,	"GL_BUFFER_ACCESS_FLAGS"),	# 0x911F
    ("glGetBufferParameter",	I,	1,	"GL_BUFFER_MAP_LENGTH"),	# 0x9120
    ("glGetBufferParameter",	I,	1,	"GL_BUFFER_MAP_OFFSET"),	# 0x9121
    ("glGet",	I,	1,	"GL_MAX_VERTEX_OUTPUT_COMPONENTS"),	# 0x9122
    ("glGet",	I,	1,	"GL_MAX_GEOMETRY_INPUT_COMPONENTS"),	# 0x9123
    ("glGet",	I,	1,	"GL_MAX_GEOMETRY_OUTPUT_COMPONENTS"),	# 0x9124
    ("glGet",	I,	1,	"GL_MAX_FRAGMENT_INPUT_COMPONENTS"),	# 0x9125
    ("glGet",	I,	1,	"GL_CONTEXT_PROFILE_MASK"),	# 0x9126
    ("glGet",	I,	1,	"GL_UNPACK_COMPRESSED_BLOCK_WIDTH"),	# 0x9127
    ("glGet",	I,	1,	"GL_UNPACK_COMPRESSED_BLOCK_HEIGHT"),	# 0x9128
    ("glGet",	I,	1,	"GL_UNPACK_COMPRESSED_BLOCK_DEPTH"),	# 0x9129
    ("glGet",	I,	1,	"GL_UNPACK_COMPRESSED_BLOCK_SIZE"),	# 0x912A
    ("glGet",	I,	1,	"GL_PACK_COMPRESSED_BLOCK_WIDTH"),	# 0x912B
    ("glGet",	I,	1,	"GL_PACK_COMPRESSED_BLOCK_HEIGHT"),	# 0x912C
    ("glGet",	I,	1,	"GL_PACK_COMPRESSED_BLOCK_DEPTH"),	# 0x912D
    ("glGet",	I,	1,	"GL_PACK_COMPRESSED_BLOCK_SIZE"),	# 0x912E
    ("glGetTexParameter",	B,	1,	"GL_TEXTURE_IMMUTABLE_FORMAT"),	# 0x912F
    ("",	X,	1,	"GL_SGX_PROGRAM_BINARY_IMG"),	# 0x9130
    ("",	I,	1,	"GL_RENDERBUFFER_SAMPLES_IMG"),	# 0x9133
    ("",	X,	1,	"GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE_IMG"),	# 0x9134
    ("",	I,	1,	"GL_MAX_SAMPLES_IMG"),	# 0x9135
    ("",	I,	1,	"GL_TEXTURE_SAMPLES_IMG"),	# 0x9136
    ("",	X,	1,	"GL_COMPRESSED_RGBA_PVRTC_2BPPV2_IMG"),	# 0x9137
    ("",	X,	1,	"GL_COMPRESSED_RGBA_PVRTC_4BPPV2_IMG"),	# 0x9138
    ("glGet",	I,	1,	"GL_MAX_DEBUG_MESSAGE_LENGTH"),	# 0x9143
    ("glGet",	I,	1,	"GL_MAX_DEBUG_LOGGED_MESSAGES"),	# 0x9144
    ("glGet",	I,	1,	"GL_DEBUG_LOGGED_MESSAGES"),	# 0x9145
    ("",	X,	1,	"GL_DEBUG_SEVERITY_HIGH"),	# 0x9146
    ("",	X,	1,	"GL_DEBUG_SEVERITY_MEDIUM"),	# 0x9147
    ("",	X,	1,	"GL_DEBUG_SEVERITY_LOW"),	# 0x9148
    ("",	X,	1,	"GL_DEBUG_CATEGORY_API_ERROR_AMD"),	# 0x9149
    ("",	X,	1,	"GL_DEBUG_CATEGORY_WINDOW_SYSTEM_AMD"),	# 0x914A
    ("",	X,	1,	"GL_DEBUG_CATEGORY_DEPRECATION_AMD"),	# 0x914B
    ("",	X,	1,	"GL_DEBUG_CATEGORY_UNDEFINED_BEHAVIOR_AMD"),	# 0x914C
    ("",	X,	1,	"GL_DEBUG_CATEGORY_PERFORMANCE_AMD"),	# 0x914D
    ("",	X,	1,	"GL_DEBUG_CATEGORY_SHADER_COMPILER_AMD"),	# 0x914E
    ("",	X,	1,	"GL_DEBUG_CATEGORY_APPLICATION_AMD"),	# 0x914F
    ("",	X,	1,	"GL_DEBUG_CATEGORY_OTHER_AMD"),	# 0x9150
    ("",	X,	1,	"GL_BUFFER_OBJECT_EXT"),	# 0x9151
    ("",	X,	1,	"GL_PERFORMANCE_MONITOR_AMD"),	# 0x9152
    ("",	X,	1,	"GL_QUERY_OBJECT_EXT"),	# 0x9153
    ("",	X,	1,	"GL_VERTEX_ARRAY_OBJECT_EXT"),	# 0x9154
    ("",	X,	1,	"GL_SAMPLER_OBJECT_AMD"),	# 0x9155
    ("",	X,	1,	"GL_EXTERNAL_VIRTUAL_MEMORY_BUFFER_AMD"),	# 0x9160
    ("",	X,	1,	"GL_QUERY_BUFFER"),	# 0x9192
    ("glGet",	I,	1,	"GL_QUERY_BUFFER_BINDING"),	# 0x9193
    ("glGetQueryObject",	I,	1,	"GL_QUERY_RESULT_NO_WAIT"),	# 0x9194
    ("",	X,	1,	"GL_VIRTUAL_PAGE_SIZE_X_ARB"),	# 0x9195
    ("",	X,	1,	"GL_VIRTUAL_PAGE_SIZE_Y_ARB"),	# 0x9196
    ("",	X,	1,	"GL_VIRTUAL_PAGE_SIZE_Z_ARB"),	# 0x9197
    ("glGet",	I,	1,	"GL_MAX_SPARSE_TEXTURE_SIZE_ARB"),	# 0x9198
    ("glGet",	I,	1,	"GL_MAX_SPARSE_3D_TEXTURE_SIZE_ARB"),	# 0x9199
    ("glGet",	I,	1,	"GL_MAX_SPARSE_ARRAY_TEXTURE_LAYERS"),	# 0x919A
    ("",	X,	1,	"GL_MIN_SPARSE_LEVEL_AMD"),	# 0x919B
    ("",	X,	1,	"GL_MIN_LOD_WARNING_AMD"),	# 0x919C
    ("glGetTexLevelParameter",	I,	1,	"GL_TEXTURE_BUFFER_OFFSET"),	# 0x919D
    ("glGetTexLevelParameter",	I,	1,	"GL_TEXTURE_BUFFER_SIZE"),	# 0x919E
    ("glGet",	I,	1,	"GL_TEXTURE_BUFFER_OFFSET_ALIGNMENT"),	# 0x919F
    ("",	X,	1,	"GL_STREAM_RASTERIZATION_AMD"),	# 0x91A0
    ("",	X,	1,	"GL_VERTEX_ELEMENT_SWIZZLE_AMD"),	# 0x91A4
    ("",	X,	1,	"GL_VERTEX_ID_SWIZZLE_AMD"),	# 0x91A5
    ("",	X,	1,	"GL_TEXTURE_SPARSE_ARB"),	# 0x91A6
    ("",	X,	1,	"GL_VIRTUAL_PAGE_SIZE_INDEX_ARB"),	# 0x91A7
    ("",	X,	1,	"GL_NUM_VIRTUAL_PAGE_SIZES_ARB"),	# 0x91A8
    ("glGet",	B,	1,	"GL_SPARSE_TEXTURE_FULL_ARRAY_CUBE_MIPMAPS_ARB"),	# 0x91A9
    ("",	X,	1,	"GL_NUM_SPARSE_LEVELS_ARB"),	# 0x91AA
    ("glGet",	I,	1,	"GL_MAX_SHADER_COMPILER_THREADS_ARB"),	# 0x91B0
    ("glGetProgram,glGetShader",	B,	1,	"GL_COMPLETION_STATUS_ARB"),	# 0x91B1
    ("glGetProgramPipeline",	I,	1,	"GL_COMPUTE_SHADER"),	# 0x91B9
    ("glGet",	I,	1,	"GL_MAX_COMPUTE_UNIFORM_BLOCKS"),	# 0x91BB
    ("glGet",	I,	1,	"GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS"),	# 0x91BC
    ("glGet",	I,	1,	"GL_MAX_COMPUTE_IMAGE_UNIFORMS"),	# 0x91BD
    ("glGet",	I,	3,	"GL_MAX_COMPUTE_WORK_GROUP_COUNT"),	# 0x91BE
    ("glGet",	I,	3,	"GL_MAX_COMPUTE_WORK_GROUP_SIZE"),	# 0x91BF
    #("",	X,	1,	"GL_UNPACK_FLIP_Y_WEBGL"),	# 0x9240
    #("",	X,	1,	"GL_UNPACK_PREMULTIPLY_ALPHA_WEBGL"),	# 0x9241
    #("",	X,	1,	"GL_CONTEXT_LOST_WEBGL"),	# 0x9242
    #("",	X,	1,	"GL_UNPACK_COLORSPACE_CONVERSION_WEBGL"),	# 0x9243
    #("",	X,	1,	"GL_BROWSER_DEFAULT_WEBGL"),	# 0x9244
    ("",	X,	1,	"GL_SHADER_BINARY_DMP"),	# 0x9250
    #("",	X,	1,	"GL_GCCSO_SHADER_BINARY_FJ"),	# 0x9260
    ("",	X,	1,	"GL_COMPRESSED_R11_EAC"),	# 0x9270
    ("",	X,	1,	"GL_COMPRESSED_SIGNED_R11_EAC"),	# 0x9271
    ("",	X,	1,	"GL_COMPRESSED_RG11_EAC"),	# 0x9272
    ("",	X,	1,	"GL_COMPRESSED_SIGNED_RG11_EAC"),	# 0x9273
    ("",	X,	1,	"GL_COMPRESSED_RGB8_ETC2"),	# 0x9274
    ("",	X,	1,	"GL_COMPRESSED_SRGB8_ETC2"),	# 0x9275
    ("",	X,	1,	"GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2"),	# 0x9276
    ("",	X,	1,	"GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2"),	# 0x9277
    ("",	X,	1,	"GL_COMPRESSED_RGBA8_ETC2_EAC"),	# 0x9278
    ("",	X,	1,	"GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC"),	# 0x9279
    ("",	X,	1,	"GL_BLEND_PREMULTIPLIED_SRC_NV"),	# 0x9280
    ("",	X,	1,	"GL_BLEND_OVERLAP_NV"),	# 0x9281
    ("",	X,	1,	"GL_UNCORRELATED_NV"),	# 0x9282
    ("",	X,	1,	"GL_DISJOINT_NV"),	# 0x9283
    ("",	X,	1,	"GL_CONJOINT_NV"),	# 0x9284
    ("",	X,	1,	"GL_BLEND_ADVANCED_COHERENT_KHR"),	# 0x9285
    ("",	X,	1,	"GL_SRC_NV"),	# 0x9286
    ("",	X,	1,	"GL_DST_NV"),	# 0x9287
    ("",	X,	1,	"GL_SRC_OVER_NV"),	# 0x9288
    ("",	X,	1,	"GL_DST_OVER_NV"),	# 0x9289
    ("",	X,	1,	"GL_SRC_IN_NV"),	# 0x928A
    ("",	X,	1,	"GL_DST_IN_NV"),	# 0x928B
    ("",	X,	1,	"GL_SRC_OUT_NV"),	# 0x928C
    ("",	X,	1,	"GL_DST_OUT_NV"),	# 0x928D
    ("",	X,	1,	"GL_SRC_ATOP_NV"),	# 0x928E
    ("",	X,	1,	"GL_DST_ATOP_NV"),	# 0x928F
    ("",	X,	1,	"GL_PLUS_NV"),	# 0x9291
    ("",	X,	1,	"GL_PLUS_DARKER_NV"),	# 0x9292
    ("",	X,	1,	"GL_MULTIPLY_KHR"),	# 0x9294
    ("",	X,	1,	"GL_SCREEN_KHR"),	# 0x9295
    ("",	X,	1,	"GL_OVERLAY_KHR"),	# 0x9296
    ("",	X,	1,	"GL_DARKEN_KHR"),	# 0x9297
    ("",	X,	1,	"GL_LIGHTEN_KHR"),	# 0x9298
    ("",	X,	1,	"GL_COLORDODGE_KHR"),	# 0x9299
    ("",	X,	1,	"GL_COLORBURN_KHR"),	# 0x929A
    ("",	X,	1,	"GL_HARDLIGHT_KHR"),	# 0x929B
    ("",	X,	1,	"GL_SOFTLIGHT_KHR"),	# 0x929C
    ("",	X,	1,	"GL_DIFFERENCE_KHR"),	# 0x929E
    ("",	X,	1,	"GL_MINUS_NV"),	# 0x929F
    ("",	X,	1,	"GL_EXCLUSION_KHR"),	# 0x92A0
    ("",	X,	1,	"GL_CONTRAST_NV"),	# 0x92A1
    ("",	X,	1,	"GL_INVERT_RGB_NV"),	# 0x92A3
    ("",	X,	1,	"GL_LINEARDODGE_NV"),	# 0x92A4
    ("",	X,	1,	"GL_LINEARBURN_NV"),	# 0x92A5
    ("",	X,	1,	"GL_VIVIDLIGHT_NV"),	# 0x92A6
    ("",	X,	1,	"GL_LINEARLIGHT_NV"),	# 0x92A7
    ("",	X,	1,	"GL_PINLIGHT_NV"),	# 0x92A8
    ("",	X,	1,	"GL_HARDMIX_NV"),	# 0x92A9
    ("",	X,	1,	"GL_HSL_HUE_KHR"),	# 0x92AD
    ("",	X,	1,	"GL_HSL_SATURATION_KHR"),	# 0x92AE
    ("",	X,	1,	"GL_HSL_COLOR_KHR"),	# 0x92AF
    ("",	X,	1,	"GL_HSL_LUMINOSITY_KHR"),	# 0x92B0
    ("",	X,	1,	"GL_PLUS_CLAMPED_NV"),	# 0x92B1
    ("",	X,	1,	"GL_PLUS_CLAMPED_ALPHA_NV"),	# 0x92B2
    ("",	X,	1,	"GL_MINUS_CLAMPED_NV"),	# 0x92B3
    ("",	X,	1,	"GL_INVERT_OVG_NV"),	# 0x92B4
    ("glGet",	F,	8,	"GL_PRIMITIVE_BOUNDING_BOX_EXT"),	# 0x92BE
    ("",	X,	1,	"GL_ATOMIC_COUNTER_BUFFER"),	# 0x92C0
    # FIXME: Causes Mesa to segfault (issue #332).  Disable for now.
    ("_glGet,_glGet_i",	I,	1,	"GL_ATOMIC_COUNTER_BUFFER_BINDING"),	# 0x92C1
    ("glGet_i",	I,	1,	"GL_ATOMIC_COUNTER_BUFFER_START"),	# 0x92C2
    ("glGet_i",	I,	1,	"GL_ATOMIC_COUNTER_BUFFER_SIZE"),	# 0x92C3
    ("",	X,	1,	"GL_ATOMIC_COUNTER_BUFFER_DATA_SIZE"),	# 0x92C4
    ("",	X,	1,	"GL_ATOMIC_COUNTER_BUFFER_ACTIVE_ATOMIC_COUNTERS"),	# 0x92C5
    ("",	X,	1,	"GL_ATOMIC_COUNTER_BUFFER_ACTIVE_ATOMIC_COUNTER_INDICES"),	# 0x92C6
    ("",	X,	1,	"GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_VERTEX_SHADER"),	# 0x92C7
    ("",	X,	1,	"GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_TESS_CONTROL_SHADER"),	# 0x92C8
    ("",	X,	1,	"GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_TESS_EVALUATION_SHADER"),	# 0x92C9
    ("",	X,	1,	"GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_GEOMETRY_SHADER"),	# 0x92CA
    ("",	X,	1,	"GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_FRAGMENT_SHADER"),	# 0x92CB
    ("glGet",	I,	1,	"GL_MAX_VERTEX_ATOMIC_COUNTER_BUFFERS"),	# 0x92CC
    ("glGet",	I,	1,	"GL_MAX_TESS_CONTROL_ATOMIC_COUNTER_BUFFERS"),	# 0x92CD
    ("glGet",	I,	1,	"GL_MAX_TESS_EVALUATION_ATOMIC_COUNTER_BUFFERS"),	# 0x92CE
    ("glGet",	I,	1,	"GL_MAX_GEOMETRY_ATOMIC_COUNTER_BUFFERS"),	# 0x92CF
    ("glGet",	I,	1,	"GL_MAX_FRAGMENT_ATOMIC_COUNTER_BUFFERS"),	# 0x92D0
    ("glGet",	I,	1,	"GL_MAX_COMBINED_ATOMIC_COUNTER_BUFFERS"),	# 0x92D1
    ("glGet",	I,	1,	"GL_MAX_VERTEX_ATOMIC_COUNTERS"),	# 0x92D2
    ("glGet",	I,	1,	"GL_MAX_TESS_CONTROL_ATOMIC_COUNTERS"),	# 0x92D3
    ("glGet",	I,	1,	"GL_MAX_TESS_EVALUATION_ATOMIC_COUNTERS"),	# 0x92D4
    ("glGet",	I,	1,	"GL_MAX_GEOMETRY_ATOMIC_COUNTERS"),	# 0x92D5
    ("glGet",	I,	1,	"GL_MAX_FRAGMENT_ATOMIC_COUNTERS"),	# 0x92D6
    ("glGet",	I,	1,	"GL_MAX_COMBINED_ATOMIC_COUNTERS"),	# 0x92D7
    ("glGet",	I,	1,	"GL_MAX_ATOMIC_COUNTER_BUFFER_SIZE"),	# 0x92D8
    ("glGetProgram",	I,	1,	"GL_ACTIVE_ATOMIC_COUNTER_BUFFERS"),	# 0x92D9
    ("",	X,	1,	"GL_UNIFORM_ATOMIC_COUNTER_BUFFER_INDEX"),	# 0x92DA
    ("",	X,	1,	"GL_UNSIGNED_INT_ATOMIC_COUNTER"),	# 0x92DB
    ("glGet",	I,	1,	"GL_MAX_ATOMIC_COUNTER_BUFFER_BINDINGS"),	# 0x92DC
    ("",	X,	1,	"GL_FRAGMENT_COVERAGE_TO_COLOR_NV"),	# 0x92DD
    ("",	X,	1,	"GL_FRAGMENT_COVERAGE_COLOR_NV"),	# 0x92DE
    ("glGet",	B,	1,	"GL_DEBUG_OUTPUT"),	# 0x92E0
    ("",	X,	1,	"GL_UNIFORM"),	# 0x92E1
    ("",	X,	1,	"GL_UNIFORM_BLOCK"),	# 0x92E2
    ("",	X,	1,	"GL_PROGRAM_INPUT"),	# 0x92E3
    ("",	X,	1,	"GL_PROGRAM_OUTPUT"),	# 0x92E4
    ("",	X,	1,	"GL_BUFFER_VARIABLE"),	# 0x92E5
    ("",	X,	1,	"GL_SHADER_STORAGE_BLOCK"),	# 0x92E6
    ("",	I,	1,	"GL_IS_PER_PATCH"),	# 0x92E7
    ("",	X,	1,	"GL_VERTEX_SUBROUTINE"),	# 0x92E8
    ("",	X,	1,	"GL_TESS_CONTROL_SUBROUTINE"),	# 0x92E9
    ("",	X,	1,	"GL_TESS_EVALUATION_SUBROUTINE"),	# 0x92EA
    ("",	X,	1,	"GL_GEOMETRY_SUBROUTINE"),	# 0x92EB
    ("",	X,	1,	"GL_FRAGMENT_SUBROUTINE"),	# 0x92EC
    ("",	X,	1,	"GL_COMPUTE_SUBROUTINE"),	# 0x92ED
    ("",	X,	1,	"GL_VERTEX_SUBROUTINE_UNIFORM"),	# 0x92EE
    ("",	X,	1,	"GL_TESS_CONTROL_SUBROUTINE_UNIFORM"),	# 0x92EF
    ("",	X,	1,	"GL_TESS_EVALUATION_SUBROUTINE_UNIFORM"),	# 0x92F0
    ("",	X,	1,	"GL_GEOMETRY_SUBROUTINE_UNIFORM"),	# 0x92F1
    ("",	X,	1,	"GL_FRAGMENT_SUBROUTINE_UNIFORM"),	# 0x92F2
    ("",	X,	1,	"GL_COMPUTE_SUBROUTINE_UNIFORM"),	# 0x92F3
    ("",	X,	1,	"GL_TRANSFORM_FEEDBACK_VARYING"),	# 0x92F4
    ("",	X,	1,	"GL_ACTIVE_RESOURCES"),	# 0x92F5
    ("",	X,	1,	"GL_MAX_NAME_LENGTH"),	# 0x92F6
    ("",	X,	1,	"GL_MAX_NUM_ACTIVE_VARIABLES"),	# 0x92F7
    ("",	X,	1,	"GL_MAX_NUM_COMPATIBLE_SUBROUTINES"),	# 0x92F8
    ("",	X,	1,	"GL_NAME_LENGTH"),	# 0x92F9
    ("",	X,	1,	"GL_TYPE"),	# 0x92FA
    ("",	X,	1,	"GL_ARRAY_SIZE"),	# 0x92FB
    ("",	X,	1,	"GL_OFFSET"),	# 0x92FC
    ("",	X,	1,	"GL_BLOCK_INDEX"),	# 0x92FD
    ("",	X,	1,	"GL_ARRAY_STRIDE"),	# 0x92FE
    ("",	X,	1,	"GL_MATRIX_STRIDE"),	# 0x92FF
    ("",	X,	1,	"GL_IS_ROW_MAJOR"),	# 0x9300
    ("",	X,	1,	"GL_ATOMIC_COUNTER_BUFFER_INDEX"),	# 0x9301
    ("",	X,	1,	"GL_BUFFER_BINDING"),	# 0x9302
    ("",	X,	1,	"GL_BUFFER_DATA_SIZE"),	# 0x9303
    ("",	X,	1,	"GL_NUM_ACTIVE_VARIABLES"),	# 0x9304
    ("",	X,	1,	"GL_ACTIVE_VARIABLES"),	# 0x9305
    ("",	X,	1,	"GL_REFERENCED_BY_VERTEX_SHADER"),	# 0x9306
    ("",	X,	1,	"GL_REFERENCED_BY_TESS_CONTROL_SHADER"),	# 0x9307
    ("",	X,	1,	"GL_REFERENCED_BY_TESS_EVALUATION_SHADER"),	# 0x9308
    ("",	X,	1,	"GL_REFERENCED_BY_GEOMETRY_SHADER"),	# 0x9309
    ("",	X,	1,	"GL_REFERENCED_BY_FRAGMENT_SHADER"),	# 0x930A
    ("",	X,	1,	"GL_REFERENCED_BY_COMPUTE_SHADER"),	# 0x930B
    ("",	X,	1,	"GL_TOP_LEVEL_ARRAY_SIZE"),	# 0x930C
    ("",	X,	1,	"GL_TOP_LEVEL_ARRAY_STRIDE"),	# 0x930D
    ("",	X,	1,	"GL_LOCATION"),	# 0x930E
    ("",	X,	1,	"GL_LOCATION_INDEX"),	# 0x930F
    ("",	X,	1,	"GL_FRAMEBUFFER_DEFAULT_WIDTH"),	# 0x9310
    ("",	X,	1,	"GL_FRAMEBUFFER_DEFAULT_HEIGHT"),	# 0x9311
    ("",	X,	1,	"GL_FRAMEBUFFER_DEFAULT_LAYERS"),	# 0x9312
    ("",	X,	1,	"GL_FRAMEBUFFER_DEFAULT_SAMPLES"),	# 0x9313
    ("",	X,	1,	"GL_FRAMEBUFFER_DEFAULT_FIXED_SAMPLE_LOCATIONS"),	# 0x9314
    ("",	X,	1,	"GL_MAX_FRAMEBUFFER_WIDTH"),	# 0x9315
    ("",	X,	1,	"GL_MAX_FRAMEBUFFER_HEIGHT"),	# 0x9316
    ("",	X,	1,	"GL_MAX_FRAMEBUFFER_LAYERS"),	# 0x9317
    ("",	X,	1,	"GL_MAX_FRAMEBUFFER_SAMPLES"),	# 0x9318
    ("",	X,	1,	"GL_RASTER_MULTISAMPLE_EXT"),	# 0x9327
    ("",	X,	1,	"GL_RASTER_SAMPLES_EXT"),	# 0x9328
    ("",	X,	1,	"GL_MAX_RASTER_SAMPLES_EXT"),	# 0x9329
    ("",	X,	1,	"GL_RASTER_FIXED_SAMPLE_LOCATIONS_EXT"),	# 0x932A
    ("",	X,	1,	"GL_MULTISAMPLE_RASTERIZATION_ALLOWED_EXT"),	# 0x932B
    ("",	X,	1,	"GL_EFFECTIVE_RASTER_SAMPLES_EXT"),	# 0x932C
    ("",	X,	1,	"GL_DEPTH_SAMPLES_NV"),	# 0x932D
    ("",	X,	1,	"GL_STENCIL_SAMPLES_NV"),	# 0x932E
    ("",	X,	1,	"GL_MIXED_DEPTH_SAMPLES_SUPPORTED_NV"),	# 0x932F
    ("",	X,	1,	"GL_MIXED_STENCIL_SAMPLES_SUPPORTED_NV"),	# 0x9330
    ("",	X,	1,	"GL_COVERAGE_MODULATION_TABLE_NV"),	# 0x9331
    ("",	X,	1,	"GL_COVERAGE_MODULATION_NV"),	# 0x9332
    ("",	X,	1,	"GL_COVERAGE_MODULATION_TABLE_SIZE_NV"),	# 0x9333
    ("",	X,	1,	"GL_WARP_SIZE_NV"),	# 0x9339
    ("",	X,	1,	"GL_WARPS_PER_SM_NV"),	# 0x933A
    ("",	X,	1,	"GL_SM_COUNT_NV"),	# 0x933B
    ("",	X,	1,	"GL_FILL_RECTANGLE_NV"),	# 0x933C
    ("glGet",	I,	1,	"GL_SAMPLE_LOCATION_SUBPIXEL_BITS_ARB"),	# 0x933D
    ("glGet",	I,	1,	"GL_SAMPLE_LOCATION_PIXEL_GRID_WIDTH_ARB"),	# 0x933E
    ("glGet",	I,	1,	"GL_SAMPLE_LOCATION_PIXEL_GRID_HEIGHT_ARB"),	# 0x933F
    ("",	X,	1,	"GL_PROGRAMMABLE_SAMPLE_LOCATION_TABLE_SIZE_ARB"),	# 0x9340
    ("glGetMultisample",	F,	2,	"GL_PROGRAMMABLE_SAMPLE_LOCATION_ARB"),	# 0x9341
    ("glGetFramebufferParameter",	B,	1,	"GL_FRAMEBUFFER_PROGRAMMABLE_SAMPLE_LOCATIONS_ARB"),	# 0x9342
    ("glGetFramebufferParameter",	B,	1,	"GL_FRAMEBUFFER_SAMPLE_LOCATION_PIXEL_GRID_ARB"),	# 0x9343
    ("glGet",	I,	1,	"GL_MAX_COMPUTE_VARIABLE_GROUP_INVOCATIONS_ARB"),	# 0x9344
    ("glGet",	I,	1,	"GL_MAX_COMPUTE_VARIABLE_GROUP_SIZE_ARB"),	# 0x9345
    ("",	X,	1,	"GL_CONSERVATIVE_RASTERIZATION_NV"),	# 0x9346
    ("",	X,	1,	"GL_SUBPIXEL_PRECISION_BIAS_X_BITS_NV"),	# 0x9347
    ("",	X,	1,	"GL_SUBPIXEL_PRECISION_BIAS_Y_BITS_NV"),	# 0x9348
    ("",	X,	1,	"GL_MAX_SUBPIXEL_PRECISION_BIAS_BITS_NV"),	# 0x9349
    ("",	X,	1,	"GL_LOCATION_COMPONENT"),	# 0x934A
    ("",	X,	1,	"GL_TRANSFORM_FEEDBACK_BUFFER_INDEX"),	# 0x934B
    ("",	X,	1,	"GL_TRANSFORM_FEEDBACK_BUFFER_STRIDE"),	# 0x934C
    ("glGet",	E,	1,	"GL_CLIP_ORIGIN"),	# 0x935C
    ("glGet",	E,	1,	"GL_CLIP_DEPTH_MODE"),	# 0x935D
    ("",	X,	1,	"GL_NEGATIVE_ONE_TO_ONE"),	# 0x935E
    ("",	X,	1,	"GL_ZERO_TO_ONE"),	# 0x935F
    ("",	X,	1,	"GL_CLEAR_TEXTURE"),	# 0x9365
    ("glGetTexParameter",	E,	1,	"GL_TEXTURE_REDUCTION_MODE_ARB"),	# 0x9366
    ("",	X,	1,	"GL_WEIGHTED_AVERAGE_ARB"),	# 0x9367
    ("",	X,	1,	"GL_FONT_GLYPHS_AVAILABLE_NV"),	# 0x9368
    ("",	X,	1,	"GL_FONT_TARGET_UNAVAILABLE_NV"),	# 0x9369
    ("",	X,	1,	"GL_FONT_UNAVAILABLE_NV"),	# 0x936A
    ("",	X,	1,	"GL_FONT_UNINTELLIGIBLE_NV"),	# 0x936B
    ("",	X,	1,	"GL_STANDARD_FONT_FORMAT_NV"),	# 0x936C
    ("",	X,	1,	"GL_FRAGMENT_INPUT_NV"),	# 0x936D
    ("",	X,	1,	"GL_UNIFORM_BUFFER_UNIFIED_NV"),	# 0x936E
    ("",	X,	1,	"GL_UNIFORM_BUFFER_ADDRESS_NV"),	# 0x936F
    ("",	X,	1,	"GL_UNIFORM_BUFFER_LENGTH_NV"),	# 0x9370
    ("",	X,	1,	"GL_MULTISAMPLES_NV"),	# 0x9371
    ("",	X,	1,	"GL_SUPERSAMPLE_SCALE_X_NV"),	# 0x9372
    ("",	X,	1,	"GL_SUPERSAMPLE_SCALE_Y_NV"),	# 0x9373
    ("",	X,	1,	"GL_CONFORMANT_NV"),	# 0x9374
    ("",	X,	1,	"GL_CONSERVATIVE_RASTER_DILATE_NV"),	# 0x9379
    ("",	X,	1,	"GL_CONSERVATIVE_RASTER_DILATE_RANGE_NV"),	# 0x937A
    ("",	X,	1,	"GL_CONSERVATIVE_RASTER_DILATE_GRANULARITY_NV"),	# 0x937B
    ("",	X,	1,	"GL_NUM_SAMPLE_COUNTS"),	# 0x9380
    ("glGet",	F,	2,	"GL_MULTISAMPLE_LINE_WIDTH_RANGE_ARB"),	# 0x9381
    ("glGet",	F,	1,	"GL_MULTISAMPLE_LINE_WIDTH_GRANULARITY_ARB"),	# 0x9382
    ("",	X,	1,	"GL_TRANSLATED_SHADER_SOURCE_LENGTH_ANGLE"),	# 0x93A0
    ("",	X,	1,	"GL_BGRA8_EXT"),	# 0x93A1
    ("",	X,	1,	"GL_TEXTURE_USAGE_ANGLE"),	# 0x93A2
    ("",	X,	1,	"GL_FRAMEBUFFER_ATTACHMENT_ANGLE"),	# 0x93A3
    ("",	X,	1,	"GL_PACK_REVERSE_ROW_ORDER_ANGLE"),	# 0x93A4
    ("",	X,	1,	"GL_PROGRAM_BINARY_ANGLE"),	# 0x93A6
    ("",	X,	1,	"GL_COMPRESSED_RGBA_ASTC_4x4_KHR"),	# 0x93B0
    ("",	X,	1,	"GL_COMPRESSED_RGBA_ASTC_5x4_KHR"),	# 0x93B1
    ("",	X,	1,	"GL_COMPRESSED_RGBA_ASTC_5x5_KHR"),	# 0x93B2
    ("",	X,	1,	"GL_COMPRESSED_RGBA_ASTC_6x5_KHR"),	# 0x93B3
    ("",	X,	1,	"GL_COMPRESSED_RGBA_ASTC_6x6_KHR"),	# 0x93B4
    ("",	X,	1,	"GL_COMPRESSED_RGBA_ASTC_8x5_KHR"),	# 0x93B5
    ("",	X,	1,	"GL_COMPRESSED_RGBA_ASTC_8x6_KHR"),	# 0x93B6
    ("",	X,	1,	"GL_COMPRESSED_RGBA_ASTC_8x8_KHR"),	# 0x93B7
    ("",	X,	1,	"GL_COMPRESSED_RGBA_ASTC_10x5_KHR"),	# 0x93B8
    ("",	X,	1,	"GL_COMPRESSED_RGBA_ASTC_10x6_KHR"),	# 0x93B9
    ("",	X,	1,	"GL_COMPRESSED_RGBA_ASTC_10x8_KHR"),	# 0x93BA
    ("",	X,	1,	"GL_COMPRESSED_RGBA_ASTC_10x10_KHR"),	# 0x93BB
    ("",	X,	1,	"GL_COMPRESSED_RGBA_ASTC_12x10_KHR"),	# 0x93BC
    ("",	X,	1,	"GL_COMPRESSED_RGBA_ASTC_12x12_KHR"),	# 0x93BD
    #("",	X,	1,	"GL_COMPRESSED_RGBA_ASTC_3x3x3_OES"),	# 0x93C0
    #("",	X,	1,	"GL_COMPRESSED_RGBA_ASTC_4x3x3_OES"),	# 0x93C1
    #("",	X,	1,	"GL_COMPRESSED_RGBA_ASTC_4x4x3_OES"),	# 0x93C2
    #("",	X,	1,	"GL_COMPRESSED_RGBA_ASTC_4x4x4_OES"),	# 0x93C3
    #("",	X,	1,	"GL_COMPRESSED_RGBA_ASTC_5x4x4_OES"),	# 0x93C4
    #("",	X,	1,	"GL_COMPRESSED_RGBA_ASTC_5x5x4_OES"),	# 0x93C5
    #("",	X,	1,	"GL_COMPRESSED_RGBA_ASTC_5x5x5_OES"),	# 0x93C6
    #("",	X,	1,	"GL_COMPRESSED_RGBA_ASTC_6x5x5_OES"),	# 0x93C7
    #("",	X,	1,	"GL_COMPRESSED_RGBA_ASTC_6x6x5_OES"),	# 0x93C8
    #("",	X,	1,	"GL_COMPRESSED_RGBA_ASTC_6x6x6_OES"),	# 0x93C9
    ("",	X,	1,	"GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR"),	# 0x93D0
    ("",	X,	1,	"GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR"),	# 0x93D1
    ("",	X,	1,	"GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR"),	# 0x93D2
    ("",	X,	1,	"GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR"),	# 0x93D3
    ("",	X,	1,	"GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR"),	# 0x93D4
    ("",	X,	1,	"GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR"),	# 0x93D5
    ("",	X,	1,	"GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR"),	# 0x93D6
    ("",	X,	1,	"GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR"),	# 0x93D7
    ("",	X,	1,	"GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR"),	# 0x93D8
    ("",	X,	1,	"GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR"),	# 0x93D9
    ("",	X,	1,	"GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR"),	# 0x93DA
    ("",	X,	1,	"GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR"),	# 0x93DB
    ("",	X,	1,	"GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR"),	# 0x93DC
    ("",	X,	1,	"GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR"),	# 0x93DD
    #("",	X,	1,	"GL_COMPRESSED_SRGB8_ALPHA8_ASTC_3x3x3_OES"),	# 0x93E0
    #("",	X,	1,	"GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x3x3_OES"),	# 0x93E1
    #("",	X,	1,	"GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4x3_OES"),	# 0x93E2
    #("",	X,	1,	"GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4x4_OES"),	# 0x93E3
    #("",	X,	1,	"GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4x4_OES"),	# 0x93E4
    #("",	X,	1,	"GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5x4_OES"),	# 0x93E5
    #("",	X,	1,	"GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5x5_OES"),	# 0x93E6
    #("",	X,	1,	"GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5x5_OES"),	# 0x93E7
    #("",	X,	1,	"GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6x5_OES"),	# 0x93E8
    #("",	X,	1,	"GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6x6_OES"),	# 0x93E9
    ("",	X,	1,	"GL_COMPRESSED_SRGB_ALPHA_PVRTC_2BPPV2_IMG"),	# 0x93F0
    ("",	X,	1,	"GL_COMPRESSED_SRGB_ALPHA_PVRTC_4BPPV2_IMG"),	# 0x93F1
    ("",	X,	1,	"GL_PERFQUERY_COUNTER_EVENT_INTEL"),	# 0x94F0
    ("",	X,	1,	"GL_PERFQUERY_COUNTER_DURATION_NORM_INTEL"),	# 0x94F1
    ("",	X,	1,	"GL_PERFQUERY_COUNTER_DURATION_RAW_INTEL"),	# 0x94F2
    ("",	X,	1,	"GL_PERFQUERY_COUNTER_THROUGHPUT_INTEL"),	# 0x94F3
    ("",	X,	1,	"GL_PERFQUERY_COUNTER_RAW_INTEL"),	# 0x94F4
    ("",	X,	1,	"GL_PERFQUERY_COUNTER_TIMESTAMP_INTEL"),	# 0x94F5
    ("",	X,	1,	"GL_PERFQUERY_COUNTER_DATA_UINT32_INTEL"),	# 0x94F8
    ("",	X,	1,	"GL_PERFQUERY_COUNTER_DATA_UINT64_INTEL"),	# 0x94F9
    ("",	X,	1,	"GL_PERFQUERY_COUNTER_DATA_FLOAT_INTEL"),	# 0x94FA
    ("",	X,	1,	"GL_PERFQUERY_COUNTER_DATA_DOUBLE_INTEL"),	# 0x94FB
    ("",	X,	1,	"GL_PERFQUERY_COUNTER_DATA_BOOL32_INTEL"),	# 0x94FC
    ("",	X,	1,	"GL_PERFQUERY_QUERY_NAME_LENGTH_MAX_INTEL"),	# 0x94FD
    ("",	X,	1,	"GL_PERFQUERY_COUNTER_NAME_LENGTH_MAX_INTEL"),	# 0x94FE
    ("",	X,	1,	"GL_PERFQUERY_COUNTER_DESC_LENGTH_MAX_INTEL"),	# 0x94FF
    ("",	X,	1,	"GL_PERFQUERY_GPA_EXTENDED_COUNTERS_INTEL"),	# 0x9500
    ("_glGet",	B,	1,	"GL_RASTER_POSITION_UNCLIPPED_IBM"),	# 0x19262
    ("",	X,	1,	"GL_CULL_VERTEX_IBM"),	# 0x1928A
    ("",	X,	1,	"GL_ALL_STATIC_DATA_IBM"),	# 0x19294
    ("",	X,	1,	"GL_STATIC_VERTEX_ARRAY_IBM"),	# 0x19295
    ("",	X,	1,	"GL_VERTEX_ARRAY_LIST_IBM"),	# 0x1929E
    ("",	X,	1,	"GL_NORMAL_ARRAY_LIST_IBM"),	# 0x1929F
    ("",	X,	1,	"GL_COLOR_ARRAY_LIST_IBM"),	# 0x192A0
    ("",	X,	1,	"GL_INDEX_ARRAY_LIST_IBM"),	# 0x192A1
    ("",	X,	1,	"GL_TEXTURE_COORD_ARRAY_LIST_IBM"),	# 0x192A2
    ("",	X,	1,	"GL_EDGE_FLAG_ARRAY_LIST_IBM"),	# 0x192A3
    ("",	X,	1,	"GL_FOG_COORDINATE_ARRAY_LIST_IBM"),	# 0x192A4
    ("",	X,	1,	"GL_SECONDARY_COLOR_ARRAY_LIST_IBM"),	# 0x192A5
    ("",	X,	1,	"GL_VERTEX_ARRAY_LIST_STRIDE_IBM"),	# 0x192A8
    ("",	X,	1,	"GL_NORMAL_ARRAY_LIST_STRIDE_IBM"),	# 0x192A9
    ("",	X,	1,	"GL_COLOR_ARRAY_LIST_STRIDE_IBM"),	# 0x192AA
    ("",	X,	1,	"GL_INDEX_ARRAY_LIST_STRIDE_IBM"),	# 0x192AB
    ("",	X,	1,	"GL_TEXTURE_COORD_ARRAY_LIST_STRIDE_IBM"),	# 0x192AC
    ("",	X,	1,	"GL_EDGE_FLAG_ARRAY_LIST_STRIDE_IBM"),	# 0x192AD
    ("",	X,	1,	"GL_FOG_COORDINATE_ARRAY_LIST_STRIDE_IBM"),	# 0x192AE
    ("",	X,	1,	"GL_SECONDARY_COLOR_ARRAY_LIST_STRIDE_IBM"),	# 0x192AF
    ("",	X,	1,	"GL_PREFER_DOUBLEBUFFER_HINT_PGI"),	# 0x1A1F8
    ("",	X,	1,	"GL_CONSERVE_MEMORY_HINT_PGI"),	# 0x1A1FD
    ("",	X,	1,	"GL_RECLAIM_MEMORY_HINT_PGI"),	# 0x1A1FE
    ("",	X,	1,	"GL_NATIVE_GRAPHICS_HANDLE_PGI"),	# 0x1A202
    ("",	X,	1,	"GL_NATIVE_GRAPHICS_BEGIN_HINT_PGI"),	# 0x1A203
    ("",	X,	1,	"GL_NATIVE_GRAPHICS_END_HINT_PGI"),	# 0x1A204
    ("",	X,	1,	"GL_ALWAYS_FAST_HINT_PGI"),	# 0x1A20C
    ("",	X,	1,	"GL_ALWAYS_SOFT_HINT_PGI"),	# 0x1A20D
    ("",	X,	1,	"GL_ALLOW_DRAW_OBJ_HINT_PGI"),	# 0x1A20E
    ("",	X,	1,	"GL_ALLOW_DRAW_WIN_HINT_PGI"),	# 0x1A20F
    ("",	X,	1,	"GL_ALLOW_DRAW_FRG_HINT_PGI"),	# 0x1A210
    ("",	X,	1,	"GL_ALLOW_DRAW_MEM_HINT_PGI"),	# 0x1A211
    ("",	X,	1,	"GL_STRICT_DEPTHFUNC_HINT_PGI"),	# 0x1A216
    ("",	X,	1,	"GL_STRICT_LIGHTING_HINT_PGI"),	# 0x1A217
    ("",	X,	1,	"GL_STRICT_SCISSOR_HINT_PGI"),	# 0x1A218
    ("",	X,	1,	"GL_FULL_STIPPLE_HINT_PGI"),	# 0x1A219
    ("",	X,	1,	"GL_CLIP_NEAR_HINT_PGI"),	# 0x1A220
    ("",	X,	1,	"GL_CLIP_FAR_HINT_PGI"),	# 0x1A221
    ("",	X,	1,	"GL_WIDE_LINE_HINT_PGI"),	# 0x1A222
    ("",	X,	1,	"GL_BACK_NORMALS_HINT_PGI"),	# 0x1A223
    ("",	X,	1,	"GL_VERTEX_DATA_HINT_PGI"),	# 0x1A22A
    ("",	X,	1,	"GL_VERTEX_CONSISTENT_HINT_PGI"),	# 0x1A22B
    ("",	X,	1,	"GL_MATERIAL_SIDE_HINT_PGI"),	# 0x1A22C
    ("",	X,	1,	"GL_MAX_VERTEX_HINT_PGI"),	# 0x1A22D
    ("",	X,	1,	"GL_INVALID_INDEX"),	# 0xFFFFFFFFu
    #("",	X,	1,	"GL_TIMEOUT_IGNORED"),	# 0xFFFFFFFFFFFFFFFFull
]

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

import copy

class GroupEntry(object):
    def __init__(self, name, value):
        self.name = name
        self.value = value
        self.versions = []
        self.extensions = []

class Group(object):
    def __init__(self, group, glxml):
        self.group = group
        self.entries = []
        
        for v in self.group.enumNames:
            self.entries.append(GroupEntry(v, glxml.enumValues[v]))

import glxml

gl = glxml.GL(False)

groups = {}
for k, v in gl.groups.iteritems():
    groups[k] = Group(v, gl)
groups["Unnamed"] = Group(glxml.Group(), glxml)

for ver_num in versions:
    ver = gl.versions[ver_num]
    vers = versions[versions.index(ver_num):]
    
    for enum in ver.new_enums:
        for other_ver in vers:
            found = False
            
            for group in groups.values():
                for entry in group.entries:
                    if entry.name == enum:
                        entry.versions.append(other_ver)
                        found = True
            
            if not found:
                entry = GroupEntry(enum, gl.enumValues[enum])
                entry.versions.append(ver_num)
                groups["Unnamed"].entries.append(entry)
    
    for enum in ver.removed_enums:
        for other_ver in vers:
            for group in groups.values():
                for entry in group.entries:
                    if entry.name == enum:
                        entry.versions.remove(other_ver)

for k, v in gl.extensions.iteritems():
    for enum in v.enums:
        found = False
        
        for group in groups.values():
            for entry in group.entries:
                if entry.name == enum:
                    entry.extensions.append(k)
        
        if not found:
            entry = GroupEntry(enum, gl.enumValues[enum])
            entry.extensions.append(k)
            groups["Unnamed"].entries.append(entry)

print "----------------------------------------------------------------"

for param in parameters:
    if param[0] == "glGet":
        vers = []
        exts = []
        
        for k, v in groups.iteritems():
            for entry in v.entries:
                if entry.name == param[3]:
                    vers += entry.versions
                    exts += entry.extensions
        
        if type(param[2]) == str:
            print "           (\"%s\", \"%s\", %d, %s, %s)," % (param[3], param[1], 1, str(list(set(vers))), str(exts))
        else:
            print "           (\"%s\", \"%s\", %d, %s, %s)," % (param[3], param[1], param[2], str(list(set(vers))), str(exts))
"""
           #Name, type, count, versions, extensions
gl_gets = [("GL_CURRENT_COLOR", "F", 4, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_CURRENT_INDEX", "F", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_CURRENT_NORMAL", "F", 3, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_CURRENT_TEXTURE_COORDS", "F", 4, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_CURRENT_RASTER_COLOR", "F", 4, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_CURRENT_RASTER_INDEX", "F", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_CURRENT_RASTER_TEXTURE_COORDS", "F", 4, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_CURRENT_RASTER_POSITION", "F", 4, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_CURRENT_RASTER_POSITION_VALID", "I", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_CURRENT_RASTER_DISTANCE", "F", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_POINT_SMOOTH", "B", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_POINT_SIZE", "F", 1, [(1, 2), (3, 2), (1, 3), (3, 3), (2, 0), (3, 0), (3, 1), (4, 4), (1, 4), (1, 5), (2, 1), (4, 3), (4, 5), (4, 2), (4, 1), (1, 1), (4, 0)], []),
           ("GL_POINT_SIZE_RANGE", "F", 2, [(1, 2), (3, 2), (1, 3), (3, 3), (2, 0), (3, 0), (3, 1), (4, 4), (1, 4), (1, 5), (2, 1), (4, 3), (4, 5), (4, 2), (4, 1), (1, 1), (4, 0)], []),
           ("GL_POINT_SIZE_GRANULARITY", "F", 1, [(1, 2), (3, 2), (1, 3), (3, 3), (2, 0), (3, 0), (3, 1), (4, 4), (1, 4), (1, 5), (2, 1), (4, 3), (4, 5), (4, 2), (4, 1), (1, 1), (4, 0)], []),
           ("GL_LINE_SMOOTH", "B", 1, [(1, 2), (3, 2), (1, 3), (3, 3), (2, 0), (3, 0), (3, 1), (4, 4), (1, 4), (1, 5), (2, 1), (4, 3), (4, 5), (4, 2), (4, 1), (1, 1), (4, 0)], []),
           ("GL_LINE_WIDTH", "F", 1, [(1, 2), (3, 2), (1, 3), (3, 3), (2, 0), (3, 0), (3, 1), (4, 4), (1, 4), (1, 5), (2, 1), (4, 3), (4, 5), (4, 2), (4, 1), (1, 1), (4, 0)], []),
           ("GL_LINE_WIDTH_RANGE", "F", 2, [(1, 2), (3, 2), (1, 3), (3, 3), (2, 0), (3, 0), (3, 1), (4, 4), (1, 4), (1, 5), (2, 1), (4, 3), (4, 5), (4, 2), (4, 1), (1, 1), (4, 0)], []),
           ("GL_LINE_WIDTH_GRANULARITY", "F", 1, [(1, 2), (3, 2), (1, 3), (3, 3), (2, 0), (3, 0), (3, 1), (4, 4), (1, 4), (1, 5), (2, 1), (4, 3), (4, 5), (4, 2), (4, 1), (1, 1), (4, 0)], []),
           ("GL_LINE_STIPPLE", "B", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_LINE_STIPPLE_PATTERN", "I", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_LINE_STIPPLE_REPEAT", "I", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_LIST_MODE", "E", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_MAX_LIST_NESTING", "I", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_LIST_BASE", "I", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_LIST_INDEX", "I", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_POLYGON_MODE", "E", 2, [(1, 2), (3, 2), (1, 3), (3, 3), (2, 0), (3, 0), (3, 1), (4, 4), (1, 4), (1, 5), (2, 1), (4, 3), (4, 5), (4, 2), (4, 1), (1, 1), (4, 0)], []),
           ("GL_POLYGON_SMOOTH", "B", 1, [(1, 2), (3, 2), (1, 3), (3, 3), (2, 0), (3, 0), (3, 1), (4, 4), (1, 4), (1, 5), (2, 1), (4, 3), (4, 5), (4, 2), (4, 1), (1, 1), (4, 0)], []),
           ("GL_POLYGON_STIPPLE", "B", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_EDGE_FLAG", "B", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_CULL_FACE", "B", 1, [(1, 2), (3, 2), (1, 3), (3, 3), (2, 0), (3, 0), (3, 1), (4, 4), (1, 4), (1, 5), (2, 1), (4, 3), (4, 5), (4, 2), (4, 1), (1, 1), (4, 0)], []),
           ("GL_CULL_FACE_MODE", "E", 1, [(1, 2), (3, 2), (1, 3), (3, 3), (2, 0), (3, 0), (3, 1), (4, 4), (1, 4), (1, 5), (2, 1), (4, 3), (4, 5), (4, 2), (4, 1), (1, 1), (4, 0)], []),
           ("GL_FRONT_FACE", "E", 1, [(1, 2), (3, 2), (1, 3), (3, 3), (2, 0), (3, 0), (3, 1), (4, 4), (1, 4), (1, 5), (2, 1), (4, 3), (4, 5), (4, 2), (4, 1), (1, 1), (4, 0)], []),
           ("GL_LIGHTING", "B", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_LIGHT_MODEL_LOCAL_VIEWER", "B", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_LIGHT_MODEL_TWO_SIDE", "B", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_LIGHT_MODEL_AMBIENT", "F", 4, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_SHADE_MODEL", "E", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_COLOR_MATERIAL_FACE", "E", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_COLOR_MATERIAL_PARAMETER", "E", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_COLOR_MATERIAL", "B", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_FOG", "B", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], ['GL_NV_register_combiners', 'GL_NV_register_combiners', 'GL_NV_register_combiners']),
           ("GL_FOG_INDEX", "I", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_FOG_DENSITY", "F", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_FOG_START", "F", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_FOG_END", "F", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_FOG_MODE", "E", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_FOG_COLOR", "F", 4, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_DEPTH_RANGE", "F", 2, [(1, 2), (3, 2), (1, 3), (3, 3), (2, 0), (3, 0), (3, 1), (4, 4), (1, 4), (1, 5), (2, 1), (4, 3), (4, 5), (4, 2), (4, 1), (1, 1), (4, 0)], ['GL_NV_viewport_array2', 'GL_ARB_viewport_array', 'GL_NV_viewport_array', 'GL_ARB_viewport_array', 'GL_NV_viewport_array', 'GL_NV_viewport_array', 'GL_NV_viewport_array2', 'GL_ARB_viewport_array', 'GL_NV_viewport_array']),
           ("GL_DEPTH_TEST", "B", 1, [(1, 2), (3, 2), (1, 3), (3, 3), (2, 0), (3, 0), (3, 1), (4, 4), (1, 4), (1, 5), (2, 1), (4, 3), (4, 5), (4, 2), (4, 1), (1, 1), (4, 0)], []),
           ("GL_DEPTH_WRITEMASK", "B", 1, [(1, 2), (3, 2), (1, 3), (3, 3), (2, 0), (3, 0), (3, 1), (4, 4), (1, 4), (1, 5), (2, 1), (4, 3), (4, 5), (4, 2), (4, 1), (1, 1), (4, 0)], []),
           ("GL_DEPTH_CLEAR_VALUE", "F", 1, [(1, 2), (3, 2), (1, 3), (3, 3), (2, 0), (3, 0), (3, 1), (4, 4), (1, 4), (1, 5), (2, 1), (4, 3), (4, 5), (4, 2), (4, 1), (1, 1), (4, 0)], []),
           ("GL_DEPTH_FUNC", "E", 1, [(1, 2), (3, 2), (1, 3), (3, 3), (2, 0), (3, 0), (3, 1), (4, 4), (1, 4), (1, 5), (2, 1), (4, 3), (4, 5), (4, 2), (4, 1), (1, 1), (4, 0)], []),
           ("GL_ACCUM_CLEAR_VALUE", "F", 4, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_STENCIL_TEST", "B", 1, [(1, 2), (3, 2), (1, 3), (3, 3), (2, 0), (3, 0), (3, 1), (4, 4), (1, 4), (1, 5), (2, 1), (4, 3), (4, 5), (4, 2), (4, 1), (1, 1), (4, 0)], []),
           ("GL_STENCIL_CLEAR_VALUE", "I", 1, [(1, 2), (3, 2), (1, 3), (3, 3), (2, 0), (3, 0), (3, 1), (4, 4), (1, 4), (1, 5), (2, 1), (4, 3), (4, 5), (4, 2), (4, 1), (1, 1), (4, 0)], []),
           ("GL_STENCIL_FUNC", "E", 1, [(1, 2), (3, 2), (1, 3), (3, 3), (2, 0), (3, 0), (3, 1), (4, 4), (1, 4), (1, 5), (2, 1), (4, 3), (4, 5), (4, 2), (4, 1), (1, 1), (4, 0)], []),
           ("GL_STENCIL_VALUE_MASK", "I", 1, [(1, 2), (3, 2), (1, 3), (3, 3), (2, 0), (3, 0), (3, 1), (4, 4), (1, 4), (1, 5), (2, 1), (4, 3), (4, 5), (4, 2), (4, 1), (1, 1), (4, 0)], []),
           ("GL_STENCIL_FAIL", "E", 1, [(1, 2), (3, 2), (1, 3), (3, 3), (2, 0), (3, 0), (3, 1), (4, 4), (1, 4), (1, 5), (2, 1), (4, 3), (4, 5), (4, 2), (4, 1), (1, 1), (4, 0)], []),
           ("GL_STENCIL_PASS_DEPTH_FAIL", "E", 1, [(1, 2), (3, 2), (1, 3), (3, 3), (2, 0), (3, 0), (3, 1), (4, 4), (1, 4), (1, 5), (2, 1), (4, 3), (4, 5), (4, 2), (4, 1), (1, 1), (4, 0)], []),
           ("GL_STENCIL_PASS_DEPTH_PASS", "E", 1, [(1, 2), (3, 2), (1, 3), (3, 3), (2, 0), (3, 0), (3, 1), (4, 4), (1, 4), (1, 5), (2, 1), (4, 3), (4, 5), (4, 2), (4, 1), (1, 1), (4, 0)], []),
           ("GL_STENCIL_REF", "I", 1, [(1, 2), (3, 2), (1, 3), (3, 3), (2, 0), (3, 0), (3, 1), (4, 4), (1, 4), (1, 5), (2, 1), (4, 3), (4, 5), (4, 2), (4, 1), (1, 1), (4, 0)], []),
           ("GL_STENCIL_WRITEMASK", "I", 1, [(1, 2), (3, 2), (1, 3), (3, 3), (2, 0), (3, 0), (3, 1), (4, 4), (1, 4), (1, 5), (2, 1), (4, 3), (4, 5), (4, 2), (4, 1), (1, 1), (4, 0)], []),
           ("GL_MATRIX_MODE", "E", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_NORMALIZE", "B", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_VIEWPORT", "I", 4, [(1, 2), (3, 2), (1, 3), (3, 3), (2, 0), (3, 0), (3, 1), (4, 4), (1, 4), (1, 5), (2, 1), (4, 3), (4, 5), (4, 2), (4, 1), (1, 1), (4, 0)], ['GL_NV_viewport_array2', 'GL_ARB_viewport_array', 'GL_NV_viewport_array', 'GL_ARB_viewport_array', 'GL_NV_viewport_array', 'GL_NV_viewport_array', 'GL_NV_viewport_array2', 'GL_ARB_viewport_array', 'GL_NV_viewport_array']),
           ("GL_MODELVIEW_STACK_DEPTH", "I", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_PROJECTION_STACK_DEPTH", "I", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_TEXTURE_STACK_DEPTH", "I", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_MODELVIEW_MATRIX", "F", 16, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_PROJECTION_MATRIX", "F", 16, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_TEXTURE_MATRIX", "F", 16, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_ATTRIB_STACK_DEPTH", "I", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_CLIENT_ATTRIB_STACK_DEPTH", "I", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_ALPHA_TEST", "B", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_ALPHA_TEST_FUNC", "E", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_ALPHA_TEST_REF", "F", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_DITHER", "B", 1, [(1, 2), (3, 2), (1, 3), (3, 3), (2, 0), (3, 0), (3, 1), (4, 4), (1, 4), (1, 5), (2, 1), (4, 3), (4, 5), (4, 2), (4, 1), (1, 1), (4, 0)], []),
           ("GL_BLEND_DST", "E", 1, [(1, 2), (3, 2), (1, 3), (3, 3), (2, 0), (3, 0), (3, 1), (4, 4), (1, 4), (1, 5), (2, 1), (4, 3), (4, 5), (4, 2), (4, 1), (1, 1), (4, 0)], []),
           ("GL_BLEND_SRC", "E", 1, [(1, 2), (3, 2), (1, 3), (3, 3), (2, 0), (3, 0), (3, 1), (4, 4), (1, 4), (1, 5), (2, 1), (4, 3), (4, 5), (4, 2), (4, 1), (1, 1), (4, 0)], []),
           ("GL_BLEND", "B", 1, [(1, 2), (3, 2), (1, 3), (3, 3), (2, 0), (3, 0), (3, 1), (4, 4), (1, 4), (1, 5), (2, 1), (4, 3), (4, 5), (4, 2), (4, 1), (1, 1), (4, 0)], ['GL_OES_draw_buffers_indexed', 'GL_EXT_draw_buffers_indexed', 'GL_EXT_draw_buffers_indexed', 'GL_OES_draw_buffers_indexed', 'GL_EXT_draw_buffers_indexed', 'GL_OES_draw_buffers_indexed', 'GL_EXT_draw_buffers_indexed', 'GL_OES_draw_buffers_indexed', 'GL_EXT_draw_buffers_indexed']),
           ("GL_LOGIC_OP_MODE", "E", 1, [(1, 2), (3, 2), (1, 3), (3, 3), (2, 0), (3, 0), (3, 1), (4, 4), (1, 4), (1, 5), (2, 1), (4, 3), (4, 5), (4, 2), (4, 1), (1, 1), (4, 0)], []),
           ("GL_INDEX_LOGIC_OP", "B", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_COLOR_LOGIC_OP", "B", 1, [(1, 2), (3, 2), (1, 3), (3, 3), (2, 0), (3, 0), (3, 1), (4, 4), (1, 4), (1, 5), (2, 1), (4, 3), (4, 5), (4, 2), (4, 1), (1, 1), (4, 0)], []),
           ("GL_AUX_BUFFERS", "I", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_DRAW_BUFFER", "E", 1, [(1, 2), (3, 2), (1, 3), (3, 3), (2, 0), (3, 0), (3, 1), (4, 4), (1, 4), (1, 5), (2, 1), (4, 3), (4, 5), (4, 2), (4, 1), (1, 1), (4, 0)], []),
           ("GL_READ_BUFFER", "E", 1, [(1, 2), (3, 2), (1, 3), (3, 3), (2, 0), (3, 0), (3, 1), (4, 4), (1, 4), (1, 5), (2, 1), (4, 3), (4, 5), (4, 2), (4, 1), (1, 1), (4, 0)], []),
           ("GL_SCISSOR_BOX", "I", 4, [(1, 2), (3, 2), (1, 3), (3, 3), (2, 0), (3, 0), (3, 1), (4, 4), (1, 4), (1, 5), (2, 1), (4, 3), (4, 5), (4, 2), (4, 1), (1, 1), (4, 0)], ['GL_NV_viewport_array2', 'GL_ARB_viewport_array', 'GL_NV_viewport_array', 'GL_ARB_viewport_array', 'GL_NV_viewport_array', 'GL_NV_viewport_array', 'GL_NV_viewport_array2', 'GL_ARB_viewport_array', 'GL_NV_viewport_array']),
           ("GL_SCISSOR_TEST", "B", 1, [(1, 2), (3, 2), (1, 3), (3, 3), (2, 0), (3, 0), (3, 1), (4, 4), (1, 4), (1, 5), (2, 1), (4, 3), (4, 5), (4, 2), (4, 1), (1, 1), (4, 0)], ['GL_NV_viewport_array2', 'GL_ARB_viewport_array', 'GL_NV_viewport_array', 'GL_ARB_viewport_array', 'GL_NV_viewport_array', 'GL_NV_viewport_array', 'GL_NV_viewport_array2', 'GL_ARB_viewport_array', 'GL_NV_viewport_array', 'GL_NV_viewport_array2', 'GL_ARB_viewport_array', 'GL_NV_viewport_array']),
           ("GL_INDEX_CLEAR_VALUE", "I", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_INDEX_WRITEMASK", "I", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_COLOR_CLEAR_VALUE", "F", 4, [(1, 2), (3, 2), (1, 3), (3, 3), (2, 0), (3, 0), (3, 1), (4, 4), (1, 4), (1, 5), (2, 1), (4, 3), (4, 5), (4, 2), (4, 1), (1, 1), (4, 0)], []),
           ("GL_COLOR_WRITEMASK", "B", 4, [(1, 2), (3, 2), (1, 3), (3, 3), (2, 0), (3, 0), (3, 1), (4, 4), (1, 4), (1, 5), (2, 1), (4, 3), (4, 5), (4, 2), (4, 1), (1, 1), (4, 0)], ['GL_OES_draw_buffers_indexed', 'GL_EXT_draw_buffers_indexed', 'GL_EXT_draw_buffers_indexed', 'GL_OES_draw_buffers_indexed', 'GL_EXT_draw_buffers_indexed']),
           ("GL_INDEX_MODE", "B", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_RGBA_MODE", "B", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_DOUBLEBUFFER", "B", 1, [(1, 2), (3, 2), (1, 3), (3, 3), (2, 0), (3, 0), (3, 1), (4, 4), (1, 4), (1, 5), (2, 1), (4, 3), (4, 5), (4, 2), (4, 1), (1, 1), (4, 0)], []),
           ("GL_STEREO", "B", 1, [(1, 2), (3, 2), (1, 3), (3, 3), (2, 0), (3, 0), (3, 1), (4, 4), (1, 4), (1, 5), (2, 1), (4, 3), (4, 5), (4, 2), (4, 1), (1, 1), (4, 0)], []),
           ("GL_RENDER_MODE", "E", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_PERSPECTIVE_CORRECTION_HINT", "E", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_POINT_SMOOTH_HINT", "E", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_LINE_SMOOTH_HINT", "E", 1, [(1, 2), (3, 2), (1, 3), (3, 3), (2, 0), (3, 0), (3, 1), (4, 4), (1, 4), (1, 5), (2, 1), (4, 3), (4, 5), (4, 2), (4, 1), (1, 1), (4, 0)], []),
           ("GL_POLYGON_SMOOTH_HINT", "E", 1, [(1, 2), (3, 2), (1, 3), (3, 3), (2, 0), (3, 0), (3, 1), (4, 4), (1, 4), (1, 5), (2, 1), (4, 3), (4, 5), (4, 2), (4, 1), (1, 1), (4, 0)], []),
           ("GL_FOG_HINT", "E", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_TEXTURE_GEN_S", "B", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_TEXTURE_GEN_T", "B", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_TEXTURE_GEN_R", "B", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_TEXTURE_GEN_Q", "B", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_PIXEL_MAP_I_TO_I_SIZE", "I", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_PIXEL_MAP_S_TO_S_SIZE", "I", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_PIXEL_MAP_I_TO_R_SIZE", "I", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_PIXEL_MAP_I_TO_G_SIZE", "I", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_PIXEL_MAP_I_TO_B_SIZE", "I", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_PIXEL_MAP_I_TO_A_SIZE", "I", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_PIXEL_MAP_R_TO_R_SIZE", "I", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_PIXEL_MAP_G_TO_G_SIZE", "I", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_PIXEL_MAP_B_TO_B_SIZE", "I", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_PIXEL_MAP_A_TO_A_SIZE", "I", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_UNPACK_SWAP_BYTES", "B", 1, [(1, 2), (3, 2), (1, 3), (3, 3), (2, 0), (3, 0), (3, 1), (4, 4), (1, 4), (1, 5), (2, 1), (4, 3), (4, 5), (4, 2), (4, 1), (1, 1), (4, 0)], []),
           ("GL_UNPACK_LSB_FIRST", "B", 1, [(1, 2), (3, 2), (1, 3), (3, 3), (2, 0), (3, 0), (3, 1), (4, 4), (1, 4), (1, 5), (2, 1), (4, 3), (4, 5), (4, 2), (4, 1), (1, 1), (4, 0)], []),
           ("GL_UNPACK_ROW_LENGTH", "I", 1, [(1, 2), (3, 2), (1, 3), (3, 3), (2, 0), (3, 0), (3, 1), (4, 4), (1, 4), (1, 5), (2, 1), (4, 3), (4, 5), (4, 2), (4, 1), (1, 1), (4, 0)], []),
           ("GL_UNPACK_SKIP_ROWS", "I", 1, [(1, 2), (3, 2), (1, 3), (3, 3), (2, 0), (3, 0), (3, 1), (4, 4), (1, 4), (1, 5), (2, 1), (4, 3), (4, 5), (4, 2), (4, 1), (1, 1), (4, 0)], []),
           ("GL_UNPACK_SKIP_PIXELS", "I", 1, [(1, 2), (3, 2), (1, 3), (3, 3), (2, 0), (3, 0), (3, 1), (4, 4), (1, 4), (1, 5), (2, 1), (4, 3), (4, 5), (4, 2), (4, 1), (1, 1), (4, 0)], []),
           ("GL_UNPACK_ALIGNMENT", "I", 1, [(1, 2), (3, 2), (1, 3), (3, 3), (2, 0), (3, 0), (3, 1), (4, 4), (1, 4), (1, 5), (2, 1), (4, 3), (4, 5), (4, 2), (4, 1), (1, 1), (4, 0)], []),
           ("GL_PACK_SWAP_BYTES", "B", 1, [(1, 2), (3, 2), (1, 3), (3, 3), (2, 0), (3, 0), (3, 1), (4, 4), (1, 4), (1, 5), (2, 1), (4, 3), (4, 5), (4, 2), (4, 1), (1, 1), (4, 0)], []),
           ("GL_PACK_LSB_FIRST", "B", 1, [(1, 2), (3, 2), (1, 3), (3, 3), (2, 0), (3, 0), (3, 1), (4, 4), (1, 4), (1, 5), (2, 1), (4, 3), (4, 5), (4, 2), (4, 1), (1, 1), (4, 0)], []),
           ("GL_PACK_ROW_LENGTH", "I", 1, [(1, 2), (3, 2), (1, 3), (3, 3), (2, 0), (3, 0), (3, 1), (4, 4), (1, 4), (1, 5), (2, 1), (4, 3), (4, 5), (4, 2), (4, 1), (1, 1), (4, 0)], []),
           ("GL_PACK_SKIP_ROWS", "I", 1, [(1, 2), (3, 2), (1, 3), (3, 3), (2, 0), (3, 0), (3, 1), (4, 4), (1, 4), (1, 5), (2, 1), (4, 3), (4, 5), (4, 2), (4, 1), (1, 1), (4, 0)], []),
           ("GL_PACK_SKIP_PIXELS", "I", 1, [(1, 2), (3, 2), (1, 3), (3, 3), (2, 0), (3, 0), (3, 1), (4, 4), (1, 4), (1, 5), (2, 1), (4, 3), (4, 5), (4, 2), (4, 1), (1, 1), (4, 0)], []),
           ("GL_PACK_ALIGNMENT", "I", 1, [(1, 2), (3, 2), (1, 3), (3, 3), (2, 0), (3, 0), (3, 1), (4, 4), (1, 4), (1, 5), (2, 1), (4, 3), (4, 5), (4, 2), (4, 1), (1, 1), (4, 0)], []),
           ("GL_MAP_COLOR", "B", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_MAP_STENCIL", "B", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_INDEX_SHIFT", "I", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_INDEX_OFFSET", "I", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_RED_SCALE", "F", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_RED_BIAS", "F", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_ZOOM_X", "F", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_ZOOM_Y", "F", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_GREEN_SCALE", "F", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_GREEN_BIAS", "F", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_BLUE_SCALE", "F", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_BLUE_BIAS", "F", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_ALPHA_BIAS", "F", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_DEPTH_SCALE", "F", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_DEPTH_BIAS", "F", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_MAX_EVAL_ORDER", "I", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_MAX_LIGHTS", "I", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_MAX_CLIP_DISTANCES", "I", 1, [(3, 2), (4, 4), (3, 3), (3, 0), (3, 1), (4, 5), (4, 3), (4, 2), (4, 1), (4, 0)], []),
           ("GL_MAX_TEXTURE_SIZE", "I", 1, [(1, 2), (3, 2), (1, 3), (3, 3), (2, 0), (3, 0), (3, 1), (4, 4), (1, 4), (1, 5), (2, 1), (4, 3), (4, 5), (4, 2), (4, 1), (1, 1), (4, 0)], []),
           ("GL_MAX_PIXEL_MAP_TABLE", "I", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_MAX_ATTRIB_STACK_DEPTH", "I", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_MAX_MODELVIEW_STACK_DEPTH", "I", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_MAX_NAME_STACK_DEPTH", "I", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_MAX_PROJECTION_STACK_DEPTH", "I", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_MAX_TEXTURE_STACK_DEPTH", "I", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_MAX_VIEWPORT_DIMS", "F", 2, [(1, 2), (3, 2), (1, 3), (3, 3), (2, 0), (3, 0), (3, 1), (4, 4), (1, 4), (1, 5), (2, 1), (4, 3), (4, 5), (4, 2), (4, 1), (1, 1), (4, 0)], []),
           ("GL_MAX_CLIENT_ATTRIB_STACK_DEPTH", "I", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_SUBPIXEL_BITS", "I", 1, [(1, 2), (3, 2), (1, 3), (3, 3), (2, 0), (3, 0), (3, 1), (4, 4), (1, 4), (1, 5), (2, 1), (4, 3), (4, 5), (4, 2), (4, 1), (1, 1), (4, 0)], []),
           ("GL_INDEX_BITS", "I", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_RED_BITS", "I", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_GREEN_BITS", "I", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_BLUE_BITS", "I", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_ALPHA_BITS", "I", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_DEPTH_BITS", "I", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_STENCIL_BITS", "I", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_ACCUM_RED_BITS", "I", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_ACCUM_GREEN_BITS", "I", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_ACCUM_BLUE_BITS", "I", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_ACCUM_ALPHA_BITS", "I", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_NAME_STACK_DEPTH", "I", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_AUTO_NORMAL", "B", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_MAP1_COLOR_4", "B", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_MAP1_INDEX", "B", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_MAP1_NORMAL", "B", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_MAP1_TEXTURE_COORD_1", "B", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_MAP1_TEXTURE_COORD_2", "B", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_MAP1_TEXTURE_COORD_3", "B", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_MAP1_TEXTURE_COORD_4", "B", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_MAP1_VERTEX_3", "B", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_MAP1_VERTEX_4", "B", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_MAP2_COLOR_4", "B", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_MAP2_INDEX", "B", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_MAP2_NORMAL", "B", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_MAP2_TEXTURE_COORD_1", "B", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_MAP2_TEXTURE_COORD_2", "B", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_MAP2_TEXTURE_COORD_3", "B", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_MAP2_TEXTURE_COORD_4", "B", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_MAP2_VERTEX_3", "B", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_MAP2_VERTEX_4", "B", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_MAP1_GRID_DOMAIN", "F", 2, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_MAP1_GRID_SEGMENTS", "I", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_MAP2_GRID_DOMAIN", "F", 4, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_MAP2_GRID_SEGMENTS", "I", 2, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_FEEDBACK_BUFFER_POINTER", "P", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_FEEDBACK_BUFFER_SIZE", "I", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_FEEDBACK_BUFFER_TYPE", "E", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_SELECTION_BUFFER_POINTER", "P", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_SELECTION_BUFFER_SIZE", "I", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_VENDOR", "S", 1, [(1, 2), (3, 2), (1, 3), (3, 3), (2, 0), (3, 0), (3, 1), (4, 4), (1, 4), (1, 5), (2, 1), (4, 3), (4, 5), (4, 2), (4, 1), (1, 1), (4, 0)], []),
           ("GL_RENDERER", "S", 1, [(1, 2), (3, 2), (1, 3), (3, 3), (2, 0), (3, 0), (3, 1), (4, 4), (1, 4), (1, 5), (2, 1), (4, 3), (4, 5), (4, 2), (4, 1), (1, 1), (4, 0)], []),
           ("GL_VERSION", "S", 1, [(1, 2), (3, 2), (1, 3), (3, 3), (2, 0), (3, 0), (3, 1), (4, 4), (1, 4), (1, 5), (2, 1), (4, 3), (4, 5), (4, 2), (4, 1), (1, 1), (4, 0)], []),
           ("GL_EXTENSIONS", "S", 1, [(1, 2), (3, 2), (1, 3), (3, 3), (2, 0), (3, 0), (3, 1), (4, 4), (1, 4), (1, 5), (2, 1), (4, 3), (4, 5), (4, 2), (4, 1), (1, 1), (4, 0)], []),
           ("GL_POLYGON_OFFSET_UNITS", "F", 1, [(1, 2), (3, 2), (1, 3), (3, 3), (2, 0), (3, 0), (3, 1), (4, 4), (1, 4), (1, 5), (2, 1), (4, 3), (4, 5), (4, 2), (4, 1), (1, 1), (4, 0)], []),
           ("GL_POLYGON_OFFSET_POINT", "B", 1, [(1, 2), (3, 2), (1, 3), (3, 3), (2, 0), (3, 0), (3, 1), (4, 4), (1, 4), (1, 5), (2, 1), (4, 3), (4, 5), (4, 2), (4, 1), (1, 1), (4, 0)], []),
           ("GL_POLYGON_OFFSET_LINE", "B", 1, [(1, 2), (3, 2), (1, 3), (3, 3), (2, 0), (3, 0), (3, 1), (4, 4), (1, 4), (1, 5), (2, 1), (4, 3), (4, 5), (4, 2), (4, 1), (1, 1), (4, 0)], []),
           ("GL_CLIP_DISTANCE0", "B", 1, [(3, 2), (4, 4), (3, 3), (3, 0), (3, 1), (4, 5), (4, 3), (4, 2), (4, 1), (4, 0)], []),
           ("GL_CLIP_DISTANCE1", "B", 1, [(3, 2), (4, 4), (3, 3), (3, 0), (3, 1), (4, 5), (4, 3), (4, 2), (4, 1), (4, 0)], []),
           ("GL_CLIP_DISTANCE2", "B", 1, [(3, 2), (4, 4), (3, 3), (3, 0), (3, 1), (4, 5), (4, 3), (4, 2), (4, 1), (4, 0)], []),
           ("GL_CLIP_DISTANCE3", "B", 1, [(3, 2), (4, 4), (3, 3), (3, 0), (3, 1), (4, 5), (4, 3), (4, 2), (4, 1), (4, 0)], []),
           ("GL_CLIP_DISTANCE4", "B", 1, [(3, 2), (4, 4), (3, 3), (3, 0), (3, 1), (4, 5), (4, 3), (4, 2), (4, 1), (4, 0)], []),
           ("GL_CLIP_DISTANCE5", "B", 1, [(3, 2), (4, 4), (3, 3), (3, 0), (3, 1), (4, 5), (4, 3), (4, 2), (4, 1), (4, 0)], []),
           ("GL_CLIP_DISTANCE6", "B", 1, [(3, 2), (4, 4), (3, 3), (3, 0), (3, 1), (4, 5), (4, 3), (4, 2), (4, 1), (4, 0)], []),
           ("GL_CLIP_DISTANCE7", "B", 1, [(3, 2), (4, 4), (3, 3), (3, 0), (3, 1), (4, 5), (4, 3), (4, 2), (4, 1), (4, 0)], []),
           ("GL_BLEND_COLOR", "F", 4, [], []),
           ("GL_BLEND_EQUATION", "E", 1, [], []),
           ("GL_PACK_CMYK_HINT_EXT", "I", 1, [], ['GL_EXT_color_buffer_float', 'GL_EXT_cmyka', 'GL_EXT_cmyka', 'GL_EXT_color_buffer_float', 'GL_EXT_cmyka', 'GL_EXT_color_buffer_float', 'GL_EXT_cmyka']),
           ("GL_UNPACK_CMYK_HINT_EXT", "I", 1, [], ['GL_EXT_color_buffer_float', 'GL_EXT_cmyka', 'GL_EXT_cmyka', 'GL_EXT_color_buffer_float', 'GL_EXT_cmyka', 'GL_EXT_color_buffer_float', 'GL_EXT_cmyka']),
           ("GL_CONVOLUTION_1D", "B", 1, [], ['GL_ARB_imaging', 'GL_ARB_imaging']),
           ("GL_CONVOLUTION_2D", "B", 1, [], ['GL_ARB_imaging', 'GL_ARB_imaging']),
           ("GL_SEPARABLE_2D", "B", 1, [], ['GL_ARB_imaging', 'GL_ARB_imaging']),
           ("GL_POST_CONVOLUTION_RED_SCALE", "F", 1, [], ['GL_ARB_imaging', 'GL_ARB_imaging']),
           ("GL_POST_CONVOLUTION_GREEN_SCALE", "F", 1, [], ['GL_ARB_imaging', 'GL_ARB_imaging']),
           ("GL_POST_CONVOLUTION_BLUE_SCALE", "F", 1, [], ['GL_ARB_imaging', 'GL_ARB_imaging']),
           ("GL_POST_CONVOLUTION_ALPHA_SCALE", "F", 1, [], ['GL_ARB_imaging', 'GL_ARB_imaging']),
           ("GL_POST_CONVOLUTION_RED_BIAS", "F", 1, [], ['GL_ARB_imaging', 'GL_ARB_imaging']),
           ("GL_POST_CONVOLUTION_GREEN_BIAS", "F", 1, [], ['GL_ARB_imaging', 'GL_ARB_imaging']),
           ("GL_POST_CONVOLUTION_BLUE_BIAS", "F", 1, [], ['GL_ARB_imaging', 'GL_ARB_imaging']),
           ("GL_POST_CONVOLUTION_ALPHA_BIAS", "F", 1, [], ['GL_ARB_imaging', 'GL_ARB_imaging']),
           ("GL_HISTOGRAM", "B", 1, [], ['GL_ARB_imaging', 'GL_ARB_imaging']),
           ("GL_MINMAX", "B", 1, [], ['GL_ARB_imaging', 'GL_ARB_imaging']),
           ("GL_POLYGON_OFFSET_FILL", "B", 1, [(1, 2), (3, 2), (1, 3), (3, 3), (2, 0), (3, 0), (3, 1), (4, 4), (1, 4), (1, 5), (2, 1), (4, 3), (4, 5), (4, 2), (4, 1), (1, 1), (4, 0)], []),
           ("GL_POLYGON_OFFSET_FACTOR", "F", 1, [(1, 2), (3, 2), (1, 3), (3, 3), (2, 0), (3, 0), (3, 1), (4, 4), (1, 4), (1, 5), (2, 1), (4, 3), (4, 5), (4, 2), (4, 1), (1, 1), (4, 0)], []),
           ("GL_POLYGON_OFFSET_BIAS_EXT", "F", 1, [], ['GL_EXT_polygon_offset', 'GL_EXT_polygon_offset']),
           ("GL_RESCALE_NORMAL", "B", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (2, 1), (1, 5), (1, 4)], []),
           ("GL_PACK_SKIP_IMAGES", "I", 1, [(1, 2), (3, 2), (1, 3), (3, 3), (2, 0), (3, 0), (3, 1), (4, 4), (2, 1), (1, 5), (1, 4), (4, 3), (4, 5), (4, 2), (4, 1), (4, 0)], []),
           ("GL_PACK_IMAGE_HEIGHT", "F", 1, [(1, 2), (3, 2), (1, 3), (3, 3), (2, 0), (3, 0), (3, 1), (4, 4), (2, 1), (1, 5), (1, 4), (4, 3), (4, 5), (4, 2), (4, 1), (4, 0)], []),
           ("GL_UNPACK_SKIP_IMAGES", "I", 1, [(1, 2), (3, 2), (1, 3), (3, 3), (2, 0), (3, 0), (3, 1), (4, 4), (2, 1), (1, 5), (1, 4), (4, 3), (4, 5), (4, 2), (4, 1), (4, 0)], []),
           ("GL_UNPACK_IMAGE_HEIGHT", "F", 1, [(1, 2), (3, 2), (1, 3), (3, 3), (2, 0), (3, 0), (3, 1), (4, 4), (2, 1), (1, 5), (1, 4), (4, 3), (4, 5), (4, 2), (4, 1), (4, 0)], []),
           ("GL_MAX_3D_TEXTURE_SIZE", "I", 1, [(1, 2), (3, 2), (1, 3), (3, 3), (2, 0), (3, 0), (3, 1), (4, 4), (2, 1), (1, 5), (1, 4), (4, 3), (4, 5), (4, 2), (4, 1), (4, 0)], []),
           ("GL_VERTEX_ARRAY", "B", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (4, 4), (1, 4), (1, 5), (2, 1), (4, 3), (4, 5), (1, 1)], []),
           ("GL_NORMAL_ARRAY", "B", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_COLOR_ARRAY", "B", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_INDEX_ARRAY", "B", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_TEXTURE_COORD_ARRAY", "B", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_EDGE_FLAG_ARRAY", "B", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_VERTEX_ARRAY_SIZE", "I", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_VERTEX_ARRAY_TYPE", "E", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_VERTEX_ARRAY_STRIDE", "I", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_VERTEX_ARRAY_COUNT_EXT", "I", 1, [], ['GL_EXT_vertex_array', 'GL_EXT_vertex_array']),
           ("GL_NORMAL_ARRAY_TYPE", "E", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_NORMAL_ARRAY_STRIDE", "I", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_NORMAL_ARRAY_COUNT_EXT", "I", 1, [], ['GL_EXT_vertex_array', 'GL_EXT_vertex_array']),
           ("GL_COLOR_ARRAY_SIZE", "I", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_COLOR_ARRAY_TYPE", "E", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_COLOR_ARRAY_STRIDE", "I", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_COLOR_ARRAY_COUNT_EXT", "I", 1, [], ['GL_EXT_vertex_array', 'GL_EXT_vertex_array']),
           ("GL_INDEX_ARRAY_TYPE", "E", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_INDEX_ARRAY_STRIDE", "I", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_INDEX_ARRAY_COUNT_EXT", "I", 1, [], ['GL_EXT_vertex_array', 'GL_EXT_vertex_array']),
           ("GL_TEXTURE_COORD_ARRAY_SIZE", "I", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_TEXTURE_COORD_ARRAY_TYPE", "E", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_TEXTURE_COORD_ARRAY_STRIDE", "I", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_TEXTURE_COORD_ARRAY_COUNT_EXT", "I", 1, [], ['GL_EXT_vertex_array', 'GL_EXT_vertex_array']),
           ("GL_EDGE_FLAG_ARRAY_STRIDE", "I", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_EDGE_FLAG_ARRAY_COUNT_EXT", "I", 1, [], ['GL_EXT_vertex_array', 'GL_EXT_vertex_array']),
           ("GL_VERTEX_ARRAY_POINTER", "P", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_NORMAL_ARRAY_POINTER", "P", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_COLOR_ARRAY_POINTER", "P", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_INDEX_ARRAY_POINTER", "P", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_TEXTURE_COORD_ARRAY_POINTER", "P", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_EDGE_FLAG_ARRAY_POINTER", "P", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (1, 4), (1, 5), (2, 1), (1, 1)], []),
           ("GL_INTERLACE_SGIX", "I", 1, [], ['GL_SGIX_interlace', 'GL_SGIX_interlace', 'GL_SGIX_interlace']),
           ("GL_DETAIL_TEXTURE_2D_BINDING_SGIS", "I", 1, [], ['GL_SGIS_detail_texture', 'GL_SGIS_detail_texture']),
           ("GL_MULTISAMPLE", "I", 1, [(3, 2), (1, 3), (3, 3), (2, 0), (3, 0), (3, 1), (4, 4), (2, 1), (1, 5), (1, 4), (4, 3), (4, 5), (4, 2), (4, 1), (4, 0)], []),
           ("GL_SAMPLE_ALPHA_TO_COVERAGE", "I", 1, [(3, 2), (1, 3), (3, 3), (2, 0), (3, 0), (3, 1), (4, 4), (2, 1), (1, 5), (1, 4), (4, 3), (4, 5), (4, 2), (4, 1), (4, 0)], []),
           ("GL_SAMPLE_ALPHA_TO_ONE", "I", 1, [(3, 2), (1, 3), (3, 3), (2, 0), (3, 0), (3, 1), (4, 4), (2, 1), (1, 5), (1, 4), (4, 3), (4, 5), (4, 2), (4, 1), (4, 0)], []),
           ("GL_SAMPLE_COVERAGE", "I", 1, [(3, 2), (1, 3), (3, 3), (2, 0), (3, 0), (3, 1), (4, 4), (2, 1), (1, 5), (1, 4), (4, 3), (4, 5), (4, 2), (4, 1), (4, 0)], []),
           ("GL_SAMPLE_BUFFERS", "I", 1, [(3, 2), (1, 3), (3, 3), (2, 0), (3, 0), (3, 1), (4, 4), (2, 1), (1, 5), (1, 4), (4, 3), (4, 5), (4, 2), (4, 1), (4, 0)], []),
           ("GL_SAMPLES", "I", 1, [(3, 2), (1, 3), (3, 3), (2, 0), (3, 0), (3, 1), (4, 4), (2, 1), (1, 5), (1, 4), (4, 3), (4, 5), (4, 2), (4, 1), (4, 0)], ['GL_ARB_internalformat_query2', 'GL_ARB_internalformat_query2']),
           ("GL_SAMPLE_COVERAGE_VALUE", "F", 1, [(3, 2), (1, 3), (3, 3), (2, 0), (3, 0), (3, 1), (4, 4), (2, 1), (1, 5), (1, 4), (4, 3), (4, 5), (4, 2), (4, 1), (4, 0)], []),
           ("GL_SAMPLE_COVERAGE_INVERT", "I", 1, [(3, 2), (1, 3), (3, 3), (2, 0), (3, 0), (3, 1), (4, 4), (2, 1), (1, 5), (1, 4), (4, 3), (4, 5), (4, 2), (4, 1), (4, 0)], []),
           ("GL_SAMPLE_PATTERN_EXT", "I", 1, [], ['GL_EXT_multisample']),
           ("GL_COLOR_MATRIX", "F", 16, [], ['GL_ARB_imaging']),
           ("GL_COLOR_MATRIX_STACK_DEPTH", "I", 1, [], ['GL_ARB_imaging']),
           ("GL_MAX_COLOR_MATRIX_STACK_DEPTH", "I", 1, [], ['GL_ARB_imaging']),
           ("GL_POST_COLOR_MATRIX_RED_SCALE", "F", 1, [], ['GL_ARB_imaging', 'GL_ARB_imaging']),
           ("GL_POST_COLOR_MATRIX_GREEN_SCALE", "F", 1, [], ['GL_ARB_imaging', 'GL_ARB_imaging']),
           ("GL_POST_COLOR_MATRIX_BLUE_SCALE", "F", 1, [], ['GL_ARB_imaging', 'GL_ARB_imaging']),
           ("GL_POST_COLOR_MATRIX_ALPHA_SCALE", "F", 1, [], ['GL_ARB_imaging', 'GL_ARB_imaging']),
           ("GL_POST_COLOR_MATRIX_RED_BIAS", "F", 1, [], ['GL_ARB_imaging', 'GL_ARB_imaging']),
           ("GL_POST_COLOR_MATRIX_GREEN_BIAS", "F", 1, [], ['GL_ARB_imaging', 'GL_ARB_imaging']),
           ("GL_POST_COLOR_MATRIX_BLUE_BIAS", "F", 1, [], ['GL_ARB_imaging', 'GL_ARB_imaging']),
           ("GL_POST_COLOR_MATRIX_ALPHA_BIAS", "F", 1, [], ['GL_ARB_imaging', 'GL_ARB_imaging']),
           ("GL_TEXTURE_COLOR_TABLE_SGI", "I", 1, [], ['GL_SGI_texture_color_table', 'GL_SGI_texture_color_table', 'GL_SGI_texture_color_table', 'GL_SGI_texture_color_table']),
           ("GL_BLEND_DST_RGB", "E", 1, [(3, 2), (4, 4), (3, 3), (2, 0), (3, 0), (3, 1), (2, 1), (1, 5), (1, 4), (4, 3), (4, 5), (4, 2), (4, 1), (4, 0)], ['GL_OES_draw_buffers_indexed', 'GL_EXT_draw_buffers_indexed', 'GL_OES_draw_buffers_indexed', 'GL_EXT_draw_buffers_indexed', 'GL_EXT_draw_buffers_indexed']),
           ("GL_BLEND_SRC_RGB", "E", 1, [(3, 2), (4, 4), (3, 3), (2, 0), (3, 0), (3, 1), (2, 1), (1, 5), (1, 4), (4, 3), (4, 5), (4, 2), (4, 1), (4, 0)], ['GL_OES_draw_buffers_indexed', 'GL_EXT_draw_buffers_indexed', 'GL_OES_draw_buffers_indexed', 'GL_EXT_draw_buffers_indexed', 'GL_EXT_draw_buffers_indexed']),
           ("GL_BLEND_DST_ALPHA", "E", 1, [(3, 2), (4, 4), (3, 3), (2, 0), (3, 0), (3, 1), (2, 1), (1, 5), (1, 4), (4, 3), (4, 5), (4, 2), (4, 1), (4, 0)], ['GL_OES_draw_buffers_indexed', 'GL_EXT_draw_buffers_indexed', 'GL_OES_draw_buffers_indexed', 'GL_EXT_draw_buffers_indexed', 'GL_EXT_draw_buffers_indexed']),
           ("GL_BLEND_SRC_ALPHA", "E", 1, [(3, 2), (4, 4), (3, 3), (2, 0), (3, 0), (3, 1), (2, 1), (1, 5), (1, 4), (4, 3), (4, 5), (4, 2), (4, 1), (4, 0)], ['GL_OES_draw_buffers_indexed', 'GL_EXT_draw_buffers_indexed', 'GL_OES_draw_buffers_indexed', 'GL_EXT_draw_buffers_indexed', 'GL_EXT_draw_buffers_indexed']),
           ("GL_COLOR_TABLE", "B", 1, [], ['GL_ARB_imaging', 'GL_ARB_imaging']),
           ("GL_POST_CONVOLUTION_COLOR_TABLE", "I", 1, [], ['GL_ARB_imaging', 'GL_ARB_imaging']),
           ("GL_POST_COLOR_MATRIX_COLOR_TABLE", "I", 1, [], ['GL_ARB_imaging', 'GL_ARB_imaging']),
           ("GL_MAX_ELEMENTS_VERTICES", "I", 1, [(1, 2), (3, 2), (1, 3), (3, 3), (2, 0), (3, 0), (3, 1), (4, 4), (2, 1), (1, 5), (1, 4), (4, 3), (4, 5), (4, 2), (4, 1), (4, 0)], []),
           ("GL_MAX_ELEMENTS_INDICES", "I", 1, [(1, 2), (3, 2), (1, 3), (3, 3), (2, 0), (3, 0), (3, 1), (4, 4), (2, 1), (1, 5), (1, 4), (4, 3), (4, 5), (4, 2), (4, 1), (4, 0)], []),
           ("GL_CLIP_VOLUME_CLIPPING_HINT_EXT", "E", 1, [], ['GL_EXT_clip_volume_hint', 'GL_EXT_clip_volume_hint']),
           ("GL_POINT_SIZE_MIN", "F", 1, [(2, 0), (3, 0), (3, 1), (2, 1), (1, 5), (1, 4)], []),
           ("GL_POINT_SIZE_MAX", "F", 1, [(2, 0), (3, 0), (3, 1), (2, 1), (1, 5), (1, 4)], []),
           ("GL_POINT_FADE_THRESHOLD_SIZE", "F", 1, [(3, 2), (4, 4), (3, 3), (2, 0), (3, 0), (3, 1), (2, 1), (1, 5), (1, 4), (4, 3), (4, 5), (4, 2), (4, 1), (4, 0)], []),
           ("GL_POINT_DISTANCE_ATTENUATION", "F", 3, [(2, 0), (3, 0), (3, 1), (2, 1), (1, 5), (1, 4)], []),
           ("GL_FOG_FUNC_POINTS_SGIS", "I", 1, [], ['GL_SGIS_fog_function', 'GL_SGIS_fog_function']),
           ("GL_MAX_FOG_FUNC_POINTS_SGIS", "I", 1, [], ['GL_SGIS_fog_function', 'GL_SGIS_fog_function']),
           ("GL_PACK_SKIP_VOLUMES_SGIS", "I", 1, [], ['GL_SGIS_texture4D', 'GL_SGIS_texture4D', 'GL_SGIS_texture4D']),
           ("GL_PACK_IMAGE_DEPTH_SGIS", "I", 1, [], ['GL_SGIS_texture4D', 'GL_SGIS_texture4D', 'GL_SGIS_texture4D']),
           ("GL_UNPACK_SKIP_VOLUMES_SGIS", "I", 1, [], ['GL_SGIS_texture4D', 'GL_SGIS_texture4D', 'GL_SGIS_texture4D']),
           ("GL_UNPACK_IMAGE_DEPTH_SGIS", "I", 1, [], ['GL_SGIS_texture4D', 'GL_SGIS_texture4D', 'GL_SGIS_texture4D']),
           ("GL_TEXTURE_4D_SGIS", "I", 1, [], ['GL_SGIS_texture4D', 'GL_SGIS_texture4D', 'GL_SGIS_texture4D', 'GL_SGIS_texture4D']),
           ("GL_MAX_4D_TEXTURE_SIZE_SGIS", "I", 1, [], ['GL_SGIS_texture4D', 'GL_SGIS_texture4D']),
           ("GL_PIXEL_TEX_GEN_SGIX", "I", 1, [], ['GL_SGIX_pixel_texture', 'GL_SGIX_pixel_texture', 'GL_SGIX_pixel_texture']),
           ("GL_PIXEL_TILE_BEST_ALIGNMENT_SGIX", "I", 1, [], ['GL_SGIX_pixel_tiles', 'GL_SGIX_pixel_tiles']),
           ("GL_PIXEL_TILE_CACHE_INCREMENT_SGIX", "I", 1, [], ['GL_SGIX_pixel_tiles', 'GL_SGIX_pixel_tiles']),
           ("GL_PIXEL_TILE_WIDTH_SGIX", "I", 1, [], ['GL_SGIX_pixel_tiles', 'GL_SGIX_pixel_tiles', 'GL_SGIX_pixel_tiles']),
           ("GL_PIXEL_TILE_HEIGHT_SGIX", "I", 1, [], ['GL_SGIX_pixel_tiles', 'GL_SGIX_pixel_tiles', 'GL_SGIX_pixel_tiles']),
           ("GL_PIXEL_TILE_GRID_WIDTH_SGIX", "I", 1, [], ['GL_SGIX_pixel_tiles', 'GL_SGIX_pixel_tiles', 'GL_SGIX_pixel_tiles']),
           ("GL_PIXEL_TILE_GRID_HEIGHT_SGIX", "I", 1, [], ['GL_SGIX_pixel_tiles', 'GL_SGIX_pixel_tiles', 'GL_SGIX_pixel_tiles']),
           ("GL_PIXEL_TILE_GRID_DEPTH_SGIX", "I", 1, [], ['GL_SGIX_pixel_tiles', 'GL_SGIX_pixel_tiles', 'GL_SGIX_pixel_tiles']),
           ("GL_PIXEL_TILE_CACHE_SIZE_SGIX", "I", 1, [], ['GL_SGIX_pixel_tiles', 'GL_SGIX_pixel_tiles', 'GL_SGIX_pixel_tiles']),
           ("GL_SPRITE_SGIX", "I", 1, [], ['GL_SGIX_sprite', 'GL_SGIX_sprite', 'GL_SGIX_sprite']),
           ("GL_SPRITE_MODE_SGIX", "I", 1, [], ['GL_SGIX_sprite', 'GL_SGIX_sprite']),
           ("GL_SPRITE_AXIS_SGIX", "F", 3, [], ['GL_SGIX_sprite', 'GL_SGIX_sprite']),
           ("GL_SPRITE_TRANSLATION_SGIX", "F", 3, [], ['GL_SGIX_sprite', 'GL_SGIX_sprite']),
           ("GL_TEXTURE_4D_BINDING_SGIS", "I", 1, [], ['GL_SGIS_texture4D', 'GL_SGIS_texture4D']),
           ("GL_MAX_CLIPMAP_DEPTH_SGIX", "I", 1, [], ['GL_SGIX_clipmap', 'GL_SGIX_clipmap']),
           ("GL_MAX_CLIPMAP_VIRTUAL_DEPTH_SGIX", "I", 1, [], ['GL_SGIX_clipmap', 'GL_SGIX_clipmap']),
           ("GL_POST_TEXTURE_FILTER_BIAS_RANGE_SGIX", "F", 2, [], ['GL_SGIX_texture_scale_bias', 'GL_SGIX_texture_scale_bias']),
           ("GL_POST_TEXTURE_FILTER_SCALE_RANGE_SGIX", "F", 2, [], ['GL_SGIX_texture_scale_bias', 'GL_SGIX_texture_scale_bias']),
           ("GL_REFERENCE_PLANE_SGIX", "I", 1, [], ['GL_SGIX_reference_plane', 'GL_SGIX_reference_plane', 'GL_SGIX_reference_plane']),
           ("GL_REFERENCE_PLANE_EQUATION_SGIX", "F", 4, [], ['GL_SGIX_reference_plane', 'GL_SGIX_reference_plane']),
           ("GL_IR_INSTRUMENT1_SGIX", "I", 1, [], ['GL_SGIX_ir_instrument1', 'GL_SGIX_ir_instrument1', 'GL_SGIX_ir_instrument1']),
           ("GL_INSTRUMENT_MEASUREMENTS_SGIX", "I", 1, [], ['GL_SGIX_instruments', 'GL_SGIX_instruments']),
           ("GL_CALLIGRAPHIC_FRAGMENT_SGIX", "I", 1, [], ['GL_SGIX_calligraphic_fragment', 'GL_SGIX_calligraphic_fragment', 'GL_SGIX_calligraphic_fragment']),
           ("GL_FRAMEZOOM_SGIX", "I", 1, [], ['GL_SGIX_framezoom', 'GL_SGIX_framezoom', 'GL_SGIX_framezoom']),
           ("GL_FRAMEZOOM_FACTOR_SGIX", "I", 1, [], ['GL_SGIX_framezoom', 'GL_SGIX_framezoom']),
           ("GL_MAX_FRAMEZOOM_FACTOR_SGIX", "I", 1, [], ['GL_SGIX_framezoom', 'GL_SGIX_framezoom']),
           ("GL_GENERATE_MIPMAP_HINT", "E", 1, [(2, 0), (3, 0), (3, 1), (2, 1), (1, 5), (1, 4)], []),
           ("GL_DEFORMATIONS_MASK_SGIX", "I", 1, [], ['GL_SGIX_polynomial_ffd', 'GL_SGIX_polynomial_ffd']),
           ("GL_FOG_OFFSET_SGIX", "I", 1, [], ['GL_SGIX_fog_offset', 'GL_SGIX_fog_offset', 'GL_SGIX_fog_offset']),
           ("GL_FOG_OFFSET_VALUE_SGIX", "F", 4, [], ['GL_SGIX_fog_offset', 'GL_SGIX_fog_offset', 'GL_SGIX_fog_offset']),
           ("GL_ARRAY_ELEMENT_LOCK_FIRST_EXT", "I", 1, [], ['GL_EXT_compiled_vertex_array']),
           ("GL_ARRAY_ELEMENT_LOCK_COUNT_EXT", "I", 1, [], ['GL_EXT_compiled_vertex_array']),
           ("GL_CULL_VERTEX_EXT", "B", 1, [], ['GL_EXT_cull_vertex']),
           ("GL_CULL_VERTEX_EYE_POSITION_EXT", "F", 4, [], ['GL_EXT_cull_vertex']),
           ("GL_CULL_VERTEX_OBJECT_POSITION_EXT", "F", 4, [], ['GL_EXT_cull_vertex']),
           ("GL_TEXTURE_COLOR_WRITEMASK_SGIS", "B", 4, [], ['GL_SGIS_texture_color_mask']),
           ("GL_LIGHT_MODEL_COLOR_CONTROL", "E", 1, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (2, 1), (1, 5), (1, 4)], []),
           ("GL_MAJOR_VERSION", "I", 1, [(3, 2), (4, 4), (3, 3), (3, 0), (3, 1), (4, 5), (4, 3), (4, 2), (4, 1), (4, 0)], []),
           ("GL_MINOR_VERSION", "I", 1, [(3, 2), (4, 4), (3, 3), (3, 0), (3, 1), (4, 5), (4, 3), (4, 2), (4, 1), (4, 0)], []),
           ("GL_NUM_EXTENSIONS", "I", 1, [(3, 2), (4, 4), (3, 3), (3, 0), (3, 1), (4, 5), (4, 3), (4, 2), (4, 1), (4, 0)], []),
           ("GL_CONTEXT_FLAGS", "I", 1, [(3, 2), (4, 4), (3, 3), (3, 0), (3, 1), (4, 5), (4, 3), (4, 2), (4, 1), (4, 0)], []),
           ("GL_PRIMITIVE_RESTART_FOR_PATCHES_SUPPORTED", "B", 1, [(4, 5), (4, 4)], ['GL_EXT_tessellation_shader', 'GL_EXT_tessellation_shader']),
           ("GL_DEBUG_OUTPUT_SYNCHRONOUS", "B", 1, [(4, 5), (4, 4), (4, 3)], []),
           ("GL_DEBUG_NEXT_LOGGED_MESSAGE_LENGTH", "I", 1, [(4, 5), (4, 4), (4, 3)], []),
           ("GL_RESET_NOTIFICATION_STRATEGY_ARB", "E", 1, [], []),
           ("GL_ACTIVE_PROGRAM", "I", 1, [(4, 5), (4, 2), (4, 4), (4, 1), (4, 3)], ['GL_ARB_shader_atomic_counter_ops', 'GL_ARB_separate_shader_objects', 'GL_ARB_shader_atomic_counter_ops', 'GL_ARB_separate_shader_objects', 'GL_ARB_separate_shader_objects']),
           ("GL_PROGRAM_PIPELINE_BINDING", "I", 1, [(4, 5), (4, 2), (4, 4), (4, 1), (4, 3)], ['GL_ARB_shader_atomic_counter_ops', 'GL_ARB_separate_shader_objects', 'GL_ARB_shader_atomic_counter_ops', 'GL_ARB_separate_shader_objects', 'GL_ARB_separate_shader_objects']),
           ("GL_MAX_VIEWPORTS", "I", 1, [(4, 5), (4, 2), (4, 4), (4, 1), (4, 3)], ['GL_ARB_viewport_array', 'GL_ARB_viewport_array']),
           ("GL_VIEWPORT_SUBPIXEL_BITS", "I", 1, [(4, 5), (4, 2), (4, 4), (4, 1), (4, 3)], ['GL_ARB_viewport_array', 'GL_ARB_viewport_array']),
           ("GL_VIEWPORT_BOUNDS_RANGE", "F", 2, [(4, 5), (4, 2), (4, 4), (4, 1), (4, 3)], ['GL_ARB_viewport_array', 'GL_ARB_viewport_array']),
           ("GL_LAYER_PROVOKING_VERTEX", "E", 1, [(4, 5), (4, 2), (4, 4), (4, 1), (4, 3)], ['GL_ARB_viewport_array', 'GL_ARB_viewport_array']),
           ("GL_VIEWPORT_INDEX_PROVOKING_VERTEX", "E", 1, [(4, 5), (4, 2), (4, 4), (4, 1), (4, 3)], ['GL_ARB_viewport_array', 'GL_ARB_viewport_array']),
           ("GL_MAX_COMPUTE_SHARED_MEMORY_SIZE", "I", 1, [(4, 5), (4, 4), (4, 3)], ['GL_ARB_compute_shader', 'GL_ARB_compute_shader']),
           ("GL_MAX_COMPUTE_UNIFORM_COMPONENTS", "I", 1, [(4, 5), (4, 4), (4, 3)], ['GL_ARB_compute_shader', 'GL_ARB_compute_shader']),
           ("GL_MAX_COMPUTE_ATOMIC_COUNTER_BUFFERS", "I", 1, [(4, 5), (4, 4), (4, 3)], ['GL_ARB_compute_shader', 'GL_ARB_compute_shader']),
           ("GL_MAX_COMPUTE_ATOMIC_COUNTERS", "I", 1, [(4, 5), (4, 4), (4, 3)], ['GL_ARB_compute_shader', 'GL_ARB_compute_shader']),
           ("GL_MAX_COMBINED_COMPUTE_UNIFORM_COMPONENTS", "I", 1, [(4, 5), (4, 4), (4, 3)], ['GL_ARB_compute_shader', 'GL_ARB_compute_shader']),
           ("GL_MAX_DEBUG_GROUP_STACK_DEPTH", "I", 1, [(4, 5), (4, 4), (4, 3)], []),
           ("GL_DEBUG_GROUP_STACK_DEPTH", "I", 1, [(4, 5), (4, 4), (4, 3)], []),
           ("GL_MAX_UNIFORM_LOCATIONS", "I", 1, [(4, 5), (4, 4), (4, 3)], ['GL_ARB_explicit_uniform_location', 'GL_ARB_fragment_layer_viewport', 'GL_ARB_fragment_coord_conventions', 'GL_ARB_explicit_uniform_location', 'GL_ARB_fragment_layer_viewport', 'GL_ARB_fragment_coord_conventions', 'GL_ARB_fragment_layer_viewport', 'GL_ARB_fragment_coord_conventions', 'GL_ARB_fragment_coord_conventions']),
           ("GL_MAX_VERTEX_ATTRIB_RELATIVE_OFFSET", "I", 1, [(4, 5), (4, 4), (4, 3)], ['GL_ARB_vertex_attrib_binding', 'GL_ARB_vertex_attrib_binding']),
           ("GL_MAX_VERTEX_ATTRIB_BINDINGS", "I", 1, [(4, 5), (4, 4), (4, 3)], ['GL_ARB_vertex_attrib_binding', 'GL_ARB_vertex_attrib_binding']),
           ("GL_MAX_LABEL_LENGTH", "I", 1, [(4, 5), (4, 4), (4, 3)], []),
           ("GL_MAX_CULL_DISTANCES", "I", 1, [(4, 5)], ['GL_ARB_cull_distance', 'GL_ARB_cull_distance']),
           ("GL_MAX_COMBINED_CLIP_AND_CULL_DISTANCES", "I", 1, [(4, 5)], ['GL_ARB_cull_distance', 'GL_ARB_cull_distance']),
           ("GL_CONTEXT_RELEASE_BEHAVIOR", "E", 1, [(4, 5)], []),
           ("GL_CONTEXT_RELEASE_BEHAVIOR_FLUSH", "E", 1, [(4, 5)], []),
           ("GL_CONVOLUTION_HINT_SGIX", "I", 1, [], ['GL_SGIX_depth_pass_instrument', 'GL_SGIX_convolution_accuracy', 'GL_SGIX_convolution_accuracy', 'GL_SGIX_depth_pass_instrument', 'GL_SGIX_convolution_accuracy', 'GL_SGIX_depth_pass_instrument', 'GL_SGIX_convolution_accuracy']),
           ("GL_PIXEL_TEX_GEN_MODE_SGIX", "I", 1, [], ['GL_SGIX_pixel_texture', 'GL_SGIX_pixel_texture']),
           ("GL_TEXTURE_APPLICATION_MODE_EXT", "I", 1, [], ['GL_EXT_light_texture']),
           ("GL_TEXTURE_LIGHT_EXT", "I", 1, [], ['GL_EXT_light_texture']),
           ("GL_TEXTURE_MATERIAL_FACE_EXT", "I", 1, [], ['GL_EXT_light_texture']),
           ("GL_TEXTURE_MATERIAL_PARAMETER_EXT", "I", 1, [], ['GL_EXT_light_texture']),
           ("GL_PIXEL_TEXTURE_SGIS", "I", 1, [], ['GL_SGIS_pixel_texture', 'GL_SGIS_pixel_texture', 'GL_SGIS_pixel_texture']),
           ("GL_PIXEL_FRAGMENT_RGB_SOURCE_SGIS", "I", 1, [], ['GL_SGIS_pixel_texture', 'GL_SGIS_pixel_texture']),
           ("GL_PIXEL_FRAGMENT_ALPHA_SOURCE_SGIS", "I", 1, [], ['GL_SGIS_pixel_texture', 'GL_SGIS_pixel_texture']),
           ("GL_PIXEL_GROUP_COLOR_SGIS", "I", 1, [], ['GL_SGIS_pixel_texture']),
           ("GL_FRAGMENT_LIGHTING_SGIX", "I", 1, [], ['GL_SGIX_fragment_lighting', 'GL_SGIX_fragment_lighting', 'GL_SGIX_fragment_lighting']),
           ("GL_FRAGMENT_COLOR_MATERIAL_SGIX", "I", 1, [], ['GL_SGIX_fragment_lighting', 'GL_SGIX_fragment_lighting', 'GL_SGIX_fragment_lighting']),
           ("GL_FRAGMENT_COLOR_MATERIAL_FACE_SGIX", "I", 1, [], ['GL_SGIX_fragment_lighting', 'GL_SGIX_fragment_lighting']),
           ("GL_FRAGMENT_COLOR_MATERIAL_PARAMETER_SGIX", "I", 1, [], ['GL_SGIX_fragment_lighting', 'GL_SGIX_fragment_lighting']),
           ("GL_MAX_FRAGMENT_LIGHTS_SGIX", "I", 1, [], ['GL_SGIX_fragment_lighting', 'GL_SGIX_fragment_lighting']),
           ("GL_MAX_ACTIVE_LIGHTS_SGIX", "I", 1, [], ['GL_SGIX_fragment_lighting', 'GL_SGIX_fragment_lighting']),
           ("GL_CURRENT_RASTER_NORMAL_SGIX", "I", 1, [], ['GL_SGIX_fragment_lighting']),
           ("GL_LIGHT_ENV_MODE_SGIX", "I", 1, [], ['GL_SGIX_fragment_lighting', 'GL_SGIX_fragment_lighting', 'GL_SGIX_fragment_lighting']),
           ("GL_FRAGMENT_LIGHT_MODEL_LOCAL_VIEWER_SGIX", "I", 1, [], ['GL_SGIX_fragment_lighting', 'GL_SGIX_fragment_lighting', 'GL_SGIX_fragment_lighting']),
           ("GL_FRAGMENT_LIGHT_MODEL_TWO_SIDE_SGIX", "I", 1, [], ['GL_SGIX_fragment_lighting', 'GL_SGIX_fragment_lighting', 'GL_SGIX_fragment_lighting']),
           ("GL_FRAGMENT_LIGHT_MODEL_AMBIENT_SGIX", "F", 4, [], ['GL_SGIX_fragment_lighting', 'GL_SGIX_fragment_lighting', 'GL_SGIX_fragment_lighting']),
           ("GL_FRAGMENT_LIGHT_MODEL_NORMAL_INTERPOLATION_SGIX", "I", 1, [], ['GL_SGIX_fragment_lighting', 'GL_SGIX_fragment_lighting', 'GL_SGIX_fragment_lighting']),
           ("GL_FRAGMENT_LIGHT0_SGIX", "I", 1, [], ['GL_SGIX_fragment_lighting', 'GL_SGIX_fragment_lighting', 'GL_SGIX_fragment_lighting', 'GL_SGIX_fragment_lighting']),
           ("GL_TANGENT_ARRAY_TYPE_EXT", "E", 1, [], ['GL_EXT_coordinate_frame']),
           ("GL_BINORMAL_ARRAY_TYPE_EXT", "E", 1, [], ['GL_EXT_coordinate_frame']),
           ("GL_TANGENT_ARRAY_POINTER_EXT", "P", 1, [], ['GL_EXT_coordinate_frame']),
           ("GL_BINORMAL_ARRAY_POINTER_EXT", "P", 1, [], ['GL_EXT_coordinate_frame']),
           ("GL_FOG_COORD_SRC", "E", 1, [(3, 0), (1, 5), (3, 1), (2, 1), (2, 0)], []),
           ("GL_CURRENT_FOG_COORD", "F", 1, [(3, 0), (1, 5), (3, 1), (2, 1), (2, 0)], []),
           ("GL_FOG_COORD_ARRAY_TYPE", "E", 1, [(3, 0), (1, 5), (3, 1), (2, 1), (2, 0)], []),
           ("GL_FOG_COORD_ARRAY_STRIDE", "I", 1, [(3, 0), (1, 5), (3, 1), (2, 1), (2, 0)], []),
           ("GL_FOG_COORD_ARRAY", "B", 1, [(3, 0), (1, 5), (3, 1), (2, 1), (2, 0)], []),
           ("GL_COLOR_SUM", "B", 1, [(2, 0), (3, 0), (3, 1), (2, 1), (1, 5), (1, 4)], []),
           ("GL_CURRENT_SECONDARY_COLOR", "F", 4, [(2, 0), (3, 0), (3, 1), (2, 1), (1, 5), (1, 4)], []),
           ("GL_SECONDARY_COLOR_ARRAY_SIZE", "I", 1, [(2, 0), (3, 0), (3, 1), (2, 1), (1, 5), (1, 4)], []),
           ("GL_SECONDARY_COLOR_ARRAY_TYPE", "E", 1, [(2, 0), (3, 0), (3, 1), (2, 1), (1, 5), (1, 4)], []),
           ("GL_SECONDARY_COLOR_ARRAY_STRIDE", "I", 1, [(2, 0), (3, 0), (3, 1), (2, 1), (1, 5), (1, 4)], []),
           ("GL_SECONDARY_COLOR_ARRAY", "B", 1, [(2, 0), (3, 0), (3, 1), (2, 1), (1, 5), (1, 4)], []),
           ("GL_ALIASED_POINT_SIZE_RANGE", "F", 2, [(1, 2), (1, 3), (2, 0), (3, 0), (3, 1), (2, 1), (1, 5), (1, 4)], []),
           ("GL_ALIASED_LINE_WIDTH_RANGE", "F", 2, [(1, 2), (3, 2), (1, 3), (3, 3), (2, 0), (3, 0), (3, 1), (4, 4), (2, 1), (1, 5), (1, 4), (4, 3), (4, 5), (4, 2), (4, 1), (4, 0)], []),
           ("GL_ACTIVE_TEXTURE", "E", 1, [(3, 2), (1, 3), (3, 3), (2, 0), (3, 0), (3, 1), (4, 4), (2, 1), (1, 5), (1, 4), (4, 3), (4, 5), (4, 2), (4, 1), (4, 0)], []),
           ("GL_CLIENT_ACTIVE_TEXTURE", "E", 1, [(1, 3), (2, 0), (3, 0), (3, 1), (2, 1), (1, 5), (1, 4)], []),
           ("GL_MAX_TEXTURE_UNITS", "I", 1, [(1, 3), (2, 0), (3, 0), (3, 1), (2, 1), (1, 5), (1, 4)], []),
           ("GL_TRANSPOSE_MODELVIEW_MATRIX", "F", 16, [(1, 3), (2, 0), (3, 0), (3, 1), (2, 1), (1, 5), (1, 4)], []),
           ("GL_TRANSPOSE_PROJECTION_MATRIX", "F", 16, [(1, 3), (2, 0), (3, 0), (3, 1), (2, 1), (1, 5), (1, 4)], []),
           ("GL_TRANSPOSE_TEXTURE_MATRIX", "F", 16, [(1, 3), (2, 0), (3, 0), (3, 1), (2, 1), (1, 5), (1, 4)], []),
           ("GL_TRANSPOSE_COLOR_MATRIX", "F", 16, [(1, 3), (2, 0), (3, 0), (3, 1), (2, 1), (1, 5), (1, 4)], []),
           ("GL_MAX_RENDERBUFFER_SIZE", "I", 1, [(3, 2), (4, 4), (3, 3), (3, 0), (3, 1), (4, 5), (4, 3), (4, 2), (4, 1), (4, 0)], []),
           ("GL_TEXTURE_COMPRESSION_HINT", "E", 1, [(3, 2), (1, 3), (3, 3), (2, 0), (3, 0), (3, 1), (4, 4), (2, 1), (1, 5), (1, 4), (4, 3), (4, 5), (4, 2), (4, 1), (4, 0)], []),
           ("GL_MAX_RECTANGLE_TEXTURE_SIZE", "I", 1, [(3, 2), (4, 4), (3, 3), (4, 5), (3, 1), (4, 3), (4, 2), (4, 1), (4, 0)], []),
           ("GL_MAX_TEXTURE_LOD_BIAS", "F", 1, [(3, 2), (4, 4), (3, 3), (2, 0), (3, 0), (3, 1), (2, 1), (1, 5), (1, 4), (4, 3), (4, 5), (4, 2), (4, 1), (4, 0)], []),
           ("GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT", "F", 1, [], ['GL_EXT_texture_filter_anisotropic']),
           ("GL_MAX_SHININESS_NV", "F", 1, [], ['GL_NV_light_max_exponent']),
           ("GL_MAX_SPOT_EXPONENT_NV", "F", 1, [], ['GL_NV_light_max_exponent']),
           ("GL_VERTEX_WEIGHT_ARRAY_TYPE_EXT", "E", 1, [], ['GL_EXT_vertex_weighting']),
           ("GL_MAX_CUBE_MAP_TEXTURE_SIZE", "I", 1, [(3, 2), (1, 3), (3, 3), (2, 0), (3, 0), (3, 1), (4, 4), (2, 1), (1, 5), (1, 4), (4, 3), (4, 5), (4, 2), (4, 1), (4, 0)], []),
           ("GL_CONSTANT_COLOR0_NV", "F", 4, [], ['GL_NV_register_combiners']),
           ("GL_CONSTANT_COLOR1_NV", "F", 4, [], ['GL_NV_register_combiners']),
           ("GL_MULTISAMPLE_FILTER_HINT_NV", "E", 1, [], ['GL_NV_multisample_filter_hint', 'GL_NV_multisample_filter_hint']),
           ("GL_MAX_GENERAL_COMBINERS_NV", "I", 1, [], ['GL_NV_register_combiners']),
           ("GL_NUM_GENERAL_COMBINERS_NV", "I", 1, [], ['GL_NV_register_combiners']),
           ("GL_COLOR_SUM_CLAMP_NV", "B", 1, [], ['GL_NV_register_combiners']),
           ("GL_PRIMITIVE_RESTART_NV", "B", 1, [], ['GL_NV_primitive_restart']),
           ("GL_PRIMITIVE_RESTART_INDEX_NV", "I", 1, [], ['GL_NV_primitive_restart']),
           ("GL_FOG_DISTANCE_MODE_NV", "E", 1, [], ['GL_NV_fog_distance']),
           ("GL_VERTEX_ARRAY_BINDING", "I", 1, [(3, 2), (4, 4), (3, 3), (3, 0), (3, 1), (4, 5), (4, 3), (4, 2), (4, 1), (4, 0)], ['GL_ARB_vertex_array_object', 'GL_ARB_vertex_array_object']),
           ("GL_REPLACEMENT_CODE_ARRAY_TYPE_SUN", "E", 1, [], ['GL_SUN_triangle_list']),
           ("GL_MAX_PROGRAM_MATRIX_STACK_DEPTH_ARB", "I", 1, [], ['GL_ARB_vertex_program']),
           ("GL_MAX_PROGRAM_MATRICES_ARB", "I", 1, [], ['GL_ARB_vertex_program']),
           ("GL_CURRENT_MATRIX_STACK_DEPTH_ARB", "I", 1, [], ['GL_ARB_vertex_program']),
           ("GL_CURRENT_MATRIX_ARB", "F", 16, [], ['GL_ARB_vertex_program']),
           ("GL_PROGRAM_POINT_SIZE", "B", 1, [(3, 2), (3, 3), (4, 5), (4, 4), (4, 3), (4, 2), (4, 1), (4, 0)], []),
           ("GL_VERTEX_PROGRAM_TWO_SIDE", "B", 1, [(3, 0), (2, 0), (3, 1), (2, 1)], []),
           ("GL_VERTEX_PROGRAM_BINDING_NV", "I", 1, [], ['GL_NV_vertex_program1_1', 'GL_NV_vertex_program', 'GL_NV_vertex_program2', 'GL_NV_vertex_program', 'GL_NV_vertex_program2', 'GL_NV_vertex_program2']),
           ("GL_PROGRAM_ERROR_POSITION_ARB", "I", 1, [], ['GL_ARB_vertex_program']),
           ("GL_DEPTH_CLAMP", "B", 1, [(3, 2), (3, 3), (4, 5), (4, 4), (4, 3), (4, 2), (4, 1), (4, 0)], ['GL_ARB_depth_clamp', 'GL_ARB_depth_clamp']),
           ("GL_NUM_COMPRESSED_TEXTURE_FORMATS", "I", 1, [(3, 2), (1, 3), (3, 3), (2, 0), (3, 0), (3, 1), (4, 4), (2, 1), (1, 5), (1, 4), (4, 3), (4, 5), (4, 2), (4, 1), (4, 0)], []),
           ("GL_COMPRESSED_TEXTURE_FORMATS", "E", 1, [(3, 2), (1, 3), (3, 3), (2, 0), (3, 0), (3, 1), (4, 4), (2, 1), (1, 5), (1, 4), (4, 3), (4, 5), (4, 2), (4, 1), (4, 0)], []),
           ("GL_MAX_VERTEX_UNITS_ARB", "I", 1, [], ['GL_ARB_vertex_blend']),
           ("GL_ACTIVE_VERTEX_UNITS_ARB", "I", 1, [], ['GL_ARB_vertex_blend']),
           ("GL_WEIGHT_SUM_UNITY_ARB", "B", 1, [], ['GL_ARB_vertex_blend']),
           ("GL_VERTEX_BLEND_ARB", "B", 1, [], ['GL_ARB_vertex_blend']),
           ("GL_CURRENT_WEIGHT_ARB", "F", 1, [], ['GL_ARB_vertex_blend']),
           ("GL_WEIGHT_ARRAY_TYPE_ARB", "E", 1, [], ['GL_ARB_vertex_blend']),
           ("GL_WEIGHT_ARRAY_STRIDE_ARB", "I", 1, [], ['GL_ARB_vertex_blend']),
           ("GL_WEIGHT_ARRAY_SIZE_ARB", "I", 1, [], ['GL_ARB_vertex_blend']),
           ("GL_WEIGHT_ARRAY_POINTER_ARB", "P", 1, [], ['GL_ARB_vertex_blend']),
           ("GL_WEIGHT_ARRAY_ARB", "B", 1, [], ['GL_ARB_vertex_blend']),
           ("GL_PACK_INVERT_MESA", "B", 1, [], ['GL_MESA_pack_invert']),
           ("GL_ELEMENT_ARRAY_TYPE_ATI", "E", 1, [], ['GL_ATI_element_array']),
           ("GL_VERTEX_SHADER_BINDING_EXT", "I", 1, [], ['GL_EXT_vertex_shader']),
           ("GL_VARIANT_ARRAY_TYPE_EXT", "E", 1, [], ['GL_EXT_vertex_shader']),
           ("GL_VBO_FREE_MEMORY_ATI", "I", 1, [], ['GL_ATI_meminfo']),
           ("GL_TEXTURE_FREE_MEMORY_ATI", "I", 1, [], ['GL_ATI_meminfo']),
           ("GL_RENDERBUFFER_FREE_MEMORY_ATI", "I", 1, [], ['GL_ATI_meminfo']),
           ("GL_NUM_PROGRAM_BINARY_FORMATS", "I", 1, [(4, 5), (4, 2), (4, 4), (4, 1), (4, 3)], ['GL_ARB_get_program_binary', 'GL_ARB_get_program_binary']),
           ("GL_PROGRAM_BINARY_FORMATS", "E", 1, [(4, 5), (4, 2), (4, 4), (4, 1), (4, 3)], ['GL_ARB_get_program_binary', 'GL_ARB_get_program_binary']),
           ("GL_STENCIL_BACK_FUNC", "E", 1, [(3, 2), (4, 4), (3, 3), (3, 0), (3, 1), (2, 1), (2, 0), (4, 3), (4, 5), (4, 2), (4, 1), (4, 0)], []),
           ("GL_STENCIL_BACK_FAIL", "E", 1, [(3, 2), (4, 4), (3, 3), (3, 0), (3, 1), (2, 1), (2, 0), (4, 3), (4, 5), (4, 2), (4, 1), (4, 0)], []),
           ("GL_STENCIL_BACK_PASS_DEPTH_FAIL", "E", 1, [(3, 2), (4, 4), (3, 3), (3, 0), (3, 1), (2, 1), (2, 0), (4, 3), (4, 5), (4, 2), (4, 1), (4, 0)], []),
           ("GL_STENCIL_BACK_PASS_DEPTH_PASS", "E", 1, [(3, 2), (4, 4), (3, 3), (3, 0), (3, 1), (2, 1), (2, 0), (4, 3), (4, 5), (4, 2), (4, 1), (4, 0)], []),
           ("GL_RGBA_FLOAT_MODE_ARB", "B", 1, [], ['GL_ARB_color_buffer_float']),
           ("GL_MAX_DRAW_BUFFERS", "I", 1, [(3, 2), (4, 4), (3, 3), (3, 0), (3, 1), (2, 1), (2, 0), (4, 3), (4, 5), (4, 2), (4, 1), (4, 0)], []),
           ("GL_DRAW_BUFFER0", "E", 1, [(3, 2), (4, 4), (3, 3), (3, 0), (3, 1), (2, 1), (2, 0), (4, 3), (4, 5), (4, 2), (4, 1), (4, 0)], []),
           ("GL_DRAW_BUFFER1", "E", 1, [(3, 2), (4, 4), (3, 3), (3, 0), (3, 1), (2, 1), (2, 0), (4, 3), (4, 5), (4, 2), (4, 1), (4, 0)], []),
           ("GL_DRAW_BUFFER2", "E", 1, [(3, 2), (4, 4), (3, 3), (3, 0), (3, 1), (2, 1), (2, 0), (4, 3), (4, 5), (4, 2), (4, 1), (4, 0)], []),
           ("GL_DRAW_BUFFER3", "E", 1, [(3, 2), (4, 4), (3, 3), (3, 0), (3, 1), (2, 1), (2, 0), (4, 3), (4, 5), (4, 2), (4, 1), (4, 0)], []),
           ("GL_DRAW_BUFFER4", "E", 1, [(3, 2), (4, 4), (3, 3), (3, 0), (3, 1), (2, 1), (2, 0), (4, 3), (4, 5), (4, 2), (4, 1), (4, 0)], []),
           ("GL_DRAW_BUFFER5", "E", 1, [(3, 2), (4, 4), (3, 3), (3, 0), (3, 1), (2, 1), (2, 0), (4, 3), (4, 5), (4, 2), (4, 1), (4, 0)], []),
           ("GL_DRAW_BUFFER6", "E", 1, [(3, 2), (4, 4), (3, 3), (3, 0), (3, 1), (2, 1), (2, 0), (4, 3), (4, 5), (4, 2), (4, 1), (4, 0)], []),
           ("GL_DRAW_BUFFER7", "E", 1, [(3, 2), (4, 4), (3, 3), (3, 0), (3, 1), (2, 1), (2, 0), (4, 3), (4, 5), (4, 2), (4, 1), (4, 0)], []),
           ("GL_BLEND_EQUATION_ALPHA", "E", 1, [(3, 2), (4, 4), (3, 3), (3, 0), (3, 1), (2, 1), (2, 0), (4, 3), (4, 5), (4, 2), (4, 1), (4, 0)], ['GL_OES_draw_buffers_indexed', 'GL_EXT_draw_buffers_indexed', 'GL_OES_draw_buffers_indexed', 'GL_EXT_draw_buffers_indexed', 'GL_EXT_draw_buffers_indexed']),
           ("GL_MATRIX_PALETTE_ARB", "B", 1, [], ['GL_ARB_matrix_palette']),
           ("GL_MAX_MATRIX_PALETTE_STACK_DEPTH_ARB", "I", 1, [], ['GL_ARB_matrix_palette']),
           ("GL_MAX_PALETTE_MATRICES_ARB", "I", 1, [], ['GL_ARB_matrix_palette']),
           ("GL_CURRENT_PALETTE_MATRIX_ARB", "I", 1, [], ['GL_ARB_matrix_palette']),
           ("GL_MATRIX_INDEX_ARRAY_ARB", "B", 1, [], ['GL_ARB_matrix_palette']),
           ("GL_CURRENT_MATRIX_INDEX_ARB", "I", 1, [], ['GL_ARB_matrix_palette']),
           ("GL_MATRIX_INDEX_ARRAY_SIZE_ARB", "I", 1, [], ['GL_ARB_matrix_palette']),
           ("GL_MATRIX_INDEX_ARRAY_TYPE_ARB", "E", 1, [], ['GL_ARB_matrix_palette']),
           ("GL_MATRIX_INDEX_ARRAY_STRIDE_ARB", "I", 1, [], ['GL_ARB_matrix_palette']),
           ("GL_MATRIX_INDEX_ARRAY_POINTER_ARB", "P", 1, [], ['GL_ARB_matrix_palette']),
           ("GL_POINT_SPRITE", "B", 1, [(3, 0), (2, 0), (3, 1), (2, 1)], []),
           ("GL_POINT_SPRITE_R_MODE_NV", "E", 1, [], ['GL_NV_point_sprite']),
           ("GL_MAX_FRAGMENT_PROGRAM_LOCAL_PARAMETERS_NV", "I", 1, [], []),
           ("GL_MAX_VERTEX_ATTRIBS", "I", 1, [(3, 2), (4, 4), (3, 3), (3, 0), (3, 1), (2, 1), (2, 0), (4, 3), (4, 5), (4, 2), (4, 1), (4, 0)], []),
           ("GL_MAX_TESS_CONTROL_INPUT_COMPONENTS", "I", 1, [(4, 5), (4, 4), (4, 3), (4, 2), (4, 1), (4, 0)], ['GL_ARB_tessellation_shader', 'GL_ARB_tessellation_shader']),
           ("GL_MAX_TESS_EVALUATION_INPUT_COMPONENTS", "I", 1, [(4, 5), (4, 4), (4, 3), (4, 2), (4, 1), (4, 0)], ['GL_ARB_tessellation_shader', 'GL_ARB_tessellation_shader']),
           ("GL_MAX_TEXTURE_COORDS", "I", 1, [(3, 0), (2, 0), (3, 1), (2, 1)], []),
           ("GL_MAX_TEXTURE_IMAGE_UNITS", "I", 1, [(3, 2), (4, 4), (3, 3), (3, 0), (3, 1), (2, 1), (2, 0), (4, 3), (4, 5), (4, 2), (4, 1), (4, 0)], []),
           ("GL_FRAGMENT_PROGRAM_BINDING_NV", "I", 1, [], []),
           ("GL_PROGRAM_ERROR_STRING_ARB", "S", 1, [], ['GL_ARB_vertex_program']),
           ("GL_DEPTH_BOUNDS_TEST_EXT", "B", 1, [], ['GL_EXT_depth_bounds_test']),
           ("GL_DEPTH_BOUNDS_EXT", "F", 2, [], ['GL_EXT_depth_bounds_test']),
           ("GL_ARRAY_BUFFER_BINDING", "I", 1, [(3, 2), (4, 4), (3, 3), (2, 0), (3, 0), (3, 1), (2, 1), (1, 5), (4, 3), (4, 5), (4, 2), (4, 1), (4, 0)], []),
           ("GL_ELEMENT_ARRAY_BUFFER_BINDING", "I", 1, [(3, 2), (4, 4), (3, 3), (2, 0), (3, 0), (3, 1), (2, 1), (1, 5), (4, 3), (4, 5), (4, 2), (4, 1), (4, 0)], []),
           ("GL_VERTEX_ARRAY_BUFFER_BINDING", "I", 1, [(3, 0), (1, 5), (3, 1), (2, 1), (2, 0)], []),
           ("GL_NORMAL_ARRAY_BUFFER_BINDING", "I", 1, [(3, 0), (1, 5), (3, 1), (2, 1), (2, 0)], []),
           ("GL_COLOR_ARRAY_BUFFER_BINDING", "I", 1, [(3, 0), (1, 5), (3, 1), (2, 1), (2, 0)], []),
           ("GL_INDEX_ARRAY_BUFFER_BINDING", "I", 1, [(3, 0), (1, 5), (3, 1), (2, 1), (2, 0)], []),
           ("GL_TEXTURE_COORD_ARRAY_BUFFER_BINDING", "I", 1, [(3, 0), (1, 5), (3, 1), (2, 1), (2, 0)], []),
           ("GL_EDGE_FLAG_ARRAY_BUFFER_BINDING", "I", 1, [(3, 0), (1, 5), (3, 1), (2, 1), (2, 0)], []),
           ("GL_SECONDARY_COLOR_ARRAY_BUFFER_BINDING", "I", 1, [(3, 0), (1, 5), (3, 1), (2, 1), (2, 0)], []),
           ("GL_FOG_COORD_ARRAY_BUFFER_BINDING", "I", 1, [(3, 0), (1, 5), (3, 1), (2, 1), (2, 0)], []),
           ("GL_WEIGHT_ARRAY_BUFFER_BINDING", "I", 1, [(3, 0), (1, 5), (3, 1), (2, 1), (2, 0)], []),
           ("GL_TRANSPOSE_CURRENT_MATRIX_ARB", "F", 16, [], ['GL_ARB_vertex_program']),
           ("GL_PIXEL_PACK_BUFFER_BINDING", "I", 1, [(3, 2), (4, 4), (3, 3), (3, 0), (3, 1), (2, 1), (4, 5), (4, 3), (4, 2), (4, 1), (4, 0)], []),
           ("GL_PIXEL_UNPACK_BUFFER_BINDING", "I", 1, [(3, 2), (4, 4), (3, 3), (3, 0), (3, 1), (2, 1), (4, 5), (4, 3), (4, 2), (4, 1), (4, 0)], []),
           ("GL_MAX_DUAL_SOURCE_DRAW_BUFFERS", "I", 1, [(3, 3), (4, 5), (4, 4), (4, 3), (4, 2), (4, 1), (4, 0)], ['GL_ARB_blend_func_extended', 'GL_ARB_blend_func_extended']),
           ("GL_MAX_ARRAY_TEXTURE_LAYERS", "I", 1, [(3, 2), (4, 4), (3, 3), (3, 0), (3, 1), (4, 5), (4, 3), (4, 2), (4, 1), (4, 0)], []),
           ("GL_MIN_PROGRAM_TEXEL_OFFSET", "F", 1, [(3, 2), (4, 4), (3, 3), (3, 0), (3, 1), (4, 5), (4, 3), (4, 2), (4, 1), (4, 0)], []),
           ("GL_MAX_PROGRAM_TEXEL_OFFSET", "F", 1, [(3, 2), (4, 4), (3, 3), (3, 0), (3, 1), (4, 5), (4, 3), (4, 2), (4, 1), (4, 0)], []),
           ("GL_STENCIL_TEST_TWO_SIDE_EXT", "B", 1, [], ['GL_EXT_stencil_two_side']),
           ("GL_ACTIVE_STENCIL_FACE_EXT", "E", 1, [], ['GL_EXT_stencil_two_side']),
           ("GL_CLAMP_VERTEX_COLOR", "E", 1, [(3, 0), (3, 1)], []),
           ("GL_CLAMP_FRAGMENT_COLOR", "E", 1, [(3, 0), (3, 1)], []),
           ("GL_CLAMP_READ_COLOR", "E", 1, [(3, 2), (4, 4), (3, 3), (3, 0), (3, 1), (4, 5), (4, 3), (4, 2), (4, 1), (4, 0)], []),
           ("GL_ELEMENT_ARRAY_TYPE_APPLE", "E", 1, [], ['GL_APPLE_element_array']),
           ("GL_MAX_VERTEX_UNIFORM_BLOCKS", "I", 1, [(3, 2), (4, 4), (3, 3), (4, 5), (3, 1), (4, 3), (4, 2), (4, 1), (4, 0)], ['GL_ARB_uniform_buffer_object', 'GL_ARB_uniform_buffer_object']),
           ("GL_MAX_GEOMETRY_UNIFORM_BLOCKS", "I", 1, [(3, 2), (4, 4), (3, 3), (4, 5), (3, 1), (4, 3), (4, 2), (4, 1), (4, 0)], ['GL_ARB_uniform_buffer_object', 'GL_ARB_uniform_buffer_object']),
           ("GL_MAX_FRAGMENT_UNIFORM_BLOCKS", "I", 1, [(3, 2), (4, 4), (3, 3), (4, 5), (3, 1), (4, 3), (4, 2), (4, 1), (4, 0)], ['GL_ARB_uniform_buffer_object', 'GL_ARB_uniform_buffer_object']),
           ("GL_MAX_COMBINED_UNIFORM_BLOCKS", "I", 1, [(3, 2), (4, 4), (3, 3), (4, 5), (3, 1), (4, 3), (4, 2), (4, 1), (4, 0)], ['GL_ARB_uniform_buffer_object', 'GL_ARB_uniform_buffer_object']),
           ("GL_MAX_UNIFORM_BUFFER_BINDINGS", "I", 1, [(3, 2), (4, 4), (3, 3), (4, 5), (3, 1), (4, 3), (4, 2), (4, 1), (4, 0)], ['GL_ARB_uniform_buffer_object', 'GL_ARB_uniform_buffer_object']),
           ("GL_MAX_UNIFORM_BLOCK_SIZE", "I", 1, [(3, 2), (4, 4), (3, 3), (4, 5), (3, 1), (4, 3), (4, 2), (4, 1), (4, 0)], ['GL_ARB_uniform_buffer_object', 'GL_ARB_uniform_buffer_object']),
           ("GL_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS", "I", 1, [(3, 2), (4, 4), (3, 3), (4, 5), (3, 1), (4, 3), (4, 2), (4, 1), (4, 0)], ['GL_ARB_uniform_buffer_object', 'GL_ARB_uniform_buffer_object']),
           ("GL_MAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS", "I", 1, [(3, 2), (4, 4), (3, 3), (4, 5), (3, 1), (4, 3), (4, 2), (4, 1), (4, 0)], ['GL_ARB_uniform_buffer_object', 'GL_ARB_uniform_buffer_object']),
           ("GL_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS", "I", 1, [(3, 2), (4, 4), (3, 3), (4, 5), (3, 1), (4, 3), (4, 2), (4, 1), (4, 0)], ['GL_ARB_uniform_buffer_object', 'GL_ARB_uniform_buffer_object']),
           ("GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT", "I", 1, [(3, 2), (4, 4), (3, 3), (4, 5), (3, 1), (4, 3), (4, 2), (4, 1), (4, 0)], ['GL_ARB_uniform_buffer_object', 'GL_ARB_uniform_buffer_object']),
           ("GL_MAX_FRAGMENT_UNIFORM_COMPONENTS", "I", 1, [(3, 2), (4, 4), (3, 3), (3, 0), (3, 1), (2, 1), (2, 0), (4, 3), (4, 5), (4, 2), (4, 1), (4, 0)], []),
           ("GL_MAX_VERTEX_UNIFORM_COMPONENTS", "I", 1, [(3, 2), (4, 4), (3, 3), (3, 0), (3, 1), (2, 1), (2, 0), (4, 3), (4, 5), (4, 2), (4, 1), (4, 0)], []),
           ("GL_MAX_VARYING_COMPONENTS", "I", 1, [(3, 2), (4, 4), (3, 3), (3, 0), (3, 1), (4, 5), (4, 3), (4, 2), (4, 1), (4, 0)], ['GL_ARB_geometry_shader4', 'GL_ARB_geometry_shader4']),
           ("GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS", "I", 1, [(3, 2), (4, 4), (3, 3), (3, 0), (3, 1), (2, 1), (2, 0), (4, 3), (4, 5), (4, 2), (4, 1), (4, 0)], []),
           ("GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS", "I", 1, [(3, 2), (4, 4), (3, 3), (3, 0), (3, 1), (2, 1), (2, 0), (4, 3), (4, 5), (4, 2), (4, 1), (4, 0)], []),
           ("GL_SHADING_LANGUAGE_VERSION", "S", 1, [(3, 2), (4, 4), (3, 3), (3, 0), (3, 1), (2, 1), (2, 0), (4, 3), (4, 5), (4, 2), (4, 1), (4, 0)], []),
           ("GL_CURRENT_PROGRAM", "I", 1, [(3, 2), (4, 4), (3, 3), (3, 0), (3, 1), (2, 1), (2, 0), (4, 3), (4, 5), (4, 2), (4, 1), (4, 0)], []),
           ("GL_IMPLEMENTATION_COLOR_READ_TYPE", "E", 1, [(4, 5), (4, 2), (4, 4), (4, 1), (4, 3)], ['GL_ARB_ES2_compatibility', 'GL_ARB_ES2_compatibility']),
           ("GL_IMPLEMENTATION_COLOR_READ_FORMAT", "E", 1, [(4, 5), (4, 2), (4, 4), (4, 1), (4, 3)], ['GL_ARB_ES2_compatibility', 'GL_ARB_ES2_compatibility']),
           ("GL_MATRIX_INDEX_ARRAY_BUFFER_BINDING_OES", "I", 1, [], ['GL_OES_matrix_palette']),
           ("GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS", "I", 1, [(3, 2), (3, 3), (4, 5), (4, 4), (4, 3), (4, 2), (4, 1), (4, 0)], []),
           ("GL_MAX_TEXTURE_BUFFER_SIZE", "I", 1, [(3, 2), (4, 4), (3, 3), (4, 5), (3, 1), (4, 3), (4, 2), (4, 1), (4, 0)], []),
           ("GL_SAMPLE_SHADING", "B", 1, [(4, 5), (4, 4), (4, 3), (4, 2), (4, 1), (4, 0)], []),
           ("GL_MIN_SAMPLE_SHADING_VALUE", "F", 1, [(4, 5), (4, 4), (4, 3), (4, 2), (4, 1), (4, 0)], []),
           ("GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_COMPONENTS", "I", 1, [(3, 2), (4, 4), (3, 3), (3, 0), (3, 1), (4, 5), (4, 3), (4, 2), (4, 1), (4, 0)], []),
           ("GL_RASTERIZER_DISCARD", "B", 1, [(3, 2), (4, 4), (3, 3), (3, 0), (3, 1), (4, 5), (4, 3), (4, 2), (4, 1), (4, 0)], []),
           ("GL_MAX_TRANSFORM_FEEDBACK_INTERLEAVED_COMPONENTS", "I", 1, [(3, 2), (4, 4), (3, 3), (3, 0), (3, 1), (4, 5), (4, 3), (4, 2), (4, 1), (4, 0)], []),
           ("GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS", "I", 1, [(3, 2), (4, 4), (3, 3), (3, 0), (3, 1), (4, 5), (4, 3), (4, 2), (4, 1), (4, 0)], []),
           ("GL_POINT_SPRITE_COORD_ORIGIN", "E", 1, [(3, 2), (4, 4), (3, 3), (3, 0), (3, 1), (2, 1), (2, 0), (4, 3), (4, 5), (4, 2), (4, 1), (4, 0)], []),
           ("GL_DRAW_FRAMEBUFFER_BINDING", "I", 1, [(3, 2), (4, 4), (3, 3), (3, 0), (3, 1), (4, 5), (4, 3), (4, 2), (4, 1), (4, 0)], []),
           ("GL_RENDERBUFFER_BINDING", "I", 1, [(3, 2), (4, 4), (3, 3), (3, 0), (3, 1), (4, 5), (4, 3), (4, 2), (4, 1), (4, 0)], []),
           ("GL_READ_FRAMEBUFFER_BINDING", "I", 1, [(3, 2), (4, 4), (3, 3), (3, 0), (3, 1), (4, 5), (4, 3), (4, 2), (4, 1), (4, 0)], []),
           ("GL_MAX_COLOR_ATTACHMENTS", "I", 1, [(3, 2), (4, 4), (3, 3), (3, 0), (3, 1), (4, 5), (4, 3), (4, 2), (4, 1), (4, 0)], []),
           ("GL_MAX_SAMPLES", "I", 1, [(3, 2), (4, 4), (3, 3), (3, 0), (3, 1), (4, 5), (4, 3), (4, 2), (4, 1), (4, 0)], []),
           ("GL_PRIMITIVE_RESTART_FIXED_INDEX", "B", 1, [(4, 5), (4, 4), (4, 3)], ['GL_ARB_arrays_of_arrays', 'GL_ARB_ES3_compatibility', 'GL_ARB_arrays_of_arrays', 'GL_ARB_ES3_compatibility', 'GL_ARB_ES3_compatibility']),
           ("GL_MAX_ELEMENT_INDEX", "I", 1, [(4, 5), (4, 4), (4, 3)], ['GL_ARB_arrays_of_arrays', 'GL_ARB_ES3_compatibility', 'GL_ARB_arrays_of_arrays', 'GL_ARB_ES3_compatibility', 'GL_ARB_ES3_compatibility']),
           ("GL_RGBA_INTEGER_MODE_EXT", "B", 1, [], ['GL_EXT_texture_integer']),
           ("GL_FRAMEBUFFER_SRGB", "B", 1, [(3, 2), (4, 4), (3, 3), (3, 0), (3, 1), (4, 5), (4, 3), (4, 2), (4, 1), (4, 0)], ['GL_ARB_framebuffer_sRGB', 'GL_ARB_framebuffer_sRGB']),
           ("GL_FRAMEBUFFER_SRGB_CAPABLE_EXT", "B", 1, [], ['GL_EXT_geometry_point_size', 'GL_EXT_framebuffer_sRGB', 'GL_EXT_framebuffer_sRGB']),
           ("GL_MAX_GEOMETRY_VARYING_COMPONENTS_ARB", "I", 1, [], ['GL_ARB_geometry_shader4']),
           ("GL_MAX_VERTEX_VARYING_COMPONENTS_ARB", "I", 1, [], ['GL_ARB_geometry_shader4']),
           ("GL_MAX_GEOMETRY_UNIFORM_COMPONENTS", "I", 1, [(3, 2), (3, 3), (4, 5), (4, 4), (4, 3), (4, 2), (4, 1), (4, 0)], []),
           ("GL_MAX_GEOMETRY_OUTPUT_VERTICES", "I", 1, [(3, 2), (3, 3), (4, 5), (4, 4), (4, 3), (4, 2), (4, 1), (4, 0)], []),
           ("GL_MAX_GEOMETRY_TOTAL_OUTPUT_COMPONENTS", "I", 1, [(3, 2), (3, 3), (4, 5), (4, 4), (4, 3), (4, 2), (4, 1), (4, 0)], []),
           ("GL_MAX_VERTEX_BINDABLE_UNIFORMS_EXT", "I", 1, [], ['GL_EXT_bindable_uniform']),
           ("GL_MAX_FRAGMENT_BINDABLE_UNIFORMS_EXT", "I", 1, [], ['GL_EXT_bindable_uniform']),
           ("GL_MAX_GEOMETRY_BINDABLE_UNIFORMS_EXT", "I", 1, [], ['GL_EXT_bindable_uniform']),
           ("GL_MAX_SUBROUTINES", "I", 1, [(4, 5), (4, 4), (4, 3), (4, 2), (4, 1), (4, 0)], ['GL_ARB_shader_texture_image_samples', 'GL_ARB_shader_texture_lod', 'GL_ARB_shader_viewport_layer_array', 'GL_ARB_shader_subroutine', 'GL_ARB_shader_texture_image_samples', 'GL_ARB_shader_texture_lod', 'GL_ARB_shader_viewport_layer_array', 'GL_ARB_shader_subroutine', 'GL_ARB_shader_texture_lod', 'GL_ARB_shader_viewport_layer_array', 'GL_ARB_shader_subroutine', 'GL_ARB_shader_viewport_layer_array', 'GL_ARB_shader_subroutine', 'GL_ARB_shader_subroutine']),
           ("GL_MAX_SUBROUTINE_UNIFORM_LOCATIONS", "I", 1, [(4, 5), (4, 4), (4, 3), (4, 2), (4, 1), (4, 0)], ['GL_ARB_shader_texture_image_samples', 'GL_ARB_shader_texture_lod', 'GL_ARB_shader_viewport_layer_array', 'GL_ARB_shader_subroutine', 'GL_ARB_shader_texture_image_samples', 'GL_ARB_shader_texture_lod', 'GL_ARB_shader_viewport_layer_array', 'GL_ARB_shader_subroutine', 'GL_ARB_shader_texture_lod', 'GL_ARB_shader_viewport_layer_array', 'GL_ARB_shader_subroutine', 'GL_ARB_shader_viewport_layer_array', 'GL_ARB_shader_subroutine', 'GL_ARB_shader_subroutine']),
           ("GL_MAX_BINDABLE_UNIFORM_SIZE_EXT", "I", 1, [], ['GL_EXT_bindable_uniform']),
           ("GL_UNIFORM_BUFFER_BINDING_EXT", "I", 1, [], ['GL_EXT_bindable_uniform']),
           ("GL_NUM_SHADER_BINARY_FORMATS", "I", 1, [(4, 5), (4, 2), (4, 4), (4, 1), (4, 3)], ['GL_ARB_ES2_compatibility', 'GL_ARB_ES2_compatibility']),
           ("GL_SHADER_COMPILER", "B", 1, [(4, 5), (4, 2), (4, 4), (4, 1), (4, 3)], ['GL_ARB_ES2_compatibility', 'GL_ARB_ES2_compatibility']),
           ("GL_MAX_VERTEX_UNIFORM_VECTORS", "I", 1, [(4, 5), (4, 2), (4, 4), (4, 1), (4, 3)], ['GL_ARB_ES2_compatibility', 'GL_ARB_ES2_compatibility']),
           ("GL_MAX_VARYING_VECTORS", "I", 1, [(4, 5), (4, 2), (4, 4), (4, 1), (4, 3)], ['GL_ARB_ES2_compatibility', 'GL_ARB_ES2_compatibility']),
           ("GL_MAX_FRAGMENT_UNIFORM_VECTORS", "I", 1, [(4, 5), (4, 2), (4, 4), (4, 1), (4, 3)], ['GL_ARB_ES2_compatibility', 'GL_ARB_ES2_compatibility']),
           ("GL_MAX_MULTISAMPLE_COVERAGE_MODES_NV", "I", 1, [], ['GL_NV_framebuffer_multisample_coverage', 'GL_NV_generate_mipmap_sRGB', 'GL_NV_generate_mipmap_sRGB']),
           ("GL_MAX_COMBINED_TESS_CONTROL_UNIFORM_COMPONENTS", "I", 1, [(4, 5), (4, 4), (4, 3), (4, 2), (4, 1), (4, 0)], ['GL_ARB_tessellation_shader', 'GL_ARB_tessellation_shader']),
           ("GL_MAX_COMBINED_TESS_EVALUATION_UNIFORM_COMPONENTS", "I", 1, [(4, 5), (4, 4), (4, 3), (4, 2), (4, 1), (4, 0)], ['GL_ARB_tessellation_shader', 'GL_ARB_tessellation_shader']),
           ("GL_TRANSFORM_FEEDBACK_BUFFER_PAUSED", "B", 1, [(4, 5), (4, 4), (4, 3), (4, 2), (4, 1), (4, 0)], ['GL_ARB_transform_feedback2', 'GL_ARB_transform_feedback2']),
           ("GL_TRANSFORM_FEEDBACK_BUFFER_ACTIVE", "B", 1, [(4, 5), (4, 4), (4, 3), (4, 2), (4, 1), (4, 0)], ['GL_ARB_transform_feedback2', 'GL_ARB_transform_feedback2']),
           ("GL_TRANSFORM_FEEDBACK_BINDING", "I", 1, [(4, 5), (4, 4), (4, 3), (4, 2), (4, 1), (4, 0)], ['GL_ARB_transform_feedback2', 'GL_ARB_transform_feedback2']),
           ("GL_QUADS_FOLLOW_PROVOKING_VERTEX_CONVENTION", "B", 1, [(3, 2), (3, 3), (4, 5), (4, 4), (4, 3), (4, 2), (4, 1), (4, 0)], ['GL_ARB_provoking_vertex', 'GL_ARB_provoking_vertex']),
           ("GL_PROVOKING_VERTEX", "E", 1, [(3, 2), (3, 3), (4, 5), (4, 4), (4, 3), (4, 2), (4, 1), (4, 0)], ['GL_ARB_provoking_vertex', 'GL_ARB_viewport_array', 'GL_ARB_provoking_vertex', 'GL_ARB_viewport_array', 'GL_ARB_viewport_array']),
           ("GL_SAMPLE_MASK", "B", 1, [(3, 2), (3, 3), (4, 5), (4, 4), (4, 3), (4, 2), (4, 1), (4, 0)], ['GL_ARB_texture_query_levels', 'GL_ARB_texture_multisample', 'GL_ARB_texture_non_power_of_two', 'GL_ARB_texture_query_lod', 'GL_ARB_texture_query_levels', 'GL_ARB_texture_multisample', 'GL_ARB_texture_non_power_of_two', 'GL_ARB_texture_query_lod', 'GL_ARB_texture_multisample', 'GL_ARB_texture_non_power_of_two', 'GL_ARB_texture_query_lod', 'GL_ARB_texture_non_power_of_two', 'GL_ARB_texture_query_lod', 'GL_ARB_texture_query_lod']),
           ("GL_SAMPLE_MASK_VALUE", "I", 1, [(3, 2), (3, 3), (4, 5), (4, 4), (4, 3), (4, 2), (4, 1), (4, 0)], ['GL_ARB_texture_query_levels', 'GL_ARB_texture_multisample', 'GL_ARB_texture_non_power_of_two', 'GL_ARB_texture_query_lod', 'GL_ARB_texture_query_levels', 'GL_ARB_texture_multisample', 'GL_ARB_texture_non_power_of_two', 'GL_ARB_texture_query_lod', 'GL_ARB_texture_multisample', 'GL_ARB_texture_non_power_of_two', 'GL_ARB_texture_query_lod', 'GL_ARB_texture_non_power_of_two', 'GL_ARB_texture_query_lod', 'GL_ARB_texture_query_lod']),
           ("GL_TEXTURE_RENDERBUFFER_DATA_STORE_BINDING_NV", "I", 1, [], ['GL_NV_explicit_multisample']),
           ("GL_MAX_SAMPLE_MASK_WORDS", "I", 1, [(3, 2), (3, 3), (4, 5), (4, 4), (4, 3), (4, 2), (4, 1), (4, 0)], ['GL_ARB_texture_query_levels', 'GL_ARB_texture_multisample', 'GL_ARB_texture_non_power_of_two', 'GL_ARB_texture_query_lod', 'GL_ARB_texture_query_levels', 'GL_ARB_texture_multisample', 'GL_ARB_texture_non_power_of_two', 'GL_ARB_texture_query_lod', 'GL_ARB_texture_multisample', 'GL_ARB_texture_non_power_of_two', 'GL_ARB_texture_query_lod', 'GL_ARB_texture_non_power_of_two', 'GL_ARB_texture_query_lod', 'GL_ARB_texture_query_lod']),
           ("GL_MAX_GEOMETRY_SHADER_INVOCATIONS", "I", 1, [(4, 5), (4, 4), (4, 3), (4, 2), (4, 1), (4, 0)], ['GL_ARB_gpu_shader5', 'GL_ARB_gpu_shader5']),
           ("GL_MIN_FRAGMENT_INTERPOLATION_OFFSET", "F", 1, [(4, 5), (4, 4), (4, 3), (4, 2), (4, 1), (4, 0)], ['GL_ARB_gpu_shader5', 'GL_ARB_gpu_shader5']),
           ("GL_MAX_FRAGMENT_INTERPOLATION_OFFSET", "F", 1, [(4, 5), (4, 4), (4, 3), (4, 2), (4, 1), (4, 0)], ['GL_ARB_gpu_shader5', 'GL_ARB_gpu_shader5']),
           ("GL_FRAGMENT_INTERPOLATION_OFFSET_BITS", "I", 1, [(4, 5), (4, 4), (4, 3), (4, 2), (4, 1), (4, 0)], ['GL_ARB_gpu_shader5', 'GL_ARB_gpu_shader5']),
           ("GL_MIN_PROGRAM_TEXTURE_GATHER_OFFSET", "I", 1, [(4, 5), (4, 4), (4, 3), (4, 2), (4, 1), (4, 0)], []),
           ("GL_MAX_PROGRAM_TEXTURE_GATHER_OFFSET", "I", 1, [(4, 5), (4, 4), (4, 3), (4, 2), (4, 1), (4, 0)], []),
           ("GL_MAX_TRANSFORM_FEEDBACK_BUFFERS", "I", 1, [(4, 5), (4, 4), (4, 3), (4, 2), (4, 1), (4, 0)], ['GL_ARB_transform_feedback3', 'GL_ARB_transform_feedback3']),
           ("GL_MAX_VERTEX_STREAMS", "I", 1, [(4, 5), (4, 4), (4, 3), (4, 2), (4, 1), (4, 0)], ['GL_ARB_transform_feedback3', 'GL_ARB_gpu_shader5', 'GL_ARB_transform_feedback3', 'GL_ARB_gpu_shader5', 'GL_ARB_gpu_shader5']),
           ("GL_PATCH_VERTICES", "I", 1, [(4, 5), (4, 4), (4, 3), (4, 2), (4, 1), (4, 0)], ['GL_ARB_tessellation_shader', 'GL_ARB_tessellation_shader']),
           ("GL_PATCH_DEFAULT_INNER_LEVEL", "F", 2, [(4, 5), (4, 4), (4, 3), (4, 2), (4, 1), (4, 0)], ['GL_ARB_tessellation_shader', 'GL_ARB_tessellation_shader']),
           ("GL_PATCH_DEFAULT_OUTER_LEVEL", "F", 4, [(4, 5), (4, 4), (4, 3), (4, 2), (4, 1), (4, 0)], ['GL_ARB_tessellation_shader', 'GL_ARB_tessellation_shader']),
           ("GL_MAX_PATCH_VERTICES", "I", 1, [(4, 5), (4, 4), (4, 3), (4, 2), (4, 1), (4, 0)], ['GL_ARB_tessellation_shader', 'GL_ARB_tessellation_shader']),
           ("GL_MAX_TESS_GEN_LEVEL", "I", 1, [(4, 5), (4, 4), (4, 3), (4, 2), (4, 1), (4, 0)], ['GL_ARB_tessellation_shader', 'GL_ARB_tessellation_shader']),
           ("GL_MAX_TESS_CONTROL_UNIFORM_COMPONENTS", "I", 1, [(4, 5), (4, 4), (4, 3), (4, 2), (4, 1), (4, 0)], ['GL_ARB_tessellation_shader', 'GL_ARB_tessellation_shader']),
           ("GL_MAX_TESS_EVALUATION_UNIFORM_COMPONENTS", "I", 1, [(4, 5), (4, 4), (4, 3), (4, 2), (4, 1), (4, 0)], ['GL_ARB_tessellation_shader', 'GL_ARB_tessellation_shader']),
           ("GL_MAX_TESS_CONTROL_TEXTURE_IMAGE_UNITS", "I", 1, [(4, 5), (4, 4), (4, 3), (4, 2), (4, 1), (4, 0)], ['GL_ARB_tessellation_shader', 'GL_ARB_tessellation_shader']),
           ("GL_MAX_TESS_EVALUATION_TEXTURE_IMAGE_UNITS", "I", 1, [(4, 5), (4, 4), (4, 3), (4, 2), (4, 1), (4, 0)], ['GL_ARB_tessellation_shader', 'GL_ARB_tessellation_shader']),
           ("GL_MAX_TESS_CONTROL_OUTPUT_COMPONENTS", "I", 1, [(4, 5), (4, 4), (4, 3), (4, 2), (4, 1), (4, 0)], ['GL_ARB_tessellation_shader', 'GL_ARB_tessellation_shader']),
           ("GL_MAX_TESS_PATCH_COMPONENTS", "I", 1, [(4, 5), (4, 4), (4, 3), (4, 2), (4, 1), (4, 0)], ['GL_ARB_tessellation_shader', 'GL_ARB_tessellation_shader']),
           ("GL_MAX_TESS_CONTROL_TOTAL_OUTPUT_COMPONENTS", "I", 1, [(4, 5), (4, 4), (4, 3), (4, 2), (4, 1), (4, 0)], ['GL_ARB_tessellation_shader', 'GL_ARB_tessellation_shader']),
           ("GL_MAX_TESS_EVALUATION_OUTPUT_COMPONENTS", "I", 1, [(4, 5), (4, 4), (4, 3), (4, 2), (4, 1), (4, 0)], ['GL_ARB_tessellation_shader', 'GL_ARB_tessellation_shader']),
           ("GL_MAX_TESS_CONTROL_UNIFORM_BLOCKS", "I", 1, [(4, 5), (4, 4), (4, 3), (4, 2), (4, 1), (4, 0)], ['GL_ARB_tessellation_shader', 'GL_ARB_tessellation_shader']),
           ("GL_MAX_TESS_EVALUATION_UNIFORM_BLOCKS", "I", 1, [(4, 5), (4, 4), (4, 3), (4, 2), (4, 1), (4, 0)], ['GL_ARB_tessellation_shader', 'GL_ARB_tessellation_shader']),
           ("GL_COPY_READ_BUFFER", "I", 1, [(3, 2), (4, 4), (3, 3), (4, 5), (3, 1), (4, 3), (4, 2), (4, 1), (4, 0)], ['GL_ARB_copy_buffer', 'GL_ARB_copy_buffer']),
           ("GL_COPY_WRITE_BUFFER", "I", 1, [(3, 2), (4, 4), (3, 3), (4, 5), (3, 1), (4, 3), (4, 2), (4, 1), (4, 0)], ['GL_ARB_copy_buffer', 'GL_ARB_copy_buffer']),
           ("GL_MAX_IMAGE_UNITS", "I", 1, [(4, 5), (4, 2), (4, 4), (4, 3)], ['GL_ARB_shader_image_size', 'GL_ARB_shader_image_load_store', 'GL_ARB_shader_image_size', 'GL_ARB_shader_image_load_store', 'GL_ARB_shader_image_load_store']),
           ("GL_MAX_COMBINED_IMAGE_UNITS_AND_FRAGMENT_OUTPUTS", "I", 1, [(4, 5), (4, 2), (4, 4), (4, 3)], ['GL_ARB_shader_storage_buffer_object', 'GL_ARB_shader_image_size', 'GL_ARB_shader_image_load_store', 'GL_ARB_shader_storage_buffer_object', 'GL_ARB_shader_image_size', 'GL_ARB_shader_image_load_store', 'GL_ARB_shader_image_size', 'GL_ARB_shader_image_load_store', 'GL_ARB_shader_image_load_store']),
           ("GL_DRAW_INDIRECT_BUFFER_BINDING", "I", 1, [(4, 5), (4, 4), (4, 3), (4, 2), (4, 1), (4, 0)], ['GL_ARB_draw_indirect', 'GL_ARB_draw_indirect']),
           ("GL_PRIMITIVE_RESTART", "B", 1, [(3, 2), (4, 4), (3, 3), (4, 5), (3, 1), (4, 3), (4, 2), (4, 1), (4, 0)], []),
           ("GL_PRIMITIVE_RESTART_INDEX", "I", 1, [(3, 2), (4, 4), (3, 3), (4, 5), (3, 1), (4, 3), (4, 2), (4, 1), (4, 0)], []),
           ("GL_MAX_PROGRAM_TEXTURE_GATHER_COMPONENTS_ARB", "I", 1, [], ['GL_ARB_texture_gather']),
           ("GL_VIDEO_BUFFER_BINDING_NV", "I", 1, [], ['GL_NV_video_capture']),
           ("GL_GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX", "I", 1, [], ['GL_NVX_gpu_memory_info']),
           ("GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX", "I", 1, [], ['GL_NVX_gpu_memory_info']),
           ("GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX", "I", 1, [], ['GL_NVX_gpu_memory_info']),
           ("GL_GPU_MEMORY_INFO_EVICTION_COUNT_NVX", "I", 1, [], ['GL_NVX_gpu_memory_info']),
           ("GL_GPU_MEMORY_INFO_EVICTED_MEMORY_NVX", "I", 1, [], ['GL_NVX_gpu_memory_info']),
           ("GL_MAX_IMAGE_SAMPLES", "I", 1, [(4, 5), (4, 2), (4, 4), (4, 3)], ['GL_ARB_shader_image_size', 'GL_ARB_shader_image_load_store', 'GL_ARB_shader_image_size', 'GL_ARB_shader_image_load_store', 'GL_ARB_shader_image_load_store']),
           ("GL_MIN_MAP_BUFFER_ALIGNMENT", "I", 1, [(4, 5), (4, 2), (4, 4), (4, 3)], ['GL_ARB_map_buffer_alignment', 'GL_ARB_map_buffer_alignment']),
           ("GL_MAX_VERTEX_IMAGE_UNIFORMS", "I", 1, [(4, 5), (4, 2), (4, 4), (4, 3)], ['GL_ARB_shader_image_size', 'GL_ARB_shader_image_load_store', 'GL_ARB_shader_image_size', 'GL_ARB_shader_image_load_store', 'GL_ARB_shader_image_load_store']),
           ("GL_MAX_TESS_CONTROL_IMAGE_UNIFORMS", "I", 1, [(4, 5), (4, 2), (4, 4), (4, 3)], ['GL_ARB_shader_image_size', 'GL_ARB_shader_image_load_store', 'GL_ARB_shader_image_size', 'GL_ARB_shader_image_load_store', 'GL_ARB_shader_image_load_store']),
           ("GL_MAX_TESS_EVALUATION_IMAGE_UNIFORMS", "I", 1, [(4, 5), (4, 2), (4, 4), (4, 3)], ['GL_ARB_shader_image_size', 'GL_ARB_shader_image_load_store', 'GL_ARB_shader_image_size', 'GL_ARB_shader_image_load_store', 'GL_ARB_shader_image_load_store']),
           ("GL_MAX_GEOMETRY_IMAGE_UNIFORMS", "I", 1, [(4, 5), (4, 2), (4, 4), (4, 3)], ['GL_ARB_shader_image_size', 'GL_ARB_shader_image_load_store', 'GL_ARB_shader_image_size', 'GL_ARB_shader_image_load_store', 'GL_ARB_shader_image_load_store']),
           ("GL_MAX_FRAGMENT_IMAGE_UNIFORMS", "I", 1, [(4, 5), (4, 2), (4, 4), (4, 3)], ['GL_ARB_shader_image_size', 'GL_ARB_shader_image_load_store', 'GL_ARB_shader_image_size', 'GL_ARB_shader_image_load_store', 'GL_ARB_shader_image_load_store']),
           ("GL_MAX_COMBINED_IMAGE_UNIFORMS", "I", 1, [(4, 5), (4, 2), (4, 4), (4, 3)], ['GL_ARB_shader_image_size', 'GL_ARB_shader_image_load_store', 'GL_ARB_shader_image_size', 'GL_ARB_shader_image_load_store', 'GL_ARB_shader_image_load_store']),
           ("GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS", "I", 1, [(4, 5), (4, 4), (4, 3)], ['GL_ARB_shader_storage_buffer_object', 'GL_ARB_shader_storage_buffer_object']),
           ("GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS", "I", 1, [(4, 5), (4, 4), (4, 3)], ['GL_ARB_compute_shader', 'GL_ARB_compute_shader']),
           ("GL_DISPATCH_INDIRECT_BUFFER_BINDING", "I", 1, [(4, 5), (4, 4), (4, 3)], ['GL_ARB_compute_shader', 'GL_ARB_compute_shader']),
           ("GL_MAX_COLOR_TEXTURE_SAMPLES", "I", 1, [(3, 2), (3, 3), (4, 5), (4, 4), (4, 3), (4, 2), (4, 1), (4, 0)], ['GL_ARB_texture_query_levels', 'GL_ARB_texture_multisample', 'GL_ARB_texture_non_power_of_two', 'GL_ARB_texture_query_lod', 'GL_ARB_texture_query_levels', 'GL_ARB_texture_multisample', 'GL_ARB_texture_non_power_of_two', 'GL_ARB_texture_query_lod', 'GL_ARB_texture_multisample', 'GL_ARB_texture_non_power_of_two', 'GL_ARB_texture_query_lod', 'GL_ARB_texture_non_power_of_two', 'GL_ARB_texture_query_lod', 'GL_ARB_texture_query_lod']),
           ("GL_MAX_DEPTH_TEXTURE_SAMPLES", "I", 1, [(3, 2), (3, 3), (4, 5), (4, 4), (4, 3), (4, 2), (4, 1), (4, 0)], ['GL_ARB_texture_query_levels', 'GL_ARB_texture_multisample', 'GL_ARB_texture_non_power_of_two', 'GL_ARB_texture_query_lod', 'GL_ARB_texture_query_levels', 'GL_ARB_texture_multisample', 'GL_ARB_texture_non_power_of_two', 'GL_ARB_texture_query_lod', 'GL_ARB_texture_multisample', 'GL_ARB_texture_non_power_of_two', 'GL_ARB_texture_query_lod', 'GL_ARB_texture_non_power_of_two', 'GL_ARB_texture_query_lod', 'GL_ARB_texture_query_lod']),
           ("GL_MAX_INTEGER_SAMPLES", "I", 1, [(3, 2), (3, 3), (4, 5), (4, 4), (4, 3), (4, 2), (4, 1), (4, 0)], ['GL_ARB_texture_query_levels', 'GL_ARB_texture_multisample', 'GL_ARB_texture_non_power_of_two', 'GL_ARB_texture_query_lod', 'GL_ARB_texture_query_levels', 'GL_ARB_texture_multisample', 'GL_ARB_texture_non_power_of_two', 'GL_ARB_texture_query_lod', 'GL_ARB_texture_multisample', 'GL_ARB_texture_non_power_of_two', 'GL_ARB_texture_query_lod', 'GL_ARB_texture_non_power_of_two', 'GL_ARB_texture_query_lod', 'GL_ARB_texture_query_lod']),
           ("GL_MAX_VERTEX_OUTPUT_COMPONENTS", "I", 1, [(3, 2), (3, 3), (4, 5), (4, 4), (4, 3), (4, 2), (4, 1), (4, 0)], []),
           ("GL_MAX_GEOMETRY_INPUT_COMPONENTS", "I", 1, [(3, 2), (3, 3), (4, 5), (4, 4), (4, 3), (4, 2), (4, 1), (4, 0)], []),
           ("GL_MAX_GEOMETRY_OUTPUT_COMPONENTS", "I", 1, [(3, 2), (3, 3), (4, 5), (4, 4), (4, 3), (4, 2), (4, 1), (4, 0)], []),
           ("GL_MAX_FRAGMENT_INPUT_COMPONENTS", "I", 1, [(3, 2), (3, 3), (4, 5), (4, 4), (4, 3), (4, 2), (4, 1), (4, 0)], []),
           ("GL_CONTEXT_PROFILE_MASK", "I", 1, [(3, 2), (3, 3), (4, 5), (4, 4), (4, 3), (4, 2), (4, 1), (4, 0)], []),
           ("GL_UNPACK_COMPRESSED_BLOCK_WIDTH", "I", 1, [(4, 5), (4, 2), (4, 4), (4, 3)], ['GL_ARB_compressed_texture_pixel_storage', 'GL_ARB_compressed_texture_pixel_storage']),
           ("GL_UNPACK_COMPRESSED_BLOCK_HEIGHT", "I", 1, [(4, 5), (4, 2), (4, 4), (4, 3)], ['GL_ARB_compressed_texture_pixel_storage', 'GL_ARB_compressed_texture_pixel_storage']),
           ("GL_UNPACK_COMPRESSED_BLOCK_DEPTH", "I", 1, [(4, 5), (4, 2), (4, 4), (4, 3)], ['GL_ARB_compressed_texture_pixel_storage', 'GL_ARB_compressed_texture_pixel_storage']),
           ("GL_UNPACK_COMPRESSED_BLOCK_SIZE", "I", 1, [(4, 5), (4, 2), (4, 4), (4, 3)], ['GL_ARB_compressed_texture_pixel_storage', 'GL_ARB_compressed_texture_pixel_storage']),
           ("GL_PACK_COMPRESSED_BLOCK_WIDTH", "I", 1, [(4, 5), (4, 2), (4, 4), (4, 3)], ['GL_ARB_compressed_texture_pixel_storage', 'GL_ARB_compressed_texture_pixel_storage']),
           ("GL_PACK_COMPRESSED_BLOCK_HEIGHT", "I", 1, [(4, 5), (4, 2), (4, 4), (4, 3)], ['GL_ARB_compressed_texture_pixel_storage', 'GL_ARB_compressed_texture_pixel_storage']),
           ("GL_PACK_COMPRESSED_BLOCK_DEPTH", "I", 1, [(4, 5), (4, 2), (4, 4), (4, 3)], ['GL_ARB_compressed_texture_pixel_storage', 'GL_ARB_compressed_texture_pixel_storage']),
           ("GL_PACK_COMPRESSED_BLOCK_SIZE", "I", 1, [(4, 5), (4, 2), (4, 4), (4, 3)], ['GL_ARB_compressed_texture_pixel_storage', 'GL_ARB_compressed_texture_pixel_storage']),
           ("GL_MAX_DEBUG_MESSAGE_LENGTH", "I", 1, [(4, 5), (4, 4), (4, 3)], []),
           ("GL_MAX_DEBUG_LOGGED_MESSAGES", "I", 1, [(4, 5), (4, 4), (4, 3)], []),
           ("GL_DEBUG_LOGGED_MESSAGES", "I", 1, [(4, 5), (4, 4), (4, 3)], []),
           ("GL_QUERY_BUFFER_BINDING", "I", 1, [(4, 5), (4, 4)], ['GL_ARB_robust_buffer_access_behavior', 'GL_ARB_query_buffer_object', 'GL_ARB_robust_buffer_access_behavior', 'GL_ARB_query_buffer_object', 'GL_ARB_query_buffer_object']),
           ("GL_MAX_SPARSE_TEXTURE_SIZE_ARB", "I", 1, [], ['GL_ARB_sparse_texture2', 'GL_ARB_sparse_texture_clamp', 'GL_ARB_sparse_texture', 'GL_ARB_sparse_texture_clamp', 'GL_ARB_sparse_texture', 'GL_ARB_sparse_texture']),
           ("GL_MAX_SPARSE_3D_TEXTURE_SIZE_ARB", "I", 1, [], ['GL_ARB_sparse_texture2', 'GL_ARB_sparse_texture_clamp', 'GL_ARB_sparse_texture', 'GL_ARB_sparse_texture_clamp', 'GL_ARB_sparse_texture', 'GL_ARB_sparse_texture']),
           ("GL_MAX_SPARSE_ARRAY_TEXTURE_LAYERS", "I", 1, [], ['GL_AMD_sparse_texture']),
           ("GL_TEXTURE_BUFFER_OFFSET_ALIGNMENT", "I", 1, [(4, 5), (4, 4), (4, 3)], ['GL_ARB_texture_buffer_range', 'GL_ARB_texture_buffer_range']),
           ("GL_SPARSE_TEXTURE_FULL_ARRAY_CUBE_MIPMAPS_ARB", "B", 1, [], ['GL_ARB_sparse_texture2', 'GL_ARB_sparse_texture_clamp', 'GL_ARB_sparse_texture', 'GL_ARB_sparse_texture_clamp', 'GL_ARB_sparse_texture', 'GL_ARB_sparse_texture']),
           ("GL_MAX_SHADER_COMPILER_THREADS_ARB", "I", 1, [], ['GL_ARB_parallel_shader_compile']),
           ("GL_MAX_COMPUTE_UNIFORM_BLOCKS", "I", 1, [(4, 5), (4, 4), (4, 3)], ['GL_ARB_compute_shader', 'GL_ARB_compute_shader']),
           ("GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS", "I", 1, [(4, 5), (4, 4), (4, 3)], ['GL_ARB_compute_shader', 'GL_ARB_compute_shader']),
           ("GL_MAX_COMPUTE_IMAGE_UNIFORMS", "I", 1, [(4, 5), (4, 4), (4, 3)], ['GL_ARB_compute_shader', 'GL_ARB_compute_shader']),
           ("GL_MAX_COMPUTE_WORK_GROUP_COUNT", "I", 3, [(4, 5), (4, 4), (4, 3)], ['GL_ARB_compute_shader', 'GL_ARB_compute_shader']),
           ("GL_MAX_COMPUTE_WORK_GROUP_SIZE", "I", 3, [(4, 5), (4, 4), (4, 3)], ['GL_ARB_compute_shader', 'GL_ARB_compute_shader']),
           ("GL_PRIMITIVE_BOUNDING_BOX_EXT", "F", 8, [], ['GL_EXT_primitive_bounding_box']),
           ("GL_MAX_VERTEX_ATOMIC_COUNTER_BUFFERS", "I", 1, [(4, 5), (4, 2), (4, 4), (4, 3)], ['GL_ARB_shader_atomic_counters', 'GL_ARB_shader_clock', 'GL_ARB_shader_ballot', 'GL_ARB_shader_group_vote', 'GL_ARB_shader_draw_parameters', 'GL_ARB_shader_bit_encoding', 'GL_ARB_shader_atomic_counters', 'GL_ARB_shader_clock', 'GL_ARB_shader_ballot', 'GL_ARB_shader_group_vote', 'GL_ARB_shader_draw_parameters', 'GL_ARB_shader_bit_encoding', 'GL_ARB_shader_clock', 'GL_ARB_shader_ballot', 'GL_ARB_shader_group_vote', 'GL_ARB_shader_draw_parameters', 'GL_ARB_shader_bit_encoding', 'GL_ARB_shader_ballot', 'GL_ARB_shader_group_vote', 'GL_ARB_shader_draw_parameters', 'GL_ARB_shader_bit_encoding', 'GL_ARB_shader_group_vote', 'GL_ARB_shader_draw_parameters', 'GL_ARB_shader_bit_encoding', 'GL_ARB_shader_draw_parameters', 'GL_ARB_shader_bit_encoding', 'GL_ARB_shader_bit_encoding']),
           ("GL_MAX_TESS_CONTROL_ATOMIC_COUNTER_BUFFERS", "I", 1, [(4, 5), (4, 2), (4, 4), (4, 3)], ['GL_ARB_shader_atomic_counters', 'GL_ARB_shader_clock', 'GL_ARB_shader_ballot', 'GL_ARB_shader_group_vote', 'GL_ARB_shader_draw_parameters', 'GL_ARB_shader_bit_encoding', 'GL_ARB_shader_atomic_counters', 'GL_ARB_shader_clock', 'GL_ARB_shader_ballot', 'GL_ARB_shader_group_vote', 'GL_ARB_shader_draw_parameters', 'GL_ARB_shader_bit_encoding', 'GL_ARB_shader_clock', 'GL_ARB_shader_ballot', 'GL_ARB_shader_group_vote', 'GL_ARB_shader_draw_parameters', 'GL_ARB_shader_bit_encoding', 'GL_ARB_shader_ballot', 'GL_ARB_shader_group_vote', 'GL_ARB_shader_draw_parameters', 'GL_ARB_shader_bit_encoding', 'GL_ARB_shader_group_vote', 'GL_ARB_shader_draw_parameters', 'GL_ARB_shader_bit_encoding', 'GL_ARB_shader_draw_parameters', 'GL_ARB_shader_bit_encoding', 'GL_ARB_shader_bit_encoding']),
           ("GL_MAX_TESS_EVALUATION_ATOMIC_COUNTER_BUFFERS", "I", 1, [(4, 5), (4, 2), (4, 4), (4, 3)], ['GL_ARB_shader_atomic_counters', 'GL_ARB_shader_clock', 'GL_ARB_shader_ballot', 'GL_ARB_shader_group_vote', 'GL_ARB_shader_draw_parameters', 'GL_ARB_shader_bit_encoding', 'GL_ARB_shader_atomic_counters', 'GL_ARB_shader_clock', 'GL_ARB_shader_ballot', 'GL_ARB_shader_group_vote', 'GL_ARB_shader_draw_parameters', 'GL_ARB_shader_bit_encoding', 'GL_ARB_shader_clock', 'GL_ARB_shader_ballot', 'GL_ARB_shader_group_vote', 'GL_ARB_shader_draw_parameters', 'GL_ARB_shader_bit_encoding', 'GL_ARB_shader_ballot', 'GL_ARB_shader_group_vote', 'GL_ARB_shader_draw_parameters', 'GL_ARB_shader_bit_encoding', 'GL_ARB_shader_group_vote', 'GL_ARB_shader_draw_parameters', 'GL_ARB_shader_bit_encoding', 'GL_ARB_shader_draw_parameters', 'GL_ARB_shader_bit_encoding', 'GL_ARB_shader_bit_encoding']),
           ("GL_MAX_GEOMETRY_ATOMIC_COUNTER_BUFFERS", "I", 1, [(4, 5), (4, 2), (4, 4), (4, 3)], ['GL_ARB_shader_atomic_counters', 'GL_ARB_shader_clock', 'GL_ARB_shader_ballot', 'GL_ARB_shader_group_vote', 'GL_ARB_shader_draw_parameters', 'GL_ARB_shader_bit_encoding', 'GL_ARB_shader_atomic_counters', 'GL_ARB_shader_clock', 'GL_ARB_shader_ballot', 'GL_ARB_shader_group_vote', 'GL_ARB_shader_draw_parameters', 'GL_ARB_shader_bit_encoding', 'GL_ARB_shader_clock', 'GL_ARB_shader_ballot', 'GL_ARB_shader_group_vote', 'GL_ARB_shader_draw_parameters', 'GL_ARB_shader_bit_encoding', 'GL_ARB_shader_ballot', 'GL_ARB_shader_group_vote', 'GL_ARB_shader_draw_parameters', 'GL_ARB_shader_bit_encoding', 'GL_ARB_shader_group_vote', 'GL_ARB_shader_draw_parameters', 'GL_ARB_shader_bit_encoding', 'GL_ARB_shader_draw_parameters', 'GL_ARB_shader_bit_encoding', 'GL_ARB_shader_bit_encoding']),
           ("GL_MAX_FRAGMENT_ATOMIC_COUNTER_BUFFERS", "I", 1, [(4, 5), (4, 2), (4, 4), (4, 3)], ['GL_ARB_shader_atomic_counters', 'GL_ARB_shader_clock', 'GL_ARB_shader_ballot', 'GL_ARB_shader_group_vote', 'GL_ARB_shader_draw_parameters', 'GL_ARB_shader_bit_encoding', 'GL_ARB_shader_atomic_counters', 'GL_ARB_shader_clock', 'GL_ARB_shader_ballot', 'GL_ARB_shader_group_vote', 'GL_ARB_shader_draw_parameters', 'GL_ARB_shader_bit_encoding', 'GL_ARB_shader_clock', 'GL_ARB_shader_ballot', 'GL_ARB_shader_group_vote', 'GL_ARB_shader_draw_parameters', 'GL_ARB_shader_bit_encoding', 'GL_ARB_shader_ballot', 'GL_ARB_shader_group_vote', 'GL_ARB_shader_draw_parameters', 'GL_ARB_shader_bit_encoding', 'GL_ARB_shader_group_vote', 'GL_ARB_shader_draw_parameters', 'GL_ARB_shader_bit_encoding', 'GL_ARB_shader_draw_parameters', 'GL_ARB_shader_bit_encoding', 'GL_ARB_shader_bit_encoding']),
           ("GL_MAX_COMBINED_ATOMIC_COUNTER_BUFFERS", "I", 1, [(4, 5), (4, 2), (4, 4), (4, 3)], ['GL_ARB_shader_atomic_counters', 'GL_ARB_shader_clock', 'GL_ARB_shader_ballot', 'GL_ARB_shader_group_vote', 'GL_ARB_shader_draw_parameters', 'GL_ARB_shader_bit_encoding', 'GL_ARB_shader_atomic_counters', 'GL_ARB_shader_clock', 'GL_ARB_shader_ballot', 'GL_ARB_shader_group_vote', 'GL_ARB_shader_draw_parameters', 'GL_ARB_shader_bit_encoding', 'GL_ARB_shader_clock', 'GL_ARB_shader_ballot', 'GL_ARB_shader_group_vote', 'GL_ARB_shader_draw_parameters', 'GL_ARB_shader_bit_encoding', 'GL_ARB_shader_ballot', 'GL_ARB_shader_group_vote', 'GL_ARB_shader_draw_parameters', 'GL_ARB_shader_bit_encoding', 'GL_ARB_shader_group_vote', 'GL_ARB_shader_draw_parameters', 'GL_ARB_shader_bit_encoding', 'GL_ARB_shader_draw_parameters', 'GL_ARB_shader_bit_encoding', 'GL_ARB_shader_bit_encoding']),
           ("GL_MAX_VERTEX_ATOMIC_COUNTERS", "I", 1, [(4, 5), (4, 2), (4, 4), (4, 3)], ['GL_ARB_shader_atomic_counters', 'GL_ARB_shader_clock', 'GL_ARB_shader_ballot', 'GL_ARB_shader_group_vote', 'GL_ARB_shader_draw_parameters', 'GL_ARB_shader_bit_encoding', 'GL_ARB_shader_atomic_counters', 'GL_ARB_shader_clock', 'GL_ARB_shader_ballot', 'GL_ARB_shader_group_vote', 'GL_ARB_shader_draw_parameters', 'GL_ARB_shader_bit_encoding', 'GL_ARB_shader_clock', 'GL_ARB_shader_ballot', 'GL_ARB_shader_group_vote', 'GL_ARB_shader_draw_parameters', 'GL_ARB_shader_bit_encoding', 'GL_ARB_shader_ballot', 'GL_ARB_shader_group_vote', 'GL_ARB_shader_draw_parameters', 'GL_ARB_shader_bit_encoding', 'GL_ARB_shader_group_vote', 'GL_ARB_shader_draw_parameters', 'GL_ARB_shader_bit_encoding', 'GL_ARB_shader_draw_parameters', 'GL_ARB_shader_bit_encoding', 'GL_ARB_shader_bit_encoding']),
           ("GL_MAX_TESS_CONTROL_ATOMIC_COUNTERS", "I", 1, [(4, 5), (4, 2), (4, 4), (4, 3)], ['GL_ARB_shader_atomic_counters', 'GL_ARB_shader_clock', 'GL_ARB_shader_ballot', 'GL_ARB_shader_group_vote', 'GL_ARB_shader_draw_parameters', 'GL_ARB_shader_bit_encoding', 'GL_ARB_shader_atomic_counters', 'GL_ARB_shader_clock', 'GL_ARB_shader_ballot', 'GL_ARB_shader_group_vote', 'GL_ARB_shader_draw_parameters', 'GL_ARB_shader_bit_encoding', 'GL_ARB_shader_clock', 'GL_ARB_shader_ballot', 'GL_ARB_shader_group_vote', 'GL_ARB_shader_draw_parameters', 'GL_ARB_shader_bit_encoding', 'GL_ARB_shader_ballot', 'GL_ARB_shader_group_vote', 'GL_ARB_shader_draw_parameters', 'GL_ARB_shader_bit_encoding', 'GL_ARB_shader_group_vote', 'GL_ARB_shader_draw_parameters', 'GL_ARB_shader_bit_encoding', 'GL_ARB_shader_draw_parameters', 'GL_ARB_shader_bit_encoding', 'GL_ARB_shader_bit_encoding']),
           ("GL_MAX_TESS_EVALUATION_ATOMIC_COUNTERS", "I", 1, [(4, 5), (4, 2), (4, 4), (4, 3)], ['GL_ARB_shader_atomic_counters', 'GL_ARB_shader_clock', 'GL_ARB_shader_ballot', 'GL_ARB_shader_group_vote', 'GL_ARB_shader_draw_parameters', 'GL_ARB_shader_bit_encoding', 'GL_ARB_shader_atomic_counters', 'GL_ARB_shader_clock', 'GL_ARB_shader_ballot', 'GL_ARB_shader_group_vote', 'GL_ARB_shader_draw_parameters', 'GL_ARB_shader_bit_encoding', 'GL_ARB_shader_clock', 'GL_ARB_shader_ballot', 'GL_ARB_shader_group_vote', 'GL_ARB_shader_draw_parameters', 'GL_ARB_shader_bit_encoding', 'GL_ARB_shader_ballot', 'GL_ARB_shader_group_vote', 'GL_ARB_shader_draw_parameters', 'GL_ARB_shader_bit_encoding', 'GL_ARB_shader_group_vote', 'GL_ARB_shader_draw_parameters', 'GL_ARB_shader_bit_encoding', 'GL_ARB_shader_draw_parameters', 'GL_ARB_shader_bit_encoding', 'GL_ARB_shader_bit_encoding']),
           ("GL_MAX_GEOMETRY_ATOMIC_COUNTERS", "I", 1, [(4, 5), (4, 2), (4, 4), (4, 3)], ['GL_ARB_shader_atomic_counters', 'GL_ARB_shader_clock', 'GL_ARB_shader_ballot', 'GL_ARB_shader_group_vote', 'GL_ARB_shader_draw_parameters', 'GL_ARB_shader_bit_encoding', 'GL_ARB_shader_atomic_counters', 'GL_ARB_shader_clock', 'GL_ARB_shader_ballot', 'GL_ARB_shader_group_vote', 'GL_ARB_shader_draw_parameters', 'GL_ARB_shader_bit_encoding', 'GL_ARB_shader_clock', 'GL_ARB_shader_ballot', 'GL_ARB_shader_group_vote', 'GL_ARB_shader_draw_parameters', 'GL_ARB_shader_bit_encoding', 'GL_ARB_shader_ballot', 'GL_ARB_shader_group_vote', 'GL_ARB_shader_draw_parameters', 'GL_ARB_shader_bit_encoding', 'GL_ARB_shader_group_vote', 'GL_ARB_shader_draw_parameters', 'GL_ARB_shader_bit_encoding', 'GL_ARB_shader_draw_parameters', 'GL_ARB_shader_bit_encoding', 'GL_ARB_shader_bit_encoding']),
           ("GL_MAX_FRAGMENT_ATOMIC_COUNTERS", "I", 1, [(4, 5), (4, 2), (4, 4), (4, 3)], ['GL_ARB_shader_atomic_counters', 'GL_ARB_shader_clock', 'GL_ARB_shader_ballot', 'GL_ARB_shader_group_vote', 'GL_ARB_shader_draw_parameters', 'GL_ARB_shader_bit_encoding', 'GL_ARB_shader_atomic_counters', 'GL_ARB_shader_clock', 'GL_ARB_shader_ballot', 'GL_ARB_shader_group_vote', 'GL_ARB_shader_draw_parameters', 'GL_ARB_shader_bit_encoding', 'GL_ARB_shader_clock', 'GL_ARB_shader_ballot', 'GL_ARB_shader_group_vote', 'GL_ARB_shader_draw_parameters', 'GL_ARB_shader_bit_encoding', 'GL_ARB_shader_ballot', 'GL_ARB_shader_group_vote', 'GL_ARB_shader_draw_parameters', 'GL_ARB_shader_bit_encoding', 'GL_ARB_shader_group_vote', 'GL_ARB_shader_draw_parameters', 'GL_ARB_shader_bit_encoding', 'GL_ARB_shader_draw_parameters', 'GL_ARB_shader_bit_encoding', 'GL_ARB_shader_bit_encoding']),
           ("GL_MAX_COMBINED_ATOMIC_COUNTERS", "I", 1, [(4, 5), (4, 2), (4, 4), (4, 3)], ['GL_ARB_shader_atomic_counters', 'GL_ARB_shader_clock', 'GL_ARB_shader_ballot', 'GL_ARB_shader_group_vote', 'GL_ARB_shader_draw_parameters', 'GL_ARB_shader_bit_encoding', 'GL_ARB_shader_atomic_counters', 'GL_ARB_shader_clock', 'GL_ARB_shader_ballot', 'GL_ARB_shader_group_vote', 'GL_ARB_shader_draw_parameters', 'GL_ARB_shader_bit_encoding', 'GL_ARB_shader_clock', 'GL_ARB_shader_ballot', 'GL_ARB_shader_group_vote', 'GL_ARB_shader_draw_parameters', 'GL_ARB_shader_bit_encoding', 'GL_ARB_shader_ballot', 'GL_ARB_shader_group_vote', 'GL_ARB_shader_draw_parameters', 'GL_ARB_shader_bit_encoding', 'GL_ARB_shader_group_vote', 'GL_ARB_shader_draw_parameters', 'GL_ARB_shader_bit_encoding', 'GL_ARB_shader_draw_parameters', 'GL_ARB_shader_bit_encoding', 'GL_ARB_shader_bit_encoding']),
           ("GL_MAX_ATOMIC_COUNTER_BUFFER_SIZE", "I", 1, [(4, 5), (4, 2), (4, 4), (4, 3)], ['GL_ARB_shader_atomic_counters', 'GL_ARB_shader_clock', 'GL_ARB_shader_ballot', 'GL_ARB_shader_group_vote', 'GL_ARB_shader_draw_parameters', 'GL_ARB_shader_bit_encoding', 'GL_ARB_shader_atomic_counters', 'GL_ARB_shader_clock', 'GL_ARB_shader_ballot', 'GL_ARB_shader_group_vote', 'GL_ARB_shader_draw_parameters', 'GL_ARB_shader_bit_encoding', 'GL_ARB_shader_clock', 'GL_ARB_shader_ballot', 'GL_ARB_shader_group_vote', 'GL_ARB_shader_draw_parameters', 'GL_ARB_shader_bit_encoding', 'GL_ARB_shader_ballot', 'GL_ARB_shader_group_vote', 'GL_ARB_shader_draw_parameters', 'GL_ARB_shader_bit_encoding', 'GL_ARB_shader_group_vote', 'GL_ARB_shader_draw_parameters', 'GL_ARB_shader_bit_encoding', 'GL_ARB_shader_draw_parameters', 'GL_ARB_shader_bit_encoding', 'GL_ARB_shader_bit_encoding']),
           ("GL_MAX_ATOMIC_COUNTER_BUFFER_BINDINGS", "I", 1, [(4, 5), (4, 2), (4, 4), (4, 3)], ['GL_ARB_shader_atomic_counters', 'GL_ARB_shader_clock', 'GL_ARB_shader_ballot', 'GL_ARB_shader_group_vote', 'GL_ARB_shader_draw_parameters', 'GL_ARB_shader_bit_encoding', 'GL_ARB_shader_atomic_counters', 'GL_ARB_shader_clock', 'GL_ARB_shader_ballot', 'GL_ARB_shader_group_vote', 'GL_ARB_shader_draw_parameters', 'GL_ARB_shader_bit_encoding', 'GL_ARB_shader_clock', 'GL_ARB_shader_ballot', 'GL_ARB_shader_group_vote', 'GL_ARB_shader_draw_parameters', 'GL_ARB_shader_bit_encoding', 'GL_ARB_shader_ballot', 'GL_ARB_shader_group_vote', 'GL_ARB_shader_draw_parameters', 'GL_ARB_shader_bit_encoding', 'GL_ARB_shader_group_vote', 'GL_ARB_shader_draw_parameters', 'GL_ARB_shader_bit_encoding', 'GL_ARB_shader_draw_parameters', 'GL_ARB_shader_bit_encoding', 'GL_ARB_shader_bit_encoding']),
           ("GL_DEBUG_OUTPUT", "B", 1, [(4, 5), (4, 4), (4, 3)], []),
           ("GL_SAMPLE_LOCATION_SUBPIXEL_BITS_ARB", "I", 1, [], ['GL_ARB_sample_locations']),
           ("GL_SAMPLE_LOCATION_PIXEL_GRID_WIDTH_ARB", "I", 1, [], ['GL_ARB_sample_locations']),
           ("GL_SAMPLE_LOCATION_PIXEL_GRID_HEIGHT_ARB", "I", 1, [], ['GL_ARB_sample_locations']),
           ("GL_MAX_COMPUTE_VARIABLE_GROUP_INVOCATIONS_ARB", "I", 1, [], ['GL_ARB_compute_variable_group_size']),
           ("GL_MAX_COMPUTE_VARIABLE_GROUP_SIZE_ARB", "I", 1, [], ['GL_ARB_compute_variable_group_size']),
           ("GL_CLIP_ORIGIN", "E", 1, [(4, 5)], ['GL_ARB_clip_control', 'GL_ARB_clip_control']),
           ("GL_CLIP_DEPTH_MODE", "E", 1, [(4, 5)], ['GL_ARB_clip_control', 'GL_ARB_clip_control']),
           ("GL_MULTISAMPLE_LINE_WIDTH_RANGE_ARB", "F", 2, [], ['GL_ARB_ES3_2_compatibility']),
           ("GL_MULTISAMPLE_LINE_WIDTH_GRANULARITY_ARB", "F", 1, [], ['GL_ARB_ES3_2_compatibility'])]
