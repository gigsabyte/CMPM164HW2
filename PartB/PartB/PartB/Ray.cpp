#include "Ray.h"

bool trace(
	const Vec3f& orig, 
	const Vec3f& dir,
	const std::vector<std::unique_ptr<Object>>& objects,
	float& tNear, uint32_t& index, Object** hitObject, Object* ignoreObject = nullptr)
{
	*hitObject = nullptr;
	for (uint32_t k = 0; k < objects.size(); ++k) {
		float tNearK = kInfinity;
		uint32_t indexK;
		Vec3f uvK;
		if (objects[k]->intersect(orig, dir, tNearK, indexK, uvK) && tNearK < tNear) {
			if (objects[k].get() == ignoreObject) continue;
			*hitObject = objects[k].get();
			tNear = tNearK;
			index = indexK;
		}
	}

	return (*hitObject != nullptr);
}

Vec3f castRay(const Vec3f& orig, const Vec3f& dir, const std::vector<std::unique_ptr<Object>>& objects, const std::vector<std::unique_ptr<Light>>& lights, const Camera& camera, uint32_t depth, bool test, Object* ignoreObject)
{
	if (depth > camera.maxRecursions) {
		return camera.bgColor;
	}

	Vec3f hitColor = camera.bgColor;
	float tnear = kInfinity;
	uint32_t index = 0;
	Object* hitObject = nullptr;
	if (trace(orig, dir, objects, tnear, index, &hitObject, ignoreObject)) {
		Vec3f hitPoint = orig + dir * tnear;
		Vec3f N; // normal 
		hitObject->getSurfaceProperties(hitPoint, dir, index, N);
		Vec3f tmp = hitPoint;
		bool inside = false;
		if (dotProduct(dir, N) > 0) {
			N = -N;
			inside = true;
		}
		switch (hitObject->materialType) {
		case REFLECTION_AND_REFRACTION:
		{
			Vec3f reflectionDirection = normalize(reflect(dir, N));
			Vec3f refractionDirection = normalize(refract(dir, N, hitObject->ior));
			Vec3f reflectionRayOrig = (dotProduct(reflectionDirection, N) < 0) ?
				hitPoint - N * camera.bias :
				hitPoint + N * camera.bias;
			Vec3f refractionRayOrig = (dotProduct(refractionDirection, N) < 0) ?
				hitPoint - N * camera.bias :
				hitPoint + N * camera.bias;
			Vec3f reflectionColor = castRay(reflectionRayOrig, reflectionDirection, objects, lights, camera, depth + 1, 1, nullptr);
			Vec3f refractionColor = castRay(refractionRayOrig, refractionDirection, objects, lights, camera, depth + 1, 1, nullptr);
			float kr = hitObject->alpha;
			fresnel(dir, N, hitObject->ior, kr);
			hitColor = reflectionColor * kr + refractionColor * (1 - kr);
			if (hitColor.x == camera.bgColor.x && hitColor.y == camera.bgColor.y && hitColor.z == camera.bgColor.z) {
				if (dotProduct(N, dir) >= 0) hitColor = Vec3f(1.0, 0.0, 1.0);
				else hitColor = camera.bgColor;
			}
			Vec3f rCol = Vec3f(hitColor.x, hitColor.y, hitColor.z);
			if (hitObject->reflection >= 1) return hitColor;
			Vec3f lightAmt = 0, specularColor = 0;
			Vec3f shadowPointOrig = (dotProduct(dir, N) < 0) ?
				hitPoint + N * camera.bias :
				hitPoint - N * camera.bias;
			Vec3f tcol = Vec3f(0);
			for (uint32_t i = 0; i < lights.size(); ++i) {

				Vec3f lightDir = lights[i]->position - hitPoint;
				// square of the distance between hitPoint and the light
				float lightDistance2 = dotProduct(lightDir, lightDir);
				lightDir = normalize(lightDir);
				float LdotN = std::max(0.f, dotProduct(lightDir, N));
				Object* shadowHitObject = nullptr;
				float tNearShadow = kInfinity;
				// is the point in shadow, and is the nearest occluding object closer to the object than the light itself?
				bool inShadow = trace(shadowPointOrig, lightDir, objects, tNearShadow, index, &shadowHitObject, nullptr) &&
					tNearShadow * tNearShadow < lightDistance2;
				float is;
				if (shadowHitObject == hitObject || shadowHitObject == nullptr) {
					inShadow = false;
					is = 0;
				}
				else {
					is = std::min(shadowHitObject->alpha * 2, 1.0f);
				}
				lightAmt = (1 - is) * lights[i]->intensity * LdotN;
				tcol += lightAmt * lights[i]->color;
				Vec3f reflectionDirection = reflect(-lightDir, N);
				specularColor += powf(std::max(0.f, -dotProduct(reflectionDirection, dir)), hitObject->specularExponent) * lights[i]->intensity;
			}
			Vec3f tCol = tcol * hitObject->evalDiffuseColor(0, 0) * hitObject->Kd + specularColor * hitObject->Ks;
			Vec3f transhit = Vec3f(0);
			if (hitObject->alpha < 1.0) {
				transhit = castRay(hitPoint, dir, objects, lights, camera, depth + 1, 1, hitObject);
			}
			tCol = tCol * hitObject->alpha + transhit * (1 - hitObject->alpha);
			hitColor = rCol * hitObject->reflection + tCol * (1 - hitObject->reflection);
			break;
		}

		default:
		{
			Vec3f lightAmt = 0, specularColor = 0;
			Vec3f shadowPointOrig = (dotProduct(dir, N) < 0) ?
				hitPoint + N * camera.bias :
				hitPoint - N * camera.bias;
			for (uint32_t i = 0; i < lights.size(); ++i) {

				Vec3f lightDir = lights[i]->position - hitPoint;
				// square of the distance between hitPoint and the light
				float lightDistance2 = dotProduct(lightDir, lightDir);
				lightDir = normalize(lightDir);
				float LdotN = std::max(0.f, dotProduct(lightDir, N));
				Object* shadowHitObject = nullptr;
				float tNearShadow = kInfinity;
				// is the point in shadow, and is the nearest occluding object closer to the object than the light itself?
				bool inShadow = trace(shadowPointOrig, lightDir, objects, tNearShadow, index, &shadowHitObject, nullptr) &&
					tNearShadow * tNearShadow < lightDistance2;
				float is;
				if (shadowHitObject == hitObject || shadowHitObject == nullptr) {
					inShadow = false;
					is = 0;
				}
				else {
					is = std::min(shadowHitObject->alpha * 2, 1.0f);
				}
				lightAmt += (1 - is) * lights[i]->intensity * LdotN; // (1 - inShadow) * 
				Vec3f reflectionDirection = reflect(-lightDir, N);
				specularColor += powf(std::max(0.f, -dotProduct(reflectionDirection, dir)), hitObject->specularExponent) * lights[i]->intensity;
			}
			hitColor = lightAmt * hitObject->evalDiffuseColor(0, 0) * hitObject->Kd + specularColor * hitObject->Ks;
			Vec3f transhit = Vec3f(0);
			if (hitObject->alpha < 1.0) {
				transhit = castRay(hitPoint, dir, objects, lights, camera, depth + 1, 1, hitObject);
			}
			hitColor = hitColor * hitObject->alpha + transhit * (1 - hitObject->alpha);
			break;
		}
		}
	}

	return hitColor;
}
