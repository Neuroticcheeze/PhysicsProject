#pragma once

#include <glm\vec3.hpp>

using glm::vec3;

namespace Physics
{
	class SphereCollider;
	class PlaneCollider;

	class IPhysicsObject;

	class Collider
	{
	public:

		enum class Type;
		struct IntersectInfo;

	public:

		Collider(const Type & p_type) : m_type(p_type) {}

		inline const Type & GetType() const { return m_type; }

		bool Intersects(Collider * p_other, IntersectInfo * p_iinfo = nullptr);

		virtual void Transform(IPhysicsObject *p_owner) {};

		//Temporary collider checkers
		static bool SphereToSphereIntersect(SphereCollider *p_col1, SphereCollider *p_col2, IntersectInfo *iinfo);
		static bool SphereToPlaneIntersect(SphereCollider *p_col1, PlaneCollider *p_col2, IntersectInfo *iinfo);

		static Collider *GetNoneInstance();

	protected:

	private:

		Type m_type;

	};

	enum class Collider::Type
	{
		NONE,
		SPHERE,
		AABB,
		PLANE
	};


	struct Collider::IntersectInfo
	{
		vec3 m_collisionVec;
	};
}