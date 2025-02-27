#pragma once
#include "Object.h"
class MeshTriangle :
	public Object
{
public:
	MeshTriangle(
		const Vec3f* verts,
		const uint32_t* vertsIndex,
		const uint32_t& numTris,
		const Vec3f* st)
	{
		uint32_t maxIndex = 0;
		for (uint32_t i = 0; i < numTris * 3; ++i)
			if (vertsIndex[i] > maxIndex) maxIndex = vertsIndex[i];
		maxIndex += 1;
		vertices = std::unique_ptr<Vec3f[]>(new Vec3f[maxIndex]);
		memcpy(vertices.get(), verts, sizeof(Vec3f) * maxIndex);
		vertexIndex = std::unique_ptr<uint32_t[]>(new uint32_t[numTris * 3]);
		memcpy(vertexIndex.get(), vertsIndex, sizeof(uint32_t) * numTris * 3);
		numTriangles = numTris;
		stCoordinates = std::unique_ptr<Vec3f[]>(new Vec3f[maxIndex]);
		memcpy(stCoordinates.get(), st, sizeof(Vec3f) * maxIndex);
	}

	bool intersect(const Vec3f& orig, const Vec3f& dir, float& tnear, uint32_t& index, Vec3f& uv) const
	{
		bool intersect = false;
		for (uint32_t k = 0; k < numTriangles; ++k) {
			const Vec3f& v0 = vertices[vertexIndex[k * 3]];
			const Vec3f& v1 = vertices[vertexIndex[k * 3 + 1]];
			const Vec3f& v2 = vertices[vertexIndex[k * 3 + 2]];
			float t, u, v;
			if (rayTriangleIntersect(v0, v1, v2, orig, dir, t, u, v) && t < tnear) {
				tnear = t;
				uv.x = u;
				uv.y = v;
				index = k;
				intersect |= true;
			}
		}

		return intersect;
	}

	void getSurfaceProperties(const Vec3f& P, const Vec3f& I, const uint32_t& index, Vec3f& N) const
	{
		const Vec3f& v0 = vertices[vertexIndex[index * 3]];
		const Vec3f& v1 = vertices[vertexIndex[index * 3 + 1]];
		const Vec3f& v2 = vertices[vertexIndex[index * 3 + 2]];
		Vec3f e0 = normalize(v1 - v0);
		Vec3f e1 = normalize(v2 - v1);
		N = normalize(crossProduct(e0, e1));
		//if (P.x > 0 || P.y > 0) N = -N;
	}

	void getSurfaceProperties(const Vec3f& P, const Vec3f& I, const uint32_t& index, const Vec3f& uv, Vec3f& N, Vec3f& st) const
	{
		const Vec3f& v0 = vertices[vertexIndex[index * 3]];
		const Vec3f& v1 = vertices[vertexIndex[index * 3 + 1]];
		const Vec3f& v2 = vertices[vertexIndex[index * 3 + 2]];
		Vec3f e0 = normalize(v1 - v0);
		Vec3f e1 = normalize(v2 - v1);
		N = normalize(crossProduct(e0, e1));
		//if (P.x > 0 || P.y > 0) N = -N;
		const Vec3f& st0 = stCoordinates[vertexIndex[index * 3]];
		const Vec3f& st1 = stCoordinates[vertexIndex[index * 3 + 1]];
		const Vec3f& st2 = stCoordinates[vertexIndex[index * 3 + 2]];
		st = st0 * (1 - uv.x - uv.y) + st1 * uv.x + st2 * uv.y;
	}

	Vec3f evalDiffuseColor(const Vec3f& st) const
	{
		return diffuseColor;
		float scale = 5;
		float pattern = (fmodf(st.x * scale, 1) > 0.5) ^ (fmodf(st.y * scale, 1) > 0.5);
		return mix(Vec3f(0.815, 0.235, 0.031), Vec3f(0.937, 0.937, 0.231), pattern);
	}

	std::unique_ptr<Vec3f[]> vertices;
	uint32_t numTriangles;
	std::unique_ptr<uint32_t[]> vertexIndex;
	std::unique_ptr<Vec3f[]> stCoordinates;
};

