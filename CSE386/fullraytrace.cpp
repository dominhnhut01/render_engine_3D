/****************************************************
 * 2016-2022 Eric Bachmann and Mike Zmuda
 * All Rights Reserved.
 * NOTICE:
 * Dissemination of this information or reproduction
 * of this material is prohibited unless prior written
 * permission is granted.
 ****************************************************/

//Red: x axis
// Blue: z axis

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

int currLight = 0;
double angle = 0.5;
const int MAX = 35;
double x = MAX;
double inc = 10;
bool isAnimated = false;
int numReflections = 0;
int antiAliasing = 1;
bool multiViewOn = false;
double spotDirX = 0.05;
double spotDirY = 0;
double spotDirZ = -1;

dvec3 cameraPos1(-10, 12, 18);
dvec3 cameraFocus1(-3, 7, 0);
dvec3 cameraUp1 = Y_AXIS;

double cameraFOV = glm::radians(120.0);

vector<PositionalLightPtr> lights = {
						new PositionalLight(dvec3(0, 25, 15), paleGreen),
						new SpotLight(dvec3(2, 10, 100),
										dvec3(spotDirX,spotDirY,spotDirZ),
										glm::radians(100.0),
										blue)
};

PositionalLightPtr posLight = lights[0];
SpotLightPtr spotLight = (SpotLightPtr)lights[1];

FrameBuffer frameBuffer(WINDOW_WIDTH, WINDOW_HEIGHT);
Image im1("usflag.ppm");
Image im2("snail.ppm");
RayTracer rayTrace(black);
IScene scene;

void render() {
	int frameStartTime = glutGet(GLUT_ELAPSED_TIME);
	int width = frameBuffer.getWindowWidth();
	int height = frameBuffer.getWindowHeight();
	scene.camera = new PerspectiveCamera(cameraPos1, cameraFocus1, cameraUp1, cameraFOV, width, height);
	rayTrace.raytraceScene(frameBuffer, numReflections, scene);

	int frameEndTime = glutGet(GLUT_ELAPSED_TIME); // Get end time
	double totalTimeSec = (frameEndTime - frameStartTime) / 1000.0;
	cout << "Render time: " << totalTimeSec << " sec. " << endl;
}

void resize(int width, int height) {
	frameBuffer.setFrameBufferSize(width, height);
	glutPostRedisplay();
}

IPlane* plane1 = new IPlane(dvec3(0.0, -20.0, 0.0), dvec3(0.5, 1.0, 0.0));
IPlane* plane2 = new IPlane(dvec3(0.0, -20.0, 0.0), dvec3(-0.5, 1.0, 0.0));
IPlane* plane3 = new IPlane(dvec3(0.0, 0.0, -12.0), dvec3(0.0, 0.0, 1.0));
ICylinderY* cylinder1 = new ICylinderY(dvec3(10, 6, 0), 8, 12);
ICylinderZ* cylinder2 = new ICylinderZ(dvec3(-5, 16, 5), 5, 9);
ICylinderZ* cylinder3 = new ICylinderZ(dvec3(30, 20, 5), 7, 14);
IClosedConeY* cone = new IClosedConeY(dvec3(18, 15, 12), 6, 7);
IPlane* clearPlane = new IPlane(dvec3(x, 0.0, 0.0), dvec3(-1.0, 0.0, 0.0));
ISphere* sphere1 = new ISphere(dvec3(-23.0, 10.0, -5.0), 7.0);
ISphere* sphere2 = new ISphere(dvec3(-10.0, 3.0, 8.5), 5.0);

void buildScene() {
	scene.addOpaqueObject(new VisibleIShape(plane1, tin));
	scene.addOpaqueObject(new VisibleIShape(plane2, tin));
	scene.addOpaqueObject(new VisibleIShape(plane3, tin));
	scene.addTransparentObject(new TransparentIShape(clearPlane, red, 0.25));

	scene.addOpaqueObject(new VisibleIShape(cylinder1, bronze, &im1));
	scene.addOpaqueObject(new VisibleIShape(cylinder2, ruby));
	scene.addOpaqueObject(new VisibleIShape(cylinder3, pewter));
	scene.addOpaqueObject(new VisibleIShape(cone, gold));
	scene.addOpaqueObject(new VisibleIShape(sphere1, polishedSilver));
	scene.addOpaqueObject(new VisibleIShape(sphere2, brass, &im2));

	scene.addLight(lights[0]);
	scene.addLight(lights[1]);
}

void incrementClamp(double& v, double delta, double lo, double hi) {
	v = glm::clamp(v + delta, lo, hi);
}

void incrementClamp(int& v, int delta, int lo, int hi) {
	v = glm::clamp(v + delta, lo, hi);
}

void timer(int id) {
	if (isAnimated) {
		if (x <= -MAX) {
			inc = -inc;
		} else if (x >= MAX) {
			inc = -inc;
		}
		x += inc;
	}
	clearPlane->a = dvec3(x, 0, 0);
	glutTimerFunc(TIME_INTERVAL, timer, 0);
	glutPostRedisplay();
}

