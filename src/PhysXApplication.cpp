#include "PhysXApplication.h"
#include "gl_core_4_4.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "Camera.h"
#include "Gizmos.h"

using glm::vec3;
using glm::vec4;
using glm::mat4;

//PhysX Includes
#include <PxPhysicsAPI.h>
using namespace physx;
// Physx Physics
PxPhysics *mPhysics = NULL;
PxFoundation *mFoundation = NULL;
PxScene * mScene = NULL;

PxRigidActor *shape0;

void initPhysX()
{
	static PxDefaultErrorCallback gDefaultErrorCallback;
	static PxDefaultAllocator gDefaultAllocatorCallback;

	// create foundation object with default error and allocator callbacks.
	mFoundation = PxCreateFoundation(
		PX_PHYSICS_VERSION,
		gDefaultAllocatorCallback,
		gDefaultErrorCallback);

	// create Physics object with the created foundation and with a 'default' scale tolerance.
	mPhysics = PxCreatePhysics(
		PX_PHYSICS_VERSION,
		*mFoundation,
		PxTolerancesScale());
}

PhysXApplication::PhysXApplication()
	: m_camera(nullptr) {

}

PhysXApplication::~PhysXApplication() {

}

bool PhysXApplication::startup()
{

	// create a basic window
	createWindow("AIE OpenGL Application", 1280, 720);

	// start the gizmo system that can draw basic shapes
	Gizmos::create();

	// create a camera
	m_camera = new Camera(glm::pi<float>() * 0.25f, 16 / 9.f, 0.1f, 1000.f);
	m_camera->setLookAtFrom(vec3(50, 10, 0), vec3(0));

	//////////////////////////////////////////////////////////////////////////
	// YOUR STARTUP CODE HERE
	//////////////////////////////////////////////////////////////////////////
	initPhysX();

	return true;
}

void PhysXApplication::shutdown() {

	//////////////////////////////////////////////////////////////////////////
	// YOUR SHUTDOWN CODE HERE
	//////////////////////////////////////////////////////////////////////////
	mPhysics->release();
	mFoundation->release();

	// delete our camera and cleanup gizmos
	delete m_camera;
	Gizmos::destroy();

	// destroy our window properly
	destroyWindow();
}

bool PhysXApplication::update(float deltaTime)
{
	// close the application if the window closes
	if (glfwWindowShouldClose(m_window) ||
		glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		return false;

	// update the camera's movement
	m_camera->update(deltaTime);

	// clear the gizmos out for this frame
	Gizmos::clear();

	//////////////////////////////////////////////////////////////////////////
	// YOUR UPDATE CODE HERE
	//////////////////////////////////////////////////////////////////////////

	// return true, else the application closes
	return true;
}

void PhysXApplication::draw() {

	// clear the screen for this frame
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//////////////////////////////////////////////////////////////////////////
	// DRAW YOUR THINGS HERE
	//////////////////////////////////////////////////////////////////////////

	// ...for now let's add a grid to the gizmos
	for (int i = 0; i < 21; ++i) {
		Gizmos::addLine(vec3(-10 + i, 0, 10), vec3(-10 + i, 0, -10),
			i == 10 ? vec4(1, 1, 1, 1) : vec4(0, 0, 0, 1));

		Gizmos::addLine(vec3(10, 0, -10 + i), vec3(-10, 0, -10 + i),
			i == 10 ? vec4(1, 1, 1, 1) : vec4(0, 0, 0, 1));
	}

	// display the 3D gizmos
	Gizmos::draw(m_camera->getProjectionView());

	// get a orthographic projection matrix and draw 2D gizmos
	int width = 0, height = 0;
	glfwGetWindowSize(m_window, &width, &height);
	mat4 guiMatrix = glm::ortho<float>(0, (float)width, 0, (float)height);

	Gizmos::draw2D(guiMatrix);
}