/* date = February 1st 2022 8:38 am */

#ifndef OPENGL_H
#define OPENGL_H

//~ OpenGL defines
#define GL_VERTEX_SHADER                  0x8B31
#define GL_FRAGMENT_SHADER                0x8B30
#define GL_ARRAY_BUFFER                   0x8892
#define GL_TEXTURE_BUFFER                 0x8C2A
#define GL_STATIC_DRAW                    0x88E4
#define GL_STREAM_DRAW                    0x88E0
#define GL_COMPILE_STATUS                 0x8B81
#define GL_LINK_STATUS                    0x8B82
//TODO est-ce qu'on a vraiment besoin de ça ? on a pas tant de triangles que ça à priori
#define GL_ELEMENT_ARRAY_BUFFER           0x8893
#define GL_INFO_LOG_LENGTH                0x8B84
#define GL_MAJOR_VERSION                  0x821B
#define GL_MINOR_VERSION                  0x821C
#define GL_R32F                           0x822E
#define GL_RG32F                          0x8230

#define WGL_CONTEXT_MAJOR_VERSION_ARB     0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB     0x2092
#define WGL_CONTEXT_LAYER_PLANE_ARB       0x2093
#define WGL_CONTEXT_FLAGS_ARB             0x2094
#define WGL_CONTEXT_DEBUG_BIT_ARB         0x00000001
#define WGL_CONTEXT_PROFILE_MASK_ARB      0x9126
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB  0x00000001

#define OPENGL_PROC(ret, name, param)  typedef ret(* name ## _t)param
#include "opengl_proc.inc"
#undef OPENGL_PROC


#define OPENGL_PROC(ret, name, param)  static name ## _t name = nullptr 
#include "opengl_proc.inc"
#undef OPENGL_PROC

GLenum opengl_check_error_(const char *file, int line)
{
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR)
    {
        const char *error;
        switch (errorCode)
        {
            case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
            case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
            case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
            case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
            case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
            case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
            default: error = "other error"; break;
        }
        printf("opengl error :  %s in %s:%d\n", error, file, line);
    }
    return errorCode;
}
#define opengl_check_error() opengl_check_error_(__FILE__, __LINE__) 

bool load_opengl_functions()
{
    
#define OPENGL_PROC(ret, function_name, param)  function_name = (function_name##_t) wglGetProcAddress(#function_name); if(!function_name) { printf("failed to load" #function_name "\n"); return false; }
#include "opengl_proc.inc"
#undef OPENGL_PROC
    
    return true;
}

OpenGL_Context_Win32 opengl_initialize_win32(HWND window)
{
    
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
    
    GLint major = 0;
    GLint minor = 0;
    glGetIntegerv(GL_MAJOR_VERSION, &major);
    glGetIntegerv(GL_MINOR_VERSION, &minor);
    
    //printf("opengl version : %d.%d\n", major, minor);
    
    const char* target_openg_version = "#version 330 core";
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    wglSwapIntervalEXT(1);
    return {
        .window_dc = window_dc,
        .opengl_rc = opengl_rc
    };
}

u32 opengl_create_program(const char *vertex_shader_source, const char* fragment_shader_source)
{
    GLint status = GL_TRUE;
    
    const u32 vertex_shader_handle = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader_handle, 1, &vertex_shader_source, NULL);
    glCompileShader(vertex_shader_handle);
    glGetShaderiv(vertex_shader_handle, GL_COMPILE_STATUS, &status);
    assert(status == GL_TRUE);
    
    const u32 fragment_shader_handle = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader_handle, 1, &fragment_shader_source, NULL);
    glCompileShader(fragment_shader_handle);
    glGetShaderiv(fragment_shader_handle, GL_COMPILE_STATUS, &status);
    assert(status == GL_TRUE);
    
    const u32 shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader_handle);
    glAttachShader(shader_program, fragment_shader_handle);
    glLinkProgram(shader_program);
    glGetProgramiv(shader_program, GL_LINK_STATUS, &status);
    assert(status == GL_TRUE);
    opengl_check_error();
    
    glDeleteShader(vertex_shader_handle);
    glDeleteShader(fragment_shader_handle);
    
    return shader_program;
}

#endif //OPENGL_H
