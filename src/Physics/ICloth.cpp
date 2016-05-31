#include "ICloth.h"
#include "Spring.h"
#include "PhysicsScene.h"
#include "PhysicsSceneRenderer.h"
#include "PointCollider.h"


#include <glm/glm.hpp>
#include <glm/ext.hpp>
using glm::vec4;

using namespace Physics;

ICloth::ICloth(
	PhysicsScene * p_scene,
	PhysicsSceneRenderer * p_renderer,
	const unsigned int & p_nodeWidth,
	const unsigned int & p_nodeHeight,
	const float & p_fabricLength,
	const float & p_fabricStiffness,
	const float & p_fabricFriction,
	const float & p_fabricStrength,
	const float & p_mass,
	const float & p_bounciness,
	const float & p_friction) :
	m_width(p_nodeWidth),
	m_height(p_nodeHeight),
	m_nodeCount(p_nodeWidth * p_nodeHeight),
	m_strandCount(
		2 * (p_nodeWidth - 1) + 
		2 * (p_nodeHeight - 1) + 
		(p_nodeWidth - 2) * 
		(p_nodeWidth - 1) + 
		(p_nodeHeight - 2) * 
		(p_nodeHeight - 1)),
	m_strands(static_cast<Spring**>(malloc(m_strandCount * sizeof(void *)))),
	m_nodes(static_cast<IPhysicsObject**>(malloc(m_nodeCount * sizeof(void *))))
{
	assert(p_nodeWidth * p_nodeHeight > 0);
	
	//Add the nodes
	for (unsigned int x = 0; x < m_width; ++x)
	{
		for (unsigned int y = 0; y < m_height; ++y)
		{
			auto obj = p_scene->CreatePhysicsObject<Physics::IPhysicsObject>();
			(*(m_nodes + x + y * m_width)) = obj;


			obj->SetCollider(new Physics::PointCollider());
			obj->SetMass(p_mass);
			obj->SetBounciness(p_bounciness);
			obj->SetFriction(p_friction);
			obj->SetPosition(vec3(x * p_fabricLength, y * p_fabricLength, 0));

			if (p_renderer != nullptr)
				p_renderer->GetRenderInfo(obj).m_colour = vec4(glm::ballRand(0.5F) + 0.5F, 0.0F);
		}
	}

	// Connect vertically
	for (unsigned int x = 0; x < m_width; ++x)
	{
		for (unsigned int y = 0; y < m_height - 1; ++y)
		{
			auto obj0 = (*(m_nodes + x + y * m_width));
			auto obj1 = (*(m_nodes + x + (y + 1) * m_width));

			(*(m_strands + y)) = p_scene->CreateConstraint<Physics::Spring>(obj0, obj1, p_fabricLength, p_fabricStiffness, p_fabricFriction, p_fabricStrength);
		}
	}

	// Connect horizontally
	for (unsigned int y = 0; y < m_height; ++y)
	{
		for (unsigned int x = 0; x < m_width - 1; ++x)
		{
			auto obj0 = (*(m_nodes + x + y * m_width));
			auto obj1 = (*(m_nodes + (x + 1) + y * m_width));

			(*(m_strands + y)) = p_scene->CreateConstraint<Physics::Spring>(obj0, obj1, p_fabricLength, p_fabricStiffness, p_fabricFriction, p_fabricStrength);
		}
	}
}

ICloth::~ICloth()
{
	for (unsigned int n = 0; n < m_strandCount; ++n)
		(*(m_strands + n))->Destroy();

	for (unsigned int n = 0; n < m_nodeCount; ++n)
		(*(m_nodes + n))->Destroy();

	delete m_strands;
	delete m_nodes;
}