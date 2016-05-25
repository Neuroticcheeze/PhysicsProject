#include "PhysX.h"

#include "Gizmos.h"

#include <glm\vec3.hpp>
using glm::vec3;

using namespace physx;
using namespace MyPhysX;

PhysXObject::PhysXObject(PhysXSystem & p_sys)
{
	float density = 1000;
	PxBoxGeometry box(2, 2, 2);
	PxTransform transform(PxVec3(1, 100, 0));
	m_actor = PxCreateDynamic(*p_sys.GetPhysics(), transform, box, *p_sys.GetMaterial("default"), density);
	//add it to the physX scene
	p_sys.GetScene()->addActor(*m_actor);
}

PhysXObject::~PhysXObject()
{
}

void PhysXObject::Update()
{
	PxMat44 m(m_actor->getGlobalPose());

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
	PxAllocatorCallback *myCallback = new MyAllocator();
	g_PhysicsFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, *myCallback,
		gDefaultErrorCallback);
	g_Physics = PxCreatePhysics(PX_PHYSICS_VERSION, *g_PhysicsFoundation,
		PxTolerancesScale());
	PxInitExtensions(*g_Physics);
	PxSceneDesc sceneDesc(g_Physics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0, -9.8f, 0);
	sceneDesc.filterShader = &PxDefaultSimulationFilterShader;
	sceneDesc.cpuDispatcher = PxDefaultCpuDispatcherCreate(1);
	g_PhysicsScene = g_Physics->createScene(sceneDesc);





	////////////////////////////////////////
	//========= CREATE MATERIAL =========///
	////////////////////////////////////////
	AddMaterial("default", 0.1f, 0.1f, 0.3f);






	////////////////////////////////////////
	//=========== SET UP FLOOR ==========///
	////////////////////////////////////////
	//add a plane
	PxTransform pose = PxTransform(PxVec3(0.0f, 0, 0.0f), PxQuat(PxHalfPi*1.0f, PxVec3(0.0f, 0.0f, 1.0f)));
	PxRigidStatic* plane = PxCreateStatic(*g_Physics, pose, PxPlaneGeometry(),
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
	PxVisualDebuggerConnectionFlags connectionFlags = PxVisualDebuggerExt::getAllConnectionFlags();
	// and now try to connectPxVisualDebuggerExt
	auto theConnection = PxVisualDebuggerExt::createConnection(
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

void PhysXSystem::AddMaterial(const char * p_name, const PxReal & p_sf, const PxReal & p_df, const PxReal & p_rt)
{
	m_materials[string(p_name)] = g_Physics->createMaterial(p_sf, p_df, p_rt);
}

PxMaterial * PhysXSystem::GetMaterial(const char * p_name)
{
	return m_materials.at(string(p_name));
}

void PhysXSystem::Add()
{
	PhysXObject * pobj = new PhysXObject(*this);

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

PxPhysics * PhysXSystem::GetPhysics()
{
	return g_Physics;
}

PxScene *	PhysXSystem::GetScene()
{
	return g_PhysicsScene;
}

PxCooking * PhysXSystem::GetCooker()
{
	return g_PhysicsCooker;
}

const vector<PhysXObject * > & PhysXSystem::GetObjects() const
{
	return m_physicsObjects;
}

void addBox(PxShape* pShape, PxRigidActor* actor) 
{ 
	//get the geometry for this PhysX collision volume 
	PxBoxGeometry geometry; 

	float width = 1, height = 1, length = 1; 
	
	bool status = pShape->getBoxGeometry(geometry); 
	
	if (status) 
	{ 
		width = geometry.halfExtents.x; 
		height = geometry.halfExtents.y; 
		length = geometry.halfExtents.z; 
	} //get the transform for this PhysX collision volume 
	
	PxMat44 m(PxShapeExt::getGlobalPose(*pShape, *actor)); 
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

void addWidget(PxShape* shape, PxRigidActor* actor) 
{ 
	PxGeometryType::Enum type = shape->getGeometryType(); 
	switch (type) 
	{ 
	case PxGeometryType::eBOX: 
		addBox(shape, actor); 
		break; 
	} 
}

void PhysXSystem::Render() const
{
	// Add widgets to represent all the phsyX actors which are in the scene 
	for (auto obj : m_physicsObjects) 
	{ 
		{ 
			PxU32 nShapes = obj->m_actor->getNbShapes(); 
			PxShape** shapes = new PxShape*[nShapes]; obj->m_actor->getShapes(shapes, nShapes); // Render all the shapes in the physx actor (for early tutorials there is just one) 
			
			while (nShapes--) 
			{ 
				addWidget(shapes[nShapes], obj->m_actor);
			} 

			delete[] shapes; 
		} 
	}
}