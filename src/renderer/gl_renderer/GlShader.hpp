#pragma once

#include <GL/glew.h>

class GlShader {
public:
    GlShader() = default;
    bool InitProgram();
    void UseProgram();
private:
    bool CompileShader(GLuint *id, const char *src, GLenum type);

private:
    GLuint m_ProgramId;
};
