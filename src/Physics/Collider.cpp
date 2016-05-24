#include "Collider.h"

#include "PlaneCollider.h"
#include "SphereCollider.h"

#include <glm\ext.hpp>

using namespace Physics;

bool Collider::Intersects(Collider * p_other, IntersectInfo * p_iinfo)
{
	if (GetType() == Type::SPHERE)
	{
		if (p_other->GetType() == Type::SPHERE)
		{
			return SphereToSphereIntersect((SphereCollider*)this, (SphereCollider*)p_other, p_iinfo);
		}

		else if (p_other->GetType() == Type::AABB)
		{

		}

		else if (p_other->GetType() == Type::PLANE)
		{
			SphereToPlaneIntersect((SphereCollider*)this, (PlaneCollider*)p_other, p_iinfo);
		}
	}

	else if (GetType() == Type::AABB)
	{
		if (p_other->GetType() == Type::SPHERE)
		{

		}

		else if (p_other->GetType() == Type::AABB)
		{

		}

		else if (p_other->GetType() == Type::PLANE)
		{

		}
	}

	else if (GetType() == Type::PLANE)
	{
		if (p_other->GetType() == Type::SPHERE)
		{
			SphereToPlaneIntersect((SphereCollider*)p_other, (PlaneCollider*)this, p_iinfo);
		}

		else if (p_other->GetType() == Type::AABB)
		{

		}

		else if (p_other->GetType() == Type::PLANE)
		{

		}
	}

	return false;
}

bool Collider::SphereToSphereIntersect(SphereCollider *p_col1, SphereCollider *p_col2, Collider::IntersectInfo *iinfo)
{
	vec3 diff = p_col1->GetPosition() - p_col2->GetPosition();
	float distance = glm::length(diff);

	float minDistance = p_col1->GetRadius() + p_col2->GetRadius();

	if (distance < minDistance)
	{
		// Intersecting
		if (iinfo != nullptr)
		{
			iinfo->m_collisionVec = glm::normalize(diff) * (minDistance - distance);
		}

		return true;
	}

	return false;
}

bool Collider::SphereToPlaneIntersect(SphereCollider *p_col1, PlaneCollider *p_col2, IntersectInfo *iinfo)
{
	p_col1->SetPosition(vec3(p_col1->GetPosition().x, 0, p_col1->GetPosition().z));

	return true;

	if (p_col1->GetPosition().y < 0.0F)
	{
		// Intersecting
		if (iinfo != nullptr)
		{
			iinfo->m_collisionVec = vec3(-9999);
		}

		return true;
	}

	return false;
}

Collider *Collider::GetNoneInstance()
{
	static Collider collider(Type::NONE);

	return &collider;
}