#pragma once

namespace Physics
{
	class IPhysicsObject;
	class PhysicsScene;

	class IConstraint
	{
	public:

		IConstraint(IPhysicsObject * p_obj1, IPhysicsObject * p_obj2);
		virtual ~IConstraint();

		virtual void Contstrain(PhysicsScene * p_system);

		inline IPhysicsObject * GetObject1()	{ return m_obj1; };
		inline IPhysicsObject * GetObject2()	{ return m_obj2; };

		inline const bool & Destroyed() const	{ return m_destroyed; }
		inline void Destroy()					{ m_destroyed = true; }

	protected:

		IPhysicsObject * m_obj1;
		IPhysicsObject * m_obj2;

		bool m_destroyed;

	private:
	};
}