#pragma once

#include "ICollider.h"
#include <glm\vec3.hpp>

using glm::vec3;


#include "IPhysicsObject.h"
namespace Physics
{
	class AABBCollider : public ICollider
	{
	public:

		AABBCollider(const vec3 & p_position = vec3(0)) : ICollider(Type::AABB), m_position(p_position) {}

		virtual ~AABBCollider() {}

		void Transform(IPhysicsObject *p_owner)
		{
			m_position = p_owner->GetPosition();
		}

		inline void SetPosition(const vec3 & p_position) { m_position = p_position; }
		inline const vec3 & GetPosition() const { return m_position; }

	protected:

		vec3 m_position;

	private:
	};
}