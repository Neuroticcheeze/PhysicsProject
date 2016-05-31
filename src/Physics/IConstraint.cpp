#include "IConstraint.h"

#include "IPhysicsObject.h"

using namespace Physics;

IConstraint::IConstraint(IPhysicsObject * p_obj1, IPhysicsObject * p_obj2) :
	m_obj1(p_obj1),
	m_obj2(p_obj2),
	m_destroyed(false)
{
	assert(m_obj1 != nullptr && m_obj2 != nullptr);

	m_obj1->m_involvedConstraints.push_back(this);
	m_obj2->m_involvedConstraints.push_back(this);
}

IConstraint::~IConstraint()
{
	if (m_obj1 != nullptr && !m_obj1->Destroyed())
	{
		m_obj1->m_involvedConstraints.erase(std::find(m_obj1->m_involvedConstraints.begin(), m_obj1->m_involvedConstraints.end(), this));
		m_obj1->m_constraintBroke = true;
	}

	if (m_obj2 != nullptr && !m_obj2->Destroyed())
	{
		m_obj2->m_involvedConstraints.erase(std::find(m_obj2->m_involvedConstraints.begin(), m_obj2->m_involvedConstraints.end(), this));
		m_obj2->m_constraintBroke = true;
	}
}

void IConstraint::Contstrain(PhysicsScene * p_scene)
{
}