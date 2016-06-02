#pragma once

#include "IConstraint.h"

namespace Physics
{
	class IPhysicsObject;

	class Spring : public IConstraint
	{
	public:

		Spring(IPhysicsObject * p_obj1, IPhysicsObject * p_obj2);
		Spring(IPhysicsObject * p_obj1, IPhysicsObject * p_obj2, float p_springLength, float p_springLengthLimit, float p_springStiffness, float p_springFriction, float p_springStrength);
		~Spring();

		void Contstrain(PhysicsScene * p_scene, const float & p_deltaTime);

	protected:

		float m_springLength;
		float m_springLengthLimit;
		float m_springStiffness;
		float m_springFriction;
		float m_springStrength;

	private:

	};
}