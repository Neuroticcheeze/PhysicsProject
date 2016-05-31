#include "Spring.h"

#include "IPhysicsObject.h"
#include "PhysicsScene.h"

#include <glm\glm.hpp>
using glm::vec3;
using glm::distance;

using namespace Physics;

Spring::Spring(IPhysicsObject * p_obj1, IPhysicsObject * p_obj2) : 
	Spring(p_obj1, p_obj2, 5.0F, 100.0F, 1.0F, 1500.0F)
{

}

Spring::Spring(IPhysicsObject * p_obj1, IPhysicsObject * p_obj2, float p_springLength, float p_springStiffness, float p_springFriction, float p_strength) :
	IConstraint(p_obj1, p_obj2),
	m_springLength(p_springLength),
	m_springStiffness(p_springStiffness),
	m_springFriction(p_springFriction),
	m_strength(p_strength)
{

}

Spring::~Spring()
{

}

void Spring::Contstrain(PhysicsScene * p_scene)
{
	vec3 springVec = m_obj1->GetPosition() - m_obj2->GetPosition();
	float dist = glm::length(springVec);

	vec3 force;

	if (dist != 0.0F)
	{
		force += -(springVec / dist) * (dist - m_springLength) * m_springStiffness;
	}

	force += -(m_obj1->GetVelocity() - m_obj2->GetVelocity()) * m_springFriction;

	m_obj1->ApplyForce(force);
	m_obj2->ApplyForce(-force);


	if (glm::length(force * 2.0F) > m_strength)
		Destroy();
}