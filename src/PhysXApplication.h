#pragma once

#include "BaseApplication.h"

// only needed for the camera picking
#include <glm/vec3.hpp>

#include <vector>

class Camera;

class PhysXApplication : public BaseApplication {
public:

	PhysXApplication();
	virtual ~PhysXApplication();

	virtual bool startup();
	virtual void shutdown();

	virtual bool update(float deltaTime);
	virtual void draw();

private:

	Camera*		m_camera;
};