#version 120

attribute vec3 position;
attribute vec2 texcoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

varying vec2 Texcoord;

void main()
{

  Texcoord = texcoord;
  gl_Position = proj * view * model * vec4(position, 1.0);
}
