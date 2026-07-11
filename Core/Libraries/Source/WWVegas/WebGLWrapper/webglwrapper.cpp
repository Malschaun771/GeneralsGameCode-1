// webglwrapper.cpp
// Teil des ZH->WebGL/WASM Ports (GPLv3, inoffiziell, basierend auf EA-Quellcode).
// Minimal-Implementierung: WebGL-Kontext, Clear, Transform, Dreieck-Draw.
// Unter Emscripten laeuft dies gegen echtes WebGL; lokal (g++) gegen GL-Stubs,
// damit Syntax + Logik verifizierbar bleiben.

#include "WebGLWrapper.h"
#include <cstdio>
#include <cstring>

#ifdef __EMSCRIPTEN__
#include <emscripten/html5.h>
#include <GLES2/gl2.h>
#else
// --- Lokale GL-Stubs (nur fuer g++ Syntax/Logik-Check) ---
typedef unsigned int GLuint;
typedef int GLint;
typedef unsigned int GLenum;
typedef float GLfloat;
typedef int GLsizei;
typedef void* EMSCRIPTEN_WEBGL_CONTEXT_HANDLE;
#define GL_TRIANGLES 0x0004
#define GL_COLOR_BUFFER_BIT 0x4000
#define GL_DEPTH_BUFFER_BIT 0x00000100
#define GL_FLOAT 0x1406
static GLuint s_dummyVBO = 1;
static inline void glClearColor(GLfloat,GLfloat,GLfloat,GLfloat){}
static inline void glClear(GLbitfield){ (void)0; }
static inline GLuint glCreateShader(GLenum){ return 1; }
static inline void glShaderSource(GLuint,GLsizei,const char*const*,const GLint*){}
static inline void glCompileShader(GLuint){}
static inline GLuint glCreateProgram(){ return 1; }
static inline void glAttachShader(GLuint,GLuint){}
static inline void glLinkProgram(GLuint){}
static inline void glUseProgram(GLuint){}
static inline GLuint glGetAttribLocation(GLuint,const char*){ return 0; }
static inline void glEnableVertexAttribArray(GLuint){}
static inline void glVertexAttribPointer(GLuint,GLint,GLenum,GLboolean,GLsizei,const void*){}
static inline GLuint glGetUniformLocation(GLuint,const char*){ return 0; }
static inline void glUniformMatrix4fv(GLuint,GLsizei,GLboolean,const GLfloat*){}
static inline void glGenBuffers(GLsizei,GLuint* p){ *p = ++s_dummyVBO; }
static inline void glBindBuffer(GLenum,GLuint){}
static inline void glBufferData(GLenum,GLsizeiptr,const void*,GLenum){}
static inline void glDrawArrays(GLenum,GLint,GLsizei){}
#endif

// --- WebGLWrapper State ---
bool WebGLWrapper::s_initialized = false;
unsigned long WebGLWrapper::s_frameCount = 0;

#ifdef __EMSCRIPTEN__
static EMSCRIPTEN_WEBGL_CONTEXT_HANDLE s_ctx = 0;
#endif
static GLuint s_program = 0;
static GLuint s_vbo = 0;
static float s_viewProj[16];

static const char* VERT_SRC =
    "attribute vec2 a_pos;\n"
    "uniform mat4 u_mvp;\n"
    "void main(){ gl_Position = u_mvp * vec4(a_pos,0.0,1.0); }";
static const char* FRAG_SRC =
    "precision mediump float;\n"
    "void main(){ gl_FragColor = vec4(0.2,0.7,1.0,1.0); }";

bool WebGLWrapper::Init(void* canvas, bool lite)
{
#ifdef __EMSCRIPTEN__
    EmscriptenWebGLContextAttributes attrs;
    emscripten_webgl_init_context_attributes(&attrs);
    attrs.majorVersion = 1;
    s_ctx = emscripten_webgl_create_context((const char*)canvas, &attrs);
    if (!s_ctx) return false;
    emscripten_webgl_make_context_current(s_ctx);
#endif
    // Minimal-Shader fuer Dreieck-Beweis
    GLuint vs = glCreateShader(0x8B31); // GL_VERTEX_SHADER
    const char* v = VERT_SRC; glShaderSource(vs,1,&v,nullptr); glCompileShader(vs);
    GLuint fs = glCreateShader(0x8B30); // GL_FRAGMENT_SHADER
    const char* f = FRAG_SRC; glShaderSource(fs,1,&f,nullptr); glCompileShader(fs);
    s_program = glCreateProgram();
    glAttachShader(s_program, vs); glAttachShader(s_program, fs); glLinkProgram(s_program);
    glUseProgram(s_program);

    // Ein Dreieck (NDC)
    float tri[6] = { -0.5f,-0.5f,  0.5f,-0.5f,  0.0f, 0.5f };
    glGenBuffers(1, &s_vbo);
    glBindBuffer(0x8892 /*GL_ARRAY_BUFFER*/, s_vbo);
    glBufferData(0x8892, sizeof(tri), tri, 0x88E0 /*GL_STATIC_DRAW*/);

    // Identity MVP als Start
    for (int i=0;i<16;i++) s_viewProj[i] = (i%5==0)?1.0f:0.0f;

    s_initialized = true;
    return true;
}

