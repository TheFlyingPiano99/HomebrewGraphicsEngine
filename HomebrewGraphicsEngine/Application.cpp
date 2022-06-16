#include "HoGraEngineApplication.h"

#define WINDOW_NAME "HomebrewGraphicsEngineApp"

using namespace hograengine;

int main()
{
	auto app = HoGraEngineApplication();
	if (-1 == app.init(WINDOW_NAME)) {
		return -1;
	}
	app.loop();
	app.destroy();
	return 0;
}
