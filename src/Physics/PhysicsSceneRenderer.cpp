#include "PhysicsSceneRenderer.h"
#include "PhysicsScene.h"
#include "IPhysicsObject.h"
#include "IConstraint.h"
#include "../Gizmos.h"
#include <glm/vec4.hpp>
#include <glm/glm.hpp>

#include "SphereCollider.h"
#include "PlaneCollider.h"
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


	int activeObjects = 0;
	auto & objects = p_scene->GetPhysicsObjects();
	for (auto iter = objects.begin(); iter != objects.end(); ++iter)
	{
		IPhysicsObject *obj = (*iter);

		activeObjects += obj->GetIsAwake();

		RenderInfo &info = GetRenderInfo(obj);

		Collider * collider = obj->GetCollider();

		switch (collider->GetType())
		{
		case Collider::Type::SPHERE:
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
			break;

		case Collider::Type::PLANE:
		{
			vec3 planeNormal = ((PlaneCollider*)collider)->GetNormal();
			float planeDistance = ((PlaneCollider*)collider)->GetDistance();

			glm::mat4 normMatrix = glm::lookAt(vec3(0), planeNormal, vec3(0.000001F, 1, 0));

			vec3 corners[4] = 
			{
				vec3(vec4(-9999, -9999, planeDistance, 1) * normMatrix),
				vec3(vec4(-9999, +9999, planeDistance, 1) * normMatrix),
				vec3(vec4(+9999, +9999, planeDistance, 1) * normMatrix),
				vec3(vec4(+9999, -9999, planeDistance, 1) * normMatrix),
			};

			Gizmos::addTri(corners[0], corners[1], corners[2], info.m_colour);
			Gizmos::addTri(corners[0], corners[2], corners[3], info.m_colour);
		}
			break;
		}
	}

	auto & constraints = p_scene->GetConstraints();
	for (auto iter = constraints.begin(); iter != constraints.end(); ++iter)
	{
		IConstraint *con = (*iter);

		Gizmos::addLine(con->GetObject1()->GetPosition(), con->GetObject2()->GetPosition(), vec4(1.0F, 0.0F, 0.0F, 0.5F));
	}

	printf("Physics Objects: %d [AWAKE] / %d [TOTAL]\n", activeObjects, objects.size());
}

PhysicsSceneRenderer::RenderInfo &PhysicsSceneRenderer::GetRenderInfo(IPhysicsObject *p_obj)
{
	return m_renderInfo[p_obj];
}