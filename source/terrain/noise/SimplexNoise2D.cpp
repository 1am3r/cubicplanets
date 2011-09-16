#include "stdafx.h"
#include "SimplexNoise2D.h"


SimplexNoise2D::SimplexNoise2D(RNGType& randState)
	: mNoise(0)
{
	mNoise = new GradientNoise2D(randState);

	mFrequency = 1.0F;
	mLacunarity = 1.5F;
	mPersistance = 0.45F;
	mNumOctaves = 6;
	mOctaveScale = 0;

	calcOctaveScale();
}


SimplexNoise2D::~SimplexNoise2D(void)
{
	if (mNoise) delete mNoise;
}


void SimplexNoise2D::calcOctaveScale()
{
	//mOctaveScale = 0;
	//double tempPersistance = 1.0F;
	//for (int octave = 0; octave <= mNumOctaves; octave++) {
	//	mOctaveScale += tempPersistance;
	//	tempPersistance *= mPersistance;
	//}
}

double SimplexNoise2D::noise(double xin, double yin, uint32_t seed)
{
	xin *= mFrequency;
	yin *= mFrequency;

	double result = 0.0;
	double curPersistance = 1.0;

	for (int octave = 0; octave <= mNumOctaves; octave++) {
		double signal = mNoise->noise(xin, yin, octave + seed);

		xin *= mLacunarity;
		yin *= mLacunarity;
		result += signal * curPersistance;
		curPersistance *= mPersistance;
	}

	//result /= mOctaveScale;

	return result / 1.25;
}
