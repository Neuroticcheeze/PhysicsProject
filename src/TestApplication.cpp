#include "TestApplication.h"
#include "gl_core_4_4.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "Camera.h"
#include "Gizmos.h"

#include "Physics\SphereCollider.h"
#include "Physics\PlaneCollider.h"
#include "Physics\PhysicsObject.h"
#include "Physics\PhysicsScene.h"
#include "Physics\PhysicsSceneRenderer.h"

using glm::vec3;
using glm::vec4;
using glm::mat4;

TestApplication::TestApplication()
	: m_camera(nullptr) {

}

TestApplication::~TestApplication() {

}

bool TestApplication::startup() {

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

	m_physicsScene = new Physics::PhysicsScene();
	m_physicsScene->SetGravity(vec3(0, -9.8F, 0));
	m_physicsSceneRenderer = new Physics::PhysicsSceneRenderer();

	return true;
}

void TestApplication::shutdown() {

	//////////////////////////////////////////////////////////////////////////
	// YOUR SHUTDOWN CODE HERE
	//////////////////////////////////////////////////////////////////////////

	//m_physicsScene->DestroyPhysicsObject(m_physicsObject0);
	delete m_physicsScene;
	delete m_physicsSceneRenderer;

	// delete our camera and cleanup gizmos
	delete m_camera;
	Gizmos::destroy();

	// destroy our window properly
	destroyWindow();
}

bool TestApplication::update(float deltaTime)
{
	// close the application if the window closes
	if (glfwWindowShouldClose(m_window) ||
		glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		return false;

	// update the camera's movement
	m_camera->update(deltaTime);

	vec4 positionVec = m_camera->getTransform()[3];
	vec4 forwardVec = m_camera->getTransform()[2];
	vec4 rightVec = m_camera->getTransform()[0];

	if (glfwGetKey(m_window, GLFW_KEY_G) == GLFW_PRESS)
	{
		const float density = 0.05F;

		float r = glm::linearRand(1.0F, 5.0F);
		Physics::IPhysicsObject *obj = m_physicsScene->CreatePhysicsObject<Physics::IPhysicsObject>();
		obj->SetCollider(new Physics::SphereCollider(r));
		obj->SetMass(r * density);
		m_physicsSceneRenderer->GetRenderInfo(obj).m_colour = vec4(glm::ballRand(0.5F) + 0.5F, 1);
		obj->SetPosition(glm::ballRand(5.0F) + vec3(0, 100, 0));
	}

	static bool leftMouseClick = false, leftMouseClickPrev = false;

	leftMouseClick = glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS;
	
	if (leftMouseClick && !leftMouseClickPrev)
	{
		const float density = 0.05F;

		float r = glm::linearRand(1.0F, 5.0F);
		Physics::IPhysicsObject *obj = m_physicsScene->CreatePhysicsObject<Physics::IPhysicsObject>();
		obj->SetCollider(new Physics::SphereCollider(r));
		obj->SetMass(r * density);
		obj->SetVelocity(vec3(-forwardVec) * 150.0F + glm::ballRand(20.0F));
		m_physicsSceneRenderer->GetRenderInfo(obj).m_colour = vec4(glm::ballRand(0.5F) + 0.5F, 1);
		obj->SetPosition(vec3(positionVec));
	}

	leftMouseClickPrev = leftMouseClick;


	m_physicsScene->Simulate(deltaTime);

	// clear the gizmos out for this frame
	Gizmos::clear();

	//////////////////////////////////////////////////////////////////////////
	// YOUR UPDATE CODE HERE
	//////////////////////////////////////////////////////////////////////////

	// return true, else the application closes
	return true;
}

void TestApplication::draw() {

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

	m_physicsSceneRenderer->Render(m_physicsScene, m_camera);

	// display the 3D gizmos
	Gizmos::draw(m_camera->getProjectionView());

	// get a orthographic projection matrix and draw 2D gizmos
	int width = 0, height = 0;
	glfwGetWindowSize(m_window, &width, &height);
	mat4 guiMatrix = glm::ortho<float>(0, (float)width, 0, (float)height);

	Gizmos::draw2D(guiMatrix);
}