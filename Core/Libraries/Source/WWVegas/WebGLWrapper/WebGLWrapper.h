// WebGLWrapper.h
// Teil des ZH→WebGL/WASM Ports (GPLv3, inoffiziell, basierend auf EA-Quellcode).
// Spiegelt die oeffentliche Schnittstelle von DX8Wrapper (dx8wrapper.h) als
// plattformneutrale Fassade, deren Implementierung WebGL statt DirectX8 nutzt.
//
// Strategie: Spiel-Logik ruft DX8Wrapper::* auf. Per Build-Schalter USE_WEBGL
// wird DX8Wrapper zu einem Alias auf WebGLWrapper, sodass KEINE Spiel-Logik
// geaendert werden muss. Alle DX8-spezifischen Typen (IDirect3D*, D3DTRANSFORMSTATETYPE,
// D3DMATERIAL8, ...) werden hier durch plattformneutrale Stubs/WebGL-Aequivalente ersetzt.

#ifndef WEBGLWRAPPER_H
#define WEBGLWRAPPER_H

#include "always.h"
#include "matrix4.h"
#include "matrix3d.h"
#include "vector3.h"
#include "ww3dformat.h"
#include "refcount.h"
#include "shader.h"
#include "texture.h"
#include "vertmaterial.h"
#include "light.h"
#include "texturebase.h"
#include "renderdevicedesc.h"
#include <cstdint>

// --- Plattformneutrale Stubs fuer ehemals DX8-spezifische Typen ---------------
// (Im WebGL-Build ersetzen wir die echten D3D-Typen durch leere/Wrapper-Strukturen,
//  damit der uebrige Code unveraendert kompiliert.)
struct WebGLTextureHandle;        // opaque: haelt GL-Texture-ID
struct WebGLSurfaceHandle;        // opaque: haelt GL-Framebuffer/RenderTarget
typedef unsigned int WebGLShaderHandle;

// WW3D_FORMAT_UNKNOWN ist ein Enum-Eintrag aus ww3dformat.h. Falls nicht
// definiert, verwenden wir den default-konstruierten WW3DFormat-Wert.
#ifndef WW3D_FORMAT_UNKNOWN
#define WW3D_FORMAT_UNKNOWN WW3DFormat()
#endif

struct RenderStateStruct_WebGL {
    enum ChangedStates {
        WORLD_CHANGED     = 1 << 0,
        VIEW_CHANGED      = 1 << 1,
        LIGHT0_CHANGED    = 1 << 2,
        LIGHT1_CHANGED    = 1 << 3,
        LIGHT2_CHANGED    = 1 << 4,
        LIGHT3_CHANGED    = 1 << 5,
        TEXTURE0_CHANGED  = 1 << 6,
        TEXTURE1_CHANGED  = 1 << 7,
        TEXTURE2_CHANGED  = 1 << 8,
        TEXTURE3_CHANGED  = 1 << 9,
        MATERIAL_CHANGED  = 1 << 14,
        SHADER_CHANGED    = 1 << 15,
        VERTEX_BUFFER_CHANGED = 1 << 16,
        INDEX_BUFFER_CHANGED  = 1 << 17,
        WORLD_IDENTITY    = 1 << 18,
        VIEW_IDENTITY     = 1 << 19,
        TEXTURES_CHANGED  = TEXTURE0_CHANGED | TEXTURE1_CHANGED | TEXTURE2_CHANGED | TEXTURE3_CHANGED,
        LIGHTS_CHANGED    = LIGHT0_CHANGED | LIGHT1_CHANGED | LIGHT2_CHANGED | LIGHT3_CHANGED,
    };
};

class WebGLWrapper
{
public:
    // --- Lebenszyklus ---
    static bool Init(void * canvas_or_hwnd, bool lite = false);
    static void Shutdown();

    // --- Frame ---
    static void Begin_Scene();
    static void End_Scene(bool flip_frame = true);
    static void Flip_To_Primary();
    static void Clear(bool clear_color, bool clear_z_stencil, const Vector3 &color,
                      float dest_alpha = 0.0f, float z = 1.0f, unsigned int stencil = 0);

    // --- Viewport / Buffer ---
    static void Set_Viewport(int x, int y, int w, int h);
    static void Set_Vertex_Buffer(const VertexBufferClass* vb, unsigned stream = 0);
    static void Set_Index_Buffer(const IndexBufferClass* ib, unsigned short index_base_offset);
    static void Set_Index_Buffer_Index_Offset(unsigned offset);

    // --- State ---
    static void Get_Render_State(RenderStateStruct_WebGL& state);
    static void Set_Render_State(const RenderStateStruct_WebGL& state);
    static void Release_Render_State();
    static void Apply_Render_State_Changes();
    static void Set_Default_Global_Render_States();

    // --- Transforms ---
    static void Set_Transform(int transform, const Matrix4x4& m);
    static void Set_Transform(int transform, const Matrix3D& m);
    static void Get_Transform(int transform, Matrix4x4& m);
    static void Set_World_Identity();
    static void Set_View_Identity();
    static bool Is_World_Identity();
    static bool Is_View_Identity();

    // --- Material / Light / Fog ---
    static void Set_Material(const VertexMaterialClass* material);
    static void Set_Light(unsigned index, const LightClass &light);
    static void Set_Light_Environment(LightEnvironmentClass* light_env);
    static void Set_Fog(bool enable, const Vector3 &color, float start, float end);
    static void Set_Ambient(const Vector3& color);

    // --- Shader / Texturen ---
    static void Set_Shader(const ShaderClass& shader);
    static void Get_Shader(ShaderClass& shader);
    static void Set_Texture(unsigned stage, TextureBaseClass* texture);

    // --- Draw (die beiden zentralen Einstiege!) ---
    static void Draw(unsigned primitive_type, unsigned short start_index,
                     unsigned short polygon_count, unsigned short min_vertex_index = 0,
                     unsigned short vertex_count = 0);
    static void Draw_Sorting_IB_VB(unsigned primitive_type, unsigned short start_index,
                                   unsigned short polygon_count, unsigned short min_vertex_index,
                                   unsigned short vertex_count);

    // --- Render Targets ---
    static void Set_Render_Target(WebGLSurfaceHandle* render_target, bool use_default_depth = false);
    static TextureClass* Create_Render_Target(int width, int height,
                                              WW3DFormat format = WW3D_FORMAT_UNKNOWN);

    // --- Statistik / Misc ---
    static unsigned long Get_FrameCount();
    static void Set_Gamma(float gamma, float bright, float contrast, bool calibrate = true, bool uselimit = true);

private:
    // WebGL-Kontext, State-Cache etc. werden in webglwrapper.cpp gehalten.
    static bool s_initialized;
    static unsigned long s_frameCount;
};

#endif // WEBGLWRAPPER_H
