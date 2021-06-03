#version 330 core
in vec3 v_normal;
in vec3 v_pos;
in float v_height;
flat in int water;
out vec3 color;

void main()
{
	vec3 n = normalize(v_normal);
	vec3 lp=vec3(-10,-10,-10);
	vec3 ld = normalize(v_pos - lp);
	float diffuse = dot(n,ld);

	color = vec3(.98, .99, 1);

	if(v_height < 3.1) {
		color = vec3(.5, .5, .7);
	}

	if(v_height < 3.08) {
		color = vec3(.3, .9, .4);
	}

	if(v_height < 3.01) {
		color = vec3(.941, .768, .47);
	}

	if(v_height < 3) {
		//float w_fac = (3 - v_height)/(.08);
		//w_fac = pow(w_fac, 2);
		//color = mix(vec3(.560700001, .890200001, .866700001), vec3(.184300001, .607800001, .929400001), w_fac);
		color = vec3(.560700001, .890200001, .866700001);
	}
	if(v_height < 2.98) {
		color = vec3(.184300001, .607800001, .929400001);
	}

	color = color * (diffuse *.5 + .5);
}