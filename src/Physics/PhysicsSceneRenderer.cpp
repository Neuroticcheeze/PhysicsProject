#include "PhysicsSceneRenderer.h"
#include "PhysicsScene.h"
#include "PhysicsObject.h"
#include "../Gizmos.h"
#include <glm/vec4.hpp>

#include "SphereCollider.h"

#include "Collider.h"

using glm::vec4;

using namespace Physics;

PhysicsSceneRenderer::PhysicsSceneRenderer()
{

}

PhysicsSceneRenderer::~PhysicsSceneRenderer()
{

}


void PhysicsSceneRenderer::Render(PhysicsScene *p_scene)
{
	auto & objects = p_scene->GetPhysicsObjects();

	for (auto iter = objects.begin(); iter != objects.end(); ++iter)
	{
		IPhysicsObject *obj = (*iter);

		RenderInfo &info = GetRenderInfo(obj);

		Collider * collider = obj->GetCollider();

		if (collider->GetType() == Collider::Type::SPHERE)
		{
			float r = ((SphereCollider*)collider)->GetRadius();

			int j = glm::clamp((int)(r * 4), 2, 4);

			Gizmos::addSphere(obj->GetPosition(), r, j, j * 2, info.m_colour);
		}
	}
}

PhysicsSceneRenderer::RenderInfo &PhysicsSceneRenderer::GetRenderInfo(IPhysicsObject *p_obj)
{
	return m_renderInfo[p_obj];
}