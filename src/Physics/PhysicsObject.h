#pragma once

#include <glm\glm.hpp>

#include "Collider.h"

using glm::vec3;

namespace Physics
{
	class Collider;

	class IPhysicsObject
	{

	public:

		IPhysicsObject();

		~IPhysicsObject();

		void Update(const float & p_deltaTime);

		void ApplyForce(const vec3 & p_force);

		// getters/setters

		inline void SetPosition(const vec3 & p_position)			{ m_position = p_position; }
		inline void SetVelocity(const vec3 & p_velocity)			{ m_velocity = p_velocity; }
		inline void SetAcceleration(const vec3 & p_acceleration)	{ m_acceleration = p_acceleration; }
		inline void SetMass(const float & p_mass)					{ m_mass = p_mass; m_invMass = 1.0F / p_mass; }
		inline void SetBounciness(const float & p_bounciness)			{ m_bounciness = p_bounciness; }
		inline void SetDampening(const float & p_dampening)			{ m_dampening = p_dampening; }
		inline void SetCollider(Collider * p_collider)				{ delete m_collider; m_collider = p_collider; }
		inline void SetIsStatic(const bool & p_static)				{ m_static = p_static; }
		
		inline const vec3 & GetPosition() const						{ return m_position; }
		inline const vec3 & GetVelocity() const						{ return m_velocity; }
		inline const vec3 & GetAcceleration() const					{ return m_acceleration; }
		inline const float & GetMass() const						{ return m_mass; }
		inline const float & GetInverseMass() const					{ return m_invMass; }
		inline const float & GetBounciness() const					{ return m_bounciness; }
		inline const float & GetDampening() const					{ return m_dampening; }
		Collider *GetCollider();
		inline const bool & GetIsStatic() const						{ return m_static; }

	protected:

		vec3 m_position;
		vec3 m_velocity;
		vec3 m_acceleration;

		float m_mass;
		float m_invMass;

		float m_bounciness;
		float m_dampening;

		Collider *m_collider;

		bool m_static;

	private:

	};

}	// Physics namespace