// You shouldn't need to edit this function
void keyboard(unsigned char key, int x, int y) {
	int W, H;
	const double INC = 0.5;
	switch (key) {
	case '[':
		cameraPos1.x++;
		cout << "camera.x " << cameraPos1.x << endl;
		break;
	case '{':
		cameraPos1.x--;
		cout << "camera.x " << cameraPos1.x << endl;
		break;
	case ']':
		cameraPos1.z++;
		cout << "camera.z " << cameraPos1.z << endl;
		break;
	case '}':
		cameraPos1.z--;
		cout << "camera.z " << cameraPos1.z << endl;
		break;
	case '=':
		cameraPos1.y++;
		break;
	case '|':
		cameraPos1.y--;
		break;
	case 'p':	currLight = 0;
		cout << *lights[0] << endl;
		break;
	case 's':	currLight = 1;
		cout << *lights[1] << endl;
		break;
	case 'n':	lights[currLight]->isOn = !lights[currLight]->isOn;
		cout << (lights[currLight]->isOn ? "ON" : "OFF") << endl;
		break;
	case 'R':
	case 'r':	incrementClamp(lights[currLight]->lightColor.r, isupper(key) ? 0.1 : -0.1, 0.0, 1.0);
		cout << lights[currLight]->lightColor << endl;
		break;
	case 'G':
	case 'g':	incrementClamp(lights[currLight]->lightColor.g, isupper(key) ? 0.1 : -0.1, 0.0, 1.0);
		cout << lights[currLight]->lightColor << endl;
		break;
	case 'B':
	case 'b':	incrementClamp(lights[currLight]->lightColor.b, isupper(key) ? 0.1 : -0.1, 0.0, 1.0);
		cout << lights[currLight]->lightColor << endl;
		break;
	case 'a':	lights[currLight]->attenuationIsTurnedOn = !lights[currLight]->attenuationIsTurnedOn;
		cout << (lights[currLight]->attenuationIsTurnedOn ? "Atten ON" : "Atten OFF") << endl;
		break;
	case 'c':
	case 'C':	incrementClamp(lights[currLight]->atParams.constant, isupper(key) ? INC : -INC, 0.0, 10.0);
		cout << lights[currLight]->atParams << endl;
		break;
	case 'l':
	case 'L':	incrementClamp(lights[currLight]->atParams.linear, isupper(key) ? INC : -INC, 0.0, 10.0);
		cout << lights[currLight]->atParams << endl;
		break;
	case 'q':
	case 'Q':	incrementClamp(lights[currLight]->atParams.quadratic, isupper(key) ? INC : -INC, 0.0, 10.0);
		cout << lights[currLight]->atParams << endl;
		break;
	case 'X':
	case 'x': lights[currLight]->pos.x += (isupper(key) ? INC : -INC);
		cout << lights[currLight]->pos << endl;
		break;
	case 'Y':
	case 'y': lights[currLight]->pos.y += (isupper(key) ? INC : -INC);
		cout << lights[currLight]->pos << endl;
		break;
	case 'Z':
	case 'z': lights[currLight]->pos.z += (isupper(key) ? INC : -INC);
		cout << lights[currLight]->pos << endl;
		break;
	case 'd':
	case 'D':	spotDirX += (isupper(key) ? INC : -INC);
		spotLight->setDir(spotDirX, spotDirY, spotDirZ);
		cout << spotLight->spotDir << endl;
		break;
	case 'F':
	case 'f':	incrementClamp(spotLight->fov, isupper(key) ? 0.2 : -0.2, 0.1, PI);
		cout << spotLight->fov << endl;
		break;
	case 'M':
	case 'm':	incrementClamp(cameraFOV, isupper(key) ? 0.2 : -0.2, glm::radians(10.0), glm::radians(160.0));
		W = frameBuffer.getWindowWidth();
		H = frameBuffer.getWindowWidth();
		cout << "camFOV: " << cameraFOV << endl;
		break;
	case '3':	antiAliasing = 3;
		cout << "Anti aliasing: " << antiAliasing << endl;
		break;
	case '1':	antiAliasing = 1;
		cout << "Anti aliasing: " << antiAliasing << endl;
		break;
	case '?':	multiViewOn = !multiViewOn;
		break;
	case '-':
		numReflections = glm::max(numReflections - 1, 0);
		cout << "Num reflections: " << numReflections << endl;
		break;
	case '+':	numReflections++;
		cout << "Num reflections: " << numReflections << endl;
		break;
	case ' ':	isAnimated = !isAnimated;
		cout << "animation: " << (isAnimated ? "On" : "Off") << endl;
		break;
	case ESCAPE:
		glutLeaveMainLoop();
		break;
	default:
		cout << (int)key << " unmapped key pressed." << endl;
	}

	glutPostRedisplay();
}

int main(int argc, char* argv[]) {
	graphicsInit(argc, argv, __FILE__);

	glutDisplayFunc(render);
	glutReshapeFunc(resize);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouseUtility);
	glutTimerFunc(TIME_INTERVAL, timer, 0);
	buildScene();

	glutMainLoop();

	return 0;
}
