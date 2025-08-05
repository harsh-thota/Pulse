#include "ui/application.hpp"

int main(int argc, char* argv[])
{
	Application app;
	if (!app.Initialize()) return 1;
	app.Run();
	app.Shutdown();
	return 0;
}