#version 120                  // GLSL 1.20

attribute vec3 a_position;
attribute vec3 a_color;

uniform mat4 u_PVM;

varying vec3 v_normal;
varying vec3 v_position;
varying vec3 v_color;
void main()
{
	gl_Position = u_PVM * vec4(a_position, 1);
	v_position = a_position;
	v_color = a_color;
}