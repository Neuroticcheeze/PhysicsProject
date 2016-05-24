#pragma once

#include "Collider.h"
#include <glm\vec3.hpp>

using glm::vec3;


#include "PhysicsObject.h"
namespace Physics
{
	class PlaneCollider : public Collider
	{
	public:

		PlaneCollider(const vec3 & p_normal = vec3(0.0F, 1.0F, 0.0F)) : Collider(Type::PLANE), m_normal(p_normal) {}

		virtual ~PlaneCollider() {}

		void Transform(IPhysicsObject *p_owner)
		{
		}

		inline void SetNormal(const vec3 & p_normal) { m_normal = p_normal; }
		inline const vec3 & GetNormal() const { return m_normal; }

	protected:

		vec3 m_normal;

	private:
	};
}