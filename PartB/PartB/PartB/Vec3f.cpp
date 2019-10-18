#include "Vec3f.h"

Vec3f normalize(const Vec3f& v)
{
	float mag2 = (v.x * v.x) + (v.y * v.y) + (v.z * v.z);
	if (mag2 > 0) {
		float invMag = sqrtf(mag2);
		return Vec3f(v.x / invMag, v.y / invMag, v.z / invMag);
	}

	return v;
}

float dotProduct(const Vec3f& a, const Vec3f& b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vec3f crossProduct(const Vec3f& a, const Vec3f& b)
{
	return Vec3f(
		a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.x
	);
}
