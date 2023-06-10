/****************************************************
 * 2016-2022 Eric Bachmann and Mike Zmuda
 * All Rights Reserved.
 * PLEASE NOTE:
 * Dissemination of this information or reproduction
 * of this material is prohibited unless prior written
 * permission is granted.
 ****************************************************/
#include "raytracer.h"
#include "ishape.h"
#include "io.h"

 /**
  * @fn	RayTracer::RayTracer(const color &defa)
  * @brief	Constructs a raytracers.
  * @param	defa	The clear color.
  */

RayTracer::RayTracer(const color& defa)
	: defaultColor(defa) {
}

/**
 * @fn	void RayTracer::raytraceScene(FrameBuffer &frameBuffer, int depth, const IScene &theScene) const
 * @brief	Raytrace scene
 * @param [in,out]	frameBuffer	Framebuffer.
 * @param 		  	depth	   	The current depth of recursion.
 * @param 		  	theScene   	The scene.
 */

void RayTracer::raytraceScene(FrameBuffer& frameBuffer, int depth,
	const IScene& theScene) const {
	const RaytracingCamera& camera = *theScene.camera;

	for (int y = 0; y < frameBuffer.getWindowHeight(); ++y) {
		for (int x = 0; x < frameBuffer.getWindowWidth(); ++x) {
			// This is for debugging a particular ray for a particular pixel
			// Set a breakpoint on the cout line below
			// Right click on a pixel
			// Make the rendering window re-render: spacebar or click on the window
			DEBUG_PIXEL = (x == xDebug && y == yDebug);
			if (DEBUG_PIXEL) {
				cout << "";
			}
			/* CSE 386 - todo  */
			// Only work with the first shape
			Ray ray = camera.getRay(x, y);
			
			color total_c = RayTracer::traceIndividualRay(ray, theScene, depth);

			total_c.x = total_c.x > 1 ? 1 : total_c.x;
			total_c.y = total_c.y > 1 ? 1 : total_c.y;
			total_c.z = total_c.z > 1 ? 1 : total_c.z;


			frameBuffer.setColor(x, y, total_c);
			//frameBuffer.showAxes(x, y, ray, 0.25);			// Displays R/x, G/y, B/z axes
		}
	}

	frameBuffer.showColorBuffer();
}



/**
 * @fn	color raytracer::traceindividualray(const ray &ray,
 *											const iscene &thescene,
 *											int recursionlevel) const
 * @brief	trace an individual ray.
 * @param	ray			  	the ray.
 * @param	thescene	  	the scene.
 * @param	recursionlevel	the recursion level.
 * @return	the color to be displayed as a result of this ray.
 */

color RayTracer::traceIndividualRay(const Ray& ray, const IScene& theScene, int recursionLevel) const {
	/* CSE 386 - todo  */
	// This might be a useful helper function.
	if (recursionLevel < 0) {
		return black;
	}
	const RaytracingCamera& camera = *theScene.camera;
	const vector<VisibleIShapePtr>& opaqueObjs = theScene.opaqueObjs;
	const vector<TransparentIShapePtr>& transObjs = theScene.transparentObjs;
	const vector<PositionalLightPtr>& lights = theScene.lights;

	OpaqueHitRecord opaqueHit;
	VisibleIShape::findIntersection(ray, opaqueObjs, opaqueHit);

	TransparentHitRecord transHit;
	TransparentIShape::findIntersection(ray, transObjs, transHit);

	// when the above is done loop through all the shapes
	// hitRecord will have the information about t, the interceptPt, normal, material and texture
	color total_c;
	color c;
	bool inShadow;
	for (auto light : lights) {
		if (opaqueHit.t != FLT_MAX) {
			inShadow = light->pointIsInAShadow(opaqueHit.interceptPt,
				opaqueHit.normal,
				opaqueObjs,
				camera.getFrame());
			color material_c = light->illuminate(opaqueHit.interceptPt,
				opaqueHit.normal,
				opaqueHit.material,
				camera.getFrame(),
				inShadow);
			if (opaqueHit.texture != nullptr) {
				color texture_c = opaqueHit.texture->getPixelUV(opaqueHit.u, opaqueHit.v);
				c = 0.5 * material_c + 0.5 * texture_c;
			}
			else {
				c = material_c;
			}
			
			if (transHit.t < opaqueHit.t) {
				color transColor = transHit.transColor;
				c = (1 - transHit.alpha) * c + transHit.alpha * transColor;
			}
		}
		else if (transHit.t != FLT_MAX) {
			c = 0.5 * defaultColor + 0.5 * transHit.transColor;
		}
		else {
			c = defaultColor;
		}
		total_c = total_c + c;
	}

	dvec3 reflect_origin = opaqueHit.interceptPt + EPSILON * opaqueHit.normal;
	dvec3 reflect_dir = ray.dir - 2 * glm::dot(ray.dir, opaqueHit.normal) * opaqueHit.normal;
	Ray reflect_ray(reflect_origin, reflect_dir);

	color reflect_color = defaultColor;
	reflect_color = RayTracer::traceIndividualRay(reflect_ray, theScene, recursionLevel - 1);
	return total_c + 0.3 * reflect_color;
}
