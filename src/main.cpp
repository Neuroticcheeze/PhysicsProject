
#define _PHYSX

#ifdef _PHYSX
#include "PhysX/PhysXApplication.h"
#define _APP PhysXApplication
#else
#include "Physics/TestApplication.h"
#define _APP TestApplication
#endif

#include <GLFW/glfw3.h>

int main()
{
	_APP* app = new _APP();
	if (app->startup())
		app->run();
	app->shutdown();

	return 0;
}