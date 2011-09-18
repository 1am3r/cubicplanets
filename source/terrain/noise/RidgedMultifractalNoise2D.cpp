#include "stdafx.h"

#include "game/randomGen.h"
#include "RidgedMultifractalNoise2D.h"
#include "GradientNoise2D.h"

RidgedMultifractalNoise2D::RidgedMultifractalNoise2D(RNGType& randState)
	: mNoise(0)
{
	mNoise = new GradientNoise2D(randState);

	mOffset = 1.0;
	gain = 2.0;
	mNumOctaves = 6;

	mH = 1.0;
	mFrequency = 1.0;
	mLacunarity = 2.0;

	mOctaveExponents = 0;

	calcOctaveExponents();
}


RidgedMultifractalNoise2D::~RidgedMultifractalNoise2D(void)
{
	if (mOctaveExponents) {
		delete[] mOctaveExponents;
	}

	if (mNoise) {
		delete mNoise;
	}
}


void RidgedMultifractalNoise2D::calcOctaveExponents()
{
	if (mOctaveExponents) {
		delete[] mOctaveExponents;
	}

	mOctaveExponents = new double[mNumOctaves + 1];

	mOctaveScale = 0.0;
	double tempFreq = mFrequency;
	for (int i = 0; i <= mNumOctaves; i++) {
		mOctaveExponents[i] = std::pow(tempFreq, -mH);
		//mOctaveScale += mOctaveExponents[i];
		tempFreq *= mLacunarity;
	}
}

double RidgedMultifractalNoise2D::noise(double xin, double yin, uint32_t seed)
{
	xin *= mFrequency;
	yin *= mFrequency;

	double result = 0.0;
	double weight = 1.0;
	double signal = 0.0;

	for (int octave = 0; octave <= mNumOctaves; octave++) {
		signal = mNoise->noise(xin, yin, octave + seed);

		if (signal < 0.0) signal = -signal;
		signal = mOffset - signal;

		signal *= signal;

		signal *= weight;

		weight = signal * gain;
		if (weight > 1.0) {
			weight = 1.0;
		} else if (weight < 0.0) {
			weight = 0.0;
		}

		result += signal * mOctaveExponents[octave];

		xin *= mLacunarity;
		yin *= mLacunarity;
	}

	//return result / mOctaveScale;
	//return (result *1.05) - 1.0;
	return result - 1.0;
}
