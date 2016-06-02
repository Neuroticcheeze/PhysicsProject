#include "ICollider.h"

#include "PlaneCollider.h"
#include "SphereCollider.h"
#include "PointCollider.h"
#include "AABBCollider.h"

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
			return AABBToSphereIntersect((AABBCollider*)p_other, (SphereCollider*)this, p_iinfo);
		}

		else if (p_other->GetType() == Type::PLANE)
		{
			return SphereToPlaneIntersect((SphereCollider*)this, (PlaneCollider*)p_other, p_iinfo);
		}

		else if (p_other->GetType() == Type::POINT)
		{
			return SphereToPointIntersect((SphereCollider*)this, (PointCollider*)p_other, p_iinfo);
		}
	}

	else if (GetType() == Type::AABB)
	{
		if (p_other->GetType() == Type::SPHERE)
		{
			return AABBToSphereIntersect((AABBCollider*)this, (SphereCollider*)p_other, p_iinfo);
		}

		else if (p_other->GetType() == Type::AABB)
		{
			return AABBToAABBIntersect((AABBCollider*)this, (AABBCollider*)p_other, p_iinfo);
		}

		else if (p_other->GetType() == Type::PLANE)
		{
			return AABBToPlaneIntersect((AABBCollider*)this, (PlaneCollider*)p_other, p_iinfo);
		}

		else if (p_other->GetType() == Type::POINT)
		{
			return AABBToPointIntersect((AABBCollider*)this, (PointCollider*)p_other, p_iinfo);
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
			return AABBToPlaneIntersect((AABBCollider*)p_other, (PlaneCollider*)this, p_iinfo);
		}

		else if (p_other->GetType() == Type::PLANE)
		{

		}

		else if (p_other->GetType() == Type::POINT)
		{
			return PointToPlaneIntersect((PointCollider*)p_other, (PlaneCollider*)this, p_iinfo);
		}
	}

	else if (GetType() == Type::POINT)
	{
		if (p_other->GetType() == Type::SPHERE)
		{
			return SphereToPointIntersect((SphereCollider*)p_other, (PointCollider*)this, p_iinfo);
		}

		else if (p_other->GetType() == Type::AABB)
		{
			return AABBToPointIntersect((AABBCollider*)p_other, (PointCollider*)this, p_iinfo);
		}

		else if (p_other->GetType() == Type::PLANE)
		{
			return PointToPlaneIntersect((PointCollider*)this, (PlaneCollider*)p_other, p_iinfo);
		}

		//Points cannot collide with each other, which is why there are no Point-Point intersection check.
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

bool ICollider::SphereToPointIntersect(SphereCollider *p_col1, PointCollider *p_col2, IntersectInfo *iinfo)
{
	vec3 diff = p_col1->GetPosition() - p_col2->GetPosition();
	float distance = glm::length(diff);

	float minDistance = p_col1->GetRadius();

	if (distance < minDistance)
	{
		// Intersecting
		if (iinfo != nullptr)
		{
			iinfo->m_collisionVec = glm::normalize(diff);
			iinfo->m_pushFactor = 0.0F;
		}

		return true;
	}

	return false;
}

bool ICollider::PointToPlaneIntersect(PointCollider *p_col1, PlaneCollider *p_col2, IntersectInfo *iinfo)
{
	vec4 plane(p_col2->GetNormal(), p_col2->GetDistance());

	vec3 planeNormal = vec3(plane);

	float d = glm::dot(planeNormal, p_col1->GetPosition()) - plane.w;

	if (d < 0.1F)
	{
		// Intersecting
		if (iinfo != nullptr)
		{
			iinfo->m_collisionVec = planeNormal;
			iinfo->m_pushFactor = 0.1F;
		}

		return true;
	}

	return false;
}



bool PointAbovePlane(vec3 normal, float dist, vec3 pointRel, float * p_dist = nullptr)
{
	float d = glm::dot(normal, pointRel) - dist;

	if (p_dist != nullptr) *p_dist = d;

	return d >= 0.0F;
}
bool BoxIntersectsSphere(vec3 Bmin, vec3 Bmax, vec3 C, float r, float & diff)
{
	float r2 = r * r;
	float dmin = 0;
	for (int i = 0; i < 3; ++i) {
		if (C[i] < Bmin[i]) dmin += (C[i] - Bmin[i])*(C[i] - Bmin[i]);
		else if (C[i] > Bmax[i]) dmin += (C[i] - Bmax[i])*(C[i] - Bmax[i]);
	}

	diff = glm::sqrt(r2) - glm::sqrt(dmin);

	return dmin <= r2;
}
bool ICollider::AABBToSphereIntersect(AABBCollider *p_col1, SphereCollider *p_col2, IntersectInfo *iinfo)
{
	vec3 diff = p_col2->GetPosition() - p_col1->GetPosition();

	vec3 P = glm::normalize(diff);
	P /= glm::max(glm::max(P.x, P.y), P.z);
	P *= glm::min(glm::min(p_col1->GetHalfExtents().x, p_col1->GetHalfExtents().y), p_col1->GetHalfExtents().z);

	float aabbRadius = glm::length(P);

	float distance = glm::length(diff);

	float minDistance =  + p_col2->GetRadius();

	//if (distance < minDistance)
	float t = 0.0F;
	if (BoxIntersectsSphere(p_col1->GetPosition() - p_col1->GetHalfExtents(), p_col1->GetPosition() + p_col1->GetHalfExtents(), p_col2->GetPosition(), p_col2->GetRadius(), t))
	{
		// Intersecting
		if (iinfo != nullptr)
		{
			//Calculate normal
			vec3 norm(0, 1, 0);
			{
				if (PointAbovePlane(vec3(+1, +0, +0), p_col1->GetHalfExtents().x, diff)) norm = vec3(+1, +0, +0);
				else if (PointAbovePlane(vec3(+0, +1, +0), p_col1->GetHalfExtents().y, diff)) norm = vec3(+0, +1, +0);
				else if (PointAbovePlane(vec3(+0, +0, +1), p_col1->GetHalfExtents().z, diff)) norm = vec3(+0, +0, +1);
				else if (PointAbovePlane(vec3(-1, +0, +0), p_col1->GetHalfExtents().x, diff)) norm = vec3(-1, +0, +0);
				else if (PointAbovePlane(vec3(+0, -1, +0), p_col1->GetHalfExtents().y, diff)) norm = vec3(+0, -1, +0);
				else if (PointAbovePlane(vec3(+0, +0, -1), p_col1->GetHalfExtents().z, diff)) norm = vec3(+0, +0, -1);
			}

			iinfo->m_collisionVec = glm::normalize(norm);
			iinfo->m_pushFactor = t;
		}

		return true;
	}

	return false;
}

bool ICollider::AABBToPlaneIntersect(AABBCollider *p_col1, PlaneCollider *p_col2, IntersectInfo *iinfo)
{
	vec4 plane(p_col2->GetNormal(), p_col2->GetDistance());

	vec3 planeNormal = vec3(plane);

	float d = glm::dot(planeNormal, p_col1->GetPosition()) - plane.w;

	float aabbRadius = abs(planeNormal.x*p_col1->GetHalfExtents().x) + abs(planeNormal.y*p_col1->GetHalfExtents().y) + abs(planeNormal.z*p_col1->GetHalfExtents().z);

	if (d < aabbRadius)
	{
		// Intersecting
		if (iinfo != nullptr)
		{
			iinfo->m_collisionVec = planeNormal;
			iinfo->m_pushFactor = glm::abs(aabbRadius - d);
		}

		return true;
	}

	return false;
}

bool ICollider::AABBToAABBIntersect(AABBCollider *p_col1, AABBCollider *p_col2, IntersectInfo *iinfo)
{
	return false;
}

bool ICollider::AABBToPointIntersect(AABBCollider *p_col1, PointCollider *p_col2, IntersectInfo *iinfo)
{
	vec3 diff = p_col2->GetPosition() - p_col1->GetPosition();

	vec3 P = glm::normalize(diff);
	P /= glm::max(glm::max(P.x, P.y), P.z);
	P *= glm::min(glm::min(p_col1->GetHalfExtents().x, p_col1->GetHalfExtents().y), p_col1->GetHalfExtents().z);

	float aabbRadius = glm::length(P);

	float distance = glm::length(diff);

	float minDistance = 0.1F;

	//if (distance < minDistance)
	float t = 0.0F;
	if (BoxIntersectsSphere(p_col1->GetPosition() - p_col1->GetHalfExtents(), p_col1->GetPosition() + p_col1->GetHalfExtents(), p_col2->GetPosition(), 0.1F, t))
	{
		// Intersecting
		if (iinfo != nullptr)
		{
			//Calculate normal
			vec3 norm(0, 1, 0);
			{
				if (PointAbovePlane(vec3(+1, +0, +0), p_col1->GetHalfExtents().x, diff)) norm = vec3(+1, +0, +0);
				else if (PointAbovePlane(vec3(+0, +1, +0), p_col1->GetHalfExtents().y, diff)) norm = vec3(+0, +1, +0);
				else if (PointAbovePlane(vec3(+0, +0, +1), p_col1->GetHalfExtents().z, diff)) norm = vec3(+0, +0, +1);
				else if (PointAbovePlane(vec3(-1, +0, +0), p_col1->GetHalfExtents().x, diff)) norm = vec3(-1, +0, +0);
				else if (PointAbovePlane(vec3(+0, -1, +0), p_col1->GetHalfExtents().y, diff)) norm = vec3(+0, -1, +0);
				else if (PointAbovePlane(vec3(+0, +0, -1), p_col1->GetHalfExtents().z, diff)) norm = vec3(+0, +0, -1);
			}

			iinfo->m_collisionVec = glm::normalize(norm);
			iinfo->m_pushFactor = t;
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