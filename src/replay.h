#ifndef REPLAY_H
#define REPLAY_H
#include "libtrace.h"
#include "libinspect.h"

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
    ReplayObjType_GLShader,
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
    uint32_t _drawable; //Drawable -> XID -> CARD32 which is a uint32_t
    replay_func_t* funcs;
    inspection_t* inspection;
};

replay_context_t* create_replay_context(inspection_t* inspection);
void destroy_replay_context(replay_context_t* context);

void* replay_get_real_object(replay_context_t* ctx, replay_obj_type_t type, void* fake);
void* replay_get_fake_object(replay_context_t* ctx, replay_obj_type_t type, void* real);
void replay_create_object(replay_context_t* ctx, replay_obj_type_t type, void* real, void *fake);
void replay_destroy_object(replay_context_t* ctx, replay_obj_type_t type, void* fake);
void replay(replay_context_t* context);
#endif
