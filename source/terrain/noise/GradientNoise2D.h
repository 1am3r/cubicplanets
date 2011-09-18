
#include <cstdint>

#include "game/randomGen.h"
#include "noise2d.h"

#ifndef _GRADIENTNOISE2D_H_
#define _GRADIENTNOISE2D_H_

class GradientNoise2D :
	public Noise2D
{
public:
	GradientNoise2D(RNGType& randState);
	virtual ~GradientNoise2D(void);

	virtual double noise(double xin, double yin, uint32_t seed);

private:
	static const double F2;	// F2 = 0.5 * (sqrt(3.0) - 1.0)
	static const double G2;	// G2 = (3.0 - sqrt(3.0)) / 6.0

	static const float grad2[8][2];
	uint8_t perm[256];

	static double dot(const float g[], double x, double y) {
		return g[0]*x + g[1]*y;
	};

	static int32_t fastfloor(double value) {
		return (value >= 0) ? (int32_t) value : ((int32_t) value) - 1;
	};
};

#endif // #ifndef _GRADIENTNOISE2D_H_
