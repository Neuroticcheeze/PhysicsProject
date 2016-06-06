#include "PhysX.h"

#include "..\Gizmos.h"

#include <glm\vec3.hpp>
#include <glm\gtc\matrix_transform.hpp>
using glm::vec3;

using namespace MyPhysX;

//derived class to overide the call backs we are interested in... 
class PhysXObject::MyCollisionCallBack : public physx::PxSimulationEventCallback
{
	virtual void onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs)
	{
		for (physx::PxU32 i = 0; i < nbPairs; i++)
		{
			const physx::PxContactPair& cp = pairs[i];
			//only interested in touches found and lost   
			if (cp.events & physx::PxPairFlag::eNOTIFY_TOUCH_FOUND)
			{
				printf("Collision Detected between: %s, and %s\n", pairHeader.actors[0]->getName(), pairHeader.actors[1]->getName());
			}
		}
	};

	virtual void onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 nbPairs)
	{
		for (physx::PxU32 i = 0; i < nbPairs; i++)
		{
			physx::PxTriggerPair* pair = pairs + i;
			physx::PxActor* triggerActor = pair->triggerActor;
			physx::PxActor* otherActor = pair->otherActor;
			

			auto obj = static_cast<PhysXObject*>(triggerActor->userData);

			if (obj != nullptr)
				obj->m_isTriggerActive = pair->status == physx::PxPairFlag::eNOTIFY_TOUCH_FOUND;
			
			printf("%s Entered Trigger %s\n", otherActor->getName(), triggerActor->getName());
		}
	};

	virtual void onConstraintBreak(physx::PxConstraintInfo*, physx::PxU32)
	{
	};

	virtual void onWake(physx::PxActor**, physx::PxU32)
	{
	};

	virtual void onSleep(physx::PxActor**, physx::PxU32)
	{
	};
};
physx::PxFilterFlags myFliterShader(physx::PxFilterObjectAttributes attributes0, physx::PxFilterData filterData0, physx::PxFilterObjectAttributes attributes1, physx::PxFilterData filterData1, physx::PxPairFlags& pairFlags, const void* constantBlock, physx::PxU32 constantBlockSize)
{
	// let triggers through
	if (physx::PxFilterObjectIsTrigger(attributes0) ||
		physx::PxFilterObjectIsTrigger(attributes1))
	{
		pairFlags = physx::PxPairFlag::eTRIGGER_DEFAULT;
		return physx::PxFilterFlag::eDEFAULT;
	}
	// generate contacts for all that were not filtered above
	pairFlags = physx::PxPairFlag::eCONTACT_DEFAULT;
	// trigger the contact callback for pairs (A,B) where
	// the filtermask of A contains the ID of B and vice versa.
	if ((filterData0.word0 & filterData1.word1) && (filterData1.word0 &
		filterData0.word1))
		pairFlags |= physx::PxPairFlag::eNOTIFY_TOUCH_FOUND |
		physx::PxPairFlag::eNOTIFY_TOUCH_LOST;
	return physx::PxFilterFlag::eDEFAULT;
}
//helper function to set up filtering
void setupFiltering(physx::PxRigidActor* actor, physx::PxU32 filterGroup, physx::PxU32 filterMask)
{
	physx::PxFilterData filterData;
	filterData.word0 = filterGroup; // word0 = own ID
	filterData.word1 = filterMask; // word1 = ID mask to filter pairs that trigger a contact callback;

	const physx::PxU32 numShapes = actor->getNbShapes();
	physx::PxShape** shapes = (physx::PxShape**)_aligned_malloc(sizeof(physx::PxShape*)*numShapes, 16);
	actor->getShapes(shapes, numShapes);
	
	for (physx::PxU32 i = 0; i < numShapes; i++)
	{
		physx::PxShape* shape = shapes[i];
		shape->setSimulationFilterData(filterData);
	}
	
	_aligned_free(shapes);
}
void setShapeAsTrigger(physx::PxRigidActor* actorIn)
{
	physx::PxRigidStatic* staticActor = actorIn->is<physx::PxRigidStatic>();
	assert(staticActor);
	
	const physx::PxU32 numShapes = staticActor->getNbShapes();
	physx::PxShape** shapes = (physx::PxShape**)_aligned_malloc(sizeof(physx::PxShape*)*numShapes, 16);
	staticActor->getShapes(shapes, numShapes);
	
	for (physx::PxU32 i = 0; i < numShapes; i++)
	{
		shapes[i]->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, false);
		shapes[i]->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, true);
	}

	_aligned_free(shapes);
}

