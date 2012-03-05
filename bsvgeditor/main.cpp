#include "SVGView.h"
#include "EditorApp.h"
#include <stdio.h>

int
main(int argc, char **argv)
{
	EditorApp *app = new EditorApp(argc, argv);
	app->Run();
	delete app;
	
	return 0;
}
