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
	//create some constants for axis of rotation to make definition of quaternions a bit neater
	const physx::PxVec3 X_AXIS = physx::PxVec3(1, 0, 0);
	const physx::PxVec3 Y_AXIS = physx::PxVec3(0, 1, 0);
	const physx::PxVec3 Z_AXIS = physx::PxVec3(0, 0, 1);
	struct RagdollNode
	{
		physx::PxQuat globalRotation;	/*rotation of this link in model space - we could have done this relative to
										the parent node but it's harder to visualize when setting up the data by hand*/

		physx::PxVec3 scaledGobalPos;	/*Position of the link centre in world space which is calculated when we process
										the node.It's easiest if we store it here so we have it when we transform the child*/

		int parentNodeIdx;				//Index of the parent node

		float halfLength;				//half length of the capsule for this node

		float radius;					//radius of capsule for thisndoe

		float parentLinkPos;			/*relative position of link centre in parent to this node. 0 is the centre of hte node, 
										-1 is left end of capsule and 1 is right end of capsule relative to x*/

		float childLinkPos;				//relative position of link centre in child

		char* name;						//name of link

		physx::PxArticulationLink* linkPtr;

		//constructor
		RagdollNode(physx::PxQuat _globalRotation, int _parentNodeIdx, float _halfLength, float _radius, float _parentLinkPos, float _childLinkPos, char* _name)
		{ 
			globalRotation	= _globalRotation;
			parentNodeIdx	= _parentNodeIdx; 
			halfLength		= _halfLength; 
			radius			= _radius; 
			parentLinkPos	= _parentLinkPos; 
			childLinkPos	= _childLinkPos; 
			name			= _name;
		};
	};

	physx::PxArticulation* makeRagdoll(physx::PxPhysics* g_Physics, RagdollNode** nodeArray, physx::PxTransform worldPos, float scaleFactor, physx::PxMaterial* ragdollMaterial);
}