PhysXObject::PhysXObject() :
	m_dynamicActor(nullptr),
	m_staticActor(nullptr),
	m_isTrigger(false),
	m_isTriggerActive(false)
{

}

PhysXObject::~PhysXObject()
{
}

void PhysXObject::Update()
{
	physx::PxMat44 m(GetActor()->getGlobalPose());

	m_transform[0] = vec4(m.column0.x, m.column0.y, m.column0.z, m.column0.w);
	m_transform[1] = vec4(m.column1.x, m.column1.y, m.column1.z, m.column1.w);
	m_transform[2] = vec4(m.column2.x, m.column2.y, m.column2.z, m.column2.w);
	m_transform[3] = vec4(m.column3.x, m.column3.y, m.column3.z, m.column3.w);
}

void PhysXObject::Destroy(PhysXSystem & p_sys)
{
	p_sys.GetScene()->removeActor(*GetActor());

	m_dynamicActor = nullptr;
	m_staticActor = nullptr;
}

PhysXSystem::PhysXSystem()
{
	////////////////////////////////////////
	//=========== SET UP PHYSX ==========///
	////////////////////////////////////////
	physx::PxAllocatorCallback *myCallback = new MyAllocator();
	g_PhysicsFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, *myCallback, gDefaultErrorCallback);
	g_Physics = PxCreatePhysics(PX_PHYSICS_VERSION, *g_PhysicsFoundation, physx::PxTolerancesScale());
	PxInitExtensions(*g_Physics);
	physx::PxSceneDesc sceneDesc(g_Physics->getTolerancesScale());
	sceneDesc.gravity = physx::PxVec3(0, -9.8f, 0);
	sceneDesc.filterShader = &myFliterShader;
	sceneDesc.cpuDispatcher = physx::PxDefaultCpuDispatcherCreate(1);
	g_PhysicsScene = g_Physics->createScene(sceneDesc);

	physx::PxSimulationEventCallback* mycollisionCallBack = new PhysXObject::MyCollisionCallBack();
	g_PhysicsScene->setSimulationEventCallback(mycollisionCallBack);

	PxTolerancesScale toleranceScale;
	toleranceScale.mass = 1000;
	toleranceScale.speed = sceneDesc.gravity.y * 2;
	bool value = toleranceScale.isValid(); // make sure this value is always true

	g_PhysicsCooker = PxCreateCooking(PX_PHYSICS_VERSION, *g_PhysicsFoundation, PxCookingParams(PxTolerancesScale()));


	////////////////////////////////////////
	//===== CREATE DEFAULT MATERIAL =====///
	////////////////////////////////////////
	AddMaterial("default", 0.5F, 0.5F, 0.5F);

	////////////////////////////////////////
	//============ SET UP PVD ===========///
	////////////////////////////////////////
	// check if PvdConnection manager is available on this platform

#define _USE_PVD
#ifdef _USE_PVD
	if (g_Physics->getPvdConnectionManager() == NULL)
		return;
	// setup connection parameters
	const char* pvd_host_ip = "127.0.0.1";
	// IP of the PC which is running PVD
	int port = 5425;
	// TCP port to connect to, where PVD is listening
	unsigned int timeout = 100;
	// timeout in milliseconds to wait for PVD to respond,
	//consoles and remote PCs need a higher timeout.
	physx::PxVisualDebuggerConnectionFlags connectionFlags = physx::PxVisualDebuggerExt::getAllConnectionFlags();
	// and now try to connectphysx::PxVisualDebuggerExt
	auto theConnection = physx::PxVisualDebuggerExt::createConnection(
		g_Physics->getPvdConnectionManager(), pvd_host_ip, port, timeout, connectionFlags);
