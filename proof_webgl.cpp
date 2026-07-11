// proof_webgl.cpp — Browser-Render-Proof: WebGL-Kontext, Clear, Dreieck-Draw.
#include "proof_webgl.h"
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <emscripten/html5.h>
#include <cstdio>

static GLuint s_prog = 0, s_vbo = 0;
static float s_mvp[16];
static bool s_ok = false;

static const char* VERT =
    "attribute vec2 a_pos; uniform mat4 u_mvp;"
    "void main(){ gl_Position = u_mvp * vec4(a_pos,0.0,1.0); }";
static const char* FRAG =
    "precision mediump float; void main(){ gl_FragColor = vec4(0.2,0.7,1.0,1.0); }";

static GLuint mk(const char* src, GLenum t){
    GLuint s = glCreateShader(t);
    glShaderSource(s,1,&src,nullptr); glCompileShader(s); return s;
}

bool WebGLWrapper::Init(void*, bool){
    // Explizit WebGL2-Context holen (Emscripten uebergibt canvas via Module)
    EmscriptenWebGLContextAttributes attrs;
    emscripten_webgl_init_context_attributes(&attrs);
    attrs.majorVersion = 2;
    EMSCRIPTEN_WEBGL_CONTEXT_HANDLE ctx = emscripten_webgl_create_context("canvas", &attrs);
    if (!ctx) { printf("WEBGL_CONTEXT_NULL\n"); return false; }
    emscripten_webgl_make_context_current(ctx);

    GLuint vs=mk(VERT,GL_VERTEX_SHADER), fs=mk(FRAG,GL_FRAGMENT_SHADER);
    s_prog=glCreateProgram(); glAttachShader(s_prog,vs); glAttachShader(s_prog,fs);
    glLinkProgram(s_prog); glUseProgram(s_prog);
    float tri[6]={-0.5f,-0.5f, 0.5f,-0.5f, 0.0f,0.5f};
    glGenBuffers(1,&s_vbo); glBindBuffer(GL_ARRAY_BUFFER,s_vbo);
    glBufferData(GL_ARRAY_BUFFER,sizeof(tri),tri,GL_STATIC_DRAW);
    for(int i=0;i<16;i++) s_mvp[i]=(i%5==0)?1.0f:0.0f;
    s_ok = true;
    return true;
}
void WebGLWrapper::Begin_Scene(){}
void WebGLWrapper::End_Scene(bool){}
void WebGLWrapper::Clear(bool c,bool z,const Vector3& col,float a,float,unsigned){
    if(!s_ok) return;
    glClearColor(col.X,col.Y,col.Z,a);
    GLbitfield m=0; if(c)m|=GL_COLOR_BUFFER_BIT; if(z)m|=GL_DEPTH_BUFFER_BIT;
    glClear(m);
}
void WebGLWrapper::Set_Transform(int,const Matrix4x4& m){ for(int i=0;i<16;i++)s_mvp[i]=m.m[i]; }
void WebGLWrapper::Draw(unsigned,unsigned short,unsigned short,unsigned short,unsigned short){
    if(!s_ok) return;
    glUseProgram(s_prog); glBindBuffer(GL_ARRAY_BUFFER,s_vbo);
    GLuint loc=glGetAttribLocation(s_prog,"a_pos");
    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc,2,GL_FLOAT,GL_FALSE,0,nullptr);
    GLuint u=glGetUniformLocation(s_prog,"u_mvp");
    glUniformMatrix4fv(u,1,GL_FALSE,s_mvp);
    glDrawArrays(GL_TRIANGLES,0,3);
    printf("DREIECK_GEZEICHNET\n");
}
