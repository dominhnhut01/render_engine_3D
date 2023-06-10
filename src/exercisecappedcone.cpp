/****************************************************
 * 2016-2022 Eric Bachmann, Mike Zmuda, Norm Krumpe
 * All Rights Reserved.
 * NOTICE:
 * Dissemination of this information or reproduction
 * of this material is prohibited unless prior written
 * permission is granted..
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

double z = 0.0;
double inc = 0.2;

dvec3 cameraPos(10.5, 12, 11);
dvec3 cameraFocus(8.5, 6, 0);
dvec3 cameraUp = Y_AXIS;
double cameraFOV = PI_2;
//PositionalLight posLight(dvec3(10, 10, 10), white);
SpotLight spotlight(dvec3(5, 20, 8), dvec3(0, -1, 0), 0.8, white);
//PositionalLight greenLight(dvec3(0, 8.5, 15), green);

FrameBuffer frameBuffer(WINDOW_WIDTH, WINDOW_HEIGHT);
RayTracer rayTrace(paleGreen);
IScene scene;

void render() {
	int frameStartTime = glutGet(GLUT_ELAPSED_TIME);
	int width = frameBuffer.getWindowWidth();
	int height = frameBuffer.getWindowHeight();

	scene.camera = new PerspectiveCamera(cameraPos, cameraFocus, cameraUp, PI/2, width, height);
	rayTrace.raytraceScene(frameBuffer, 0, scene);

	int frameEndTime = glutGet(GLUT_ELAPSED_TIME); // Get end time
	double totalTimeSec = (frameEndTime - frameStartTime) / 1000.0;
	cout << "Render time: " << totalTimeSec << " sec." << endl;
}

void resize(int width, int height) {
	frameBuffer.setFrameBufferSize(width, height);
	glutPostRedisplay();
}

IConeY* coneY = new IConeY(dvec3(0.0, 12.0, 0.0), 5.0, 6.0);
IClosedConeY* coneY2 = new IClosedConeY(dvec3(17.0, 12.0, 0.0), 5.0, 6.0);
IPlane* plane = new IPlane(dvec3(0, 0, 0), dvec3(0, 1, 0));

void buildScene() {
	scene.addOpaqueObject(new VisibleIShape(plane, red));
	scene.addOpaqueObject(new VisibleIShape(coneY, gold));
	scene.addOpaqueObject(new VisibleIShape(coneY2, gold));

	//scene.addLight(&posLight);
	scene.addLight(&spotlight);
	//scene.addLight(&greenLight);
}

void keyboard(unsigned char key, int x, int y) {
	const double INC = 0.5;
	switch (key) {
	case 'A':
	case 'a':	
		cameraPos.x++;
		break;
	case 'D':
	case 'd':
		cameraPos.x--;
		break;
	case 'W':
	case 'w':
		cameraPos.z--;
		break;
	case 'S':
	case 's':
		cameraPos.z++;
		break;
	case 'J':
	case 'j':
		cameraPos.y++;
		break;
	case 'M':
	case 'm':
		cameraPos.y--;
		break;
	case ESCAPE:
		glutLeaveMainLoop();
		break;
	default:	cout << (int)key << "unmapped key pressed." << endl;
	}

	glutPostRedisplay();
}

int main(int argc, char* argv[]) {
	graphicsInit(argc, argv, __FILE__);

	glutDisplayFunc(render);
	glutReshapeFunc(resize);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouseUtility);
	buildScene();

	rayTrace.defaultColor = black;
	glutMainLoop();

	return 0;
}
