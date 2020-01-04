#pragma	once

#include <vector>
#include <iostream>
#include <fstream>
#include <string>

#include "Vec2.hpp"

#ifdef _WIN32
#define LEVELS_FOLDER "levels\\"
#elif defined __linux__
#define LEVELS_FOLDER "levels/"
#endif

// BRUSH_SOLID		- Blocks player and rockets
// BRUSH_NON_SOLID	- Blocks player, doesn't block rockets
// BRUSH_KILL		- Kills player upon contact
// BRUSH_NEXT_LEVEL	- Loads the next level whose filename is the brushes 'key', if 'key' is empty, goes back to menu
// BRUSH_DECAL		- Decal of the texture whos name is the brushes 'key'
enum BRUSH_TYPE { BRUSH_SOLID , BRUSH_NON_SOLID , BRUSH_KILL , BRUSH_NEXT_LEVEL , BRUSH_DECAL };

struct Brush {
	Brush() = default;
	Brush(Rect_F _brush, BRUSH_TYPE _type, const std::string& str);

	Rect_F brush = Rect_F(0.0f, 0.0f, 0.0f, 0.0f);
	BRUSH_TYPE type = BRUSH_SOLID;
	std::string key = "";
};

/*
	LEVEL FILE FORMAT
	One brush = one line
	X Y W H TYPE KEY
	X Y W H are required, TYPE is default to BRUSH_SOLID and key can be omitted
	NO WHITESPACES ALLOWED OUTSIDE OF SPLITTING APART VALUES AND NEWLINE FOR A NEW BRUSH
*/

class Level {
public:
	Level() = default;
	std::vector<Brush> brushes = std::vector<Brush>();
};

enum LEVEL_OPEN_STATE { LEVEL_OPEN_SUCCESS , LEVEL_OPEN_IO_ERROR , LEVEL_OPEN_NO_FILE , LEVEL_OPEN_ERROR };
LEVEL_OPEN_STATE OpenLevel(const std::string& filename, Level& level);
std::string GetLevelName(const std::string& filename);