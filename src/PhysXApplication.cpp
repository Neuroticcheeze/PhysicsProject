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



void PhysXApplication::setUpPhysX()
{
	PxAllocatorCallback *myCallback = new myAllocator();
	g_PhysicsFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, *myCallback,
		gDefaultErrorCallback);
	g_Physics = PxCreatePhysics(PX_PHYSICS_VERSION, *g_PhysicsFoundation,
		PxTolerancesScale());
	PxInitExtensions(*g_Physics);
	//create physics material 
	g_PhysicsMaterial = g_Physics->createMaterial(0.5f, 0.5f, .5f);
	PxSceneDesc sceneDesc(g_Physics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0, -10.0f, 0);
	sceneDesc.filterShader = &physx::PxDefaultSimulationFilterShader;
	sceneDesc.cpuDispatcher = PxDefaultCpuDispatcherCreate(1);
	g_PhysicsScene = g_Physics->createScene(sceneDesc);
}

void PhysXApplication::upDatePhysX(float a_deltaTime)
{
	if (a_deltaTime <= 0)
	{
		return;
	}
	g_PhysicsScene->simulate(a_deltaTime);
	while (g_PhysicsScene->fetchResults() == false)
	{
		// don’t need to do anything here yet but we have to fetch results
	}
}

void PhysXApplication::setUpVisualDebugger()
{
	// check if PvdConnection manager is available on this platform
	if (g_Physics->getPvdConnectionManager() == NULL)
		return;
	// setup connection parameters
	const char* pvd_host_ip = "127.0.0.1";
	// IP of the PC which is running PVD
	int port = 5425;
	// TCP port to connect to, where PVD is listening
	unsigned int timeout = 100;
	// timeout in milliseconds to wait for PVD to respond,
	//consoles and remote PCs need a higher timeout.
	PxVisualDebuggerConnectionFlags connectionFlags = PxVisualDebuggerExt::getAllConnectionFlags();
	// and now try to connectPxVisualDebuggerExt
	auto theConnection = PxVisualDebuggerExt::createConnection(
		g_Physics->getPvdConnectionManager(), pvd_host_ip, port, timeout, connectionFlags);
}

void PhysXApplication::setupTutorial1()
{
	//add a plane
	PxTransform pose = PxTransform(PxVec3(0.0f, 0, 0.0f), PxQuat(PxHalfPi*1.0f,
		PxVec3(0.0f, 0.0f, 1.0f)));
	PxRigidStatic* plane = PxCreateStatic(*g_Physics, pose, PxPlaneGeometry(),
		*g_PhysicsMaterial);
	//add it to the physX scene
	g_PhysicsScene->addActor(*plane);

	//add a box
	float density = 10;
	PxBoxGeometry box(2, 2, 2);
	PxTransform transform(PxVec3(0, 20, 0));
	PxRigidDynamic* dynamicActor = PxCreateDynamic(*g_Physics, transform, box,
		*g_PhysicsMaterial, density);
	//add it to the physX scene
	g_PhysicsScene->addActor(*dynamicActor);
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
	setUpPhysX();
	setUpVisualDebugger();
	setupTutorial1();

	return true;
}

void PhysXApplication::shutdown() {

	//////////////////////////////////////////////////////////////////////////
	// YOUR SHUTDOWN CODE HERE
	//////////////////////////////////////////////////////////////////////////
	g_PhysicsScene->release();
	g_Physics->release();
	g_PhysicsFoundation->release();

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
	upDatePhysX(deltaTime);

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