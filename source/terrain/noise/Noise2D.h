#include "stdafx.h"

#ifndef _NOISE2D_H_
#define _NOISE2D_H_

class Noise2D
{
public:
	virtual double noise(double xin, double yin, uint32_t seed) = 0;
	virtual ~Noise2D(void);

protected:
	Noise2D(void);

};

#endif // #ifndef _NOISE2D_H_
