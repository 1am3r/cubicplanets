#include <cstdint>
#include <iostream>
#include <string>


#ifndef _PREREQUISITES_H_
#define _PREREQUISITES_H_

typedef int32_t wCoord;
#define WCOORD_MIN INT32_MIN
#define WCOORD_MAX INT32_MAX

static inline wCoord sign(wCoord val)
{
	return static_cast<wCoord>((val >= 0) - (val <= 0));
};

static inline size_t positiveMod(wCoord val, wCoord mod)
{
	val %= mod;
	return (val < 0 ? val + mod : val);
}

struct Point3 {
	int32_t x;
	int32_t y;
	int32_t z;

	Point3() : x(0), y(0), z(0) {};

	Point3(int32_t xVal, int32_t yVal, int32_t zVal)
		: x(xVal), y(yVal), z(zVal) {};

	Point3 operator+(const Point3& right) const
	{
		Point3 result;
		result.x = this->x + right.x;
		result.y = this->y + right.y;
		result.z = this->z + right.z;
		
		return result;
	};

	bool operator==(const Point3& right) const
	{
		return ((this->x == right.x) && (this->y == right.y) && (this->z == right.z));
	};

	bool operator<(const Point3& right) const
	{
		return ((this->x < right.x) && (this->y < right.y) && (this->z < right.z));
	}
};

#endif // #ifndef _PREREQUISITES_H_