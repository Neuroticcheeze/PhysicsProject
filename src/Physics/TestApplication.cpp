#include "TestApplication.h"
#include "..\gl_core_4_4.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "..\Camera.h"
#include "..\Gizmos.h"

#include "SphereCollider.h"
#include "PlaneCollider.h"
#include "AABBCollider.h"
#include "IPhysicsObject.h"
#include "ICloth.h"
#include "PhysicsScene.h"
#include "PhysicsSceneRenderer.h"
#include "Spring.h"

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
	m_camera->setLookAtFrom(vec3(40, 40, 80), vec3(0));
	
	//////////////////////////////////////////////////////////////////////////
	// YOUR STARTUP CODE HERE
	//////////////////////////////////////////////////////////////////////////

	m_physicsScene = new Physics::PhysicsScene();
	m_physicsScene->SetGravity(vec3(0, -9.8F, 0));
	m_physicsSceneRenderer = new Physics::PhysicsSceneRenderer();

	{
		Physics::IPhysicsObject *obj0 = m_physicsScene->CreatePhysicsObject<Physics::IPhysicsObject>();
		obj0->SetCollider(new Physics::PlaneCollider(vec3(0, 1, 0.3F), -10));
		obj0->SetIsStatic(true);
		obj0->SetBounciness(0.3F);
		obj0->SetFriction(50000.0F);
		obj0->SetMass(1000.0F);
		m_physicsSceneRenderer->GetRenderInfo(obj0).m_colour = vec4(0.2F, 0.5F, 0.1F, 1);
	}

	{
		Physics::IPhysicsObject *obj0 = m_physicsScene->CreatePhysicsObject<Physics::IPhysicsObject>();
		obj0->SetCollider(new Physics::AABBCollider(vec3(30, 15, 30)));
		obj0->SetIsStatic(true);
		obj0->SetBounciness(0.3F);
		obj0->SetFriction(500.0F);
		obj0->SetMass(50.0F);
		obj0->SetPosition(vec3(5, 5, 5));
		m_physicsSceneRenderer->GetRenderInfo(obj0).m_colour = vec4(0, 0, 1, 0.5F);
	}

	//for (int t = 0; t < 1; ++t)
	//{
	//	Physics::ICloth *cloth = m_physicsScene->CreateCloth<Physics::ICloth>(m_physicsSceneRenderer, vec3(t * 8, 15, 0));
	//	for (int i = 0; i < cloth->GetWidth(); ++i) cloth->SetHook(cloth->GetWidth() * (cloth->GetHeight() - 1) + i);
	//}



	glLineWidth(2.0F);

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
		const float density = 2;

		float r = glm::linearRand(1.0F, 5.0F);
		Physics::IPhysicsObject *obj = m_physicsScene->CreatePhysicsObject<Physics::IPhysicsObject>();
		obj->SetCollider(new Physics::SphereCollider(r));
		obj->SetMass(20);
		obj->SetBounciness(0.0F);
		obj->SetFriction(18.0F);
		m_physicsSceneRenderer->GetRenderInfo(obj).m_colour = vec4(glm::ballRand(0.5F) + 0.5F, 1);
		obj->SetPosition(glm::ballRand(5.0F) + vec3(0, 20, 0));
	}

	static bool leftMouseClick = false, leftMouseClickPrev = false;

	leftMouseClick = glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS;
	
	if (leftMouseClick && !leftMouseClickPrev)
	{
		const float density = 5.5F;

		float r = glm::linearRand(3.0F, 6.0F);
		Physics::IPhysicsObject *obj = m_physicsScene->CreatePhysicsObject<Physics::IPhysicsObject>();
		obj->SetCollider(new Physics::AABBCollider(vec3(10, 10, 10)));
		obj->SetMass(15);
		obj->SetBounciness(0.8F);
		obj->SetFriction(18.0F);
		obj->SetVelocity(vec3(-forwardVec) * 100.0F + glm::ballRand(7.0F));
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

	m_physicsSceneRenderer->Render(m_physicsScene, m_camera);

	// display the 3D gizmos
	Gizmos::draw(m_camera->getProjectionView());

	// get a orthographic projection matrix and draw 2D gizmos
	int width = 0, height = 0;
	glfwGetWindowSize(m_window, &width, &height);
	mat4 guiMatrix = glm::ortho<float>(0, (float)width, 0, (float)height);

	Gizmos::draw2D(guiMatrix);
}