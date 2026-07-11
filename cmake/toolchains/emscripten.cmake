# Emscripten (WebAssembly) toolchain for the ZH WebGL port.
# Use via CMakePresets.json "emscripten" preset (toolchainFile).
# Maps the DX8 backend to WebGL (USE_WEBGL=ON) and disables native-only deps.

set(CMAKE_SYSTEM_NAME Emscripten)
set(CMAKE_SYSTEM_VERSION 1)

# Force the emscripten SDK compilers (override any host gcc/clang).
set(CMAKE_C_COMPILER emcc)
set(CMAKE_CXX_COMPILER em++)

# wasm32 is 32-bit; keep pointer size consistent with the win32/mingw builds.
set(CMAKE_SIZEOF_VOID_P 4 CACHE STRING "" FORCE)

# TheSuperHackers build expects WIN32+32bit to pull in dx8.cmake (min-dx8-sdk).
# Under Emscripten we deliberately do NOT define WIN32, so dx8.cmake is skipped
# and the WebGL backend (USE_WEBGL) replaces dx8wrapper.cpp instead.
set(USE_WEBGL ON CACHE BOOL "Replace DX8Wrapper with WebGLWrapper backend" FORCE)

# vcpkg deps (zlib, ffmpeg) are not available the same way under Emscripten.
# zlib ships with emscripten; ffmpeg would need emscripten-ports or a stub.
# We disable vcpkg integration here and let emscripten provide what it can.
set(VCPKG_MANIFEST_MODE OFF CACHE BOOL "" FORCE)
