#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "shader.h"
#include "file_utils.h"

shader *shader_init(const char *vs, const char *fs) {

  shader *s = malloc(sizeof(shader));
  strcpy(s->fragment_filename, fs);
  strcpy(s->vertex_filename, vs);

  return s;

}

GLuint shader_program_create(shader *s) {
  GLuint vs, fs, shader_program;
  if((vs = shader_create(s->vertex_filename,
          GL_VERTEX_SHADER)) == 0) return EXIT_FAILURE;
  if((fs = shader_create(s->fragment_filename,
          GL_FRAGMENT_SHADER)) == 0) return EXIT_FAILURE;
  shader_program = glCreateProgram();

  if(!shader_program) {
    fprintf(stderr, "Error creating shader shader_program\n");
    return -1;
  }

  glAttachShader(shader_program, vs);
  glAttachShader(shader_program, fs);
  glLinkProgram(shader_program);

  s->id = shader_program;
  return 1;
}


void shader_print_log(GLuint object) {

  GLint log_length = 0;
  if(glIsShader(object))
    glGetShaderiv(object, GL_INFO_LOG_LENGTH, &log_length);
  else if(glIsProgram(object))
    glGetProgramiv(object, GL_INFO_LOG_LENGTH, &log_length);
  else {
    fprintf(stderr, "print_log: Not a shader or a program\n");
    return;
  }

  char *log = (char *) malloc(log_length);

  if(glIsShader(object))
    glGetShaderInfoLog(object, log_length, NULL, log);
  else if(glIsProgram(object))
    glGetProgramInfoLog(object, log_length, NULL, log);

  fprintf(stderr, "%s", log);
  free(log);

}

GLuint shader_create(const char *filename, GLenum type) {

  const GLchar *source = file_read(filename);
  if(source == NULL) {
    fprintf(stderr, "Error opening %s: ", filename);
    perror("");
    return 0;
  }

  GLuint res = glCreateShader(type);
  glShaderSource(res, 1, &source, NULL);
  free((void *) source);

  glCompileShader(res);
  GLint compile_ok = GL_FALSE;
  glGetShaderiv(res, GL_COMPILE_STATUS, &compile_ok);
  if(compile_ok == GL_FALSE) {
    fprintf(stderr, "%s:", filename);
    shader_print_log(res);
    glDeleteShader(res);
    return 0;
  }

  return res;

}
