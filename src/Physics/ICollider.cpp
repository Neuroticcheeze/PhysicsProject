#include "ICollider.h"

#include "PlaneCollider.h"
#include "SphereCollider.h"

#include <glm\ext.hpp>

#include <glm\vec4.hpp>
using glm::vec4;

using namespace Physics;

bool ICollider::Intersects(ICollider * p_other, IntersectInfo * p_iinfo)
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
			return SphereToPlaneIntersect((SphereCollider*)this, (PlaneCollider*)p_other, p_iinfo);
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
			return SphereToPlaneIntersect((SphereCollider*)p_other, (PlaneCollider*)this, p_iinfo);
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

bool ICollider::SphereToSphereIntersect(SphereCollider *p_col1, SphereCollider *p_col2, ICollider::IntersectInfo *iinfo)
{
	vec3 diff = p_col1->GetPosition() - p_col2->GetPosition();
	float distance = glm::length(diff);

	float minDistance = p_col1->GetRadius() + p_col2->GetRadius();

	if (distance < minDistance)
	{
		// Intersecting
		if (iinfo != nullptr)
		{
			iinfo->m_collisionVec = -glm::normalize(diff);
			iinfo->m_pushFactor = minDistance - distance;
		}

		return true;
	}

	return false;
}

bool ICollider::SphereToPlaneIntersect(SphereCollider *p_col1, PlaneCollider *p_col2, IntersectInfo *iinfo)
{
	vec4 plane(p_col2->GetNormal(), p_col2->GetDistance());

	vec3 planeNormal = vec3(plane);

	float d = glm::dot(planeNormal, p_col1->GetPosition()) - plane.w;

	if (d < p_col1->GetRadius())
	{
		// Intersecting
		if (iinfo != nullptr)
		{
			iinfo->m_collisionVec = planeNormal;
			iinfo->m_pushFactor = p_col1->GetRadius() - d;
		}

		return true;
	}

	return false;
}

ICollider *ICollider::GetNoneInstance()
{
	static ICollider collider(Type::NONE);

	return &collider;
}