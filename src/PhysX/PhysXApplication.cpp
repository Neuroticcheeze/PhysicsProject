#include "PhysXApplication.h"
#include "..\gl_core_4_4.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "..\Camera.h"
#include "..\Gizmos.h"

#include "PhysX.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "..\tiny_obj_loader.h"

#include <string>
using std::string;

using namespace physx;

using glm::vec3;
using glm::vec4;
using glm::mat4;

Gizmos::GizmoMesh complexMesh, complexImpMesh, blobMesh, blobImpMesh;

PhysXApplication::PhysXApplication()
	: m_camera(nullptr) {

}

PhysXApplication::~PhysXApplication() {

}

bool PhysXApplication::startup()
{

	// create a basic window
	createWindow("PhysX Demonstration", 1280, 720);

	//Set the point rendering size for Gizmos::addPoint(...)
	glPointSize(20.0F);

	// start the gizmo system that can draw basic shapes
	Gizmos::create();

	// create a camera
	{
		m_camera = new Camera(glm::pi<float>() * 0.25f, 16 / 9.f, 0.1f, 1000.f);
		m_camera->setLookAtFrom(vec3(50, 10, 0), vec3(0));
	}

	//Load meshes
	{ 
		{
			vector<float> verts;
			vector<unsigned int> indices;
			if (LoadObj("objs/complex_shape_torus.obj", verts, indices, vec3(5, 5, 5)))
				complexMesh.Create(verts, indices);
		}
		{
			vector<float> verts;
			vector<unsigned int> indices;
			if (LoadObj("objs/complex_shape_blob.obj", verts, indices, vec3(1, 1, 1)))
				blobMesh.Create(verts, indices);
		}
	}

	//Create physics system
	m_psys = new MyPhysX::PhysXSystem;

	//New type of material
	m_psys->AddMaterial("box", 0.85F, 0.85F, 0.1F);

	//Add ground and trigger
	{
		m_psys->Add("Ground", MyPhysX::PhysXFilter::eNONE, MyPhysX::PhysXFilter::eNONE, "default", 0, physx::PxVec3(0), physx::PxPlaneGeometry(),
			physx::PxVec3(0.0f, 0, 0.0f),
			physx::PxQuat(
				physx::PxHalfPi,
				physx::PxVec3(0.0f, 0.0f, 1.0f).getNormalized()),
			true);

		m_psys->Add("Trigger Region", MyPhysX::PhysXFilter::ePLAYER, MyPhysX::PhysXFilter::ePLAYER | MyPhysX::PhysXFilter::eGROUND, "box", glm::linearRand(10, 1000), physx::PxVec3(0), physx::PxBoxGeometry(10, 10, 10),
			physx::PxVec3(0, 10, 0),
			physx::PxQuat(physx::PxIDENTITY::PxIdentity),
			true, true);
	}

	//complex humanoid ragdoll example
	{
		enum RagDollParts
		{
			NO_PARENT = -1,
			LOWER_SPINE,
			LEFT_PELVIS,
			RIGHT_PELVIS,
			LEFT_UPPER_LEG,
			RIGHT_UPPER_LEG,
			LEFT_LOWER_LEG,
			RIGHT_LOWER_LEG,
			UPPER_SPINE,
			LEFT_CLAVICLE,
			RIGHT_CLAVICLE,
			NECK,
			HEAD,
			LEFT_UPPER_ARM,
			RIGHT_UPPER_ARM,
			LEFT_LOWER_ARM,
			RIGHT_LOWER_ARM,
		};
		MyPhysX::RagdollNode* ragdollData[] =
		{
			new MyPhysX::RagdollNode(physx::PxQuat(physx::PxPi / 2.0f, MyPhysX::Z_AXIS), NO_PARENT,1,3,1,1,"lower spine"),
			new MyPhysX::RagdollNode(physx::PxQuat(physx::PxPi, MyPhysX::Z_AXIS), LOWER_SPINE, 1,1,-1,1,"left pelvis"),
			new MyPhysX::RagdollNode(physx::PxQuat(0, MyPhysX::Z_AXIS), LOWER_SPINE, 1,1,-1, 1,"right pelvis"),
			new MyPhysX::RagdollNode(physx::PxQuat(physx::PxPi / 2.0f + 0.2f, MyPhysX::Z_AXIS),LEFT_PELVIS,5,2,-1,1,"L upper leg"),
			new MyPhysX::RagdollNode(physx::PxQuat(physx::PxPi / 2.0f - 0.2f, MyPhysX::Z_AXIS),RIGHT_PELVIS,5,2,-1,1,"R upper leg"),
			new MyPhysX::RagdollNode(physx::PxQuat(physx::PxPi / 2.0f + 0.2f, MyPhysX::Z_AXIS),LEFT_UPPER_LEG,5,1.75,-1,1,"L lower leg"),
			new MyPhysX::RagdollNode(physx::PxQuat(physx::PxPi / 2.0f - 0.2f, MyPhysX::Z_AXIS),RIGHT_UPPER_LEG,5,1.75,-1,1,"R lowerleg"),
			new MyPhysX::RagdollNode(physx::PxQuat(physx::PxPi / 2.0f, MyPhysX::Z_AXIS), LOWER_SPINE, 1, 3, 1, -1, "upper spine"),
			new MyPhysX::RagdollNode(physx::PxQuat(physx::PxPi, MyPhysX::Z_AXIS), UPPER_SPINE, 1, 1.5, 1, 1, "left clavicle"),
			new MyPhysX::RagdollNode(physx::PxQuat(0, MyPhysX::Z_AXIS), UPPER_SPINE, 1, 1.5, 1, 1, "right clavicle"),
			new MyPhysX::RagdollNode(physx::PxQuat(physx::PxPi / 2.0f, MyPhysX::Z_AXIS), UPPER_SPINE, 1, 1, 1, -1, "neck"),
			new MyPhysX::RagdollNode(physx::PxQuat(physx::PxPi / 2.0f, MyPhysX::Z_AXIS), NECK, 1, 3, 1, -1, "head"),
			new MyPhysX::RagdollNode(physx::PxQuat(physx::PxPi - .3, MyPhysX::Z_AXIS), LEFT_CLAVICLE, 3, 1.5, -1, 1, "left upper arm"),
			new MyPhysX::RagdollNode(physx::PxQuat(0.3, MyPhysX::Z_AXIS), RIGHT_CLAVICLE, 3, 1.5, -1, 1, "right upper arm"),
			new MyPhysX::RagdollNode(physx::PxQuat(physx::PxPi - .3, MyPhysX::Z_AXIS), LEFT_UPPER_ARM, 3, 1, -1, 1, "left lower arm"),
			new MyPhysX::RagdollNode(physx::PxQuat(0.3, MyPhysX::Z_AXIS), RIGHT_UPPER_ARM, 3, 1, -1, 1, "right lower arm"),
			new MyPhysX::RagdollNode(physx::PxQuat(physx::PxPi / 2.0f, MyPhysX::Y_AXIS),LEFT_LOWER_LEG,1,1.5F,-1,1,"L foot"),
			new MyPhysX::RagdollNode(physx::PxQuat(physx::PxPi / 2.0f, MyPhysX::Y_AXIS),RIGHT_LOWER_LEG,1,1.5F,-1,1,"R foot"),
			new MyPhysX::RagdollNode(physx::PxQuat(physx::PxPi / 2.0f, MyPhysX::Y_AXIS),LEFT_LOWER_ARM,0.5F,1.5F,-1,1,"L arm"),
			new MyPhysX::RagdollNode(physx::PxQuat(physx::PxPi / 2.0f, MyPhysX::Y_AXIS),RIGHT_LOWER_ARM,0.5F,1.5F,-1,1,"R arm"),
			NULL
		};
		m_psys->AddArticulation(ragdollData, 0.3F, "box", physx::PxVec3(20, 20, 20), physx::PxQuat(glm::radians(glm::linearRand(0.0F, 360.0F)), physx::PxVec3(0, 1, 0)));
	}
	//////////////////////////////////

	//Add liquid box
	{
		PxBoxGeometry side1(4.5, 1, .5);
		PxBoxGeometry side2(.5, 1, 4.5);

		m_psys->Add("wall0", MyPhysX::PhysXFilter::ePLATFORM, MyPhysX::PhysXFilter::ePLAYER | MyPhysX::PhysXFilter::eGROUND, "default", 10.0F, physx::PxVec3(0),
			side1,
			PxVec3(0.0f, 0.5, 4.0f), physx::PxQuat(physx::PxIDENTITY::PxIdentity), true, false);

		m_psys->Add("wall1", MyPhysX::PhysXFilter::ePLATFORM, MyPhysX::PhysXFilter::ePLAYER | MyPhysX::PhysXFilter::eGROUND, "default", 10.0F, physx::PxVec3(0),
			side1,
			PxVec3(0.0f, 0.5, -4.0f), physx::PxQuat(physx::PxIDENTITY::PxIdentity), true, false);

		m_psys->Add("wall2", MyPhysX::PhysXFilter::ePLATFORM, MyPhysX::PhysXFilter::ePLAYER | MyPhysX::PhysXFilter::eGROUND, "default", 10.0F, physx::PxVec3(0),
			side2,
			PxVec3(4.0f, 0.5, 0), physx::PxQuat(physx::PxIDENTITY::PxIdentity), true, false);

		m_psys->Add("wall3", MyPhysX::PhysXFilter::ePLATFORM, MyPhysX::PhysXFilter::ePLAYER | MyPhysX::PhysXFilter::eGROUND, "default", 10.0F, physx::PxVec3(0),
			side2,
			PxVec3(-4.0f, 0.5, 0), physx::PxQuat(physx::PxIDENTITY::PxIdentity), true, false);
	}
	////////////////

	//Add the liquid
	m_psys->AddLiquidSource(true, vec4(1, 0.5F, 0, 0.85F), vec3(0, 20, 0), vec3(10, 0, 0), 0.5F, 0.075F, 200.0F, 0.8F, 0.75F, 0.2F, 0.0F, 0.0F, 0.3F, 6000);
	m_psys->AddLiquidSource(true, vec4(0.3F, 0.3F, 0.5F, 1.0F), vec3(10, 10, 6), vec3(-10, 22,  0), 0.2F, 0.075F, 5.0F, 0.8F, 0.75F, 0.0F, 0.2F, 0.0F, 0.3F, 6000);
	////////////////

	//Create convex hull mesh
	{
		vector<float> verts;
		vector<unsigned int> inds;
		physx::PxConvexMesh * cm = m_psys->GenerateConvexHullMesh(complexMesh.GetVertices(), verts, inds, 128);
		if (cm)
		{
			complexImpMesh.Create(verts, inds);
			m_psys->Add("test convex", 0, 0, "box", 100.0F, physx::PxVec3(0), physx::PxConvexMeshGeometry(cm), physx::PxVec3(30, 100, 30),
				physx::PxQuat(glm::radians(glm::linearRand(0.0F, 360.0F)), physx::PxVec3(0, 1, 0)), false, false, &complexMesh, &complexImpMesh);
		}

		else
		{
			printf("Failed to create the convex mesh.\n");
			system("pause");
		}
	}
	/////////////////////////

	//Build janga tower
	for (int t = 0; t < 50; ++t)
	{
		int alt = t % 2;
		//float ang =  alt * 180.0F;
		physx::PxQuat quat(0, PxVec3(0, 1, 0));

		PxBoxGeometry side = alt ? PxBoxGeometry(8, 0.998F, 2) : PxBoxGeometry(2, 0.998F, 8);

		float d = 2.0F * (float)(70 - t) / 50;// block density decreases as tower goes up.

		m_psys->Add("jwall0", MyPhysX::PhysXFilter::ePLATFORM, MyPhysX::PhysXFilter::ePLAYER | MyPhysX::PhysXFilter::eGROUND, "box", d, physx::PxVec3(0),
			side,
			PxVec3(alt ? 0.0f : -5.0F, t * 2, alt ? -5.0f : 0.0F) + PxVec3(100, 1.0F, 0), quat);

		m_psys->Add("jwall0", MyPhysX::PhysXFilter::ePLATFORM, MyPhysX::PhysXFilter::ePLAYER | MyPhysX::PhysXFilter::eGROUND, "box", d, physx::PxVec3(0),
			side,
			PxVec3(alt ? 0.0f : 5.0F, t * 2, alt ? 5.0f : 0.0F) + PxVec3(100, 1.0F, 0), quat);
	}
	///////////////////

	return true;
}