void WebGLWrapper::Shutdown()
{
    s_initialized = false;
}

void WebGLWrapper::Begin_Scene() {}
void WebGLWrapper::End_Scene(bool flip) { s_frameCount++; (void)flip; }
void WebGLWrapper::Flip_To_Primary() {}

void WebGLWrapper::Clear(bool color, bool zstencil, const Vector3& c, float a, float z, unsigned stencil)
{
    glClearColor(c.X, c.Y, c.Z, a);
    GLbitfield mask = 0;
    if (color)    mask |= GL_COLOR_BUFFER_BIT;
    if (zstencil) mask |= GL_DEPTH_BUFFER_BIT;
    (void)z; (void)stencil;
    glClear(mask);
}

void WebGLWrapper::Set_Viewport(int, int, int, int) {}
void WebGLWrapper::Set_Vertex_Buffer(const VertexBufferClass*, unsigned) {}
void WebGLWrapper::Set_Index_Buffer(const IndexBufferClass*, unsigned short) {}
void WebGLWrapper::Set_Index_Buffer_Index_Offset(unsigned) {}

void WebGLWrapper::Get_Render_State(RenderStateStruct_WebGL&) {}
void WebGLWrapper::Set_Render_State(const RenderStateStruct_WebGL&) {}
void WebGLWrapper::Release_Render_State() {}
void WebGLWrapper::Apply_Render_State_Changes() {}

void WebGLWrapper::Set_Transform(int, const Matrix4x4& m)
{
    for (int i=0;i<16;i++) s_viewProj[i] = m.m[i]; // kopiert 4x4 (row-major Annahme)
}
void WebGLWrapper::Set_Transform(int, const Matrix3D&) {}
void WebGLWrapper::Get_Transform(int, Matrix4x4&) {}
void WebGLWrapper::Set_World_Identity() {}
void WebGLWrapper::Set_View_Identity() {}
bool WebGLWrapper::Is_World_Identity() { return false; }
bool WebGLWrapper::Is_View_Identity() { return false; }

void WebGLWrapper::Set_Material(const VertexMaterialClass*) {}
void WebGLWrapper::Set_Light(unsigned, const LightClass&) {}
void WebGLWrapper::Set_Light_Environment(LightEnvironmentClass*) {}
void WebGLWrapper::Set_Fog(bool, const Vector3&, float, float) {}
void WebGLWrapper::Set_Ambient(const Vector3&) {}

void WebGLWrapper::Set_Shader(const ShaderClass&) {}
void WebGLWrapper::Get_Shader(ShaderClass&) {}
void WebGLWrapper::Set_Texture(unsigned, TextureBaseClass*) {}

void WebGLWrapper::Draw(unsigned primitive_type, unsigned short, unsigned short,
                        unsigned short, unsigned short)
{
    (void)primitive_type;
    glUseProgram(s_program);
    glBindBuffer(0x8892, s_vbo);
    GLuint loc = glGetAttribLocation(s_program, "a_pos");
    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, 2, GL_FLOAT, false, 0, nullptr);
    GLuint u = glGetUniformLocation(s_program, "u_mvp");
    glUniformMatrix4fv(u, 1, false, s_viewProj);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

void WebGLWrapper::Draw_Sorting_IB_VB(unsigned, unsigned short, unsigned short,
                                      unsigned short, unsigned short)
{
    // Minimal: gleicher Pfad wie Draw (Beweis-Stufe)
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

void WebGLWrapper::Set_Render_Target(WebGLSurfaceHandle*, bool) {}
TextureClass* WebGLWrapper::Create_Render_Target(int, int, WW3DFormat) { return nullptr; }

unsigned long WebGLWrapper::Get_FrameCount() { return s_frameCount; }
void WebGLWrapper::Set_Default_Global_Render_States() {}
void WebGLWrapper::Set_Gamma(float, float, float, bool, bool) {}
