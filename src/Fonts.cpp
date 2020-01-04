#include "Fonts.hpp"

struct Fonts Fonts;

void Fonts::Init() {
	std::cout << "Loaded " << this->LoadFonts() << " fonts" << std::endl;
}

void Fonts::Quit() {
	for (auto F : this->fonts) {
		this->FreeFont(std::get<1>(F));
	}
}

int Fonts::OpenFont(const std::string & filename, Font & font) {
	font._8pt = TTF_OpenFont(filename.c_str(), 8);
	font._12pt = TTF_OpenFont(filename.c_str(), 12);
	font._16pt = TTF_OpenFont(filename.c_str(), 16);
	font._24pt = TTF_OpenFont(filename.c_str(), 24);
	font._32pt = TTF_OpenFont(filename.c_str(), 32);
	font._64pt = TTF_OpenFont(filename.c_str(), 64);

	if ((font._8pt == nullptr) || (font._12pt == nullptr) || (font._16pt == nullptr)
		|| (font._24pt == nullptr) || (font._32pt == nullptr) || (font._64pt == nullptr))
	{
		Log::PrintTTFError();
		return -1;
	}

	return 0;
}

void Fonts::FreeFont(Font & f) {
	TTF_CloseFont(f._8pt);
	TTF_CloseFont(f._12pt);
	TTF_CloseFont(f._16pt);
	TTF_CloseFont(f._24pt);
	TTF_CloseFont(f._32pt);
	TTF_CloseFont(f._64pt);
}

int Fonts::LoadFonts() {
	std::vector<std::string> files = GetAllFilesInDir(TTF_FOLDER);

	int count = 0;
	for (auto f : files) {
		if (!this->AddFont(f)) {
			++count;
			std::cout << "Loaded font \"" << f << "\"" << std::endl;
		}
	}

	return count;
}

int Fonts::AddFont(const std::string & filename) {
	Font font;
	if (!this->OpenFont(filename, font)) {
		std::string str = filename;
		for (auto c = str.begin(); c != str.end(); ++c)
			if (*c == '/')
				*c = '\\';

		this->fonts[str] = font;
		return 0;
	}
	return -1;
}

TTF_Font* Fonts::GetFont(const std::string & str, FONT_SIZE size) {
	auto t = this->fonts.find(str);

	// if not found
	if (t == this->fonts.end()) {
		return nullptr;
	}

	switch (size) {
	case FONT_8PT:
		return t->second._8pt;
	case FONT_12PT:
		return t->second._12pt;
	case FONT_16PT:
		return t->second._16pt;
	case FONT_24PT:
		return t->second._24pt;
	case FONT_32PT:
		return t->second._32pt;
	case FONT_64PT:
		return t->second._64pt;
	}

	return nullptr;
}
