// main.cpp — Browser-Render-Proof des ZH-WebGL-Ports.
#include "proof_webgl.h"
#include <emscripten/html5.h>

extern "C" {
    EMSCRIPTEN_KEEPALIVE
    void run_render_proof() {
        WebGLWrapper::Init(nullptr, false);
        WebGLWrapper::Begin_Scene();
        WebGLWrapper::Clear(true, true, Vector3{0.1f,0.1f,0.15f}, 1.0f, 1.0f, 0);
        WebGLWrapper::Draw(4 /*GL_TRIANGLES*/, 0, 1);
        WebGLWrapper::End_Scene(true);
    }
}
