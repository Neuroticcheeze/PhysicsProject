#include "IConstraint.h"

using namespace Physics;

IConstraint::IConstraint(IPhysicsObject * p_obj1, IPhysicsObject * p_obj2) :
	m_obj1(p_obj1),
	m_obj2(p_obj2),
	m_destroyed(false)
{

}

IConstraint::~IConstraint()
{

}

void IConstraint::Contstrain(PhysicsScene * p_system)
{
}