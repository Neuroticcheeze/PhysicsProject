#include "PhysXRagdoll.h"

using namespace MyPhysX;

physx::PxArticulation* MyPhysX::makeRagdoll(physx::PxPhysics* g_Physics, RagdollNode** nodeArray, physx::PxTransform worldPos, float scaleFactor, physx::PxMaterial* ragdollMaterial)
{
	//create the articulation for our ragdoll
	physx::PxArticulation *articulation = g_Physics->createArticulation();
	RagdollNode** currentNode = nodeArray;

	//while there are more nodes to process...
	while (*currentNode != NULL)
	{
		//get a pointer to the current node
		RagdollNode* currentNodePtr = *currentNode;

		//create a pointer ready to hold the parent node pointer if there is one
		RagdollNode* parentNode = nullptr;
		//get scaled values for capsule
		float radius = currentNodePtr->radius*scaleFactor;
		float halfLength = currentNodePtr->halfLength*scaleFactor;
		float childHalfLength = radius + halfLength;
		float parentHalfLength = 0; //will be set later if there is a parent
									//get a pointer to the parent
		physx::PxArticulationLink* parentLinkPtr = NULL;
		currentNodePtr->scaledGobalPos = worldPos.p;

		if (currentNodePtr->parentNodeIdx != -1)
		{
			//if there is a parent then we need to work out our local position for the link
			//get a pointer to the parent node
			parentNode = *(nodeArray + currentNodePtr->parentNodeIdx);
			//get a pointer to the link for the parent
			parentLinkPtr = parentNode->linkPtr;
			parentHalfLength = (parentNode->radius + parentNode->halfLength) *scaleFactor;
			//work out the local position of the node
			physx::PxVec3 currentRelative = currentNodePtr->childLinkPos * currentNodePtr->globalRotation.rotate(physx::PxVec3(childHalfLength + 0.5F, 0, 0));
			physx::PxVec3 parentRelative = -currentNodePtr->parentLinkPos * parentNode->globalRotation.rotate(physx::PxVec3(parentHalfLength + 0.5F, 0, 0));
			currentNodePtr->scaledGobalPos = parentNode->scaledGobalPos - (parentRelative + currentRelative);
		}

		//build the transform for the link
		physx::PxTransform linkTransform = physx::PxTransform(currentNodePtr->scaledGobalPos, currentNodePtr->globalRotation);
		//create the link in the articulation
		physx::PxArticulationLink* link = articulation->createLink(parentLinkPtr, linkTransform);
		//add the pointer to this link into the ragdoll data so we have it for later when we want to link to it
		currentNodePtr->linkPtr = link;
		float jointSpace = .01f; //gap between joints
		float capsuleHalfLength = (halfLength > jointSpace ? halfLength - jointSpace : 0) + .01f;
		physx::PxCapsuleGeometry capsule(radius, capsuleHalfLength);
		link->createShape(capsule, *ragdollMaterial); //adds a capsule collider to the link
		physx::PxRigidBodyExt::updateMassAndInertia(*link, 50.0f); //adds some mass, mass should really be part of the data!

		if (currentNodePtr->parentNodeIdx != -1)
		{
			//get the pointer to the joint from the link
			physx::PxArticulationJoint *joint = link->getInboundJoint();
			//get the relative rotation of this link
			physx::PxQuat frameRotation = parentNode->globalRotation.getConjugate() *
				currentNodePtr->globalRotation;
			//set the parent contraint frame
			physx::PxTransform parentConstraintFrame = physx::PxTransform(physx::PxVec3(currentNodePtr->parentLinkPos * parentHalfLength, 0, 0), frameRotation);
			//set the child constraint frame (this the constraint frame of the newly added link)
			physx::PxTransform thisConstraintFrame = physx::PxTransform(physx::PxVec3(currentNodePtr->childLinkPos * childHalfLength, 0, 0));
			//set up the poses for the joint so it is in the correct place
			joint->setParentPose(parentConstraintFrame);
			joint->setChildPose(thisConstraintFrame);
			//set up some constraints to stop it flopping around
			joint->setStiffness(20);
			joint->setDamping(20);
			joint->setSwingLimit(0.4f, 0.4f);
			joint->setSwingLimitEnabled(true);
			joint->setTwistLimit(-0.1f, 0.1f);
			joint->setTwistLimitEnabled(true);
		}

		currentNode++;
	}
	return articulation;
}