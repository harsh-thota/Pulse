//#include <iostream>
#include "ui/application.h"

int main(int argc, char* argv[])
{
	Application app;
	if (!app.Initialize()) return 1;
	app.Run();
	app.Shutdown();
	return 0;
}