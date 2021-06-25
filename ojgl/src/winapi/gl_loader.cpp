#include "gl_loader.h"
#include "Windows.h"
#include <crtdbg.h>

void* GetAnyGLFuncAddress(const char* name)
{
    void* p = (void*)wglGetProcAddress(name);
    if (p == 0 || (p == (void*)0x1) || (p == (void*)0x2) || (p == (void*)0x3) || (p == (void*)-1)) {
        HMODULE module = LoadLibraryA("opengl32.dll");
        p = (void*)GetProcAddress(module, name);
    }

    return p;
}

const char* gl_function_names[] = {
    "glActiveTexture",
    "glDebugMessageCallbackAMD",
    "glDebugMessageEnableAMD",
    "glDebugMessageInsertAMD",
    "glGetDebugMessageLogAMD",
    "glDebugMessageCallback",
    "glDebugMessageControl",
    "glGetActiveUniformBlockName",
    "glGetActiveUniformBlockiv",
    "glGetActiveUniformsiv",
    "glGetActiveUniformName",
    "glCompileShader",
    "glGetShaderiv",
    "glGetShaderInfoLog",
    "glCreateProgram",
    "glProgramBinary",
    "glCreateShader",
    "glShaderSource",
    "glDeleteShader",
    "glGetProgramiv",
    "glAttachShader",
    "glProgramParameteri",
    "glLinkProgram",
    "glDeleteProgram",
    "glUseProgram",
    "glGetUniformLocation",
    "glGetUniformBlockIndex",
    "glGetProgramBinary",
    "glGenVertexArrays",
    "glBindVertexArray",
    "glEnableVertexAttribArray",
    "glGenBuffers",
    "glBindBuffer",
    "glBufferData",
    "glVertexAttribPointer",
    "glDeleteBuffers",
    "glDeleteVertexArrays",
    "glUniform1f",
    "glUniform3fv",
    "glUniform4fv",
    "glUniformMatrix4fv",
    "glBindBufferBase",
    "glDispatchCompute",
    "glMemoryBarrier",
    "glMultiDrawArrays",
    "glDrawBuffers",
    "glGenFramebuffers",
    "glFramebufferTexture2D",
    "glCheckFramebufferStatus",
    "glBindFramebuffer",
    "glDeleteFramebuffers",
    "glGetQueryObjectiv",
    "glGetQueryObjectui64v",
    "glTexImage2DMultisample",
    "glGenerateMipmap",
    "glUniform1i",
    "glGetAttribLocation",
    "glBufferSubData",
    "glUniform4iv",
    "glDetachShader",
    "glValidateProgram",
    "glUniform1fv",
    "glUniform2f",
    "glGetProgramInfoLog"
};

void* gl_function_pointers[sizeof(gl_function_names) / sizeof(const char*)];

#ifdef _DEBUG
const char* gl_debug_function_names[] = {
    "glGetTextureParameteriv",
    "glDetachShader",
    "glGenQueries",
    "glEndQuery",
    "glBeginQuery",
    "glGetProgramiv",
    "glMapBuffer",
    "glUnmapBuffer"
};
void* gl_debug_function_pointers[sizeof(gl_function_names) / sizeof(const char*)];
#endif

int load_gl_functions()
{
    int failed = 0;
    for (int i = 0; i < sizeof(gl_function_names) / sizeof(const char*); i++) {
        const char* name = gl_function_names[i];
        void* ptr = GetAnyGLFuncAddress(name);
        gl_function_pointers[i] = ptr;
        if (ptr == NULL) {
#ifdef _DEBUG
            // Do not assert on mising AMD specific functions
            if (name != "glDebugMessageCallbackAMD" && name != "glDebugMessageEnableAMD" && name != "glDebugMessageInsertAMD" && name != "glGetDebugMessageLogAMD") {
                _ASSERT_EXPR(false, "Failed to load GL func");
            }
#endif
            failed++;
        }
    }

#ifdef _DEBUG
    for (int i = 0; i < sizeof(gl_debug_function_names) / sizeof(const char*); i++) {
        const char* name = gl_debug_function_names[i];
        void* ptr = GetAnyGLFuncAddress(name);
        gl_debug_function_pointers[i] = ptr;
        if (ptr == NULL) {
            _ASSERT_EXPR(false, "Failed to load debug GL func");
            failed++;
        }
    }
#endif

    return failed;
}
