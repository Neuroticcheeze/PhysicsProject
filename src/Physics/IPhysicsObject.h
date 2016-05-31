#pragma once

#include <glm\glm.hpp>
#include <vector>

#include "ICollider.h"

using glm::vec3;

namespace Physics
{
	class ICollider;

	class IPhysicsObject
	{
		friend class IConstraint;

	public:

		IPhysicsObject();

		~IPhysicsObject();

		void Update(const float & p_deltaTime);

		void ApplyForce(const vec3 & p_force);

		// getters/setters

		inline void SetPosition(const vec3 & p_position)			{ m_position = p_position; }
		inline void SetPositionDelta(const vec3 & p_positionDelta)	{ m_positionDelta = p_positionDelta; }
		inline void SetVelocity(const vec3 & p_velocity)			{ m_velocity = p_velocity; }
		inline void SetAcceleration(const vec3 & p_acceleration)	{ m_acceleration = p_acceleration; }
		inline void SetMass(const float & p_mass)					{ m_mass = p_mass; m_invMass = 1.0F / p_mass; }
		inline void SetBounciness(const float & p_bounciness)		{ m_bounciness = p_bounciness; }
		inline void SetFriction(const float & p_friction)			{ m_friction = p_friction; }
		inline void SetCollider(ICollider * p_collider)				{ delete m_collider; m_collider = p_collider; }
		inline void SetIsStatic(const bool & p_static)				{ m_static = p_static; }
		inline void Wake()											{ m_awake = true; }
		inline void Sleep()											{ m_awake = false; }
		
		inline const vec3 & GetPosition() const						{ return m_position; }
		inline const vec3 & GetVelocity() const						{ return m_velocity; }
		inline const vec3 & GetAcceleration() const					{ return m_acceleration; }
		inline const float & GetMass() const						{ return m_mass; }
		inline const float & GetInverseMass() const					{ return m_invMass; }
		inline const float & GetBounciness() const					{ return m_bounciness; }
		inline const float & GetFriction() const					{ return m_friction; }
		inline const bool & GetIsStatic() const						{ return m_static; }
		inline const bool & GetIsAwake() const						{ return m_awake; }
		inline const vec3 & GetPositionDelta() const				{ return m_positionDelta; }

		inline const bool & Destroyed() const						{ return m_destroyed; }
		inline void Destroy()										{ m_destroyed = true; }

		inline const std::vector<IConstraint *> & GetInvolvedConstraints() const 
																	{ return m_involvedConstraints; }
		ICollider *GetCollider();

	private:

		vec3 m_position, m_positionDelta;
		vec3 m_velocity;
		vec3 m_acceleration;

		float m_mass;
		float m_invMass;

		float m_bounciness;
		float m_friction;

		ICollider *m_collider;

		bool m_static;
		bool m_awake;

		bool m_destroyed;

		std::vector<IConstraint *> m_involvedConstraints;
	};

}	// Physics namespace