#endif
}

PhysXSystem::~PhysXSystem()
{
	Clear();

	g_PhysicsScene->release();
	g_Physics->release();
	g_PhysicsFoundation->release();
}

void PhysXSystem::Update(const float & p_deltaTime)
{
	if (p_deltaTime <= 0)
		return;

	for (PhysXFluid * fluid : m_fluids)
	{
		fluid->m_emitter->update(p_deltaTime);
	}

	g_PhysicsScene->simulate(glm::min(p_deltaTime, 60.0F / 1000.0F));//A workaround to stop everything from shooting across the sky 
																	 //when the window bar is held a while then released.

	while (g_PhysicsScene->fetchResults() == false)
	{
		// don’t need to do anything here yet but we have to fetch results
	}
}

void PhysXSystem::AddMaterial(const char * p_name, const physx::PxReal & p_sf, const physx::PxReal & p_df, const physx::PxReal & p_rt)
{
	m_materials[string(p_name)] = g_Physics->createMaterial(p_sf, p_df, p_rt);
}

physx::PxMaterial * PhysXSystem::GetMaterial(const char * p_name)
{
	return m_materials.at(string(p_name));
}

void PhysXSystem::Add(const char* p_name, physx::PxU32 p_filterGroup, physx::PxU32 p_filterMask, const char * p_material, const float & p_density, const physx::PxVec3 & p_startVelocity, const physx::PxGeometry & p_geometry, physx::PxVec3 p_pos, physx::PxQuat p_rot, const bool & p_isStatic, const bool & p_isTrigger, Gizmos::GizmoMesh * p_potMesh, Gizmos::GizmoMesh * p_impMesh)
{
	assert(p_isTrigger ? p_isStatic : true);

	auto material = GetMaterial(p_material);

	material = material == nullptr ? GetMaterial("default") : material;

	PhysXObject * pobj = new PhysXObject;

	if (p_isStatic)
	{
		pobj->m_staticActor = physx::PxCreateStatic(*GetPhysics(), physx::PxTransform(p_pos, p_rot), p_geometry, *material);
		pobj->m_staticActor->userData = pobj;
		GetScene()->addActor(*pobj->m_staticActor);

		if (p_isTrigger)
		{
			pobj->m_isTrigger = true;
			setShapeAsTrigger(pobj->m_staticActor);
		}
	}

	else
	{
		pobj->m_dynamicActor = physx::PxCreateDynamic(*GetPhysics(), physx::PxTransform(p_pos, p_rot), p_geometry, *material, p_density);
		pobj->m_dynamicActor->userData = pobj;
		GetScene()->addActor(*pobj->m_dynamicActor);
	}

	if (p_geometry.getType() == physx::PxGeometryType::eCONVEXMESH)
	{
		assert(p_potMesh != nullptr && p_impMesh != nullptr);

		pobj->m_potMesh = Gizmos::GizmoMesh(*p_potMesh);
		pobj->m_impMesh = Gizmos::GizmoMesh(*p_impMesh);
	}

	setupFiltering(pobj->GetActor(), p_filterGroup, p_filterMask);
	pobj->GetActor()->setName(p_name);

	if (pobj->GetActor()->isRigidBody())
		pobj->GetActor()->isRigidBody()->setLinearVelocity(p_startVelocity);

	m_physicsObjects.push_back(pobj);
}

void PhysXSystem::AddArticulation(RagdollNode ** p_ragdollData, const float & p_scaleFactor, const char * p_material, physx::PxVec3 p_pos, physx::PxQuat p_rot)
{
	auto material = GetMaterial(p_material);

	material = material == nullptr ? GetMaterial("default") : material;

	physx::PxArticulation * art = makeRagdoll(
		g_Physics,
		p_ragdollData,
		physx::PxTransform(p_pos, p_rot),
		p_scaleFactor, material);

	g_PhysicsScene->addArticulation(*art);

	m_articulations.push_back(art);
}

