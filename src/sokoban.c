#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

#include "shader.h"
#include "mesh.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define torad(x) ((x) * M_PI / 180.0f)
#define todeg(x) ((x) * 180.0f / M_PI)

#define GAME_TITLE "Przygoda kuli"

#define WIN_WIDTH 640
#define WIN_HEIGHT 480

float fov_angle = 90.0f;

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
  (void) window;

  printf("x: %f, y: %f\n", xoffset, yoffset);
  fov_angle += yoffset;

  return;

}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{

  (void) scancode;
  (void) mods;

  if (action != GLFW_REPEAT && action != GLFW_PRESS)
      return;

  switch (key)
  {

      case GLFW_KEY_ESCAPE:

          glfwSetWindowShouldClose(window, GL_TRUE);
          break;

      default:

          break;
  }
}

int main()
{

  if(!glfwInit()) {

    fprintf(stderr, "Error initializing glfw\n");
    return EXIT_FAILURE;

  }

  GLFWwindow *window;
  window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT,
      GAME_TITLE, NULL, NULL);

  if(!window) {

    fprintf(stderr, "Error initializing glfw\n");
    glfwTerminate();
    return EXIT_FAILURE;

  }

  glfwSetKeyCallback(window, key_callback);
  glfwSetScrollCallback(window, scroll_callback);
  glfwMakeContextCurrent(window);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwSwapInterval(2);

  glewExperimental = GL_TRUE;
  glewInit();

  glEnable(GL_DEPTH_TEST);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

  shader *s = shader_init("src/shaders/vertex.glsl", "src/shaders/fragment.glsl");
  shader_program_create(s);
  glUseProgram(s->id);

  float trans = 2.0f;
  float mid_x = trans  / 2;
  float mid_y = trans  / 2;

  kmMat4 proj;

  kmMat4PerspectiveProjection(&proj, fov_angle, (float) WIN_WIDTH / (float) WIN_HEIGHT, 1.0f, 100.0f);
  GLint uniProj = glGetUniformLocation(s->id, "proj");
  glUniformMatrix4fv(uniProj, 1, GL_FALSE, &proj.mat[0]);


  kmVec3 p_eye = { 15.0f, 10.0f, -5.0f};
  kmVec3 p_ctr = { mid_x, 0.0f, mid_y};
  kmVec3 p_up  = { 0.0f, 1.0f, 0.0f };
  kmMat4 view;
  kmMat4LookAt(&view, &p_eye, &p_ctr, &p_up);
  GLint uniView = glGetUniformLocation(s->id, "view");
  glUniformMatrix4fv(uniView, 1, GL_FALSE, &view.mat[0]);

  GLint uniCam = glGetUniformLocation(s->id, "cam_pos");
  glUniform3f(uniCam, p_eye.x, p_eye.y, p_eye.z);

  mesh *block = (mesh *) mesh_create("box.obj");
  mesh_load_shader(block, s);
  mesh_load_mesh(block);
  mesh_load_texture(block, "texture.png");

  double x,y;
  double p_x, p_y;

  kmMat4 yaw;
  kmVec3 Axis;
  kmVec3 Diff;

  float angle_x= 0.0f;
  float sum_angle_x = 0.0f;

  while(!glfwWindowShouldClose(window)) {


    kmMat4PerspectiveProjection(&proj, fov_angle, (float) WIN_WIDTH / (float) WIN_HEIGHT, 1.0f, 100.0f);
    GLint uniProj = glGetUniformLocation(s->id, "proj");
    glUniformMatrix4fv(uniProj, 1, GL_FALSE, &proj.mat[0]);

    kmMat4LookAt(&view, &p_eye, &p_ctr, &p_up);
    GLint uniView = glGetUniformLocation(s->id, "view");
    glUniformMatrix4fv(uniView, 1, GL_FALSE, &view.mat[0]);

    p_x = x;
    p_y = y;
    glfwGetCursorPos(window, &x, &y);

    sum_angle_x += angle_x;

    if(p_x > x)
      angle_x= 0.5f * (p_x - x);
    else if(p_x < x)
      angle_x= -0.5f * (x - p_x);
    else if(p_x == x)
      angle_x= 0.0f;

    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    kmVec3Fill(&Axis, 0.0f, 1.0f, 0.0f);

    kmMat4RotationAxisAngle(&yaw, &Axis, torad(angle_x));
    kmVec3Subtract(&Diff, &p_eye, &p_ctr);
    kmVec3MultiplyMat4(&p_eye, &Diff, &yaw);
    kmVec3Add(&p_eye, &p_eye, &p_ctr);

    kmVec4 x_axis = { 1.0f, 0.0f, 0.0f, 0.0f };
    kmVec4MultiplyMat4(&x_axis, &x_axis, &view);
    kmVec3 right_axis = { x_axis.x, x_axis.y, x_axis.z };
    kmVec3Fill(&right_axis, 1.0f, 0.0f, 0.0f);

    GLint uniCam = glGetUniformLocation(s->id, "cam_pos");
    glUniform3f(uniCam, p_eye.x, p_eye.y, p_eye.z);

          glBindVertexArray(block->vao);
          kmMat4 model;
          kmMat4Identity(&model);
          kmMat4Translation(&model, mid_x, 0, mid_y);
          GLint uniModel = glGetUniformLocation(s->id, "model");
          glUniformMatrix4fv(uniModel, 1, GL_FALSE, &model.mat[0]);
          glDrawArrays(GL_TRIANGLES, 0, block->v_no);

    glfwSwapBuffers(window);

    glfwPollEvents();

  }

  glfwTerminate();

  return EXIT_SUCCESS;
}
