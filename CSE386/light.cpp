/****************************************************
 * 2016-2022 Eric Bachmann and Mike Zmuda
 * All Rights Reserved.
 * PLEASE NOTE:
 * Dissemination of this information or reproduction
 * of this material is prohibited unless prior written
 * permission is granted.
 ****************************************************/

#include "light.h"
#include "io.h"
#include "ishape.h"

 /**
  * @fn	color ambientColor(const color &matAmbient, const color &lightColor)
  * @brief	Computes the ambient color produced by a single light at a single point.
  * @param	matAmbient  	Ambient material property.
  * @param	lightColor	Light's color.
  * @return	Ambient color.
   */

color ambientColor(const color& matAmbient, const color& lightColor) {
	/* CSE 386 - todo  */
	return lightColor * matAmbient;
}

/**
 * @fn	color diffuseColor(const color &matDiffuse, const color &light, const dvec3 &l, const dvec3 &n)
 * @brief	Computes diffuse color produce by a single light at a single point.
 * @param	matDiffuse		 	Diffuse material property.
 * @param	lightColor	 	The light's color.
 * @param	l		 	Light vector.
 * @param	n		 	Normal vector.
 * @return	Diffuse color.
 */

color diffuseColor(const color& matDiffuse, const color& lightColor,
	const dvec3& l, const dvec3& n) {
	/* CSE 386 - todo  */
	return lightColor * matDiffuse * max(0, glm::dot(l, n), -FLT_MAX);
}

/**
 * @fn	color specularColor(const color &matSpecular, const color &lightColor, double shininess,
 *							const dvec3 &r, const dvec3 &v)
 * @brief	Computes specular color produce by a single light at a single point.
 * @param	matSpecular		 	Material.
 * @param	lightColor	 	The light's color.
 * @param	shininess	Material shininess.
 * @param	r		 	Reflection vector.
 * @param	v		 	Viewing vector.
 * @return	Specular color.
 */

color specularColor(const color& matSpecular, const color& lightColor,
	double shininess,
	const dvec3& r, const dvec3& v) {
	/* CSE 386 - todo  */

	return lightColor * matSpecular * pow(max(0, glm::dot(r, v), -FLT_MAX), shininess);
}

/**
 * @fn	color totalColor(const Material &mat, const color &lightColor,
 *						const dvec3 &viewingDir, const dvec3 &normal,
 *						const dvec3 &lightPos, const dvec3 &intersectionPt,
 *						bool attenuationOn, const LightAttenuationParameters &ATparams)
 * @brief	Color produced by a single light at a single point.
 * @param	mat			  	Material.
 * @param	lightColor	  	The light's color.
 * @param	v	  			The v vector.
 * @param	n   		  	Normal vector.
 * @param	lightPos	  	Light position.
 * @param	intersectionPt	(x,y,z) of intersection point.
 * @param	attenuationOn 	true if attenuation is on.
 * @param	ATparams	  	Attenuation parameters.
 * @return	Color produced by a single light at a single point.
 */

color totalColor(const Material& mat, const color& lightColor,
	const dvec3& v, const dvec3& n,
	const dvec3& lightPos, const dvec3& intersectionPt,
	bool attenuationOn, const LightATParams& ATparams) {
	/* CSE 386 - todo  */
	dvec3 l = glm::normalize(lightPos - intersectionPt);
	double l_len = glm::length(lightPos - intersectionPt);
	dvec3 r = glm::normalize(2 * glm::dot(l, n) * n - l);
	color ambient = ambientColor(mat.ambient, lightColor);
	color diffuse = diffuseColor(mat.diffuse, lightColor, l, n);
	color specular = specularColor(mat.specular, lightColor, mat.shininess, r, v);
	
	double AT;
	if (attenuationOn) {
		AT = 1 / (ATparams.constant + ATparams.linear * l_len + ATparams.quadratic * pow(l_len, 2.0));
	}
	else {
		AT = 1.0;
	}
	return ambient + AT * diffuse + AT * specular;
}

/**
 * @fn	color PositionalLight::illuminate(const dvec3 &interceptWorldCoords,
 *										const dvec3 &normal, const Material &material,
 *										const Frame &eyeFrame, bool inShadow) const
 * @brief	Computes the color this light produces in RAYTRACING applications.
 * @param	interceptWorldCoords	(x, y, z) at the intercept point.
 * @param	normal				The normal vector.
 * @param	material			The object's material properties.
 * @param	eyeFrame			The coordinate frame of the camera.
 * @param	inShadow			true if the point is in a shadow.
 * @return	The color produced at the intercept point, given this light.
 */

color PositionalLight::illuminate(const dvec3& interceptWorldCoords,
	const dvec3& normal,
	const Material& material,
	const Frame& eyeFrame,
	bool inShadow) const {

	/* CSE 386 - todo  */
	if (!isOn)
		return black;
	if (isOn && inShadow)
		return ambientColor(material.ambient, lightColor);
	if (isOn && !inShadow)
		return totalColor(material,
			lightColor,
			glm::normalize(eyeFrame.origin - interceptWorldCoords),
			normal,
			pos,
			interceptWorldCoords,
			attenuationIsTurnedOn,
			atParams);
}

