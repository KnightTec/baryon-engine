#pragma once
#include "Renderer.h"
#include "EntityManager.h"
#include "System.h"



namespace Baryon
{
	class testsys : public System<Transform, StaticMesh>
	{
		using super = System<Transform, StaticMesh>;
	public:
		testsys(EntityManager*  entityManager) : super(entityManager)
		{

		}
		void update(Transform& t, StaticMesh& sm) override
		{
			t.position.x += 1;
			int i = 1 + 3;
			i--;
		}
	};


/*
 * The core class of the Baryon Engine
 */
class Engine
{
public:
	Engine();
	/*
	 * Must be called before any other function
	 */
	bool initialize();
	void mainLoopIteration();
	void terminate();
	Renderer& getRenderer();
private:
	Renderer renderer;
	EntityManager em;
	testsys tst;
};

inline Engine::Engine() : tst(&em)
{
	
}


inline Renderer& Engine::getRenderer()
{
	return renderer;
}

}
