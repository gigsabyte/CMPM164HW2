#pragma once
#include <cstdio> 
#include <cstdlib> 
#include <memory> 
#include <vector> 
#include <utility> 
#include <cstdint> 
#include <iostream> 
#include <fstream> 
#include <cmath> 
#include <algorithm>

const float kInfinity = std::numeric_limits<float>::max();

class Vec3f {
public:
	Vec3f() : x(0), y(0), z(0) {}
	Vec3f(float xx) : x(xx), y(xx), z(xx) {}
	Vec3f(float xx, float yy, float zz) : x(xx), y(yy), z(zz) {}
	Vec3f operator * (const float& r) const { return Vec3f(x * r, y * r, z * r); }
	Vec3f operator * (const Vec3f& v) const { return Vec3f(x * v.x, y * v.y, z * v.z); }
	Vec3f operator - (const Vec3f& v) const { return Vec3f(x - v.x, y - v.y, z - v.z); }
	Vec3f operator + (const Vec3f& v) const { return Vec3f(x + v.x, y + v.y, z + v.z); }
	Vec3f operator - () const { return Vec3f(-x, -y, -z); }
	Vec3f& operator += (const Vec3f& v) { x += v.x, y += v.y, z += v.z; return *this; }
	friend Vec3f operator * (const float& r, const Vec3f& v) {
		return Vec3f(v.x * r, v.y * r, v.z * r);
	}
	friend std::ostream& operator << (std::ostream& os, const Vec3f& v) {
		return os << v.x << ", " << v.y << ", " << v.z;
	}
	float x, y, z;
};

Vec3f normalize(const Vec3f& v);

float dotProduct(const Vec3f& a, const Vec3f& b);
Vec3f crossProduct(const Vec3f& a, const Vec3f& b);

inline Vec3f mix(const Vec3f& a, const Vec3f& b, const float& mixValue)
{
	return a * (1 - mixValue) + b * mixValue;
}

inline Vec3f reflect(const Vec3f& I, const Vec3f& N)
{
	return I - 2 * dotProduct(I, N) * N;
}

inline float clampf(const float& lo, const float& hi, const float& v) {
	return std::max(lo, std::min(hi, v));
}

inline float deg2rad(const float& deg) { return deg * std::atan(1.0) * 4 / 180; }

inline Vec3f refract(const Vec3f& I, const Vec3f& N, const float& ior)
{
	float cosi = clampf(-1, 1, dotProduct(I, N));
	float etai = 1, etat = ior;
	Vec3f n = N;
	if (cosi < 0) { cosi = -cosi; }
	else { std::swap(etai, etat); n = -N; }
	float eta = etai / etat;
	float k = 1 - eta * eta * (1 - cosi * cosi);
	return k < 0 ? 0 : eta * I + (eta * cosi - sqrtf(k)) * n;
}

inline void fresnel(const Vec3f& I, const Vec3f& N, const float& ior, float& kr)
{
	float cosi = clampf(-1, 1, dotProduct(I, N));
	float etai = 1, etat = ior;
	if (cosi > 0) { std::swap(etai, etat); }
	// Compute sini using Snell's law
	float sint = etai / etat * sqrtf(std::max(0.f, 1 - cosi * cosi));
	// Total internal reflection
	if (sint >= 1) {
		kr = 1;
	}
	else {
		float cost = sqrtf(std::max(0.f, 1 - sint * sint));
		cosi = fabsf(cosi);
		float Rs = ((etat * cosi) - (etai * cost)) / ((etat * cosi) + (etai * cost));
		float Rp = ((etai * cosi) - (etat * cost)) / ((etai * cosi) + (etat * cost));
		kr = (Rs * Rs + Rp * Rp) / 2;
	}
	// As a consequence of the conservation of energy, transmittance is given by:
	// kt = 1 - kr;
}

inline bool rayTriangleIntersect(
	const Vec3f& v0, const Vec3f& v1, const Vec3f& v2,
	const Vec3f& orig, const Vec3f& dir,
	float& tnear, float& u, float& v)
{
	Vec3f edge1 = v1 - v0;
	Vec3f edge2 = v2 - v0;
	Vec3f pvec = crossProduct(dir, edge2);
	float det = dotProduct(edge1, pvec);
	if (det == 0 || det < 0) return false;

	Vec3f tvec = orig - v0;
	u = dotProduct(tvec, pvec);
	if (u < 0 || u > det) return false;

	Vec3f qvec = crossProduct(tvec, edge1);
	v = dotProduct(dir, qvec);
	if (v < 0 || u + v > det) return false;

	float invDet = 1 / det;

	tnear = dotProduct(edge2, qvec) * invDet;
	u *= invDet;
	v *= invDet;

	return true;
}
