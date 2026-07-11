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

### Direkte DX8-Nutzung (Bypass-Gefahr) – 46 Dateien
Am `DX8Wrapper` vorbei greifen u.a. direkt auf Direct3D zu:
- `W3DShaderManager.cpp`  ← HÄRTESTE Stelle: DX8 Vertex/Pixel-Shader (müssen als WebGL-Programme neu geschrieben werden)
- `W3DWater.cpp`, `W3DSnow.cpp`, `W3DTreeBuffer.cpp` (Wasser/Schnee/Bäume)
- `texture.cpp`, `surfaceclass.cpp`, `texproject.cpp` (Texturen / Render-Targets)
- `W3DProjectedShadow.cpp`, `W3DVolumetricShadow.cpp` (Schatten)
- `assetmgr.cpp`, `ddsfile.cpp` (Asset/DDS-Textur-Laden)

Die meisten dieser 46 Stellen nutzen DX8 nur für Textur-Erzeugung und Render-Targets →
abfangbar über neue `WebGLTexture` / `WebGLSurface`-Klassen. Der echte harte Kern ist `W3DShaderManager`.

### Build-Pipeline (TheSuperHackers)
- CMake-Presets: `win32`, Linux via Docker.
- vcpkg-Manifest (`vcpkg.json`) für Abhängigkeiten.
- Cross-Compile nach WASM erfordert Emscripten (`emcc`) als Toolchain — eigener CMake-Preset nötig.

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
