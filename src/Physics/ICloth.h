#pragma once

#include "IPhysicsObject.h"

namespace Physics
{
	class PhysicsSceneRenderer;
	class PhysicsScene;
	class Spring;

	class ICloth
	{

	public:

		ICloth(
			PhysicsScene * p_scene,
			PhysicsSceneRenderer * p_renderer = nullptr,
			const unsigned int & p_nodeWidth = 10, 
			const unsigned int & p_nodeHeight = 20, 
			const float & p_fabricLength = 2.0F, 
			const float & p_fabricStiffness = 100.0F, 
			const float & p_fabricFriction = 0.7F, 
			const float & p_fabricStrength = 1500.0F, 
			const float & p_mass = 0.5F, 
			const float & p_bounciness = 0.1F, 
			const float & p_friction = 70.0F);

		virtual ~ICloth();

		inline void SetHook(const unsigned int & p_index)
		{
			assert(p_index < m_nodeCount);

			m_nodes[p_index]->SetIsStatic(true);
		}

		inline IPhysicsObject ** GetNodes() const { return m_nodes; }
		inline const unsigned int & GetWidth() const { return m_width; }
		inline const unsigned int & GetHeight() const { return m_height; }

	protected:

	private:

		unsigned int m_width, m_height, m_nodeCount, m_strandCount;
		Spring ** m_strands;
		IPhysicsObject ** m_nodes;
	};
}