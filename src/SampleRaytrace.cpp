/****************************************************
 * 2016-2022 Eric Bachmann and Mike Zmuda
 * All Rights Reserved.
 * NOTICE:
 * Dissemination of this information or reproduction
 * of this material is prohibited unless prior written
 * permission is granted.
 ****************************************************/

#include <ctime>
#include "defs.h"
#include "io.h"
#include "ishape.h"
#include "framebuffer.h"
#include "raytracer.h"
#include "iscene.h"
#include "light.h"
#include "image.h"
#include "camera.h"
#include "rasterization.h"

PositionalLightPtr posLight1 = new PositionalLight(dvec3(-15, 20, 1), white);
PositionalLightPtr posLight2 = new PositionalLight(dvec3(-8, 2, 0), white);
PositionalLightPtr posLight3 = new PositionalLight(dvec3(8, 2, 0), blue);
PositionalLightPtr posLight4 = new PositionalLight(dvec3(0.5, 2.5, 0.5), white);
vector<PositionalLightPtr> lights = { posLight1, posLight2, posLight3, posLight4 };

FrameBuffer frameBuffer(WINDOW_WIDTH, WINDOW_HEIGHT);
RayTracer rayTrace(black);

dvec3 cameraPos(3, 3, 8);
dvec3 cameraFocus(6, 10, -1);
dvec3 cameraUp = Y_AXIS;
double cameraFOV = PI_2;

IScene scene;

void render() {
	int frameStartTime = glutGet(GLUT_ELAPSED_TIME);
	int width = frameBuffer.getWindowWidth();
	int height = frameBuffer.getWindowHeight();

	scene.camera = new PerspectiveCamera(cameraPos, cameraFocus, cameraUp, cameraFOV, width, height);
	// scene.camera = new OrthographicCamera(cameraPos, cameraFocus, cameraUp, width, height, 0.1);

	rayTrace.raytraceScene(frameBuffer, 0, scene);

	int frameEndTime = glutGet(GLUT_ELAPSED_TIME); // Get end time
	double totalTimeSec = (frameEndTime - frameStartTime) / 1000.0;
	cout << "Render time: " << totalTimeSec << " sec." << endl;
}

void resize(int width, int height) {
	frameBuffer.setFrameBufferSize(width, height);
	glutPostRedisplay();
}

void buildScene() {

	
	//IShape* cylinder = new ICylinderY(dvec3(0.0, 0.0, 0.0), 3, 2); //center, radius, height
	//IShape* plane = new IPlane(dvec3(5.0, 0.0, 0.0), dvec3(1.0, 0.0, 0.0));
	//ISphere* sphere1 = new ISphere(dvec3(0.0, 0.0, 0.0), 2.0);
	//ISphere* sphere2 = new ISphere(dvec3(-2.0, 0.0, -8.0), 2.0);
	//IEllipsoid* ellipsoid = new IEllipsoid(dvec3(4.0, 0.0, 3.0), dvec3(2.0, 1.0, 2.0));
	//IDisk disk(dvec3(15.0, 0.0, 0.0), dvec3(0.0, 0.0, 1.0), 5.0);


	//scene.addOpaqueObject(new VisibleIShape(cylinder, silver));
	/*scene.addOpaqueObject(new VisibleIShape(plane, tin));
	scene.addOpaqueObject(new VisibleIShape(sphere1, silver));*/
	//scene.addOpaqueObject(new VisibleIShape(sphere2, bronze));
	//scene.addOpaqueObject(new VisibleIShape(ellipsoid, redPlastic));
	//scene.addOpaqueObject(new VisibleIShape(disk, cyanPlastic));

	double baseY = 9.0;
	double coneHeight = 6.0;
	double cylinderHeight = 5.0;
	IShape* plane = new IPlane(dvec3(0.0, 0.0, 0.0), dvec3(0.0, 1.0, 0.0));
	// IShape* coneLeft = new IClosedConeY(dvec3(-10.0, baseY + coneHeight, 0.0), 3, coneHeight);
	IShape* coneLeft = new IClosedConeY(dvec3(0.0, 20, 0.0), 3, coneHeight);
	IShape* coneRight = new IConeY(dvec3(10.0, baseY + coneHeight, 0.0), 3, coneHeight);
	IShape* cylinder = new ICylinderY(dvec3(0.0, baseY + cylinderHeight / 2, 0.0), 4, cylinderHeight);
	
	// scene.addOpaqueObject(new VisibleIShape(plane, greenPlastic));
	scene.addOpaqueObject(new VisibleIShape(coneLeft, cyanPlastic));
	scene.addOpaqueObject(new VisibleIShape(coneRight, redPlastic));
	scene.addOpaqueObject(new VisibleIShape(cylinder, silver));


	/*for (int i = 0; i < 5; i++) {
		ISphere* sphere = new ISphere(dvec3(2 * i + 1, 0.0, 0.0), 1.0);
		scene.addOpaqueObject(new VisibleIShape(sphere, redPlastic));
	}*/
	//HitRecord hit;
	//Ray ray(dvec3(0.0, 3.0, 2.0), dvec3(1.0, 2.0, 1.0))
	for (int i = 0; i < lights.size(); ++i) {
		scene.addLight(lights[i]);
	};
	for (int i = 0; i < scene.lights.size(); ++i) {
		scene.lights[i]->atParams.constant = 2;
		scene.lights[i]->atParams.linear = 1;
		scene.lights[i]->atParams.quadratic = 1;
	}
	
}


int main(int argc, char* argv[]) {
	graphicsInit(argc, argv, __FILE__);

	glutDisplayFunc(render);
	glutReshapeFunc(resize);
	glutKeyboardFunc(keyboardUtility);
	glutMouseFunc(mouseUtility);

	buildScene();

	glutMainLoop();
	
	return 0;
}