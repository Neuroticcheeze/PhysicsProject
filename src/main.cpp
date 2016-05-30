
#define _PHYSXw

#ifdef _PHYSX
#include "PhysXApplication.h"
#define _APP PhysXApplication
#else
#include "TestApplication.h"
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