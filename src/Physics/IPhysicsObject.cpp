#include "IPhysicsObject.h"

#include "IConstraint.h"
#include "ICollider.h"

using namespace Physics;

IPhysicsObject::IPhysicsObject() :
	m_collider(nullptr),
	m_awake(false),
	m_destroyed(false)
{
	SetMass(1.0F);
	SetBounciness(0.5F);
	SetFriction(0.5F);
	SetIsStatic(false);
}

IPhysicsObject::~IPhysicsObject()
{
	for (auto iter = m_involvedConstraints.begin(); iter != m_involvedConstraints.end(); ++iter)
	{
		(*iter)->Destroy();
	}

	delete m_collider;
}

void IPhysicsObject::Update(const float & p_deltaTime)
{
	if (!GetIsStatic())
	{
		// physics on velocity and position
		m_velocity += m_acceleration * p_deltaTime;
		m_position += m_velocity * p_deltaTime;
	}

	m_acceleration = vec3(0.0F);

	if (m_collider != nullptr)
	{
		m_collider->Transform(this);
	}
}

void IPhysicsObject::ApplyForce(const vec3 & p_force)
{
	m_acceleration += p_force * m_invMass;
}

ICollider *IPhysicsObject::GetCollider()
{
	if (m_collider == nullptr)
		return ICollider::GetNoneInstance();
	
	return m_collider;
}