/*
* @fn PositionalLight::actualPosition(const Frame& eyeFrame) const
* @brief	Returns the global world coordinates of this light.
* @param	eyeFrame	The camera's frame
* @return	The global world coordinates of this light. This will be the light's
*			raw position. Or, it will be the position relative to the camera's
*			frame (transformed from camera's frame into the world coordinate frame).
*/

dvec3 PositionalLight::actualPosition(const Frame& eyeFrame) const {
	return isTiedToWorld ? pos : eyeFrame.globalCoordToFrameCoords(pos);
}

/**
* @fn	bool PositionalLight::pointIsInAShadow(const dvec3& intercept, const dvec3& normal, const vector<VisibleIShapePtr>& objects, const Frame& eyeFrame) const
* @brief	Determines if an intercept point falls in a shadow.
* @param	intercept	the position of the intercept.
* @param	normal		the normal vector at the intercept point
* @param	objects		the collection of opaque objects in the scene
*/

bool PositionalLight::pointIsInAShadow(const dvec3& intercept,
	const dvec3& normal,
	const vector<VisibleIShapePtr>& objects,
	const Frame& eyeFrame) const {
	/* CSE 386 - todo  */

	Ray shadowFeeler = getShadowFeeler(intercept, normal, eyeFrame);
	double lightDist = glm::length(pos - intercept);
	
	if (objects.size() == 0)
		return false;
	for (int i = 0; i < objects.size(); ++i) {
		OpaqueHitRecord hit;
		objects[i]->findClosestIntersection(shadowFeeler, hit);
		if (hit.t < lightDist) {
			return true;
		}
	}

	return false;
}

/**
* @fn	Ray PositionalLight::getShadowFeeler(const dvec3& interceptWorldCoords, const dvec3& normal, const Frame &eyeFrame) const
* @brief	Returns the shadow feeler for this light.
* @param	interceptWorldCoords	the position of the intercept.
* @param	normal		The normal vector at the intercept point
* @param	eyeFrame	The coordinate frame of the camera.
*/

Ray PositionalLight::getShadowFeeler(const dvec3& interceptWorldCoords,
	const dvec3& normal,
	const Frame& eyeFrame) const {
	/* 386 - todo */
	dvec3 newPos = interceptWorldCoords + EPSILON * normal;
	Ray shadowFeeler(newPos, glm::normalize(pos - newPos));
	return shadowFeeler;
}

/**
 * @fn	color SpotLight::illuminate(const dvec3 &interceptWorldCoords,
 *									const dvec3 &normal, const Material &material,
 *									const Frame &eyeFrame, bool inShadow) const
 * @brief	Computes the color this light produces in raytracing applications.
 * @param	interceptWorldCoords				The surface properties of the intercept point.
 * @param	normal					The normal vector.
 * @param	material			The object's material properties.
 * @param	eyeFrame			The coordinate frame of the camera.
 * @param	inShadow			true if the point is in a shadow.
 * @return	The color produced at the intercept point, given this light.
 */

color SpotLight::illuminate(const dvec3& interceptWorldCoords,
	const dvec3& normal,
	const Material& material,
	const Frame& eyeFrame,
	bool inShadow) const {
	/* CSE 386 - todo  */
	
	if (!isInSpotlightCone(pos, spotDir, fov, interceptWorldCoords))
		return black;
	return PositionalLight::illuminate(interceptWorldCoords,
										normal,
										material,
										eyeFrame,
										inShadow);
}

/**
* @fn	void setDir (double dx, double dy, double dz)
* @brief	Sets the direction of the spotlight.
* @param	dx		x component of the direction
* @param	dy		y component of the direction
* @param	dz		z component of the direction
*/

void SpotLight::setDir(double dx, double dy, double dz) {
	spotDir = glm::normalize(dvec3(dx, dy, dz));
}

/**
* @fn	SpotLight::isInSpotlightCone(const dvec3& spotPos, const dvec3& spotDir, double spotFOV, const dvec3& intercept)
* @brief	Determines if an intercept point falls within a spotlight's cone.
* @param	spotPos		where the spotlight is positioned
* @param	spotDir		normalized direction of spotlight's pointing direction
* @param	spotFOV		spotlight's field of view, which is 2X of the angle from the viewing axis
* @param	intercept	the position of the intercept.
*/

bool SpotLight::isInSpotlightCone(const dvec3& spotPos,
	const dvec3& spotDir,
	double spotFOV,
	const dvec3& intercept) {
	/* CSE 386 - todo  */
	dvec3 l = glm::normalize(spotPos - intercept);
	double spotCosine = -glm::dot(l, spotDir);
	return spotCosine > (spotFOV/2);
}
