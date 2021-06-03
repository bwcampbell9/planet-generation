#version 330 core
in vec3 v_normal;
in vec3 v_pos;
out vec3 color;

void main()
{
	vec3 n = normalize(v_normal);
	vec3 lp=vec3(100,10,100);
	vec3 ld = normalize(v_pos - lp);
	float diffuse = dot(n,ld);
	color = vec3(.5, .6, .7);
}