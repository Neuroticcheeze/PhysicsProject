#pragma once

#include <vector>
using std::vector;

#include <unordered_map>
using std::unordered_map;

#include <string>
using std::string;

//PhysX Includes
#include <PxPhysicsAPI.h>
#include <PxScene.h>
#include <pvd/PxVisualDebugger.h>

#include "PhysXRagdoll.h"
#include "ParticleEmitter.h"
#include "ParticleFluidEmitter.h"

#include <glm\mat4x4.hpp>
using glm::mat4;
using glm::vec4;
using glm::vec3;

#include "..\Gizmos.h"

namespace MyPhysX
{
	struct PhysXFilter
	{
		enum Types
		{
			eNONE = (0),
			ePLAYER = (1 << 0),
			ePLATFORM = (1 << 1),
			eGROUND = (1 << 2),
		};
	};

	class MyAllocator : public physx::PxAllocatorCallback
	{
	public:
		virtual ~MyAllocator() {}
		virtual void* allocate(size_t size, const char* typeName, const char* filename, int line)
		{
			void* pointer = _aligned_malloc(size, 16);
			return pointer;
		}
		virtual void deallocate(void* ptr)
		{
			_aligned_free(ptr);
		}
	};

	class PhysXObject
	{
		friend class PhysXSystem;

	public:


		class MyCollisionCallBack;

		PhysXObject();
		~PhysXObject();
		void Destroy(PhysXSystem & p_sys);
		void Update();

		inline physx::PxRigidActor * GetActor() { return m_dynamicActor != nullptr ? static_cast<physx::PxRigidActor *>(m_dynamicActor) : static_cast<physx::PxRigidActor *>(m_staticActor); }
		inline bool GetIsTriggerActive() { return m_isTriggerActive; }
		inline Gizmos::GizmoMesh GetPotentialMesh() { return m_potMesh; }
		inline Gizmos::GizmoMesh GetImposterMesh() { return m_impMesh; }

	private:

		physx::PxRigidStatic * m_staticActor;
		physx::PxRigidDynamic * m_dynamicActor;

		bool m_isTrigger, m_isTriggerActive;

		mat4 m_transform;

		Gizmos::GizmoMesh m_potMesh, m_impMesh;
	};

	struct PhysXFluid
	{
		PxParticleFluid * m_particleSystem;
		ParticleFluidEmitter * m_emitter;
	};

	class PhysXSystem
	{
	public:

		PhysXSystem();
		~PhysXSystem();

		void Update(const float & p_deltaTime);

		void AddMaterial(const char * p_name, const physx::PxReal & p_sf, const physx::PxReal & p_df, const physx::PxReal & p_rt);
		physx::PxMaterial * GetMaterial(const char * p_name);

		void Add(const char* p_name, physx::PxU32 p_filterGroup, physx::PxU32 p_filterMask, const char * p_material, const float & p_density, const physx::PxVec3 & p_startVelocity, const physx::PxGeometry & p_geometry, physx::PxVec3 p_pos, physx::PxQuat p_rot, const bool & p_isStatic = false, const bool & p_isTrigger = false, Gizmos::GizmoMesh * p_potMesh = nullptr, Gizmos::GizmoMesh * p_impMesh = nullptr);

		void AddArticulation(RagdollNode ** p_ragdollData, const float & p_scaleFactor, const char * p_material, physx::PxVec3 p_pos, physx::PxQuat p_rot);

		void AddLiquidSource(const bool & p_renderAsPoints, const vec4 & p_colour, const vec3 & p_position, vec3 p_startVelocity, float p_startVelAccuracy = 0.9F, float p_releaseDelay = 0.075F, float p_stiffness = 40.0F, float p_restitution = 0.15F, float p_particleMass = 0.1F, float p_damping = 0.01F, float p_staticFriction = 0.02F, float p_dynamicFriction = 0.02F, float p_restParticleDistance = 0.5F, PxU32 p_maxParticles = 2000);

		physx::PxConvexMesh * GenerateConvexHullMesh(vector<float> p_inVertices, vector<float> & p_outVertices, vector<unsigned int> & p_outIndices, const unsigned short & p_targetVerts = 128);

		void Clear();

		inline physx::PxPhysics * GetPhysics()
		{
			return g_Physics;
		}

		inline physx::PxScene * GetScene()
		{
			return g_PhysicsScene;
		}

		inline physx::PxCooking * GetCooker()
		{
			return g_PhysicsCooker;
		}

		const vector<PhysXObject * > & GetObjects() const;

		void Render() const;

	private:

		vector<PhysXObject * > m_physicsObjects;
		vector<physx::PxArticulation * > m_articulations;
		vector<PhysXFluid * > m_fluids;
		unordered_map<string, physx::PxMaterial *> m_materials;

		physx::PxFoundation* g_PhysicsFoundation;
		physx::PxPhysics* g_Physics;
		physx::PxScene* g_PhysicsScene;
		physx::PxDefaultErrorCallback gDefaultErrorCallback;
		physx::PxDefaultAllocator gDefaultAllocatorCallback;
		physx::PxSimulationFilterShader gDefaultFilterShader = physx::PxDefaultSimulationFilterShader;
		physx::PxCooking* g_PhysicsCooker;
	};
}