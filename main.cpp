
#define INITIAL_WINDOW_WIDTH 800.0f
#define INITIAL_WINDOW_HEIGHT 600.0f
#define ATLAS_MAX_VERTEX_COUNT (4096)

#include "stdio.h"
#include "math.h"
#include "assert.h"
#include "windows.h"
#include "string.h"

#define OffsetOf(type, member)  ((size_t)&(((type*)0)->member))

typedef unsigned int u32;
typedef float real32;
typedef int i32;
typedef long long i64;

typedef struct {
    union{
        struct {
            real32 x;
            real32 y;
        };
        struct {
            real32 a;
            real32 b;
        };
    };
} Vec2;

#include <GL/GL.h>

typedef struct 
{
    HDC window_dc;
    HGLRC opengl_rc;
} OpenGL_Context_Win32;

#include "opengl.h"


LRESULT CALLBACK WindowProc(HWND window_handle, UINT message, WPARAM w_param, LPARAM l_param)
{
    switch(message)
    {
        case WM_SIZE : 
        {
            return 0;
        } break;
        
        case WM_SIZING : 
        {
            return TRUE;
        } break;
        
        case WM_DESTROY : 
        case WM_QUIT : 
        {
            PostQuitMessage(0);
            return 0;
        } break;
        
        
        default : 
        {
            return CallWindowProc(DefWindowProc, window_handle, message, w_param, l_param); 
        } break;
    }
}



typedef struct {
    Vec2 pos;
} Vertex;


i32 main()
{
    Vec2 window_dim = { INITIAL_WINDOW_WIDTH, INITIAL_WINDOW_HEIGHT};
    
    HINSTANCE instance = GetModuleHandle(0);
    CoInitializeEx(NULL, COINIT_MULTITHREADED); 
    WNDCLASSEX main_class{
        .cbSize        = sizeof main_class,
        .style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC,
        .lpfnWndProc   = WindowProc,
        .hInstance     = instance,
        .hCursor       = LoadCursor(NULL, IDC_ARROW),
        .lpszClassName = "Main Class"
    };
    RegisterClassEx(&main_class);
    HWND window = CreateWindow("Main Class", "DSP Bench", 
                               WS_OVERLAPPEDWINDOW,
                               CW_USEDEFAULT, 
                               CW_USEDEFAULT, 
                               (unsigned int )window_dim.x, (unsigned int)window_dim.y + 30,
                               0,0,
                               instance, 
                               nullptr);
    OpenGL_Context_Win32 ctx_win32 = opengl_initialize_win32(window);
    
    const char *vertex_shader_source = 
        "#version 330 core\n"
        "in vec2 pos;\n"
        
        "void main()\n"
        "{\n"
        "    gl_Position = vec4(pos, 1.0, 1.0 );\n"
        "}\n";
    
    const char *fragment_shader_source =  
        "#version 330 core\n"
        
        "void main()\n"
        "{\n"
        
        "}\n";
    
    const unsigned int shader_program = opengl_create_program(vertex_shader_source, fragment_shader_source);
    const GLint attribute_pos = glGetAttribLocation(shader_program, "pos");
    
    printf("before\n");
    glVertexAttribPointer(attribute_pos, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)OffsetOf(Vertex, pos));
    
    printf("after\n");
    return 0;
}