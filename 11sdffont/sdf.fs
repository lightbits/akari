#version 150 core

in vec2 vTexel;
//uniform sampler2D tex;
out vec4 outColor;

const vec3 camForward = vec3(0.0, 0.0, 1.0);
const vec3 camRight = vec3(1.0, 0.0, 0.0);
const vec3 camUp = vec3(0.0, 1.0, 0.0);
const vec3 eye = vec3(0.0, 0.0, -4.0);
const float focalLength = 2.3;
const float aspectRatio = 1.0;
const float rmEps = 0.01;
const int rmSteps = 64;

float sdSphere(vec3 p, float r)
{
	return length(p) - r;
}

float sdScene(vec3 p)
{
	float left = sdSphere(p - vec3(-100.0, 0.0, 0.0), 99.0);
	float floor = sdSphere(p - vec3(0.0, -100.0, 0.0), 99.0);
	float back = sdSphere(p - vec3(0.0, 0.0, 100.0), 99.0);
	float sphere0 = sdSphere(p - vec3(-0.8, -1.0, 0.0), 0.5);
	float sphere1 = sdSphere(p - vec3(-0.6, -0.7, 0.0), 0.4);
	return min(min(min(left, back), floor), min(sphere0, sphere1));
}

bool raymarch(vec3 ro, vec3 rd, out float t)
{
	t = 0.0;
	for (int i = 0; i < rmSteps; i++)
	{
		float dist = sdScene(ro + rd * t);
		if (dist < rmEps)
			return true;
		t += dist;
	}
	return false;
}

float visibility(vec3 p, vec3 w, float d)
{
	float t;
	if (!raymarch(p, w, t))
		return 1.0f;
	else if (t <= d)
		return 0.0f;
	return 1.0f;
}

float ambientOcclusion(vec3 p, vec3 n)
{
	const int aoSamples = 4;
	const float TWO_PI = 6.28318530718;
	const float PI = 3.14159265359;

	// create tangent frame
	vec3 up, right;
	if (abs(n.y) > 0.99)
	{
		right = vec3(1.0, 0.0, 0.0);
		up = normalize(cross(right, n));
		right = normalize(cross(n, up));
	}
	else
	{
		up = vec3(0.0, 1.0, 0.0);
		right = normalize(cross(n, up));
		up = normalize(cross(right, n));
	}

	float ao = 0.0f;
	vec3 ro = p + n * rmEps * 4.0;

	int u_strats = 8;
	int v_strats = 8;
	int n_samples = u_strats * v_strats;
	for (int ui = 0; ui < u_strats; ui++)
	{
		for (int vi = 0; vi < v_strats; vi++)
		{
			float u = (ui) * TWO_PI / float(u_strats);
			float v = (vi) * PI / float(v_strats);
			float r = cos(v);
			float x = r * sin(u);
			float y = sin(v);
			float z = r * cos(u);
			vec3 w = normalize(x * right + y * up + z * n);
			ao += visibility(ro, w, 0.25);
		}
	}

	return ao / float(n_samples);
}

vec3 getNormal(vec3 p)
{
	vec2 eps = vec2(rmEps, 0.0);
	return normalize(vec3(
		sdScene(p + eps.xyy) - sdScene(p - eps.xyy),
		sdScene(p + eps.yxy) - sdScene(p - eps.yxy),
		sdScene(p + eps.yyx) - sdScene(p - eps.yyx)));
}

vec3 computeColor(vec3 ro, vec3 rd)
{
	float t;
	bool hit = raymarch(ro, rd, t);
	if (!hit)
		return vec3(0.0);
	vec3 p = ro + rd * t;
	vec3 n = getNormal(p);
	float ao = ambientOcclusion(p, n);
	//return vec3(ao);
	return (0.5 * n + vec3(0.5)) * ao;
}

void main()
{
	vec2 uv = vTexel * 2.0 - vec2(1.0);

	vec3 ro = eye;
	vec3 rd = normalize(camForward * focalLength + camUp * uv.y + camRight * uv.x * aspectRatio);

	outColor.rgb = computeColor(ro, rd);
	outColor.a = 1.0;

	//float texValue = 1.0 - texture(tex, vTexel).r;
	//float dist = 2.0 * texValue - 1.0;
	//float du = fwidth(vTexel.x);
	//float alpha = smoothstep(-50.0 * du, 50.0 * du, dist);
	//float outline = smoothstep(200.0 * du, 400.0 * du, dist);
	//outColor.a = alpha;
	//outColor.rgb = vec3(outline);
}