// webgl_device.h — WebGL-Geraetekern fuer den ZH-WebGL-Port.
// Ersetzt die DX8-Geraetekette (LoadLibrary/Direct3DCreate8/CreateDevice/Present)
// durch einen Emscripten-WebGL2-Context. Nur fuer USE_WEBGL builds.
#pragma once
#include <cstdint>

namespace WebGLDevice {
    // Erzeugt den WebGL2-Context aus dem uebergebenen Canvas (id) und macht ihn current.
    // Liefert true bei Erfolg. Muss vor allen GL-Calls stehen.
    bool Create_Context(const char* canvas_id = "canvas");
    // Tauscht den Backbuffer sichtbar (Emscripten erledigt das implizit pro Frame).
    void Present();
    // Gibt den Context frei.
    void Release_Context();
    // Liefert einen opaque Pointer auf den aktuellen Context (fuer Debug/State).
    void* Current_Context();
}
