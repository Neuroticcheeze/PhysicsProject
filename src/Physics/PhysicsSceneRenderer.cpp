#include "PhysicsSceneRenderer.h"
#include "PhysicsScene.h"
#include "IPhysicsObject.h"
#include "IConstraint.h"
#include "ICloth.h"
#include "../Gizmos.h"
#include <glm/vec4.hpp>
#include <glm/glm.hpp>

#include "SphereCollider.h"
#include "PlaneCollider.h"
#include "AABBCollider.h"
#include "ICollider.h"

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


	auto & cloths = p_scene->GetCloths();
	for (auto iter = cloths.begin(); iter != cloths.end(); ++iter)
	{
		ICloth *cloth = (*iter);
		IPhysicsObject ** nodes = cloth->GetNodes();

		for (unsigned int r = 0; r < cloth->GetHeight() - 1; ++r)
		{
			for (unsigned int c = 0; c < cloth->GetWidth() - 1; ++c)
			{
				{
					auto obj0 = nodes[r * cloth->GetWidth() + c];
					auto obj1 = nodes[(r + 1) * cloth->GetWidth() + c];
					auto obj2 = nodes[(r + 1) * cloth->GetWidth() + (c + 1)];

					auto col0 = GetRenderInfo(obj0).m_colour;
					auto col1 = GetRenderInfo(obj1).m_colour;
					auto col2 = GetRenderInfo(obj2).m_colour;

					auto col = glm::mix(col0, glm::mix(col1, col2, 0.5F), 0.6667F);

					col.a = 1.0F;

					if (!(obj0->GetHasConstraintBroke() || obj1->GetHasConstraintBroke() || obj2->GetHasConstraintBroke()))
					Gizmos::addTri(obj0->GetPosition(), obj1->GetPosition(), obj2->GetPosition(), col);
				}

				{
					auto obj0 = nodes[r * cloth->GetWidth() + c];
					auto obj1 = nodes[(r + 1) * cloth->GetWidth() + (c + 1)];
					auto obj2 = nodes[r * cloth->GetWidth() + (c + 1)];

					auto col0 = GetRenderInfo(obj0).m_colour;
					auto col1 = GetRenderInfo(obj1).m_colour;
					auto col2 = GetRenderInfo(obj2).m_colour;

					auto col = glm::mix(col0, glm::mix(col1, col2, 0.5F), 0.6667F);

					col.a = 1.0F;

					if (!(obj0->GetHasConstraintBroke() || obj1->GetHasConstraintBroke() || obj2->GetHasConstraintBroke()))
					Gizmos::addTri(obj0->GetPosition(), obj1->GetPosition(), obj2->GetPosition(), col);
				}
			}
		}
	}


	int activeObjects = 0;
	auto & objects = p_scene->GetPhysicsObjects();
	for (auto iter = objects.begin(); iter != objects.end(); ++iter)
	{
		IPhysicsObject *obj = (*iter);

		activeObjects += obj->GetIsAwake();

		RenderInfo &info = GetRenderInfo(obj);

		auto col = info.m_colour;

		if (col.a == 0.0F)
			continue;

		ICollider * collider = obj->GetCollider();

		switch (collider->GetType())
		{
		case ICollider::Type::SPHERE:
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

		case ICollider::Type::PLANE:
		{
			vec3 planeNormal = ((PlaneCollider*)collider)->GetNormal();
			float planeDistance = ((PlaneCollider*)collider)->GetDistance();

			glm::mat4 normMatrix = glm::lookAt(vec3(0), planeNormal, vec3(0.000001F, 1, 0));

			vec3 corners[4] = 
			{
				vec3(vec4(-9999, -9999, -planeDistance, 1) * normMatrix),
				vec3(vec4(-9999, +9999, -planeDistance, 1) * normMatrix),
				vec3(vec4(+9999, +9999, -planeDistance, 1) * normMatrix),
				vec3(vec4(+9999, -9999, -planeDistance, 1) * normMatrix),
			};

			Gizmos::addTri(corners[0], corners[1], corners[2], col);
			Gizmos::addTri(corners[0], corners[2], corners[3], col);
		}
			break;

		case ICollider::Type::AABB:
		{
			const vec3 & position = ((AABBCollider*)collider)->GetPosition();
			const vec3 & extends = ((AABBCollider*)collider)->GetHalfExtents();

			Gizmos::addAABBFilled(position, extends, col);
		}
			break;
		}
	}

	auto & constraints = p_scene->GetConstraints();
	for (auto iter = constraints.begin(); iter != constraints.end(); ++iter)
	{
		IConstraint *con = (*iter);

		auto col0 = GetRenderInfo(con->GetObject1()).m_colour;
/*
		if (col0.a == 0.0F)
			continue;*/

		auto col1 = GetRenderInfo(con->GetObject1()).m_colour;
/*
		if (col1.a == 0.0F)
			continue;*/

		auto col = glm::mix(col0, col1, 0.5F);
		col.a = 1.0F;

		Gizmos::addLine(con->GetObject1()->GetPosition(), con->GetObject2()->GetPosition(), col);
	}

	printf("Physics Objects: %d [AWAKE] / %d [TOTAL]\n", activeObjects, objects.size());
}

PhysicsSceneRenderer::RenderInfo &PhysicsSceneRenderer::GetRenderInfo(IPhysicsObject *p_obj)
{
	return m_renderInfo[p_obj];
}