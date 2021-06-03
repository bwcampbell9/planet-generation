#version 410 core

layout (triangles, equal_spacing, cw) in;	//equal_spacing
//in vec2 TE_vertex_tex[];
//in vec3 color[];
uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
//uniform sampler2D tex;
uniform vec3 campos;
uniform float InterpFract;
//out vec3 vertex_color[];
//out vec2 frag_tex;
out vec3 v_normal;
out vec3 v_pos;

vec3 convertPosition(vec4 v) {
    return v.xyz / v.w;
}

float height(vec3 loc) {
    return noise1(loc);
}

void main()
{
    vec3 pos = convertPosition(gl_TessCoord.x * gl_in[0].gl_Position +
                gl_TessCoord.y * gl_in[1].gl_Position +
                gl_TessCoord.z * gl_in[2].gl_Position);

	//vec2 Tex = (gl_TessCoord.x * TE_vertex_tex[0] +
    //               gl_TessCoord.y * TE_vertex_tex[1] +
    //               gl_TessCoord.z * TE_vertex_tex[2]);
	//			   
	//frag_tex = Tex;
    //vertex_color = color;
    //vec4 p = vec4(normalize(pos), 1);
    vec4 p = vec4(mix(pos, normalize(pos), InterpFract), 1.0);
    //p = p * vec4(4, 4, 4, 1) + height(pos) * .5;
	
    gl_Position = P * V * p;
    v_pos = convertPosition(gl_Position);
}