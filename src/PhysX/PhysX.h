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

#include <glm\mat4x4.hpp>
using glm::mat4;
using glm::vec4;

namespace MyPhysX
{
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
		PhysXObject();
		~PhysXObject();
		void Destroy(PhysXSystem & p_sys);
		void Update();

	private:

		physx::PxRigidDynamic * m_actor;

		mat4 m_transform;
	};

	class PhysXSystem
	{
	public:

		PhysXSystem();
		~PhysXSystem();

		void Update(const float & p_deltaTime);

		void AddMaterial(const char * p_name, const physx::PxReal & p_sf, const physx::PxReal & p_df, const physx::PxReal & p_rt);
		physx::PxMaterial * GetMaterial(const char * p_name);

		void Add(const char * p_material, const float & p_density, const physx::PxGeometry & p_geometry, physx::PxVec3 p_pos, physx::PxQuat p_rot);

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