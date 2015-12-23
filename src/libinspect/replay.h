#ifndef REPLAY_H
#define REPLAY_H
#include "libtrace/libtrace.h"
#include "libinspect/libinspect.h"

#include <stdint.h>

typedef enum {
    ReplayObjType_GLBuffer,
    ReplayObjType_GLSampler,
    ReplayObjType_GLTexture,
    ReplayObjType_GLQuery,
    ReplayObjType_GLFramebuffer,
    ReplayObjType_GLRenderbuffer,
    ReplayObjType_GLSync,
    ReplayObjType_GLProgram,
    ReplayObjType_GLProgramPipeline,
    ReplayObjType_GLShader,
    ReplayObjType_GLVAO,
    ReplayObjType_GLDisplayList,
    ReplayObjType_GLeglImageOES,
    ReplayObjType_GLvdpauSurfaceNV,
    ReplayObjType_GLXVideoDeviceNV,
    ReplayObjType_GLXFBConfig,
    ReplayObjType_GLXVideoCaptureDeviceNV,
    ReplayObjType_GLXFBConfigSGIX,
    ReplayObjType_GLXPbufferSGIX,
    ReplayObjType_GLXVideoSourceSGIX,
    ReplayObjType_GLXContext,
    ReplayObjType_XID
} replay_obj_type_t;

typedef struct replay_context_t replay_context_t;

typedef void (*replay_func_t)(replay_context_t*, trace_command_t*, inspect_command_t*);

struct replay_context_t {
    void* _replay_gl;
    void* _internal;
    void* _display; //Pointer to X display
    void* _fbconfig; //GLXFBConfig
    uint32_t _drawable; //Drawable -> XID -> CARD32 which is a uint32_t
    uint32_t _glx_drawable; //GLXDrawable -> XID -> CARD32 which is a uint32_t
    uint32_t _colormap; //Colormap -> XID -> CARD32 which is a uint32_t
    void* _visual; //Pointer to XVisualInfo
    replay_func_t* funcs;
    inspection_t* inspection;
    void* _current_context;
    bool _in_begin_end;
    const char* current_test_name;
};

replay_context_t* create_replay_context(inspection_t* inspection);
void destroy_replay_context(replay_context_t* context);

uint64_t replay_get_real_object(replay_context_t* ctx, replay_obj_type_t type, uint64_t fake);
uint64_t replay_get_fake_object(replay_context_t* ctx, replay_obj_type_t type, uint64_t real);
void replay_create_object(replay_context_t* ctx, replay_obj_type_t type, uint64_t real, uint64_t fake);
void replay_destroy_object(replay_context_t* ctx, replay_obj_type_t type, uint64_t fake);
size_t replay_get_obj_count(replay_context_t* ctx, replay_obj_type_t type);
void replay_list_real_objects(replay_context_t* ctx, replay_obj_type_t type, uint64_t* real);
void replay_list_fake_objects(replay_context_t* ctx, replay_obj_type_t type, uint64_t* fake);
void replay(replay_context_t* context);

//Negative on failure.
int replay_conv_uniform_location(replay_context_t* ctx, uint64_t fake_prog, unsigned int fake_loc);
void replay_add_uniform(replay_context_t* ctx, uint64_t fake_prog, unsigned int fake, unsigned int real);
//Negative on failure.
int replay_conv_attrib_index(replay_context_t* ctx, uint64_t fake_prog, unsigned int fake_idx);
void replay_add_attrib(replay_context_t* ctx, uint64_t fake_prog, unsigned int fake, unsigned int real);
#endif
