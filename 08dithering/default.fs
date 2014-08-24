#version 150

in vec3 vPosition;
in vec3 vNormal;

uniform vec3 palette;
uniform vec3 lightPos;
uniform sampler2D texDither;

out vec4 outColor;

void main()
{
	vec3 N = normalize(vNormal);
	vec3 L = normalize(lightPos - vPosition);
	float NdotL = max(dot(N, L), 0.0);

	float outIntensity = NdotL;

	// Gamma correct
	outIntensity = sqrt(outIntensity);

	// Clamp to 0, 50 and 100 % with dither levels inbetween
	vec2 ditherCoords = mod(floor(gl_FragCoord.xy), vec2(2.0, 2.0));
	float dither = texture(texDither, ditherCoords).r;
	outIntensity = floor(outIntensity * 4.1) / 4.0;
	float i = mod(outIntensity * 4.0, 2.0);
	outIntensity -= 0.25 * i;
	outIntensity += 0.5 * dither * i;

	// if (outIntensity < 0.25)
	// {
	// 	outIntensity = 0.00;
	// }
	// else if (outIntensity < 0.50)
	// {
	// 	outIntensity = 0.0 + dither * 0.5;
	// }
	// else if (outIntensity < 0.75)
	// {
	// 	outIntensity = 0.5;
	// }
	// else if (outIntensity < 0.99)
	// {
	// 	outIntensity = 0.5 + dither * 0.5;
	// }
	// else
	// {
	// 	outIntensity = 1.00;
	// } 

	// Final monochrome color
	outColor.rgb = palette * outIntensity;
	outColor.a = 1.0;
}