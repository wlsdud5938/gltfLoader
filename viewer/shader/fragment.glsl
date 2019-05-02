#version 120                  // GLSL 1.20

varying vec3 v_position;
varying vec3 v_color;

uniform sampler2D tex;
uniform vec3 sun_position;
uniform vec3 sun_color;

void main()
{
	gl_FragColor =vec4(v_color, 1.0f);
}