#include "Levels.hpp"

Brush::Brush(Rect_F _brush, BRUSH_TYPE _type, const std::string & str):
	brush(_brush), type(_type), key(str)
{ ; }

LEVEL_OPEN_STATE OpenLevel(const std::string & filename, Level & level) {
	std::string fname = filename;
#ifdef __linux
	for (auto c = fname.begin(); c != fname.end(); ++c) {
		if (*c == '\\')	
			*c = '/';
	}
#endif
	std::ifstream file;
	file.open(fname);
	if (!file.is_open()) {
		file.open(LEVELS_FOLDER + fname);
		if (!file.is_open()) {
			return LEVEL_OPEN_NO_FILE;
		}
	}

	level = Level();

	int line_count = 1;
	std::string line;
	while (!file.eof()) {
		std::getline(file, line);
		if (file.fail()) {
			continue;
		}

		if (line.size() <= 1)
			continue;

		//std::cout << line << std::endl;

		std::string temp = "";
		std::vector<std::string> split = std::vector<std::string>();
		for (auto c : line) {
			if (c == ' ') {
				split.push_back(temp);
				temp = "";
				continue;
			}

			temp += c;
		}

		split.push_back(temp);

#ifdef __linux__
		split.back() = split.back().substr(0, split.back().size() - 1);
#endif

		Brush brush;
		if (split.size() < 4) {
			std::cerr << "OpenLevel() error : line " << line_count << " in file " <<
				fname << " expected at least X Y W H" << std::endl;
			file.close();
			return LEVEL_OPEN_ERROR;
		}

		try {
			brush.brush.pos.x  = std::stof(split[0]);
			brush.brush.pos.y  = std::stof(split[1]);
			brush.brush.size.w = std::stof(split[2]);
			brush.brush.size.h = std::stof(split[3]);

			brush.brush = brush.brush.Absolute();
		} catch (...) {
			std::cerr << "OpenLevel() error : line " << line_count << " in file " <<
				fname << " : X Y W H expected valid numbers" << std::endl;
		}

		if (split.size() > 4) {
			if      (split[4] == "BRUSH_SOLID")
				brush.type = BRUSH_SOLID;
			else if (split[4] == "BRUSH_NON_SOLID")
				brush.type = BRUSH_NON_SOLID;
			else if (split[4] == "BRUSH_KILL")
				brush.type = BRUSH_KILL;
			else if (split[4] == "BRUSH_NEXT_LEVEL")
				brush.type = BRUSH_NEXT_LEVEL;
			else if (split[4] == "BRUSH_DECAL")
				brush.type = BRUSH_DECAL;
			else {
				std::cerr << "OpenLevel() warning : line " << line_count << " in file " <<
					fname << " : TYPE expected a valid brush type (defaulting to BRUSH_SOLID)" << std::endl;
				brush.type = BRUSH_SOLID;
			}

			if (split.size() > 5) {
				brush.key = split[5];

				if (split.size() - 6 > 0) {
					std::cerr << "OpenLevel() warning : line " << line_count << " in file " <<
						fname << " : " << split.size() - 6 << " extra bit(s) of data omitted" << std::endl;
				}
			}
		}

		level.brushes.push_back(brush);

		++line_count;
	}

	return LEVEL_OPEN_SUCCESS;
}

std::string GetLevelName(const std::string & filename) {
	std::string new_str = "";

	for (auto c = filename.end() - 1;; --c) {
		if (*c == '\\' || *c == '/')
			return new_str;

		char C = *c;
		if (C == '_')
			C = ' ';
		new_str = C + new_str;

		if (c == filename.begin())
			break;
	}

	return new_str;
}
