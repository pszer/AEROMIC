#include "Core.hpp"

int main(int argc, char** argv) {
	if (!Core.Init(argc, argv)) {
		Core.MainLoop();
	}

	Core.Quit();

	return 0;
}