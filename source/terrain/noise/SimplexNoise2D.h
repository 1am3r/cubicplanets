#include "stdafx.h"

#include "noise2d.h"
#include "GradientNoise2D.h"

#ifndef _SIMPLEXNOISE2D_H_
#define _SIMPLEXNOISE2D_H_

class SimplexNoise2D :
	public Noise2D
{
public:
	SimplexNoise2D(RNGType& randState);
	virtual ~SimplexNoise2D(void);

	virtual double noise(double xin, double yin, uint32_t seed);

private:
	Noise2D* mNoise;
	
	double mFrequency;
	double mLacunarity;
	double mPersistance;
	uint8_t mNumOctaves;
	double mOctaveScale;

	void calcOctaveScale();
};

#endif // #ifndef _SIMPLEXNOISE2D_H_
