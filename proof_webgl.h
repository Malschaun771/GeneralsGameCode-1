// proof_webgl.h — schlanker WebGLWrapper fuer den Browser-Render-Proof.
// (Der volle WebGLWrapper.h im Repo bindet das Projekt-Header-Netz ein und
//  gehoert in den spaeteren Gesamtbuild mit korrekten Include-Pfaden.)
#pragma once
#include <cstdint>
struct Vector3 { float X=0, Y=0, Z=0; };
struct Matrix4x4 { float m[16]={}; };
class WebGLWrapper {
public:
    static bool Init(void*, bool lite=false);
    static void Begin_Scene();
    static void End_Scene(bool flip=true);
    static void Clear(bool color, bool zstencil, const Vector3& c, float a=1, float z=1, unsigned st=0);
    static void Draw(unsigned prim, unsigned short, unsigned short,
                     unsigned short=0, unsigned short=0);
    static void Set_Transform(int, const Matrix4x4&);
};