void PhysXApplication::shutdown() {

	complexMesh.Destroy();
	complexImpMesh.Destroy();
	blobMesh.Destroy();
	blobImpMesh.Destroy();

	//////////////////////////////////////////////////////////////////////////
	// YOUR SHUTDOWN CODE HERE
	//////////////////////////////////////////////////////////////////////////
	delete m_psys;

	// delete our camera and cleanup gizmos
	delete m_camera;
	Gizmos::destroy();

	// destroy our window properly
	destroyWindow();
}

bool PhysXApplication::update(float deltaTime)
{
	// close the application if the window closes
	{
		if (glfwWindowShouldClose(m_window) ||
			glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			return false;
	}

	// update the camera's movement
	m_camera->update(deltaTime);

	// clear the gizmos out for this frame
	Gizmos::clear();

	//Update the physics system
	m_psys->Update(deltaTime);

	//Ball gun
	{
		vec4 positionVec = m_camera->getTransform()[3];
		vec4 forwardVec = m_camera->getTransform()[2];
		vec4 rightVec = m_camera->getTransform()[0];

		static bool leftMouseClick = false, leftMouseClickPrev = false;

		leftMouseClick = glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS;

		if (leftMouseClick && !leftMouseClickPrev)
		{
			const float density = 5.5F;

			float r = glm::linearRand(3.0F, 6.0F);
			vec3 v = vec3(-forwardVec) * 50.0F + glm::ballRand(7.0F);

			m_psys->Add("Bullet",
				MyPhysX::PhysXFilter::ePLAYER, MyPhysX::PhysXFilter::ePLAYER | MyPhysX::PhysXFilter::eGROUND,
				"box",
				10,
				physx::PxVec3(v.x, v.y, v.z),
				physx::PxSphereGeometry(1.0F),
				physx::PxVec3(positionVec.x, positionVec.y, positionVec.z),
				physx::PxQuat(glm::radians(glm::linearRand(0.0F, 360.0F)), physx::PxVec3(0, 1, 0)));
			

		}

		leftMouseClickPrev = leftMouseClick;
	}

	//Create falling shapes
	/*{
		{
			static float t = 0;
			t += deltaTime;
			if (t > 2.0F)
			{
				m_psys->Add("FallingSphere",
					MyPhysX::PhysXFilter::ePLAYER, MyPhysX::PhysXFilter::ePLAYER | MyPhysX::PhysXFilter::eGROUND,
					"box",
					1000,
					physx::PxVec3(0),
					physx::PxSphereGeometry(glm::linearRand(0.5F, 10.0F)),
					physx::PxVec3(30, 50, 30),
					physx::PxQuat(glm::radians(glm::linearRand(0.0F, 360.0F)), physx::PxVec3(0, 1, 0)));
				t = 0.0F;
			}
		}
		{
			static float t = 1;
			t += deltaTime;
			if (t > 2.0F)
			{
				m_psys->Add("FallingCapsule",
					MyPhysX::PhysXFilter::ePLAYER, MyPhysX::PhysXFilter::ePLAYER | MyPhysX::PhysXFilter::eGROUND,
					"box",
					1000,
					physx::PxVec3(0),
					physx::PxCapsuleGeometry(glm::linearRand(0.5F, 3.0F), glm::linearRand(5, 9)),
					physx::PxVec3(30, 50, 30),
					physx::PxQuat(glm::radians(glm::linearRand(0.0F, 360.0F)), physx::PxVec3(0, 1, 0)));
				t = 0.0F;
			}
		}
		{
			static float t = 2;
			t += deltaTime;
			if (t > 2.0F)
			{
				m_psys->Add("FallingBox", MyPhysX::PhysXFilter::eGROUND, MyPhysX::PhysXFilter::ePLAYER | MyPhysX::PhysXFilter::eGROUND, "box",
					1000,
					physx::PxVec3(0),
					physx::PxBoxGeometry(glm::linearRand(0.5F, 3.0F), glm::linearRand(0.5F, 3.0F), glm::linearRand(0.5F, 3.0F)),
					physx::PxVec3(30, 50, 30),
					physx::PxQuat(glm::radians(glm::linearRand(0.0F, 360.0F)), physx::PxVec3(0, 1, 0)));
				t = 0.0F;
			}
		}
	}*/

	//Render our two meshes and a test point.
	{
		Gizmos::addMesh(complexMesh, vec3(0, 10, 10), vec4(1, 0, 0, 0.5F));
		Gizmos::addMesh(blobMesh, vec3(-10, 10, -10), vec4(1, 1, 0, 0.5F));
		Gizmos::addPoint(vec3(0, 10, 0), vec4(1, 0, 0, 1));
	}

	//Clear-scene code. Press C
	{
		if (glfwGetKey(m_window, GLFW_KEY_C) == GLFW_PRESS)
		{
			m_psys->Clear();
			m_psys->Add("Ground", MyPhysX::PhysXFilter::eNONE, MyPhysX::PhysXFilter::eNONE, "default", 0, physx::PxVec3(0), physx::PxPlaneGeometry(),
				physx::PxVec3(0.0f, 0, 0.0f),
				physx::PxQuat(
					physx::PxHalfPi,
					physx::PxVec3(0.0f, 0.0f, 1.0f).getNormalized()),
				true);
		}
	}

	// return true, else the application closes
	return true;
}

void PhysXApplication::draw() {

	// clear the screen for this frame
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//////////////////////////////////////////////////////////////////////////
	// DRAW YOUR THINGS HERE
	//////////////////////////////////////////////////////////////////////////

	m_psys->Render();

	// display the 3D gizmos
	Gizmos::draw(m_camera->getProjectionView());

	// get a orthographic projection matrix and draw 2D gizmos
	int width = 0, height = 0;
	glfwGetWindowSize(m_window, &width, &height);
	mat4 guiMatrix = glm::ortho<float>(0, (float)width, 0, (float)height);

	Gizmos::draw2D(guiMatrix);
}

bool PhysXApplication::LoadObj(const char * p_path, vector<float> & p_vertices, vector<unsigned int> & p_indices, const vec3 & p_scale)
{
	vector<tinyobj::shape_t> shapes;
	vector<tinyobj::material_t> materials;//Not used
	string err;

	if (tinyobj::LoadObj(shapes, materials, err, p_path, nullptr, false))
	{
		tinyobj::mesh_t & mesh = shapes[0].mesh;

		unsigned int vertCount = mesh.positions.size() / 3;

		p_vertices.resize(vertCount * 8);
		for (int n = 0; n < vertCount; ++n)
		{
			for (int j = 0; j < 3; ++j)
				p_vertices[n * 8 + j] = mesh.positions[n * 3 + j] * p_scale[j];

			for (int j = 3; j < 8; ++j)
				p_vertices[n * 8 + j] = 1.0F;
		}

		p_indices = mesh.indices;

		return true;
	}

	printf("Failed to load '%s': %s\n", p_path == nullptr ? "null" : p_path, err.c_str());

	return false;
}