void PhysXSystem::AddLiquidSource(const bool & p_renderAsPoints, const vec4 & p_colour, const vec3 & p_position, vec3 p_startVelocity, float p_startVelAccuracy, float p_releaseDelay, float p_stiffness, float p_restitution, float p_particleMass, float p_damping, float p_staticFriction, float p_dynamicFriction, float p_restParticleDistance, PxU32 p_maxParticles)
{
	vec3 devi = vec3(p_startVelocity.length()) * (1.0F - glm::clamp(p_startVelAccuracy, 0.0F, 1.0F));
	vec3 minv = p_startVelocity - devi;
	vec3 maxv = p_startVelocity + devi;

	//create our particle system
	PxParticleFluid* pf;
	// create particle system in PhysX SDK
	// set immutable properties.

	pf = g_Physics->createParticleFluid(p_maxParticles);

	pf->setRestParticleDistance(p_restParticleDistance);
	pf->setStaticFriction(p_staticFriction);
	pf->setDynamicFriction(p_dynamicFriction);
	pf->setDamping(p_damping);
	pf->setParticleMass(p_particleMass);
	pf->setRestitution(p_restitution);
	pf->setParticleBaseFlag(PxParticleBaseFlag::eCOLLISION_TWOWAY, true);
	pf->setStiffness(p_stiffness);
	if (pf)
	{
		g_PhysicsScene->addActor(*pf);

		PhysXFluid * fluid = new PhysXFluid;

		fluid->m_emitter = new ParticleFluidEmitter(p_renderAsPoints, p_colour, p_maxParticles, PxVec3(p_position.x, p_position.y, p_position.z), pf, p_releaseDelay);
		fluid->m_emitter->setStartVelocityRange(minv.x, minv.y, minv.z, maxv.x, maxv.y, maxv.z);

		fluid->m_particleSystem = pf;

		m_fluids.push_back(fluid);
	}
}

physx::PxConvexMesh * PhysXSystem::GenerateConvexHullMesh(vector<float> p_inVertices, vector<float> & p_outVertices, vector<unsigned int> & p_outIndices, const unsigned short & p_targetVerts)
{
	//reserve space for vert buffer
	int numberVerts = p_inVertices.size() / 8;

	if (p_inVertices.size() % 8 != 0)
		return nullptr;

	PxVec3 *verts = new PxVec3[numberVerts]; //temporary buffer for our verts

											 //Fill the buffer.
	for (int vert = 0; vert < numberVerts; ++vert)
	{
		verts[vert] = PxVec3(
			p_inVertices[vert * 8 + 0],
			p_inVertices[vert * 8 + 1],
			p_inVertices[vert * 8 + 2]
			);
	}

	PxConvexMeshDesc convexDesc;
	convexDesc.points.count = numberVerts;
	convexDesc.points.stride = sizeof(PxVec3);
	convexDesc.points.data = verts;
	convexDesc.flags = PxConvexFlag::eCOMPUTE_CONVEX;
	convexDesc.vertexLimit = p_targetVerts;
	PxDefaultMemoryOutputStream* buf = new PxDefaultMemoryOutputStream();
	physx::PxConvexMeshCookingResult::Enum e;
	if (!g_PhysicsCooker->cookConvexMesh(convexDesc, *buf, &e))
	{
		printf("Failed to cook mesh.");
		return nullptr;
	}
	PxU8* contents = buf->getData();
	PxU32 size = buf->getSize();
	PxDefaultMemoryInputData input(contents, size);
	PxConvexMesh* convexMesh = g_Physics->createConvexMesh(input);

	//Convert PxConvexMesh data to GizmoMesh data.
	{
		int vertCount = convexMesh->getNbVertices();
		int indCount = convexMesh->getNbPolygons();
		auto pverts = convexMesh->getVertices();
		auto pinds = convexMesh->getIndexBuffer();


		

		p_outVertices.resize(vertCount * 8);
		for (int vert = 0; vert < vertCount; ++vert)
		{
			p_outVertices[vert * 8 + 0] = pverts[vert].x;
			p_outVertices[vert * 8 + 1] = pverts[vert].y;
			p_outVertices[vert * 8 + 2] = pverts[vert].z;
			p_outVertices[vert * 8 + 3] = 1;
			p_outVertices[vert * 8 + 4] = 1;
			p_outVertices[vert * 8 + 5] = 1;
			p_outVertices[vert * 8 + 6] = 1;
			p_outVertices[vert * 8 + 7] = 1;
		}

		p_outIndices.resize(indCount);
		for (int ind = 0; ind < indCount; ++ind)
		{
			p_outIndices[ind] = static_cast<unsigned int>(pinds[ind]);
		}
	}

	return convexMesh;
}

