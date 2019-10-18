#pragma once
#include "Object.h"
class Sphere : public Object {
	public:
		Sphere(const Vec3f& c, const float& r) : center(c), radius(r), radius2(r* r) {}
		bool intersect(const Vec3f& orig, const Vec3f& dir, float& tnear, uint32_t& index, Vec3f& uv) const;
		
		Vec3f center;
		float radius, radius2;

		// Inherited via Object
		virtual void getSurfaceProperties(const Vec3f& P, const Vec3f& I, const uint32_t& index, Vec3f& N) const override;
};

