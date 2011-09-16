#include "stdafx.h"
#include "BillowNoise2D.h"


BillowNoise2D::BillowNoise2D(RNGType& randState)
	: mNoise(0)
{
	mNoise = new GradientNoise2D(randState);

	mFrequency = 1.0F;
	mLacunarity = 2.0F;
	mPersistance = 0.45F;
	mNumOctaves = 6;
}


BillowNoise2D::~BillowNoise2D(void)
{
	if (mNoise)	delete mNoise;
}


double BillowNoise2D::noise(double xin, double yin, uint32_t seed)
{
	xin *= mFrequency;
	yin *= mFrequency;

	double result = 0.0;
	double curPersistance = 1.0;

	for (int octave = 0; octave <= mNumOctaves; octave++) {
		double signal = mNoise->noise(xin, yin, octave + seed);

		signal = (2.0 * fabs(signal)) - 1.0;

		xin *= mLacunarity;
		yin *= mLacunarity;
		result += signal * curPersistance;
		curPersistance *= mPersistance;
	}

	return (result + 0.6) / 1.3;;
}