#include "noise.h"
#include <iostream>
#include <random>

float clamp(float x, float min, float max)
{
	return x < min ? min : x > max ? max : x;
}

float max(float x, float y)
{
	return x > y ? x : y;
}

float min(float x, float y)
{
	return x < y ? x : y;
}

float smoothstep(float t) 
{
	return t * t * (3.0f - 2.0f * t); 
}

float sinestep(float t) 
{
	return sinf(1.57079632679f * (2.0f * t - 1.0f)) * 0.5f + 0.5f; 
}

float lerp(float a, float b, float t) 
{
	return a + (b - a) * t; 
}

float noise1f(int x)
{
	x = (x<<13) ^ x;
    return ( 1.0 - ( (x * (x * x * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0f);
}

float noise2f(int x, int y)
{
	int n = x + y * 57;
	n = (n<<13) ^ n;
    return ( 1.0 - ( (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0f);
}

// http://en.wikipedia.org/wiki/Xorshift
// Returns a random number with a period of 2^128 - 1
unsigned int xor128()
{
	static unsigned int x = 123456789;
	static unsigned int y = 362436069;
	static unsigned int z = 521288629;
	static unsigned int w = 88675123;
	unsigned int t;

	t = x ^ (x << 11);
	x = y; y = z; z = w;
	return w = w ^ (w >> 19) ^ (t ^ (t >>8));
}

// Returns a single precision floating-point value uniformly over the interval [0.0, 1.0]
float frand()
{
	return xor128() / float(4294967295.0f);
}

// Returns the smoothed linear interpolation of the noise values containing x
float snoise(float x)
{
	float xi = floor(x);
	float xf = x - xi;

	float h0 = noise1f(xi);
	float h1 = noise1f(xi + 1);

	float t = smoothstep(xf);
	return lerp(h0, h1, t);
}

// Returns the smoothed bilinear interpolation of the noise values in the square containing (x, y)
float snoise(float x, float y)
{
	float xi = floor(x);
	float yi = floor(y);
	float xf = x - xi;
	float yf = y - yi;

	float h00 = noise2f(xi, yi);
	float h10 = noise2f(xi + 1, yi);
	float h01 = noise2f(xi, yi + 1);
	float h11 = noise2f(xi + 1, yi + 1);

	float u = smoothstep(xf);
	float v = smoothstep(yf);

	float x0 = lerp(h00, h10, u);
	float x1 = lerp(h01, h11, u);

	return lerp(x0, x1, v);
}

// A 1-dimensional fractional brownian motion function (noise in layers)
float fBm(float x)
{
	float max = 0.0f;
	float sum = 0.0f;
	float amplitude = 1.0f;
	float frequency = 1.0f;
	for(int i = 0; i < 5; ++i)
	{
		max += amplitude;
		sum += snoise(x * frequency) * amplitude;
		frequency *= 2.0f;
		amplitude *= 0.5f;
	}

	// Normalize
	return sum / max;
}

// A 2-dimensional fractional brownian motion function (noise in layers)
float fBm(float x, float y)
{
	float max = 0.0f;
	float sum = 0.0f;
	float amplitude = 1.0f;
	float frequency = 1.0f;
	for(int i = 0; i < 8; ++i)
	{
		max += amplitude;
		sum += snoise(x * frequency, y * frequency) * amplitude;
		frequency *= 2.0f;
		amplitude *= 0.5f;
	}

	// Normalize
	return sum / max;
}

float fBm(float x, float y, float lacunarity, float gain, int layers)
{
	float max = 0.0f;
	float sum = 0.0f;
	float amplitude = 1.0f;
	float frequency = 1.0f;
	for(int i = 0; i < layers; ++i)
	{
		max += amplitude;
		sum += snoise(x * frequency, y * frequency) * amplitude;
		frequency *= lacunarity;
		amplitude *= gain;
	}

	// Normalize
	return sum / max;
}

float fBm(float x, float y, float lacunarity, float gain, int layers, int seed)
{
	x = x + seed % 256;
	y = y + seed / 256;
	float max = 0.0f;
	float sum = 0.0f;
	float amplitude = 1.0f;
	float frequency = 1.0f;
	for(int i = 0; i < layers; ++i)
	{
		max += amplitude;
		sum += snoise(x * frequency, y * frequency) * amplitude;
		frequency *= lacunarity;
		amplitude *= gain;
	}

	// Normalize
	return sum / max;
}