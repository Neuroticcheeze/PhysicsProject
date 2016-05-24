#include "PhysXApplication.h"
#include <GLFW/glfw3.h>

int main()
{
	PhysXApplication* app = new PhysXApplication();
	if (app->startup())
		app->run();
	app->shutdown();

	return 0;
}