#version 330 core
layout(location = 0) in vec3 vertPos;
layout(location = 1) in vec3 vertColor;

uniform mat4 M;
uniform mat4 P;
uniform mat4 V;
uniform float resolution;

uniform vec3 camoff;


void main()
{
	gl_Position = M * vec4(vertPos, 1.0);
}