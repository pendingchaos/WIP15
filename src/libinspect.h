#ifndef LIB_INSPECT_H
#define LIB_INSPECT_H
#include "libtrace.h"
#include <stdbool.h>

typedef enum {
    AttachType_Info,
    AttachType_Warning,
    AttachType_Error
} attachment_type_t;

typedef enum {
    StateEnableEntry_AlphaTest,
    StateEnableEntry_AutoNormal,
    StateEnableEntry_Blend,
    //TODO: Clip plane,
    StateEnableEntry_ColorArray,
    StateEnableEntry_ColorLogicOp,
    StateEnableEntry_ColorMaterial,
    StateEnableEntry_ColorSum,
    StateEnableEntry_ColorTable,
    StateEnableEntry_Convolution1D,
    StateEnableEntry_Convolution2D,
    StateEnableEntry_CullFace,
    StateEnableEntry_DepthTest,
    StateEnableEntry_Dither,
    StateEnableEntry_EdgeFlagArray,
    StateEnableEntry_Fog,
    StateEnableEntry_FogCoordArray,
    StateEnableEntry_Histogram,
    StateEnableEntry_IndexArray,
    StateEnableEntry_IndexLogicOp,
    //TODO: Lights
    StateEnableEntry_Lighting,
    StateEnableEntry_LineSmooth,
    StateEnableEntry_LineStipple,
    StateEnableEntry_Map1Color4,
    StateEnableEntry_Map1Index,
    StateEnableEntry_Map1Normal,
    StateEnableEntry_Map1TexCoord1,
    StateEnableEntry_Map1TexCoord2,
    StateEnableEntry_Map1TexCoord3,
    StateEnableEntry_Map1TexCoord4,
    StateEnableEntry_Map2Color4,
    StateEnableEntry_Map2Index,
    StateEnableEntry_Map2Normal,
    StateEnableEntry_Map2TexCoord1,
    StateEnableEntry_Map2TexCoord2,
    StateEnableEntry_Map2TexCoord3,
    StateEnableEntry_Map2TexCoord4,
    StateEnableEntry_Map2Vertex3,
    StateEnableEntry_Map2Vertex4,
    StateEnableEntry_MinMax,
    StateEnableEntry_Multisample,
    StateEnableEntry_NormalArray,
    StateEnableEntry_Normalize,
    StateEnableEntry_PointSmooth,
    StateEnableEntry_PointSprite,
    StateEnableEntry_PolygonSmooth,
    StateEnableEntry_PolygonOffsetFill,
    StateEnableEntry_PolygonOffsetLine,
    StateEnableEntry_PolygonOffsetPoint,
    StateEnableEntry_PolygonStipple,
    StateEnableEntry_PostColorMatrixColorTable,
    StateEnableEntry_PostConvolutionColorTable,
    StateEnableEntry_RescaleNormal,
    StateEnableEntry_SampleAlphaToCoverage,
    StateEnableEntry_SampleAlphaToOne,
    StateEnableEntry_SampleCoverage,
    StateEnableEntry_ScissorTest,
    StateEnableEntry_SecondaryColorArray,
    StateEnableEntry_Separable2D,
    StateEnableEntry_StencilTest,
    StateEnableEntry_Texture1D,
    StateEnableEntry_Texture2D,
    StateEnableEntry_Texture3D,
    StateEnableEntry_TextureCoordArray,
    StateEnableEntry_TextureCubeMap,
    StateEnableEntry_TextureGenQ,
    StateEnableEntry_TextureGenR,
    StateEnableEntry_TextureGenS,
    StateEnableEntry_TextureGenT,
    StateEnableEntry_VertexArray,
    StateEnableEntry_VertexProgramPointSize,
    StateEnableEntry_VertexProgramTwoSide,
    
    StateEnableEntry_Max
} state_enable_entry_t;

typedef struct inspect_attachment_t {
    attachment_type_t type;
    char* message;
    struct inspect_attachment_t* next;
} inspect_attachment_t;

typedef struct {
    bool enable[StateEnableEntry_Max];
} inspect_gl_state_t;

typedef struct inspect_command_t {
    trace_command_t* trace_cmd;
    inspect_attachment_t* attachments;
    uint64_t gl_context;
    char *name;
    uint64_t cpu_duration; //Nanoseconds
    uint64_t gpu_duration; //Nanoseconds
    inspect_gl_state_t state;
    struct inspect_command_t* next;
} inspect_command_t;

typedef struct inspect_frame_t {
    inspect_command_t* commands;
    trace_frame_t* trace_frame;
    struct inspect_frame_t* next;
} inspect_frame_t;

typedef struct {
    const trace_t *trace;
    inspect_frame_t* frames;
} inspection_t;

inspection_t* create_inspection(const trace_t* trace);
void free_inspection(inspection_t* inspection);
void inspect(inspection_t* inspection);
void inspect_add_info(inspect_command_t* command, const char* format, ...);
void inspect_add_warning(inspect_command_t* command, const char* format, ...);
void inspect_add_error(inspect_command_t* command, const char* format, ...);
void inspect_add_attachment(inspect_command_t* command, inspect_attachment_t* attach);
#endif
