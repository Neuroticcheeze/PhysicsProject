#pragma once

#include "..\BaseApplication.h"

// only needed for the camera picking
#include <glm/vec3.hpp>

#include <vector>
using std::vector;

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

	bool LoadObj(const char * p_path, vector<float> & p_vertices, vector<unsigned int> & p_indices, const glm::vec3 & p_scale = glm::vec3(1));

private:

	MyPhysX::PhysXSystem * m_psys;

	Camera*		m_camera;
};