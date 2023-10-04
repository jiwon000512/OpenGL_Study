#version 120                  // GLSL 1.20

attribute vec3 a_position;    // per-vertex position (per-vertex input)
attribute vec3 a_normal;

uniform mat4 u_PVM;

varying vec3 normal;
varying vec3 position;


void main()
{
  gl_Position = u_PVM * vec4(a_position, 1.0f);
  normal = a_normal;
  position = a_position;
}