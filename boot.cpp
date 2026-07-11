// boot.cpp — ZH-WebGL Boot-Harness.
// Simuliert die Engine-Sequenz (Init -> Frame-Loop -> Shutdown) ueber die
// WebGLWrapper-Fassade. main() wird von Emscripten automatisch nach Runtime-Init
// aufgerufen; die Frame-Schleife laeuft ueber emscripten_set_main_loop.
// Endlos-Frame-Loop mit FPS-Zaehler (Beweis: Render-Pfad laeuft stabil).
#include "boot_webgl.h"
#include <emscripten.h>
#include <cstdio>
#include <chrono>

static int s_frames_done = 0;
static bool s_running = false;
static double s_fps = 0.0;
static std::chrono::steady_clock::time_point s_t0;

static void frame_loop()
{
    WebGLWrapper::Begin_Scene();
    WebGLWrapper::Clear(true, true, Vector3(0.05f, 0.07f, 0.12f), 1.0f, 1.0f, 0);
    Matrix4x4 ident;
    for (int i=0;i<16;i++) ident.m[i] = (i%5==0)?1.0f:0.0f;
    WebGLWrapper::Set_Transform(0 /*D3DTS_WORLD*/, ident);
    WebGLWrapper::Set_Transform(2 /*D3DTS_PROJECTION*/, ident);
    WebGLWrapper::Draw(/*primitive*/ 4, /*start*/ 0, /*polygons*/ 1, /*min_vtx*/ 0, /*vtx*/ 0);
    WebGLWrapper::End_Scene(true);

    s_frames_done++;
    if (s_frames_done % 30 == 0) {
        auto now = std::chrono::steady_clock::now();
        double secs = std::chrono::duration<double>(now - s_t0).count();
        if (secs > 0.0) s_fps = 30.0 / secs;
        s_t0 = now;
        printf("BOOT-HARNESS: frame=%d fps=%.1f\n", s_frames_done, s_fps);
    }
}

static void boot_shutdown()
{
    printf("BOOT-HARNESS: %d frames -> Shutdown\n", s_frames_done);
    WebGLWrapper::Shutdown();
    s_running = false;
    emscripten_cancel_main_loop();
}

int main()
{
    printf("BOOT-HARNESS: Init...\n");
    if (!WebGLWrapper::Init(nullptr, false)) {
        printf("BOOT-HARNESS: Init FAILED (WebGL unavailable?)\n");
        return 1;
    }
    s_frames_done = 0;
    s_running = true;
    s_t0 = std::chrono::steady_clock::now();
    printf("BOOT-HARNESS: Frame-Loop startet\n");
    emscripten_set_main_loop(frame_loop, 0, 1);
    return 0;
}
