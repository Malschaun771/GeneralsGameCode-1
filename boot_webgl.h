// boot_webgl.h — schlanke WebGLWrapper-Fassade fuer den Boot-Harness.
// Self-contained (keine Projekt-Header), da der Build sonst das volle
// WW3D2-Header-Netz braucht. Spiegelt die Kern-DX8Wrapper-Signaturen,
// die der Boot-Frame-Loop nutzt. (Der volle WebGLWrapper.h ist fuer den
// spaeteren CMake-Emscripten-Build mit korrekten Include-Pfaden.)
#pragma once
#include <cstdint>

struct Vector3 { float X=0, Y=0, Z=0; Vector3()=default; Vector3(float x,float y,float z):X(x),Y(y),Z(z){} };
struct Matrix3D { float m[12]={}; };
struct Matrix4x4 { float m[16]={}; };
struct Vector4 { float X=0,Y=0,Z=0,W=0; };
struct RenderStateStruct {};
class VertexBufferClass {};
class IndexBufferClass {};
class ShaderClass {};
class VertexMaterialClass {};
class LightClass {};
class LightEnvironmentClass {};
class TextureBaseClass {};
class TextureClass {};
class RenderDeviceDescClass {};
// WW3D_FORMAT_UNKNOWN als Default-Wert (im echten Build aus ww3dformat.h)
#ifndef WW3D_FORMAT_UNKNOWN
#define WW3D_FORMAT_UNKNOWN 0
#endif

class WebGLWrapper {
public:
    static bool Init(void* hwnd, bool lite = false);
    static void Shutdown();
    static bool Is_Initted() { return s_initted; }
    static bool Is_Device_Lost() { return false; }
    static void Do_Onetime_Device_Dependent_Inits() {}
    static void Do_Onetime_Device_Dependent_Shutdowns() {}
    static void Begin_Scene();
    static void End_Scene(bool flip_frame = true);
    static void Flip_To_Primary() {}
    static void Clear(bool clear_color, bool clear_z_stencil, const Vector3& color,
                      float dest_alpha = 0.0f, float z = 1.0f, unsigned int stencil = 0);
    static void Set_Transform(int transform, const Matrix4x4& m);
    static void Set_Transform(int transform, const Matrix3D& m);
    static void Get_Transform(int transform, Matrix4x4& m);
    static void Set_World_Identity() {}
    static void Set_View_Identity() {}
    static bool Is_World_Identity() { return true; }
    static bool Is_View_Identity() { return true; }
    static void Set_Projection_Transform_With_Z_Bias(const Matrix4x4& m, float zn, float zf);
    static void Draw(unsigned primitive_type, unsigned short start_index,
                     unsigned short polygon_count, unsigned short min_vertex_index = 0,
                     unsigned short vertex_count = 0);
    static void Draw_Sorting_IB_VB(unsigned primitive_type, unsigned short start_index,
                                   unsigned short polygon_count, unsigned short min_vertex_index,
                                   unsigned short vertex_count);
    static void Set_Render_State(const RenderStateStruct&) {}
    static void Get_Render_State(RenderStateStruct&) {}
    static void Release_Render_State() {}
    static void Apply_Render_State_Changes() {}
    static void Set_Default_Global_Render_States() {}
    static void Apply_Default_State() {}
    static void Invalidate_Cached_Render_States() {}
    static void Set_Viewport(const void* vp) {}
    static void Set_Vertex_Buffer(const VertexBufferClass*, unsigned = 0) {}
    static void Set_Index_Buffer(const IndexBufferClass*, unsigned short = 0) {}
    static void Set_Index_Buffer_Index_Offset(unsigned) {}
    static void Set_Shader(const ShaderClass&) {}
    static void Get_Shader(ShaderClass&) {}
    static void Set_Texture(unsigned, TextureBaseClass*) {}
    static void Set_Material(const VertexMaterialClass*) {}
    static void Set_Light(unsigned, const LightClass&) {}
    static void Set_Fog(bool, const Vector3&, float, float) {}
    static void Set_Ambient(const Vector3&) {}
    static void Set_Gamma(float, float, float, bool = true, bool = true) {}
    static TextureClass* Create_Render_Target(int, int, int = WW3D_FORMAT_UNKNOWN) { return nullptr; }
    static void Set_Render_Target(void* = nullptr, bool = false) {}
    static unsigned Get_Free_Texture_RAM() { return 0; }
    static void Flush_DX8_Resource_Manager(unsigned = 0) {}
    static bool Set_Render_Device(int = -1, int = -1, int = -1, int = -1, int = -1, bool = false) { return true; }
    static bool Set_Device_Resolution(int = -1, int = -1, int = -1, int = -1, bool = false) { return true; }
    static bool Toggle_Windowed() { return false; }
    static int  Get_Render_Device_Count() { return 1; }
    static int  Get_Render_Device() { return 0; }
    static bool Reset_Device(bool = true) { return true; }
    static unsigned long Get_FrameCount() { return s_frame; }
    static void Begin_Statistics() {}
    static void End_Statistics() {}
    static void Reset_Statistics() {}
    static unsigned int Convert_Color(const Vector3&, float = 1.0f) { return 0; }
    static unsigned int Convert_Color_Clamp(const Vector4&) { return 0; }
private:
    static bool s_initted;
    static unsigned long s_frame;
    static float s_mvp[16];
    static float s_viewproj[16];
};
