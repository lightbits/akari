#version 140

in vec4 worldPos; // world-space vertex position
in vec3 worldNormal; // world-space normal direction
in float dist;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 ambient;
uniform vec3 fogColor;
uniform float fogDensity;

out vec4 outColor;

void main()
{
    // Diffuse lighting
    vec3 dir_to_light = normalize(lightPos - worldPos.xyz);
    float ndotl = dot(dir_to_light, worldNormal);
    float incidence = max(ndotl, 0.0);
    vec3 diffuse = incidence * lightColor + (1.0 - incidence) * ambient;

    // Gooch lighting
    // https://lva.cg.tuwien.ac.at/ecg/wiki/doku.php?id=students:gooch
    float gooch_factor = 0.5 + ndotl * 0.5;
    vec3 cool = vec3(0.2, 0.2, 0.4);
    vec3 warm = vec3(0.84, 0.6, 0.5);
    float alpha = 0.1;
    float beta = 0.9;
    vec3 cdiff = cool + alpha * diffuse;
    vec3 wdiff = warm + beta * diffuse;
    vec3 gooch = gooch_factor * cdiff + (1.0 - gooch_factor) * wdiff;

    gooch = diffuse;
    vec3 color = gooch;

    float slope = abs(worldNormal.y);
    if (slope < 0.8)
        color -= vec3(0.07, 0.1, 0.10) * 2;
    if (slope > 0.8 && worldPos.y > 0.05)
        color += vec3(0.35) * 0.7;
    if (slope > 0.8 && worldPos.y > -0.04 && worldPos.y < 0.02)
        color *= vec3(0.5, 0.95, 0.55);

	// Fog
    const float LOG2 = 1.442695;
	float h = worldPos.y + 0.5;
	float fogAlpha = 1.0 - clamp(exp2(-fogDensity * fogDensity * LOG2 * (dist * dist + 1.0 / (h * h))), 0.0, 1.0);
    color = mix(color, fogColor, fogAlpha);

    // vignetting
    vec2 q = gl_FragCoord.xy / vec2(720.0, 480.0);
    // q = q * 2.0 - vec2(1.0);
    // outColor *= exp(-pow((q.x * q.x + q.y * q.y), 2.4) * 0.3);
    color *= 0.5 + 0.5 * pow(16.0 * q.x * q.y * (1.0 - q.x) * (1.0 - q.y), 0.3);

	// gamma
    //color = sqrt(color);

    outColor.rgb = color;
    outColor.a = 1.0;

    // Color based on normal
    // outColor = vec4(worldNormal * 0.5 + vec3(0.5), 1.0);
	outColor.rgb *= 0.0001;
	outColor.rgb += vec3(0.8, 0.5, 0.8);
}