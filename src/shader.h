#ifndef SHADERS_UTILS_H
#define SHADERS_UTILS_H

#include "opengl_includes.h"
#include <linux/limits.h>

typedef struct shader {

  GLuint id;
  GLuint vertex_id;
  GLuint fragment_id;
  GLchar vertex_filename[PATH_MAX];
  GLchar fragment_filename[PATH_MAX];

} shader;

shader *shader_init(const char *vs, const char *fs);
void shader_print_log(GLuint object);
GLuint shader_create(const char *filename, GLenum type);
GLuint shader_program_create(shader *s);

#endif
