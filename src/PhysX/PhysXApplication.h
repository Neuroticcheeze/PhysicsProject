#pragma once

#include "..\BaseApplication.h"

// only needed for the camera picking
#include <glm/vec3.hpp>

class Camera;

namespace MyPhysX
{
	class PhysXSystem;
}

class PhysXApplication : public BaseApplication {
public:

	PhysXApplication();
	virtual ~PhysXApplication();

	virtual bool startup();
	virtual void shutdown();

	virtual bool update(float deltaTime);
	virtual void draw();

private:

	MyPhysX::PhysXSystem * m_psys;

	Camera*		m_camera;
};