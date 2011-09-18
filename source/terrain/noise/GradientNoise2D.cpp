#include "stdafx.h"

#include "game/randomGen.h"
#include "GradientNoise2D.h"

const double GradientNoise2D::F2 = 0.366025403;	// F2 = 0.5 * (sqrt(3.0) - 1.0)
const double GradientNoise2D::G2 = 0.211324865;	// G2 = (3.0 - sqrt(3.0)) / 6.0

const float GradientNoise2D::grad2[8][2] = {
		{0.707106781F,  0.707106781F}, {-0.707106781F,  0.707106781F},
		{0.707106781F, -0.707106781F}, {-0.707106781F, -0.707106781F},
		{1, 0}, {-1, 0}, {0, 1}, {0,-1}};

GradientNoise2D::GradientNoise2D(RNGType& randState)
{
	for(int i = 0; i < 256; i++) {
		perm[i] = static_cast<uint8_t> (i);
	}

	shuffle rand(randState);
	std::random_shuffle(perm, perm + 256, rand);
}

GradientNoise2D::~GradientNoise2D(void)
{
}

// 2D simplex noise
double GradientNoise2D::noise(double xin, double yin, uint32_t seed)
{
	double noiseEdge0, noiseEdge1, noiseEdge2; // Noise contributions from the three corners
	
	double skew = (xin + yin) * F2;	// Hairy factor for 2D
	int32_t i = fastfloor(xin + skew);
	int32_t j = fastfloor(yin + skew);
	
	double t = (i + j) * G2;
	double X0 = i - t;		// Unskew the cell origin back to (x,y) space
	double Y0 = j - t;
	double x0 = xin - X0;	// The x,y distances from the cell origin
	double y0 = yin - Y0;
	
	// For the 2D case, the simplex shape is an equilateral triangle.
	// Determine which simplex we are in.
	uint8_t i1, j1;				// Offsets for second (middle) corner of simplex in (i,j) coords
	if (x0 > y0) {
		i1 = 1;				// lower triangle, XY order: (0,0)->(1,0)->(1,1)
		j1 = 0;
	} else {
		i1 = 0;				// upper triangle, YX order: (0,0)->(0,1)->(1,1)
		j1 = 1;
	}
	
	// A step of (1,0) in (i,j) means a step of (1-c,-c) in (x,y), and
	// a step of (0,1) in (i,j) means a step of (-c,1-c) in (x,y), where
	// c = (3-sqrt(3))/6
	
	double x1 = x0 - i1 + G2; // Offsets for middle corner in (x,y) unskewed coords
	double y1 = y0 - j1 + G2;
	double x2 = x0 - 1.0 + 2.0 * G2; // Offsets for last corner in (x,y) unskewed coords
	double y2 = y0 - 1.0 + 2.0 * G2;
	
	// Work out the hashed gradient indices of the three simplex corners
	uint8_t ii = static_cast<uint8_t> (i + seed * 1013);
	uint8_t jj = static_cast<uint8_t> (j);
	uint8_t gi0 = perm[(ii      + perm[jj]) & 255]              & 7;
	uint8_t gi1 = perm[(ii + i1 + perm[(jj + j1) & 255]) & 255] & 7;
	uint8_t gi2 = perm[(ii +  1 + perm[(jj +  1) & 255]) & 255] & 7;
	
	// Calculate the contribution from the three corners
	double t0 = 0.5 - x0 * x0 - y0 * y0;
	if (t0 < 0) {
		noiseEdge0 = 0.0;
	} else {
		t0 *= t0;
		noiseEdge0  = t0 * t0 * dot(grad2[gi0], x0, y0); // (x,y) of grad3 used for 2D gradient
	}
	
	double t1 = 0.5 - x1 * x1 - y1 * y1;
	if (t1 < 0) {
		noiseEdge1 = 0.0;
	} else {
		t1 *= t1;
		noiseEdge1  = t1 * t1 * dot(grad2[gi1], x1, y1);
	}
	
	double t2 = 0.5 - x2 * x2 - y2 * y2;
	if (t2 < 0) {
		noiseEdge2 = 0.0;
	}else {
		t2 *= t2;
		noiseEdge2  = t2 * t2 * dot(grad2[gi2], x2, y2);
	}
	
	// Add contributions from each corner to get the final noise value.
	// The result is scaled to return values in the interval [-1,1].
	return 70.0 * (noiseEdge0 + noiseEdge1 + noiseEdge2);
}
