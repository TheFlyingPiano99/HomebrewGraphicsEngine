#include "HoGraEngineApplication.h"

#define WINDOW_NAME "HograApp"

using namespace Hogra;

int main(int argc, char* argv[])
{
	auto app = HoGraEngineApplication();
	
	if (-1 == app.Init(WINDOW_NAME, argc, argv)) {
		return -1;
	}
	app.Loop();
	app.Destroy();

	return 0;
}
