#ifndef SHADER_H
#define SHADER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Shader Shader;

void shader_link(const char* vertex_path, const char* fragment_path);
void shader_use();
void shader_set_bool(const char* name, int value);
void shader_set_int(const char* name, int value);
void shader_set_float(const char* name, float value);
void shader_set_int_array(const char* name, int* value);
void shader_delete();

void checkCompileErrors(unsigned int shader, const char* type) {
    int success;
    char infoLog[1024];
    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            printf("ERROR::SHADER_COMPILATION_ERROR of type: %s\nInfo log: %s", type, infoLog);
        }
    } else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            printf("ERROR::PROGRAM_LINKING_ERROR of type: %s\nInfo log: %s", type, infoLog);
        }
    }
}

unsigned int vertex_shader, fragment_shader;
unsigned int ID;

typedef struct Shader {
    void (*link)(const char*, const char*);
    void (*use)();

    void (*set_bool)(const char*, int);
    void (*set_int)(const char*, int);
    void (*set_float)(const char*, float);
    // void (*set_int_array)(const char*, int*)

    void (*delete)();
} Shader;

void shader_link(const char* vertex_path, const char* fragment_path) {
    char* vertex_code;
    char* fragment_code;
    FILE* vertex_file;
    FILE* fragment_file;

    // Get vertex file
    vertex_file = fopen(vertex_path, "r");
    if (vertex_file == NULL) {
        printf("ERROR::SHADER::FILE_NOT_FOUND: %s", vertex_path);
        return;
    }
    fseek(vertex_file, 0, SEEK_END);
    long vertex_length = ftell(vertex_file);
    fseek(vertex_file, 0, SEEK_SET);
    vertex_code = (char*)malloc(vertex_length + 1);
    fread(vertex_code, 1, vertex_length, vertex_file);
    vertex_code[vertex_length] = '\0';
    fclose(vertex_file);

    // Get fragment file
    fragment_file = fopen(fragment_path, "r");
    if (fragment_file == NULL) {
        printf("ERROR::SHADER::FILE_NOT_FOUND: %s", fragment_path);
        return;
    }
    fseek(fragment_file, 0, SEEK_END);
    long fragment_length = ftell(fragment_file);
    fseek(fragment_file, 0, SEEK_SET);
    fragment_code = (char*)malloc(fragment_length + 1);
    fread(fragment_code, 1, fragment_length, fragment_file);
    fragment_code[fragment_length] = '\0';
    fclose(fragment_file);

    // Compiole vertex shader and stuff
    const char* vertex_shader_code = vertex_code;
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_code, NULL);
    glCompileShader(vertex_shader);
    checkCompileErrors(vertex_shader, "VERTEX");

    const char* fragment_shader_code = fragment_code;
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_code, NULL);
    glCompileShader(fragment_shader);
    checkCompileErrors(fragment_shader, "FRAGMENT");

    ID = glCreateProgram();
    glAttachShader(ID, vertex_shader);
    glAttachShader(ID, fragment_shader);
    glLinkProgram(ID);
    checkCompileErrors(ID, "PROGRAM");


    // Clean up
    free(vertex_code);
    free(fragment_code);

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
}

void shader_use() {
    glUseProgram(ID);
}

void shader_set_bool(const char* name, int value) {
    glUniform1i(glGetUniformLocation(ID, name), value); 
}

void shader_set_int(const char* name, int value) {
    glUniform1i(glGetUniformLocation(ID, name), value); 
}

void shader_set_float(const char* name, float value) {
    glUniform1f(glGetUniformLocation(ID, name), value);
}

// void shader_set_int_array(const char* name, int* value) {

// }

void shader_delete() {
    glDeleteProgram(ID);
}

Shader init_shader(const char* vertex_path, const char* fragment_path) {
    Shader s;

    s.link = shader_link;
    s.use = shader_use;

    s.set_bool = shader_set_bool;
    s.set_float = shader_set_float;
    s.set_int = shader_set_int;

    s.delete = shader_delete;


    // Starting shader functions
    s.link(vertex_path, fragment_path);

    return s;
}

#endif