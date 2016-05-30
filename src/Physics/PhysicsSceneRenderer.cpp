#include "PhysicsSceneRenderer.h"
#include "PhysicsScene.h"
#include "PhysicsObject.h"
#include "../Gizmos.h"
#include <glm/vec4.hpp>

#include "SphereCollider.h"
#include "Collider.h"

#include "..\Camera.h"

#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtx\quaternion.hpp>

using glm::vec4;

using namespace Physics;

PhysicsSceneRenderer::PhysicsSceneRenderer()
{

}

PhysicsSceneRenderer::~PhysicsSceneRenderer()
{

}


void PhysicsSceneRenderer::Render(PhysicsScene *p_scene, Camera *p_camera)
{
	vec4 planes[6];
	p_camera->getFrustumPlanes(planes);

	auto & objects = p_scene->GetPhysicsObjects();

	int activeObjects = 0;

	for (auto iter = objects.begin(); iter != objects.end(); ++iter)
	{
		IPhysicsObject *obj = (*iter);

		activeObjects += obj->GetIsAwake();

		RenderInfo &info = GetRenderInfo(obj);

		Collider * collider = obj->GetCollider();

		if (collider->GetType() == Collider::Type::SPHERE)
		{
			float r = ((SphereCollider*)collider)->GetRadius();

			bool hidden = false;
			for (int i = 0; i < 6; i++) {
				float d = glm::dot(vec3(planes[i]), obj->GetPosition()) +
					planes[i].w;
				if (d < -r)
				{
					hidden = true;
					continue;
				}
			}

			if (hidden)
				continue;

			auto col = info.m_colour;

			if (obj->GetIsAwake())
			{
				int j = glm::clamp((int)(r * 4), 2, 4);
				Gizmos::addSphere(obj->GetPosition(), r, j, j * 2, col);
			}

			else
			{
				float r = ((SphereCollider*)collider)->GetRadius();
				Gizmos::addAABBFilled(obj->GetPosition(), vec3(r * 0.875F), col);
			}
		}
	}

	printf("Physics Objects: %d [AWAKE] / %d [TOTAL]\n", activeObjects, objects.size());
}

PhysicsSceneRenderer::RenderInfo &PhysicsSceneRenderer::GetRenderInfo(IPhysicsObject *p_obj)
{
	return m_renderInfo[p_obj];
}