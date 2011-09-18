
#include <cstdint>

#include "game/randomGen.h"
#include "noise2d.h"

#ifndef _RIDGEDMULTIFRACTALNOISE2D_H_
#define _RIDGEDMULTIFRACTALNOISE2D_H_

class RidgedMultifractalNoise2D :
	public Noise2D
{
public:
	RidgedMultifractalNoise2D(RNGType& randState);
	virtual ~RidgedMultifractalNoise2D(void);

	virtual double noise(double xin, double yin, uint32_t seed);

private:
	Noise2D* mNoise;

	double mOffset;
	double gain;
	uint8_t mNumOctaves;

	double* mOctaveExponents;
	double mOctaveScale;
	double mH;
	double mFrequency;
	double mLacunarity;

	void calcOctaveExponents();
};

#endif // #ifndef _RIDGEDMULTIFRACTALNOISE2D_H_
