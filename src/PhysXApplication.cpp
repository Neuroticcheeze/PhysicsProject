#include "PhysXApplication.h"
#include "gl_core_4_4.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "Camera.h"
#include "Gizmos.h"

#include "PhysX.h"

using glm::vec3;
using glm::vec4;
using glm::mat4;

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
	m_psys = new MyPhysX::PhysXSystem;

	m_psys->AddMaterial("box", 0.5F, 0.2F, 0.2F);

	return true;
}

void PhysXApplication::shutdown() {

	//////////////////////////////////////////////////////////////////////////
	// YOUR SHUTDOWN CODE HERE
	//////////////////////////////////////////////////////////////////////////
	delete m_psys;

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
	m_psys->Update(deltaTime);

	static float t = 0;
	t += deltaTime;

	if (t > 2.0F)
	{
		m_psys->Add("box", glm::linearRand(10, 1000), physx::PxBoxGeometry(glm::linearRand(0.5F, 3.0F), glm::linearRand(0.5F, 3.0F), glm::linearRand(0.5F, 3.0F)),
			physx::PxVec3(0, 10, 0),
			physx::PxQuat(glm::radians(45.0F), physx::PxVec3(0, 1, 0)));
		t = 0.0F;
	}

	else if (t > 1.0F && t < 1.01F)
	{
		m_psys->Add("box", glm::linearRand(10, 1000), physx::PxSphereGeometry(glm::linearRand(0.5F, 3.0F)),
			physx::PxVec3(0, 10, 0),
			physx::PxQuat(glm::radians(45.0F), physx::PxVec3(0, 1, 0)));
	}

	if (glfwGetKey(m_window, GLFW_KEY_C) == GLFW_PRESS)
		m_psys->Clear();

	// return true, else the application closes
	return true;
}

void PhysXApplication::draw() {

	// clear the screen for this frame
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//////////////////////////////////////////////////////////////////////////
	// DRAW YOUR THINGS HERE
	//////////////////////////////////////////////////////////////////////////

	m_psys->Render();

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