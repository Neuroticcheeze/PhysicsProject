#pragma once

#include "..\BaseApplication.h"

// only needed for the camera picking
#include <glm/vec3.hpp>

class Camera;

namespace Physics
{
	class IPhysicsObject;
	class PhysicsScene;
	class PhysicsSceneRenderer;
}

class TestApplication : public BaseApplication {
public:

	TestApplication();
	virtual ~TestApplication();

	virtual bool startup();
	virtual void shutdown();

	virtual bool update(float deltaTime);
	virtual void draw();

private:

	Camera*		m_camera;

	Physics::PhysicsScene *m_physicsScene;
	Physics::PhysicsSceneRenderer *m_physicsSceneRenderer;
};