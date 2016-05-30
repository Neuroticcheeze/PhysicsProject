#include "PhysicsScene.h"
#include "IPhysicsObject.h"
#include "IConstraint.h"

#include "SphereCollider.h"

#include "Collider.h"

#include <algorithm>

using namespace Physics;

PhysicsScene::PhysicsScene() {}

PhysicsScene::~PhysicsScene()
{
	//TODO: delete physics objects in m_physicsObjects
}

void PhysicsScene::Simulate(const float &p_deltaTime)
{
	bool anObjIsMoving = false;

	for (auto iter = m_physicsObjects.begin(); iter != m_physicsObjects.end(); ++iter)
	{
		IPhysicsObject *obj = (*iter);

		//Use as a previous position so that tthe delta can be calculated later on.
		obj->SetPositionDelta(obj->GetPosition());
	}

	for (auto iter = m_constraints.begin(); iter != m_constraints.end(); ++iter)
	{
		IConstraint *con = (*iter);

		if (!con->Destroyed())
		{
			con->Contstrain(this);
		}
	}

	for (auto iter = m_physicsObjects.begin(); iter != m_physicsObjects.end(); ++iter)
	{
		IPhysicsObject *obj = (*iter);

		obj->ApplyForce(m_gravity * obj->GetMass());

		obj->Update(p_deltaTime);

		////-----------Temporary collision-----------

		const Collider * col = obj->GetCollider();
		const vec3 & pos = obj->GetPosition();
		const vec3 & vel = obj->GetVelocity();

		if (col != nullptr && col->GetType() == Collider::Type::SPHERE)
		{
			float k = ((SphereCollider*)col)->GetRadius();

			if (pos.y < k)//On the ground
			{

				obj->ApplyForce(-obj->GetVelocity() * obj->GetDampening());

				obj->SetPosition(vec3(pos.x, k, pos.z));
				obj->SetVelocity(vec3(vel.x, -vel.y * obj->GetBounciness(), vel.z));
			}
		}
	}

	DetectCollisions();
	ResolveCollisions();

	for (auto iter = m_physicsObjects.begin(); iter != m_physicsObjects.end(); ++iter)
	{
		IPhysicsObject *obj = (*iter);

		if (glm::distance(obj->GetPosition(), obj->GetPositionDelta()) < 0.00001F)
		{
			obj->Sleep();
		}

		else
		{
			obj->Wake();
		}
	}

	//Delete any destroyed constraints.
	m_constraints.erase(std::remove_if(m_constraints.begin(), m_constraints.end(),
		[](IConstraint * p_constraint)
	{
		bool destroyed = p_constraint->Destroyed();

		if (destroyed)
			delete p_constraint;

		return destroyed;
	}
	), m_constraints.end());

	//Delete any destroyed game objects.
	m_physicsObjects.erase(std::remove_if(m_physicsObjects.begin(), m_physicsObjects.end(),
		[](IPhysicsObject * p_object)
	{
		bool destroyed = p_object->Destroyed();

		if (destroyed)
			delete p_object;

		return destroyed;
	}
	), m_physicsObjects.end());
}

void PhysicsScene::DestroyPhysicsObject(IPhysicsObject *p_physicsObject)
{
	auto iter = std::find(m_physicsObjects.begin(), m_physicsObjects.end(), p_physicsObject);

	if (iter != m_physicsObjects.end())
	{
		m_physicsObjects.erase(iter);
	}

	for (auto iter = m_constraints.begin(); iter != m_constraints.end(); ++iter)
	{
		IConstraint *con = (*iter);

		if (con->GetObject1() == p_physicsObject || con->GetObject2() == p_physicsObject)
		{
			DestroyConstraint(con);
		}
	}

	delete p_physicsObject;
}

void PhysicsScene::DestroyConstraint(IConstraint *p_constraint)
{
	auto iter = std::find(m_constraints.begin(), m_constraints.end(), p_constraint);

	if (iter != m_constraints.end())
	{
		m_constraints.erase(iter);
	}

	delete p_constraint;
}

void PhysicsScene::DetectCollisions()
{
	m_collisions.clear();

	for (auto iter0 = m_physicsObjects.begin(); iter0 != m_physicsObjects.end(); ++iter0)
	{
		for (auto iter1 = iter0 + 1; iter1 != m_physicsObjects.end(); ++iter1)
		{
			CollisionInfo cinfo;

			if (!(*iter0)->GetIsAwake() && !(*iter1)->GetIsAwake())
				continue;

			if ((*iter0)->GetCollider()->Intersects((*iter1)->GetCollider(), &cinfo.m_intersectInfo))
			{
				cinfo.m_partner1 = (*iter0);
				cinfo.m_partner2 = (*iter1);

				m_collisions.push_back(cinfo);
			}
		}
	}
}

void PhysicsScene::ResolveCollisions()
{
	for (auto iter = m_collisions.begin(); iter != m_collisions.end(); ++iter)
	{
		IPhysicsObject *obj1 = (*iter).m_partner1;
		IPhysicsObject *obj2 = (*iter).m_partner2;
		Collider::IntersectInfo &iinfo = (*iter).m_intersectInfo;

		vec3 collideNormal = -glm::normalize(iinfo.m_collisionVec);
		vec3 relVel = obj2->GetVelocity() - obj1->GetVelocity();

		float velAlongNormal = glm::dot(relVel, collideNormal);

		if (velAlongNormal > 0)
			continue;

		float bounciness = glm::min(obj1->GetBounciness(), obj2->GetBounciness());

		float impulseLen = -(1.0F + bounciness) * velAlongNormal;
		impulseLen /= obj1->GetInverseMass() + obj2->GetInverseMass();

		vec3 impulse = impulseLen * collideNormal;

		if (!obj1->GetIsStatic()) obj1->SetVelocity(obj1->GetVelocity() - obj1->GetInverseMass() * impulse);
		if (!obj2->GetIsStatic()) obj2->SetVelocity(obj2->GetVelocity() + obj2->GetInverseMass() * impulse);
	}
}