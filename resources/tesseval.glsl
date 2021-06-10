#version 410 core

layout (quads, equal_spacing) in;	//equal_spacing
//in vec2 TE_vertex_tex[];
//in vec3 color[];
in vec3 tcPosition[];
uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
uniform mat4 rotation;
//uniform sampler2D tex;
uniform vec3 campos;
uniform float InterpFract;
uniform vec4 pParam; 
//out vec3 vertex_color[];
//out vec2 frag_tex;
out vec3 v_normal;
out vec3 v_pos;
out float v_height;
out float v_climate;
out vec2 v_wind;
out float v_precipitation;
flat out int water;

#define octaves 20
#define baseFrequency .55
#define baseAmplitude .08
#define roughness 3
#define persistence .5
#define center vec3(0, 0, 0)
#define PI 3.14159265

float snoise(vec3 v, out vec3 grad);

vec3 convertPosition(vec4 v) {
    return v.xyz / v.w;
}

float height(vec3 loc, out vec3 normal) {
    float noiseValue = 0.0;

    float frequency = baseFrequency;
    float amplitude = baseAmplitude;
    float weight = 1;

    for(int i=0; i < octaves; i++)
    {
        vec3 grad;
        float v = snoise(loc * frequency + center, grad);// Sum weighted noise value
        
        noiseValue += v * amplitude;
        vec3 h =  grad - dot(grad, loc) * loc;
        normal += loc - amplitude * h;
        frequency *= roughness;
        amplitude *= persistence;
     }

    return noiseValue;
}

vec3 rotateY(vec3 v, float angle) {
    return vec3(v.x*cos(angle)+v.z*sin(angle), v.y, -v.x*sin(angle) + v.z*cos(angle));
}
vec3 rotateZ(vec3 v, float angle) {
    return vec3(v.x, v.y*cos(angle)-v.z*sin(angle), v.y*sin(angle) + v.z*cos(angle));
}

void main()
{
// interpolate in horizontal direction between vert. 0 and 3
    vec3 p0 = mix(convertPosition(gl_in[0].gl_Position), convertPosition(gl_in[3].gl_Position), gl_TessCoord.x);
    // interpolate in horizontal direction between vert. 1 and 2
    vec3 p1 = mix(convertPosition(gl_in[1].gl_Position), convertPosition(gl_in[2].gl_Position), gl_TessCoord.x);
    // interpolate in vert direction
    vec3 pos = mix(p0, p1, gl_TessCoord.y);

    vec3 x = normalize(pos);

    float r = 3;
    float s = 1;
    float base = .02;
    vec3 grad;
    float hVal = height(x, grad);
    vec3 p = x * r * (hVal + 1 - base);

    // climate
    v_climate = clamp(1 - pow(abs(x.y), .8) - hVal*3*pow(1-abs(x.y), 1), 0, 1);
    
    // wind
    int spinDirection = 1; // 1 for west to east -1 for east to west
    v_wind = vec2(spinDirection * -cos(((6*PI)/4) * abs(x.y)), (x.y > 0 ? -1 : 1) * .75*sin(2.4*PI*abs(x.y) - .3)-.25);

    // Precipitation
    vec3 grad2;
    vec3 xPrecip = rotateZ(rotateY(x, v_wind.x*-.2), v_wind.y*.2);
    float hValPrecip = height(xPrecip, grad);
    v_precipitation = clamp(.6*clamp(pow(abs(v_wind.x), 1) + 1.3*(v_climate-.5), 0, 1) + .8*((hVal-hValPrecip)/baseAmplitude), 0, 1) * .9;

    //vec3 h =  grad - dot(grad, x) * x;
    //v_normal = x - s * h;
	v_normal = grad;
    v_height = abs(length(p));

    if(v_height < r) {
        water = 1;
        gl_Position = P * V * rotation * vec4(x * r, 1);
        // Cube
        //gl_Position = P * V * rotation * vec4(pos, 1);
    } else {
        water = 0;
        gl_Position = P * V * rotation * vec4(p, 1);
        // Cube
        //gl_Position = P * V * rotation * vec4(pos, 1);
    }

    v_pos = convertPosition(gl_Position);
}

