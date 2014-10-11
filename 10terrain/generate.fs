#version 140

in vec2 texel;
uniform float time;

out vec4 outColor;

float hash(vec3 x)
{
	int n = int(127.0 * (x.x + x.y + x.z + x.x * x.y + x.x * x.z + x.y * x.z + x.z * x.z));
	n = (n << 13) ^ n;
	return (1.0 - ((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);
}

vec4 dnoise3f(vec3 x)
{
	vec3 xi = floor(x);
	vec3 xf = fract(x);
	vec3 dr = 30.0 * xf * xf * (xf * (xf - vec3(2.0)) + vec3(1.0));

	xf = xf * xf * xf * (xf * (xf * vec3(6.0) - vec3(15.0)) + vec3(10.0));

	vec2 E = vec2(1.0, 0.0);
    float a = hash(x + E.yyy);
    float b = hash(x + E.xyy);
    float c = hash(x + E.yxy);
    float d = hash(x + E.xxy);
    float e = hash(x + E.yyx);
    float f = hash(x + E.xyx);
    float g = hash(x + E.yxx);
    float h = hash(x + E.xxx);

    float k0 =   a;
    float k1 =   b - a;
    float k2 =   c - a;
    float k3 =   e - a;
    float k4 =   a - b - c + d;
    float k5 =   a - c - e + g;
    float k6 =   a - b - e + f;
    float k7 = - a + b + c - d + e - f - g + h;

	float n = k0 + k1*dr.x + k2*dr.y + k3*dr.z + k4*dr.x*dr.y + k5*dr.y*dr.z + k6*dr.z*dr.x + k7*dr.x*dr.y*dr.z;
	float dndx = dr.x * (k1 + k4 * dr.y + k6 * dr.z + k7 * dr.y * dr.z);
	float dndy = dr.y * (k2 + k5 * dr.z + k4 * dr.x + k7 * dr.z * dr.x);
	float dndz = dr.z * (k3 + k6 * dr.x + k5 * dr.y + k7 * dr.x * dr.y);
	return vec4(n, dndx, dndy, dndz);
}

float fbm(vec3 p)
{
	float f = 0.0;
    float w = 0.5;
    float dx = 0.0;
    float dz = 0.0;
	p *= 100.0;
    for( int i=0; i < 2 ; i++ )
    {
		vec4 n = dnoise3f(p);
		dx += n.y;
		dz += n.z;
        f += w * n.x / (1.0 + dx*dx + dz*dz); // replace with "w * n[0]" for a classic fbm()
		w *= 0.5;
		p *= 2.0;
    }
	return f;
}

void main()
{
	float r = sqrt(dot(texel, texel));
	float h = 0.3 * exp(-2.0 * r);
	h = cos(100.0 * h + 0.1 * time);
	outColor = vec4(h);
	// float r = sqrt(dot(texel, texel));
	// float h = exp(-2.0 * r);
	// vec3 p = vec3(texel, 0.0);
	// h += 0.5 * fbm(p);
    outColor = vec4(h, 0.0, 0.0, 0.0);
}