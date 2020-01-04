#include "Log.hpp"

void Log::PrintSDLError() {
	std::cerr << "SDL::ERROR::" << SDL_GetError() << std::endl;
}

void Log::PrintIMGError() {
	std::cerr << "IMG::ERROR::" << IMG_GetError() << std::endl;
}

void Log::PrintMIXError() {
	std::cerr << "MIX::ERROR::" << Mix_GetError() << std::endl;
}

void Log::PrintTTFError() {
	std::cerr << "TTF::ERROR::" << TTF_GetError() << std::endl;
}

void Log::PrintLogError(const std::string& s) {
	std::cerr << "ERROR::" << s << std::endl;
}