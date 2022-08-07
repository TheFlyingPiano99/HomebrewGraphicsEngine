#include "HoGraEngineApplication.h"

#define WINDOW_NAME "HomebrewGraphicsEngineApp"

using namespace Hogra;

int main()
{
	auto app = HoGraEngineApplication();
	if (-1 == app.Init(WINDOW_NAME)) {
		return -1;
	}
	app.Loop();
	app.Destroy();
	return 0;
}
