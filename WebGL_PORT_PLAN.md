# ZH → WebGL / WebAssembly Port — Analyse & Plan

Ziel: Generals Zero Hour im Browser (Safari/Chrome) spielbar machen.
Basis: EA-Quellcode (GPLv3) + TheSuperHackers/GeneralsGameCode (CMake, VS2022, C++20, vcpkg, Linux/Docker).

## Erkenntnisse aus der Code-Analyse (Stand 2026-07-11)

### Render-Architektur
- Grafik läuft über die **WW3D2-Engine** (Westwood 3D), Kern: `dx8wrapper.cpp` (4440 Zeilen).
- `DX8Wrapper` ist eine **statische Singleton-Fassade** mit ~80 öffentlichen Methoden.
  Sie kapselt ALLES DX8-spezifische hinter einer plattformneutralen C++-Schnittstelle.
- Alle Draw-Aufrufe laufen über genau ZWEI Einstiege:
  - `DX8Wrapper::Draw(primitive_type, start_index, polygon_count, min_vertex_index, vertex_count)`
  - `DX8Wrapper::Draw_Sorting_IB_VB(primitive_type, start_index, polygon_count, min_vertex_index, vertex_count)`
- State wird über `RenderStateStruct` (World/View/Light/Texture/Shader/Material Changed-Flags) abgebildet.

### Abhängigkeiten (Schritt 2 – präzisiert 2026-07-11)
- `DX8Wrapper::` wird in **512 Stellen** aufgerufen — NICHT nur in der Game-Schicht,
  sondern **innerhalb von `WW3D2` selbst** (der Core-Render-Bibliothek):
  - `dx8wrapper.cpp`: 181 Calls (+ eigene D3D-Device-Erzeugung)
  - `dx8renderer.cpp`: 39 Calls
  - `sortingrenderer.cpp`: 34, `dynamesh.cpp`: 18, `seglinerenderer.cpp`: 17
  - `pointgr.cpp`: 25, `texture.cpp`: 18, `textureloader.cpp`: 20, `sphereobj.cpp`: 12, ...
- **Harte Kernstelle:** `dx8wrapper.cpp` lädt `d3d8.dll` per `LoadLibrary` +
  `Direct3DCreate8` + `CreateDevice`. Das geht unter WASM **gar nicht** →
  diese Funktion muss komplett durch WebGL-Context-Erzeugung ersetzt werden.
- **Bypass (direkter DX8-Zugriff) in 8 Dateien** (nicht über die Fassade):
  `dx8vertexbuffer.cpp`, `dx8indexbuffer.cpp`, `dx8caps.cpp`,
  `dx8polygonrenderer.h`, `dx8renderer.h`, `dx8texman.h`, `dx8webbrowser.*`
  → diese müssen auf WebGL-Buffer/State umgestellt werden.
- **Härteste Logik-Stelle:** `W3DShaderManager.cpp` (GameEngineDevice) —
  DX8 Vertex/Pixel-Shader → müssen als WebGL-Programme (GLSL) neu geschrieben werden.

### Build-Pipeline (Schritt 3 – präzisiert 2026-07-11)
`CMakePresets.json` (version 6) bietet bereits:
- `vc6`, `win32` (MSVC/Ninja), `unix` (vcpkg), **`mingw-w64-i686`** (MinGW! Unix Makefiles)
- **KEIN Emscripten-Preset** → muss neu angelegt werden
  (`toolchainFile: <emsdk>/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake`).
- `mingw-w64-i686` beweist: Projekt kompiliert schon nicht-MSVC → Brücke zu Emscripten.
- vcpkg (SDL etc.) muss unter Emscripten durch `emscripten-ports` ersetzt werden.

## Port-Strategie
1. `WebGLWrapper` implementiert dieselbe ~80-Methoden-Schnittstelle wie `DX8Wrapper`.
2. Spiel-Logik ruft weiter `DX8Wrapper::*` auf → wir lenken das per Compile-Schalter
   (`#ifdef USE_WEBGL`) oder Link-Ziel auf `WebGLWrapper` um.
3. Schrittweise: Clear/Transform → Textur-Upload → ShaderClass→GLSL → Draw-Bindings → Emscripten-Build.
4. Audio (Miles) und Input (Mouse/Keyboard/Touch) sind eigene, spätere Module.

## Rechtliches (EA GPLv3-Zusatzbedingungen)
- Fork erlaubt, Modifikationen müssen als solche gekennzeichnet sein.
- Keine EA-Marken: Projekt-Namen z.B. "Generals-ZH WebEdition (inoffiziell, GPLv3)".
- Geänderter Quellcode bei Verbreitung offenzulegen.