void PhysXSystem::Clear()
{
	for (PhysXFluid * fluid : m_fluids)
	{
		GetScene()->removeActor(*fluid->m_particleSystem);
		delete fluid->m_emitter;
	}
	for (PhysXObject * obj : m_physicsObjects)
	{
		obj->Destroy(*this);
		delete obj;
	}
	for (physx::PxArticulation * obj : m_articulations)
	{
		GetScene()->removeArticulation(*obj);
	}

	m_fluids.clear();
	m_physicsObjects.clear();
	m_articulations.clear();
}

const vector<PhysXObject * > & PhysXSystem::GetObjects() const
{
	return m_physicsObjects;
}

void addBox(physx::PxShape* pShape, physx::PxRigidActor* actor, const bool & p_triggerRenderMode)
{ 
	//get the geometry for this PhysX collision volume 
	physx::PxBoxGeometry geometry; 

	float width = 1, height = 1, length = 1; 
	
	bool status = pShape->getBoxGeometry(geometry); 
	
	if (status) 
	{ 
		width = geometry.halfExtents.x; 
		height = geometry.halfExtents.y; 
		length = geometry.halfExtents.z; 
	} //get the transform for this PhysX collision volume 
	
	physx::PxMat44 m(physx::PxShapeExt::getGlobalPose(*pShape, *actor)); 
	mat4 M(m.column0.x, m.column0.y, m.column0.z, m.column0.w, m.column1.x, m.column1.y, m.column1.z, m.column1.w, m.column2.x, m.column2.y, m.column2.z, m.column2.w, m.column3.x, m.column3.y, m.column3.z, m.column3.w); 
	vec3 position; 
	
	//get the position out of the transform 
	position.x = m.getPosition().x; 
	position.y = m.getPosition().y; 
	position.z = m.getPosition().z; 
	
	vec3 extents = vec3(width, height, length); 
	vec4 colour = (actor->userData != nullptr && static_cast<PhysXObject*>(actor->userData)->GetIsTriggerActive()) ? vec4(1, 1, 1, 1) : vec4(0, 0, 1, 1);
	
	//create our box gizmo 
	if (p_triggerRenderMode)
		Gizmos::addAABB(position, extents, colour, &M);
	else
		Gizmos::addAABBFilled(position, extents, colour, &M);
}
void addSphere(physx::PxShape* pShape, physx::PxRigidActor* actor, const bool & p_triggerRenderMode)
{
	//get the geometry for this PhysX collision volume 
	physx::PxSphereGeometry geometry;

	float radius = 1;

	bool status = pShape->getSphereGeometry(geometry);

	if (status)
	{
		radius = geometry.radius;
	} //get the transform for this PhysX collision volume 

	physx::PxMat44 m(physx::PxShapeExt::getGlobalPose(*pShape, *actor));
	mat4 M(m.column0.x, m.column0.y, m.column0.z, m.column0.w, m.column1.x, m.column1.y, m.column1.z, m.column1.w, m.column2.x, m.column2.y, m.column2.z, m.column2.w, m.column3.x, m.column3.y, m.column3.z, m.column3.w);
	vec3 position;

	//get the position out of the transform 
	position.x = m.getPosition().x;
	position.y = m.getPosition().y;
	position.z = m.getPosition().z;

	vec4 colour = (actor->userData != nullptr && static_cast<PhysXObject*>(actor->userData)->GetIsTriggerActive()) ? vec4(1, 1, 1, 1) : vec4(0, 1, 0, 1);

	//create our sphere gizmo 	
	if (p_triggerRenderMode)
		colour.a = 0.0F;

	Gizmos::addSphere(position, radius, 4, 8, colour, &M);
}
void addCapsule(physx::PxShape* pShape, physx::PxRigidActor* actor, const bool & p_triggerRenderMode)
{
	//get the geometry for this PhysX collision volume 
	physx::PxCapsuleGeometry geometry;

	float radius = 1, halfHeight = 1;

	bool status = pShape->getCapsuleGeometry(geometry);

	if (status)
	{
		halfHeight = geometry.halfHeight;
		radius = geometry.radius;
	} //get the transform for this PhysX collision volume 

	physx::PxMat44 m(physx::PxShapeExt::getGlobalPose(*pShape, *actor));
	mat4 M(m.column0.x, m.column0.y, m.column0.z, m.column0.w, m.column1.x, m.column1.y, m.column1.z, m.column1.w, m.column2.x, m.column2.y, m.column2.z, m.column2.w, m.column3.x, m.column3.y, m.column3.z, m.column3.w);
	vec3 position;

	//get the position out of the transform 
	position.x = m.getPosition().x;
	position.y = m.getPosition().y;
	position.z = m.getPosition().z;

	vec4 normal = (M * glm::vec4(halfHeight, 0, 0, 0));

	vec4 colour = (actor->userData != nullptr && static_cast<PhysXObject*>(actor->userData)->GetIsTriggerActive()) ? vec4(1, 1, 1, 1) : vec4(1, 0.85F, 0, 1);

	float f = radius * 0.7071F;
	vec3 shaftExtents(halfHeight, f, f);

	//create our capsule gizmo 
	if (p_triggerRenderMode)
	{
		colour.a = 0.0F;

		Gizmos::addSphere(position + vec3(normal), radius, 4, 8, colour, &M);
		Gizmos::addSphere(position - vec3(normal), radius, 4, 8, colour, &M);
		Gizmos::addAABB(position, shaftExtents, colour, &M);
		M = glm::rotate(M, glm::quarter_pi<float>(), vec3(1, 0, 0));
		Gizmos::addAABB(position, shaftExtents, colour, &M);

		return;
	}

	Gizmos::addSphere(position + vec3(normal), radius, 4, 8, colour, &M);
	Gizmos::addSphere(position - vec3(normal), radius, 4, 8, colour, &M);
	Gizmos::addAABBFilled(position, shaftExtents, colour, &M);
	M = glm::rotate(M, glm::quarter_pi<float>(), vec3(1, 0, 0));
	Gizmos::addAABBFilled(position, shaftExtents, colour, &M);
}
void addPlane(physx::PxShape* pShape, physx::PxRigidActor* actor, const bool & p_triggerRenderMode)
{
	physx::PxMat44 m(physx::PxShapeExt::getGlobalPose(*pShape, *actor));
	mat4 M(m.column0.x, m.column0.y, m.column0.z, m.column0.w, m.column1.x, m.column1.y, m.column1.z, m.column1.w, m.column2.x, m.column2.y, m.column2.z, m.column2.w, m.column3.x, m.column3.y, m.column3.z, m.column3.w);

	vec4 colour = (actor->userData != nullptr && static_cast<PhysXObject*>(actor->userData)->GetIsTriggerActive()) ? vec4(1, 1, 1, 1) : vec4(0.3F, 0.3F, 0.3F, 1);

	M = glm::rotate(M, glm::radians(90.0F), vec3(0, 0, 1));

	Gizmos::addDisk(vec3(0), 99999.0F, 4, colour, &M);
}
void addConvexMesh(physx::PxShape* pShape, physx::PxRigidActor* actor, const bool & p_triggerRenderMode)
{
	//get the geometry for this PhysX collision volume 
	physx::PxConvexMeshGeometry geometry;


	bool status = pShape->getConvexMeshGeometry(geometry);

	if (status)
	{
	} 
	
	//get the transform for this PhysX collision volume 
	physx::PxMat44 m(physx::PxShapeExt::getGlobalPose(*pShape, *actor));
	mat4 M(m.column0.x, m.column0.y, m.column0.z, m.column0.w, m.column1.x, m.column1.y, m.column1.z, m.column1.w, m.column2.x, m.column2.y, m.column2.z, m.column2.w, m.column3.x, m.column3.y, m.column3.z, m.column3.w);

	vec4 colour = (actor->userData != nullptr && static_cast<PhysXObject*>(actor->userData)->GetIsTriggerActive()) ? vec4(1, 1, 1, 1) : vec4(0.5F, 0, 1, 1);

	//create our sphere gizmo 	
	if (p_triggerRenderMode)
		colour.a = 0.0F;

	if (actor->userData != nullptr)
	{
		Gizmos::addMesh(static_cast<PhysXObject*>(actor->userData)->GetPotentialMesh(), vec3(0), colour, &M);
		colour.a = 0.0F;
		Gizmos::addMesh(static_cast<PhysXObject*>(actor->userData)->GetImposterMesh(), vec3(0), colour, &M, 8.0F);
	}
}
void _addWidget(physx::PxShape* shape, physx::PxRigidActor* actor, const bool & p_triggerRenderMode) 
{ 
	physx::PxGeometryType::Enum type = shape->getGeometryType(); 
	switch (type) 
	{ 
	case physx::PxGeometryType::eBOX: 
		addBox(shape, actor, p_triggerRenderMode);
		break; 
	case physx::PxGeometryType::eSPHERE:
		addSphere(shape, actor, p_triggerRenderMode);
		break;
	case physx::PxGeometryType::eCAPSULE:
		addCapsule(shape, actor, p_triggerRenderMode);
		break;
	case physx::PxGeometryType::ePLANE:
		addPlane(shape, actor, p_triggerRenderMode);
		break;
	case physx::PxGeometryType::eCONVEXMESH:
		addConvexMesh(shape, actor, p_triggerRenderMode);
		break;
	} 
}
void PhysXSystem::Render() const
{
	for (PhysXFluid * fluid : m_fluids)
	{
		fluid->m_emitter->renderParticles();
	}

	for (auto articulation : m_articulations)
	{
		physx::PxU32 nLinks = articulation->getNbLinks();
		physx::PxArticulationLink** links = new physx::PxArticulationLink*[nLinks];
		articulation->getLinks(links, nLinks);
		// Render all the shapes in the physx actor (for early tutorials there is just one)
		while (nLinks--)
		{
			physx::PxArticulationLink* link = links[nLinks];
			physx::PxU32 nShapes = link->getNbShapes();
			physx::PxShape** shapes = new physx::PxShape*[nShapes];
			link->getShapes(shapes, nShapes);
			while (nShapes--)
			{
				_addWidget(shapes[nShapes], link, false);
			}
		}
		delete[] links;
	}

	// Add widgets to represent all the phsyX actors which are in the scene 
	for (auto obj : m_physicsObjects) 
	{ 
		{ 
			physx::PxU32 nShapes = obj->GetActor()->getNbShapes(); 
			physx::PxShape** shapes = new physx::PxShape*[nShapes]; obj->GetActor()->getShapes(shapes, nShapes); // Render all the shapes in the physx actor (for early tutorials there is just one) 
			
			while (nShapes--) 
			{ 
				_addWidget(shapes[nShapes], obj->GetActor(), obj->m_isTrigger);
			} 

			delete[] shapes; 
		} 
	}
}