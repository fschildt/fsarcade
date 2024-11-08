#include <renderer/gl_renderer/GlShader.hpp>
#include <GL/glew.h>
#include <stdio.h>

static const char *s_vertex_shader_src = 
"#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"\n"
"out vec4 vertexColor;\n"
"\n"
"void main()\n"
"{\n"
"    gl_Position = vec4(aPos.xy, 0.0, 1.0);\n"
"    vertexColor = vec4(aColor, 1.0);\n"
"}\n"
;

static const char *s_fragment_shader_src =
"#version 330 core\n"
"in vec4 vertexColor;\n"
"\n"
"out vec4 FragColor;\n"
"\n"
"void main()\n"
"{\n"
"    FragColor = vertexColor;\n"
"}\n"
;

bool GlShader::InitProgram() {
    GLuint vertex_shader;
    GLuint fragment_shader;
    if (!CompileShader(&vertex_shader, s_vertex_shader_src, GL_VERTEX_SHADER) ||
        !CompileShader(&fragment_shader, s_fragment_shader_src, GL_FRAGMENT_SHADER)) {
        return false;
    }

    GLuint program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    int success;
    char info_log[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        printf("error linking shader program\n%s\n", info_log);
        return false;
    }

    m_ProgramId = program;
    return true;
}

void GlShader::UseProgram() {
    glUseProgram(m_ProgramId);
}

bool GlShader::CompileShader(GLuint *id, const char *src, GLenum type) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, 0);
    glCompileShader(shader);

    int  success;
    char info_log[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(shader, 512, 0, info_log);
        printf("error %s shader compilation failed\n%s\n", type == GL_VERTEX_SHADER ? "vertex" : "fragment", info_log);
        glDeleteShader(shader);
        return false;
    }

    *id = shader;
    return true;
}
