#ifndef NOISE_H
#define NOISE_H

float clamp(float x, float min, float max);
float max(float x, float y);
float min(float x, float y);

// Smoothly interpolates a unit step for t between 0 and 1
float smoothstep(float t);

// Smoothly interpolates a unit step for t between 0 and 1
float sinestep(float t);

// Linearly interpolates between a and b using t
float lerp(float a, float b, float t);

// Returns a random floating point value between -1 and 1
float noise1f(int x);

// Returns a random floating point value between -1 and 1
float noise2f(int x, int y);

// Returns a random integer with a period of 2^128 - 1
// http://en.wikipedia.org/wiki/Xorshift
unsigned int xor128();

// Returns a single precision floating-point value uniformly over the interval [0.0, 1.0]
float frand();

// Returns a smoothed linear interpolation of the noise values containing x
float snoise(float x);

// Returns a smoothed bilinear interpolation of the noise values in the square containing (x, y)
float snoise(float x, float y);

// Defines a 1-dimensional fractional brownian motion function (noise in layers)
float fBm(float x);

// Defines a 2-dimensional fractional brownian motion function (noise in layers)
float fBm(float x, float y);

// Defines a 2-dimensional generic fractal sum function
// lacunarity: rate of change of frequency per layer
// gain: rate of change of amplitude per layer
float fBm(float x, float y, float lacunarity, float gain, int layers);

// Defines a 2-dimensional generic fractal sum function
// lacunarity: rate of change of frequency per layer
// gain: rate of change of amplitude per layer
float fBm(float x, float y, float lacunarity, float gain, int layers, int seed);

#endif