vec3 mod289(vec3 x) {
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec4 mod289(vec4 x) {
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec4 permute(vec4 x) {
     return mod289(((x*34.0)+1.0)*x);
}

vec4 taylorInvSqrt(vec4 r)
{
  return 1.79284291400159 - 0.85373472095314 * r;
}

float snoise(vec3 v, out vec3 grad)
{ 
    const vec2  C = vec2(1.0/6.0, 1.0/3.0) ;
    const vec4  D = vec4(0.0, 0.5, 1.0, 2.0);

    // First corner
    vec3 i  = floor(v + dot(v, C.yyy) );
    vec3 x0 =   v - i + dot(i, C.xxx) ;

    // Other corners
    vec3 g = step(x0.yzx, x0.xyz);
    vec3 l = 1.0 - g;
    vec3 i1 = min( g.xyz, l.zxy );
    vec3 i2 = max( g.xyz, l.zxy );

    //   x0 = x0 - 0.0 + 0.0 * C.xxx;
    //   x1 = x0 - i1  + 1.0 * C.xxx;
    //   x2 = x0 - i2  + 2.0 * C.xxx;
    //   x3 = x0 - 1.0 + 3.0 * C.xxx;
    vec3 x1 = x0 - i1 + C.xxx;
    vec3 x2 = x0 - i2 + C.yyy; // 2.0*C.x = 1/3 = C.y
    vec3 x3 = x0 - D.yyy;      // -1.0+3.0*C.x = -0.5 = -D.y

    // Permutations
    i = mod289(i); 
    vec4 p = permute( permute( permute( 
                i.z + vec4(0.0, i1.z, i2.z, 1.0 ))
            + i.y + vec4(0.0, i1.y, i2.y, 1.0 )) 
            + i.x + vec4(0.0, i1.x, i2.x, 1.0 ));

    // Gradients: 7x7 points over a square, mapped onto an octahedron.
    // The ring size 17*17 = 289 is close to a multiple of 49 (49*6 = 294)
    float n_ = 0.142857142857; // 1.0/7.0
    vec3  ns = n_ * D.wyz - D.xzx;

    vec4 j = p - 49.0 * floor(p * ns.z * ns.z);  //  mod(p,7*7)

    vec4 x_ = floor(j * ns.z);
    vec4 y_ = floor(j - 7.0 * x_ );    // mod(j,N)

    vec4 x = x_ *ns.x + ns.yyyy;
    vec4 y = y_ *ns.x + ns.yyyy;
    vec4 h = 1.0 - abs(x) - abs(y);

    vec4 b0 = vec4( x.xy, y.xy );
    vec4 b1 = vec4( x.zw, y.zw );

    //vec4 s0 = vec4(lessThan(b0,0.0))*2.0 - 1.0;
    //vec4 s1 = vec4(lessThan(b1,0.0))*2.0 - 1.0;
    vec4 s0 = floor(b0)*2.0 + 1.0;
    vec4 s1 = floor(b1)*2.0 + 1.0;
    vec4 sh = -step(h, vec4(0.0));

    vec4 a0 = b0.xzyw + s0.xzyw*sh.xxyy ;
    vec4 a1 = b1.xzyw + s1.xzyw*sh.zzww ;

    vec3 p0 = vec3(a0.xy,h.x);
    vec3 p1 = vec3(a0.zw,h.y);
    vec3 p2 = vec3(a1.xy,h.z);
    vec3 p3 = vec3(a1.zw,h.w);

    //Normalise gradients
    vec4 norm = taylorInvSqrt(vec4(dot(p0,p0), dot(p1,p1), dot(p2, p2), dot(p3,p3)));
    p0 *= norm.x;
    p1 *= norm.y;
    p2 *= norm.z;
    p3 *= norm.w;

    // Mix final noise value
    vec4 m = max(0.6 - vec4(dot(x0,x0), dot(x1,x1), dot(x2,x2), dot(x3,x3)), 0.0);
    vec4 m2 = m * m;
    vec4 m4 = m2 * m2;

    vec4 pdotx = vec4(dot(p0,x0), dot(p1,x1), dot(p2,x2), dot(p3,x3));

    vec4 temp = m2 * m * pdotx;
    grad = -8.0 * (temp.x * x0 + temp.y * x1 + temp.z * x2 + temp.w * x3);
    grad += m4.x * p0 + m4.y * p1 + m4.z * p2 + m4.w * p3;
    grad *= 42.0;

    return 42.0 * dot(m4, pdotx);
}