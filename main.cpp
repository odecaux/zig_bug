
#define INITIAL_WINDOW_WIDTH 800.0f
#define INITIAL_WINDOW_HEIGHT 600.0f
#define ATLAS_MAX_VERTEX_COUNT (4096)

#include "stdio.h"
#include "math.h"
#include "assert.h"
#include "windows.h"
#include "string.h"

#include <GL/GL.h>

#define OffsetOf(type, member)  ((size_t)&(((type*)0)->member))

typedef unsigned int u32;
typedef float real32;
typedef int i32;
typedef long long i64;

typedef struct {
    real32 x;
    real32 y;
} Vec2;

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
        
        HDC window_dc = GetDC(window);
        
        const PIXELFORMATDESCRIPTOR pixel_format = {
            sizeof(PIXELFORMATDESCRIPTOR),
            1,
            PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
            PFD_TYPE_RGBA,        
            32,                   
            0, 0, 0, 0, 0, 0,
            0,
            0,
            0,
            0, 0, 0, 0,
            24,                   
            8,                   
            0,
            PFD_MAIN_PLANE,
            0,
            0, 0, 0
        };
        
        const int pixel_format_idx = ChoosePixelFormat(window_dc, &pixel_format);
        if(pixel_format_idx == 0)
        {
            printf("couldn't find a valid pixel format\n");
            exit(1);
        }
        
        SetPixelFormat(window_dc, pixel_format_idx, &pixel_format);
        
        const HGLRC tempContext = wglCreateContext(window_dc);
        if(wglMakeCurrent(window_dc, tempContext) == FALSE)
        {
            printf("couldn't open opengl temp context\n");
            exit(1);
        }
        
        const auto wglCreateContextAttribsARB = (wglCreateContextAttribsARB_t)wglGetProcAddress("wglCreateContextAttribsARB");
        
        const int attribs[] = {
            WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
            WGL_CONTEXT_MINOR_VERSION_ARB, 3,
            WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
            WGL_CONTEXT_FLAGS_ARB, true ? WGL_CONTEXT_DEBUG_BIT_ARB : 0, //TODO debug / release 
            0
        };
        
        const HGLRC opengl_rc = wglCreateContextAttribsARB(window_dc, 0, attribs);
        if (opengl_rc == 0)
        {
            printf("couldn't create the elevated opengl context\n");
            exit(0);
        }
        
        
        wglMakeCurrent(NULL, NULL);
        wglDeleteContext(tempContext);
        if(wglMakeCurrent(window_dc, opengl_rc) == FALSE)
        {
            printf("couldn't open opengl elevated context\n");
            exit(1);
        }
        
        if(!load_opengl_functions())
        {
            printf("failed to load opengl functions\n");
            exit(1);
        }
    }
    
    {
        const char *vertex_shader_source = 
            "#version 330 core\n"
            "in vec2 pos;\n"
            
            "void main()\n"
            "{\n"
            "    gl_Position = vec4(pos, 1.0, 1.0 );\n"
            "}\n";
        
        const char *fragment_shader_source =  
            "#version 330 core\n"
            "out vec4 fragColor;"
            
            "void main()\n"
            "{\n"
            "    fragColor = vec4(0.0);\n"
            "}\n";
        
        const unsigned int shader_program = opengl_create_program(vertex_shader_source, fragment_shader_source);
        const GLint attribute_pos = glGetAttribLocation(shader_program, "pos");
        
        printf("before\n");
        glVertexAttribPointer(attribute_pos, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)OffsetOf(Vertex, pos));
        printf("after\n");
    }
    return 0;
}