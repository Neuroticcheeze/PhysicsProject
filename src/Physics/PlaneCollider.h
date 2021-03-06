#pragma once

#include "ICollider.h"
#include <glm\vec3.hpp>

using glm::vec3;


#include "IPhysicsObject.h"
namespace Physics
{
	class PlaneCollider : public ICollider
	{
	public:

		PlaneCollider(const vec3 & p_normal = vec3(0.0F, 1.0F, 0.0F), const float & p_distance = 0.0F) : ICollider(Type::PLANE), m_normal(glm::normalize(p_normal)), m_distance(p_distance) {}

		virtual ~PlaneCollider() {}

		void Transform(IPhysicsObject *p_owner)
		{
		}

		inline void SetNormal(const vec3 & p_normal)		{ m_normal = p_normal; }
		inline void SetDistance(const float & p_distance)	{ m_distance = p_distance; }
		inline const vec3 & GetNormal() const				{ return m_normal; }
		inline const float & GetDistance() const			{ return m_distance; }

	protected:

		vec3 m_normal;
		float m_distance;

	private:
	};
}