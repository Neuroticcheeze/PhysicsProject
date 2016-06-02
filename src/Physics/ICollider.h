#pragma once

#include <glm\vec3.hpp>

using glm::vec3;

namespace Physics
{
	class SphereCollider;
	class PlaneCollider;
	class PointCollider;
	class AABBCollider;

	class IPhysicsObject;

	class ICollider
	{
	public:

		enum class Type;
		struct IntersectInfo;

	public:

		ICollider(const Type & p_type) : m_type(p_type) {}

		inline const Type & GetType() const { return m_type; }

		bool Intersects(ICollider * p_other, IntersectInfo * p_iinfo = nullptr);

		virtual void Transform(IPhysicsObject *p_owner) {};

		//Temporary collider checkers
		static bool SphereToSphereIntersect(SphereCollider *p_col1, SphereCollider *p_col2, IntersectInfo *iinfo);
		static bool SphereToPlaneIntersect(SphereCollider *p_col1, PlaneCollider *p_col2, IntersectInfo *iinfo);
		static bool SphereToPointIntersect(SphereCollider *p_col1, PointCollider *p_col2, IntersectInfo *iinfo);
		static bool PointToPlaneIntersect(PointCollider *p_col1, PlaneCollider *p_col2, IntersectInfo *iinfo);
		static bool AABBToSphereIntersect(AABBCollider *p_col1, SphereCollider *p_col2, IntersectInfo *iinfo);
		static bool AABBToPlaneIntersect(AABBCollider *p_col1, PlaneCollider *p_col2, IntersectInfo *iinfo);
		static bool AABBToAABBIntersect(AABBCollider *p_col1, AABBCollider *p_col2, IntersectInfo *iinfo);
		static bool AABBToPointIntersect(AABBCollider *p_col1, PointCollider *p_col2, IntersectInfo *iinfo);

		static ICollider *GetNoneInstance();

	protected:

	private:

		Type m_type;

	};

	enum class ICollider::Type
	{
		NONE,
		SPHERE,
		AABB,
		PLANE,
		POINT
	};


	struct ICollider::IntersectInfo
	{
		vec3 m_collisionVec;
		float m_pushFactor;
	};
}