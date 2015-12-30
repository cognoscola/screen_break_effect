//
// Created by alvaregd on 27/11/15.
// Files that assist with loading and creating shader programs into memory
//

#ifndef SIMPLE_LEVEL_EDITOR_SHADER_LOADER_H
#define SIMPLE_LEVEL_EDITOR_SHADER_LOADER_H

#include <GL/glew.h>
#include <stdio.h>
#include <utils/log_utils/logger.h>
#include <string.h>
#include <assert.h>

#define MAX_SHADER_LENGTH 262144

bool parse_file_into_str (const char* file_name, char* shader_str, int max_len);
void print_shader_info_log (GLuint shader_index);
bool create_shader (const char* file_name, GLuint* shader, GLenum type);
bool is_programme_valid (GLuint sp);
bool create_programme (GLuint vert, GLuint frag, GLuint* programme);
/* just use this func to create most shaders; give it vertex and frag files */
GLuint create_programme_from_files (  const char* vert_file_name, const char* frag_file_name);

GLuint create_programme_from_files_and_bind (const char* vert_file_name,const char* frag_file_name,const char* var_name);
bool create_programme_and_bind(GLuint vert, GLuint frag, GLuint* programme, const char* name);


#endif //SIMPLE_LEVEL_EDITOR_SHADER_LOADER_H
