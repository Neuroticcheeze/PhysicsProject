#pragma once

#include <unordered_map>
#include <glm/vec4.hpp>

using glm::vec4;

namespace Physics
{
	class PhysicsScene;
	class IPhysicsObject;

	class PhysicsSceneRenderer
	{

	public:

		struct RenderInfo
		{
			vec4 m_colour;
		};

		PhysicsSceneRenderer();
		virtual ~PhysicsSceneRenderer();

		void Render(PhysicsScene *p_scene);

		RenderInfo &GetRenderInfo(IPhysicsObject *p_obj);

	protected:

		std::unordered_map<IPhysicsObject*, RenderInfo> m_renderInfo;

	private:

	};
}