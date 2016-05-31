#include "PhysX.h"

#include "..\Gizmos.h"

#include <glm\vec3.hpp>
using glm::vec3;

using namespace MyPhysX;

PhysXObject::PhysXObject()
{

}

PhysXObject::~PhysXObject()
{
}

void PhysXObject::Update()
{
	physx::PxMat44 m(m_actor->getGlobalPose());

	m_transform[0] = vec4(m.column0.x, m.column0.y, m.column0.z, m.column0.w);
	m_transform[1] = vec4(m.column1.x, m.column1.y, m.column1.z, m.column1.w);
	m_transform[2] = vec4(m.column2.x, m.column2.y, m.column2.z, m.column2.w);
	m_transform[3] = vec4(m.column3.x, m.column3.y, m.column3.z, m.column3.w);
}

void PhysXObject::Destroy(PhysXSystem & p_sys)
{
	if (m_actor == nullptr)
		return;

	p_sys.GetScene()->removeActor(*m_actor);
	m_actor = nullptr;
}



PhysXSystem::PhysXSystem()
{
	////////////////////////////////////////
	//=========== SET UP PHYSX ==========///
	////////////////////////////////////////
	physx::PxAllocatorCallback *myCallback = new MyAllocator();
	g_PhysicsFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, *myCallback,
		gDefaultErrorCallback);
	g_Physics = PxCreatePhysics(PX_PHYSICS_VERSION, *g_PhysicsFoundation,
		physx::PxTolerancesScale());
	PxInitExtensions(*g_Physics);
	physx::PxSceneDesc sceneDesc(g_Physics->getTolerancesScale());
	sceneDesc.gravity = physx::PxVec3(0, -9.8f, 0);
	sceneDesc.filterShader = &physx::PxDefaultSimulationFilterShader;
	sceneDesc.cpuDispatcher = physx::PxDefaultCpuDispatcherCreate(1);
	g_PhysicsScene = g_Physics->createScene(sceneDesc);





	////////////////////////////////////////
	//========= CREATE MATERIAL =========///
	////////////////////////////////////////
	AddMaterial("default", 0.1f, 0.1f, 0.3f);






	////////////////////////////////////////
	//=========== SET UP FLOOR ==========///
	////////////////////////////////////////
	//add a plane
	physx::PxTransform pose = physx::PxTransform(physx::PxVec3(0.0f, 0, 0.0f), physx::PxQuat(physx::PxHalfPi*1.0f, physx::PxVec3(0.0f, 0.0f, 1.0f)));
	physx::PxRigidStatic* plane = physx::PxCreateStatic(*g_Physics, pose, physx::PxPlaneGeometry(),
		*GetMaterial("default"));
	//add it to the physX scene
	g_PhysicsScene->addActor(*plane);







	////////////////////////////////////////
	//============ SET UP PVD ===========///
	////////////////////////////////////////
	// check if PvdConnection manager is available on this platform
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

	g_PhysicsScene->simulate(p_deltaTime);

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

void PhysXSystem::Add(const char * p_material, const float & p_density, const physx::PxGeometry & p_geometry, physx::PxVec3 p_pos, physx::PxQuat p_rot)
{
	auto material = GetMaterial(p_material);

	material = material == nullptr ? GetMaterial("default") : material;

	PhysXObject * pobj = new PhysXObject;

	pobj->m_actor = physx::PxCreateDynamic(*GetPhysics(), physx::PxTransform(p_pos, p_rot), p_geometry, *material, p_density);

	GetScene()->addActor(*pobj->m_actor);

	m_physicsObjects.push_back(pobj);
}

void PhysXSystem::Clear()
{
	for (PhysXObject * obj : m_physicsObjects)
	{
		obj->Destroy(*this);
		delete obj;
	}

	m_physicsObjects.clear();
}

const vector<PhysXObject * > & PhysXSystem::GetObjects() const
{
	return m_physicsObjects;
}

void addBox(physx::PxShape* pShape, physx::PxRigidActor* actor) 
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
	vec4 colour = vec4(1, 0, 0, 1); 
	
	if (actor->getName() != NULL && strcmp(actor->getName(), "Pickup1")) //seriously horrid hack so I can show pickups a different colour 
	{ 
		colour = vec4(0, 1, 0, 1); 
	} 
	
	//create our box gizmo 
	Gizmos::addAABBFilled(position, extents, colour, &M); 
}
void addSphere(physx::PxShape* pShape, physx::PxRigidActor* actor)
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

	vec4 colour = vec4(1, 0, 0, 1);

	if (actor->getName() != NULL && strcmp(actor->getName(), "Pickup1")) //seriously horrid hack so I can show pickups a different colour 
	{
		colour = vec4(0, 1, 0, 1);
	}

	//create our box gizmo 
	Gizmos::addSphere(position, radius, 16, 16, colour, &M);
}

void addWidget(physx::PxShape* shape, physx::PxRigidActor* actor) 
{ 
	physx::PxGeometryType::Enum type = shape->getGeometryType(); 
	switch (type) 
	{ 
	case physx::PxGeometryType::eBOX: 
		addBox(shape, actor); 
		break; 
	case physx::PxGeometryType::eSPHERE:
		addSphere(shape, actor);
		break;
	} 
}

void PhysXSystem::Render() const
{
	// Add widgets to represent all the phsyX actors which are in the scene 
	for (auto obj : m_physicsObjects) 
	{ 
		{ 
			physx::PxU32 nShapes = obj->m_actor->getNbShapes(); 
			physx::PxShape** shapes = new physx::PxShape*[nShapes]; obj->m_actor->getShapes(shapes, nShapes); // Render all the shapes in the physx actor (for early tutorials there is just one) 
			
			while (nShapes--) 
			{ 
				addWidget(shapes[nShapes], obj->m_actor);
			} 

			delete[] shapes; 
		} 
	}
}