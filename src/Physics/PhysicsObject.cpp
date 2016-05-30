#include "PhysicsObject.h"

#include "Collider.h"

using namespace Physics;

IPhysicsObject::IPhysicsObject() :
	m_collider(nullptr),
	m_awake(false)
{
	SetMass(1.0F);
	SetBounciness(0.5F);
	SetDampening(0.5F);
	SetIsStatic(false);
}

IPhysicsObject::~IPhysicsObject()
{
	delete m_collider;
}

void IPhysicsObject::Update(const float & p_deltaTime)
{
	if (GetIsStatic())
		return;

	// physics on velocity and position
	m_velocity += m_acceleration * p_deltaTime;
	m_position += m_velocity * p_deltaTime;

	// clear acceleration
	m_acceleration.x = m_acceleration.y = m_acceleration.z = 0.0F;

	if (m_collider != nullptr)
	{
		m_collider->Transform(this);
	}
}

void IPhysicsObject::ApplyForce(const vec3 & p_force)
{
	m_acceleration += p_force * m_invMass;
}

Collider *IPhysicsObject::GetCollider()
{
	if (m_collider == nullptr)
		return Collider::GetNoneInstance();
	
	return m_collider;
}