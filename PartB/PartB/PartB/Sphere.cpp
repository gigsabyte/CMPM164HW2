#include "Sphere.h"

bool Sphere::intersect(const Vec3f& orig, const Vec3f& dir, float& tnear, uint32_t& index, Vec3f& uv) const
{
	// analytic solution
	Vec3f L = orig - center;
	float a = dotProduct(dir, dir);
	float b = 2 * dotProduct(dir, L);
	float c = dotProduct(L, L) - radius2;
	float t0, t1;
	if (!solveQuadratic(a, b, c, t0, t1)) return false;
	if (t0 < 0) t0 = t1;
	if (t0 < 0) return false;
	tnear = t0;

	return true;
}

void Sphere::getSurfaceProperties(const Vec3f& P, const Vec3f& I, const uint32_t& index, Vec3f& N) const
{
	N = normalize(P - center);
}
