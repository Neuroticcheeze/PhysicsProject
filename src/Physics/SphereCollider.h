#pragma once

#include "Collider.h"
#include <glm\vec3.hpp>

using glm::vec3;


#include "IPhysicsObject.h"
namespace Physics
{
	class SphereCollider : public Collider
	{
	public:

		SphereCollider(const float & p_radius = 0.0F, const vec3 & p_position = vec3(0)) : Collider(Type::SPHERE), m_position(p_position), m_radius(p_radius) {}

		virtual ~SphereCollider() {}

		void Transform(IPhysicsObject *p_owner)
		{
			m_position = p_owner->GetPosition();
		}

		inline void SetPosition(const vec3 & p_position)	{ m_position = p_position; }
		inline void SetRadius(const float & p_radius)		{ m_radius = p_radius; }
		inline const vec3 & GetPosition() const				{ return m_position; }
		inline const float & GetRadius() const				{ return m_radius; }

	protected:

		vec3 m_position;
		float m_radius;

	private:
	};
}