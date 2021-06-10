#version 330 core
in vec3 v_normal;
in vec3 v_pos;
in float v_height;
in float v_climate;
in vec2 v_wind;
in float v_precipitation;
flat in int water;
out vec3 color;

uniform sampler2D biomeTex;

void main()
{
	vec3 n = normalize(v_normal);
	vec3 lp=vec3(10,10,10);
	vec3 ld = normalize(v_pos - lp);
	float diffuse = dot(n,ld);

	// Simple colors
	/*color = vec3(.98, .99, 1);

	if(v_height < 3.1) {
		color = vec3(.5, .5, .7);
	}

	if(v_height < 3.08) {
		color = vec3(.3, .9, .4);
	}

	if(v_height < 3.01) {
		color = vec3(.941, .768, .47);
	}*/

	// Biome Colors
	color = texture(biomeTex, vec2(clamp(v_climate, 0.00001, 0.999999), clamp(v_precipitation, 0.00001, 0.999999))).xyz;

	if(v_height < 3) {
		//float w_fac = (3 - v_height)/(.08);
		//w_fac = pow(w_fac, 2);
		//color = mix(vec3(.560700001, .890200001, .866700001), vec3(.184300001, .607800001, .929400001), w_fac);
		color = vec3(.560700001, .890200001, .866700001);
	} else {
	}
	if(v_height < 2.98) {
		color = vec3(.184300001, .607800001, .929400001);
	}

	color = color * (diffuse *.5 + .5);

	// Show climate
	//color = vec3(v_climate, 0, 1-v_climate);
	// Show wind
	//color = vec3(clamp(-v_wind.x, 0, 1), clamp(v_wind.x, 0, 1), (v_wind.y + 1)/2);
	// Show precipitation
	//color = vec3(1-v_precipitation, 0, v_precipitation);
	//color = vec3(clamp(v_climate, 0.00001, 0.999999), clamp(v_precipitation, 0.00001, 0.999999), 0);
}