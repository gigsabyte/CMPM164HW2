#pragma once

#include "Vec3f.h"

enum MaterialType { DIFFUSE_AND_GLOSSY, REFLECTION_AND_REFRACTION, REFLECTION };
class Object
{
public:
	Object() :
		materialType(DIFFUSE_AND_GLOSSY),
		alpha(1.0), reflection(0.0), ior(1.3), Kd(0.8), Ks(0.2), diffuseColor(0.2), emissionColor(0.0), specularExponent(25) {}
	virtual ~Object() {}
	virtual bool intersect(const Vec3f&, const Vec3f&, float&, uint32_t&, Vec3f&) const = 0;
	virtual void getSurfaceProperties(const Vec3f& P, const Vec3f& I, const uint32_t& index, Vec3f& N) const = 0;
	virtual Vec3f evalDiffuseColor(const float&, const float&) const { return diffuseColor; }

	// material properties
	MaterialType materialType;
	
	float Kd, Ks;
	Vec3f diffuseColor;
	Vec3f emissionColor;
	float specularExponent;

	float ior;
	float alpha;
	float reflection;
};

inline bool solveQuadratic(const float& a, const float& b, const float& c, float& x0, float& x1)
{
	float discr = b * b - 4 * a * c;
	if (discr < 0) return false;
	else if (discr == 0) x0 = x1 = -0.5 * b / a;
	else {
		float q = (b > 0) ?
			-0.5 * (b + std::sqrt(discr)) :
			-0.5 * (b - std::sqrt(discr));
		x0 = q / a;
		x1 = c / q;
	}
	if (x0 > x1) std::swap(x0, x1);
	return true;
}

class Light
{
public:
	Light(const Vec3f& p, const Vec3f& i) : position(p), intensity(i), color(1.0, 1.0, 1.0) {}
	Vec3f position;
	Vec3f intensity;
	Vec3f color;
};

