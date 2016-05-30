#pragma once

#include "Collider.h"

#include <vector>
#include <glm\vec3.hpp>

using glm::vec3;

namespace Physics
{
	class IConstraint;
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

		// Game objects ----------------------------------------------------------
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

		// Constraints -----------------------------------------------------------
		template <class T, typename ... TArgs>
		T *CreateConstraint(TArgs ... p_args)
		{
			static_assert(std::is_base_of<IConstraint, T>::value, "CreateConstraint");

			T *pObject = new T(p_args ...);

			m_constraints.push_back(pObject);

			return pObject;
		}

		void DestroyConstraint(IConstraint *p_constraint);
		// -----------------------------------------------------------------------

		const std::vector<IPhysicsObject *> &GetPhysicsObjects() const
		{
			return m_physicsObjects;
		}

		const std::vector<IConstraint *> &GetConstraints() const
		{
			return m_constraints;
		}

		void SetGravity(const vec3 &p_gravity) { m_gravity = p_gravity * 10.0F; }
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
		std::vector<IConstraint *> m_constraints;
		std::vector<CollisionInfo> m_collisions;

		vec3 m_gravity;

	private:

	};
}