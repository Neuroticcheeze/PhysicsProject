#pragma once

#include "ICollider.h"

#include <vector>
#include <glm\vec3.hpp>

using glm::vec3;

namespace Physics
{
	class ICloth;
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

		// Cloths ----------------------------------------------------------------
		template <class T, typename ... TArgs>
		T *CreateCloth(TArgs ... p_args)
		{
			static_assert(std::is_base_of<ICloth, T>::value, "CreateCloth");

			T *pObject = new T(this, p_args ...);

			m_cloths.push_back(pObject);

			return pObject;
		}

		void DestroyCloth(ICloth *p_cloth);
		// -----------------------------------------------------------------------

		const std::vector<IPhysicsObject *> &GetPhysicsObjects() const
		{
			return m_physicsObjects;
		}

		const std::vector<IConstraint *> &GetConstraints() const
		{
			return m_constraints;
		}

		const std::vector<ICloth *> &GetCloths() const
		{
			return m_cloths;
		}

		void SetGravity(const vec3 &p_gravity) { m_gravity = p_gravity; }
		const vec3 &GetGravity() const { return m_gravity;  }

	private:
		
		void DetectCollisions();
		void ResolveCollisions(float p_deltaTime);

	private:

		struct CollisionInfo
		{
			IPhysicsObject *m_partner1;
			IPhysicsObject *m_partner2;
			ICollider::IntersectInfo m_intersectInfo;
		};

		std::vector<IPhysicsObject *>	m_physicsObjects;
		std::vector<ICloth *>			m_cloths;
		std::vector<IConstraint *>		m_constraints;
		std::vector<CollisionInfo>		m_collisions;

		vec3 m_gravity;


	};
}