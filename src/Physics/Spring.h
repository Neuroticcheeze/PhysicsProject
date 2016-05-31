#pragma once

#include "IConstraint.h"

namespace Physics
{
	class IPhysicsObject;

	class Spring : public IConstraint
	{
	public:

		Spring(IPhysicsObject * p_obj1, IPhysicsObject * p_obj2);
		Spring(IPhysicsObject * p_obj1, IPhysicsObject * p_obj2, float p_springLength, float p_springStiffness, float p_springFriction, float p_strength);
		~Spring();

		void Contstrain(PhysicsScene * p_scene);

	protected:

		float m_springLength;
		float m_springStiffness;
		float m_springFriction;
		float m_strength;

	private:

	};
}