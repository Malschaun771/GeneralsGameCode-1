// webglwrapper.cpp — DX8Wrapper-kompatible Fassade (WebGL-Backend).
// Boot-Harness: Kern-Render-Pfade funktional, Rest no-op.
#include "boot_webgl.h"
#include "webgl_device.h"
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <emscripten/html5.h>
#include <cstdio>
#include <cmath>

bool WebGLWrapper::s_initted = false;
unsigned long WebGLWrapper::s_frame = 0;
float WebGLWrapper::s_mvp[16];
float WebGLWrapper::s_viewproj[16];

static GLuint s_prog = 0, s_vbo = 0;
static const char* VERT =
    "attribute vec2 a_pos;\n"
    "uniform mat4 u_mvp;\n"
    "void main(){ gl_Position = u_mvp * vec4(a_pos,0.0,1.0); }";
static const char* FRAG =
    "precision mediump float;\n"
    "void main(){ gl_FragColor = vec4(0.2,0.7,1.0,1.0); }";
static GLuint mk(const char* src, GLenum t){
    GLuint s = glCreateShader(t);
    glShaderSource(s,1,&src,nullptr); glCompileShader(s); return s;
}

bool WebGLWrapper::Init(void* hwnd, bool lite)
{
    if (!WebGLDevice::Create_Context("canvas")) return false;
    GLuint vs=mk(VERT,GL_VERTEX_SHADER), fs=mk(FRAG,GL_FRAGMENT_SHADER);
    s_prog=glCreateProgram(); glAttachShader(s_prog,vs); glAttachShader(s_prog,fs);
    glLinkProgram(s_prog); glUseProgram(s_prog);
    float tri[6]={-0.5f,-0.5f, 0.5f,-0.5f, 0.0f,0.5f};
    glGenBuffers(1,&s_vbo); glBindBuffer(GL_ARRAY_BUFFER,s_vbo);
    glBufferData(GL_ARRAY_BUFFER,sizeof(tri),tri,GL_STATIC_DRAW);
    for(int i=0;i<16;i++){ s_mvp[i]=(i%5==0)?1.0f:0.0f; s_viewproj[i]=s_mvp[i]; }
    s_initted = true;
    printf("WebGLWrapper: Init OK (WebGL2)\n");
    return true;
}

void WebGLWrapper::Shutdown()
{
    WebGLDevice::Release_Context();
    s_initted = false;
    printf("WebGLWrapper: Shutdown\n");
}

void WebGLWrapper::Begin_Scene()
{
    // nothing needed; clear happens in Clear()
}

void WebGLWrapper::End_Scene(bool flip_frame)
{
    // implicit present via Emscripten; advance frame counter
    if (flip_frame) s_frame++;
}

void WebGLWrapper::Clear(bool clear_color, bool clear_z_stencil, const Vector3& color,
                         float dest_alpha, float z, unsigned int)
{
    glClearColor(color.X, color.Y, color.Z, dest_alpha);
    GLbitfield m=0;
    if (clear_color)    m |= GL_COLOR_BUFFER_BIT;
    if (clear_z_stencil) m |= GL_DEPTH_BUFFER_BIT;
    glClear(m);
}

void WebGLWrapper::Set_Transform(int, const Matrix4x4& m)
{
    for (int i=0;i<16;i++) s_viewproj[i] = m.m[i];
}

void WebGLWrapper::Set_Transform(int, const Matrix3D& m)
{
    for (int i=0;i<12;i++) s_viewproj[i] = m.m[i];
    s_viewproj[12]=s_viewproj[13]=s_viewproj[14]=0.0f; s_viewproj[15]=1.0f;
}

void WebGLWrapper::Get_Transform(int, Matrix4x4& m)
{
    for (int i=0;i<16;i++) m.m[i] = s_viewproj[i];
}

void WebGLWrapper::Set_Projection_Transform_With_Z_Bias(const Matrix4x4& m, float, float)
{
    for (int i=0;i<16;i++) s_mvp[i] = m.m[i];
}

void WebGLWrapper::Draw(unsigned primitive_type, unsigned short, unsigned short polygon_count,
                         unsigned short, unsigned short)
{
    (void)primitive_type;
    if (polygon_count == 0) return;
    glUseProgram(s_prog);
    glBindBuffer(GL_ARRAY_BUFFER, s_vbo);
    GLuint loc = glGetAttribLocation(s_prog, "a_pos");
    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    GLuint u = glGetUniformLocation(s_prog, "u_mvp");
    // simple animated rotation around Z so the boot frame-loop is visible
    float a = (float)(s_frame % 360) * 3.14159265f / 180.0f;
    float c = cosf(a), s = sinf(a);
    float mvp[16] = { c,-s,0,0,  s,c,0,0,  0,0,1,0,  0,0,0,1 };
    glUniformMatrix4fv(u, 1, GL_FALSE, mvp);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    printf("WebGLWrapper: Draw frame=%lu\n", s_frame);
}

void WebGLWrapper::Draw_Sorting_IB_VB(unsigned primitive_type, unsigned short si,
                                       unsigned short pc, unsigned short mv, unsigned short vc)
{
    Draw(primitive_type, si, pc, mv, vc);
}
