#pragma once

#include "Collider.h"

#include <vector>
#include <glm\vec3.hpp>

using glm::vec3;

namespace Physics
{
	class IPhysicsObject;

	class PhysicsScene
	{

	public:

		// Constructors/ Destructors ---------------------------------------------
		PhysicsScene();
		
		virtual ~PhysicsScene();
		// -----------------------------------------------------------------------

	public:

		void Simulate(const float &p_deltaTime);

		// Factory methods -------------------------------------------------------
		template <class T, typename ... TArgs>
		T *CreatePhysicsObject(TArgs ... p_args)
		{
			static_assert(std::is_base_of<IPhysicsObject, T>::value, "CreatePhysicsObject");

			T *pObject = new T(p_args ...);

			m_physicsObjects.push_back(pObject);

			return pObject;
		}

		void DestroyPhysicsObject(IPhysicsObject *p_physicsObject);
		// -----------------------------------------------------------------------

		const std::vector<IPhysicsObject *> &GetPhysicsObjects() const
		{
			return m_physicsObjects;
		}

		void SetGravity(const vec3 &p_gravity) { m_gravity = p_gravity; }
		const vec3 &GetGravity() const { return m_gravity;  }

	protected:
		
		void DetectCollisions();
		void ResolveCollisions();

	protected:

		struct CollisionInfo
		{
			IPhysicsObject *m_partner1;
			IPhysicsObject *m_partner2;
			Collider::IntersectInfo m_intersectInfo;
		};

		std::vector<IPhysicsObject *> m_physicsObjects;
		std::vector<CollisionInfo> m_collisions;

		vec3 m_gravity;

	private:

	};
}