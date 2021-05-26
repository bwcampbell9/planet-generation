#version 330 core
layout(location = 0) in vec3 vertPos;
layout(location = 1) in vec2 vertTex;

uniform mat4 M;
uniform float resolution;
out vec3 vertex_pos;
out vec2 vertex_tex;

uniform vec3 camoff;


void main()
{
	vertex_pos = vertPos;
	vertex_tex = vertTex;
}