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
