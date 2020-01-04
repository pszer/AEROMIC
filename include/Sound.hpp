#pragma once

#ifdef _WIN32
#include "SDL.h"
#include "SDL_mixer.h"
#elif defined __linux__
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#endif

#include <cstdlib>
#include <map>
#include <algorithm>

#include "Log.hpp"
#include "Filesystem.hpp"

#define SFX_FOLDER "sfx"
#define MUSIC_FOLDER "music"

#define MENU_SONG ""

void PlayNextSong();
void RemoveChunkPlaying(int channel);

struct Chunk_Playing {
	int channel;
	bool affect_by_zoom = true;
	Mix_Chunk* chunk;
};

struct Music {
	Mix_Music* music;
	std::string name;
};

extern struct Sound {
	void Init();
	void Quit();

	/* returns channel played on
	 * -1 for error */
	int PlayChunk(std::string& filename, int volume = MIX_MAX_VOLUME, int loops = 0, bool affect_by_zoom = false);
	void PlayMusic(std::string& filename);
	void ChangeSong(int change = 1);

	void StopMusic();
	void StopAllSound();
	void PauseAllSound();
	void ResumeAllSound();

	std::vector<Chunk_Playing> chunks_playing;

	std::map<std::string, Mix_Chunk*> chunks;
	//std::map<std::string, Mix_Music*> music;
	std::vector<Music> music;
	bool music_changed = false;
	int curr_track = 0;
	std::string song_filename = "";

	void ToggleMusic();

	Mix_Chunk* GetChunk(const std::string& str);
	Music* GetMusic(const std::string& str);

	/* returns how many chunks were loaded
	*/
	int LoadChunks();
	/* adds chunk to std::map<std::string, Mix_Chunk*> chunks
	* returns: 0 for success, -1 for error
	*/
	int AddChunk(const std::string& filename);

	/* returns how many tracks were loaded
	*/
	int LoadMusic();
	/* adds track to std::map<std::string, Mix_Chunk*> music
	* returns: 0 for success, -1 for error
	*/
	int AddMusic(const std::string& filename);
	void SortMusicTracks();

	std::string GetTrackName(std::string& name);
	int GetTrackNumber(std::string trackname); // returns -1 for an error
} Sound;

// used for sorting tracks
bool _CompareTrackNumbers(Music& a, Music& b);