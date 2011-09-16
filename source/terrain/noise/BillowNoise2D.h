#include "stdafx.h"

#include "noise2d.h"
#include "GradientNoise2D.h"

#ifndef _BILLOWNOISE2D_H_
#define _BILLOWNOISE2D_H_

class BillowNoise2D :
	public Noise2D
{
public:
	BillowNoise2D(RNGType& randState);
	virtual ~BillowNoise2D(void);

	virtual double noise(double xin, double yin, uint32_t seed);

private:
	Noise2D* mNoise;
	
	double mFrequency;
	double mLacunarity;
	double mPersistance;
	uint8_t mNumOctaves;
};

#endif // #ifndef _BILLOWNOISE2D_H_

