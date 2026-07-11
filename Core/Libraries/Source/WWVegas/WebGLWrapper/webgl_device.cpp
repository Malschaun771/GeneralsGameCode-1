// webgl_device.cpp — WebGL-Geraetekern (Emscripten/WebGL2).
// Bildet die DX8-Geraetekette auf einen Browser-WebGL-Context ab.
#include "webgl_device.h"
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <emscripten/html5.h>
#include <cstdio>

static EMSCRIPTEN_WEBGL_CONTEXT_HANDLE s_ctx = 0;

namespace WebGLDevice {

bool Create_Context(const char* canvas_id)
{
    if (s_ctx) return true;  // idempotent

    EmscriptenWebGLContextAttributes attrs;
    emscripten_webgl_init_context_attributes(&attrs);
    attrs.majorVersion = 2;
    attrs.minorVersion = 0;
    attrs.alpha = false;
    attrs.depth = true;
    attrs.stencil = false;
    attrs.antialias = true;
    attrs.preserveDrawingBuffer = false;

    s_ctx = emscripten_webgl_create_context(canvas_id, &attrs);
    if (!s_ctx) {
        printf("WebGLDevice: context creation failed for '%s'\n", canvas_id ? canvas_id : "(null)");
        return false;
    }
    EMSCRIPTEN_RESULT r = emscripten_webgl_make_context_current(s_ctx);
    if (r != EMSCRIPTEN_RESULT_SUCCESS) {
        printf("WebGLDevice: make_context_current failed (%d)\n", (int)r);
        return false;
    }
    printf("WebGLDevice: WebGL2 context ready (canvas='%s')\n", canvas_id ? canvas_id : "(null)");
    return true;
}

void Present()
{
    // Emscripten praesentiert den Backbuffer automatisch nach dem Frame-Callback.
    // Ein explizites Swap ist unter WebGL nicht noetig; hier nur Marker fuer Profiling.
}

void Release_Context()
{
    if (s_ctx) {
        emscripten_webgl_destroy_context(s_ctx);
        s_ctx = 0;
    }
}

void* Current_Context()
{
    return (void*)(uintptr_t)s_ctx;
}

} // namespace WebGLDevice
