#include "Sound.hpp"

struct Sound Sound;

void Sound::Init() {
	std::cout << "Loaded " << this->LoadChunks() << " audio chunks" << std::endl;
	int music_loaded = this->LoadMusic();
	std::cout << "Loaded " <<  music_loaded << " music tracks" << std::endl;

	if (music_loaded > 0) {
		Mix_HookMusicFinished(&PlayNextSong);
		this->SortMusicTracks();
	}

	Mix_ChannelFinished(&RemoveChunkPlaying);
}

void Sound::Quit() {
	for (auto T : this->chunks) {
		Mix_FreeChunk( std::get<1>(T) );
	}

	for (auto T : this->music) {
		Mix_FreeMusic( T.music );
	}

	this->chunks.clear();
	this->music.clear();
}

void Sound::ToggleMusic() {
	if (Mix_PausedMusic()) {
		Mix_ResumeMusic();
	}
	else {
		Mix_PauseMusic();
	}
}

Mix_Chunk * Sound::GetChunk(const std::string & str) {
	auto t = this->chunks.find(str);

	// if not found
	if (t == this->chunks.end())
		return nullptr;

	return t->second;
}

Music* Sound::GetMusic(const std::string & str) {
	auto t = this->music.begin();
	for (; t != this->music.end(); ++t) {
		if (t->name == str)
			break;
	}

	// if not found
	if (t == this->music.end())
		return nullptr;

	return &(*t);
}

int Sound::PlayChunk(std::string & filename, int volume, int loops, bool affect_by_zoom) {
	Mix_Chunk* chunk = this->GetChunk(filename);
	if (chunk == nullptr)
		return -1;

	Chunk_Playing playing;
	playing.affect_by_zoom = affect_by_zoom;
	playing.chunk = chunk;
	Mix_VolumeChunk(chunk, volume);
	playing.channel = Mix_PlayChannel(-1, chunk, loops);
	if (playing.channel == -1) {
		Log::PrintMIXError();
		return -1;
	}

	this->chunks_playing.push_back(playing);
	return playing.channel;
}

void Sound::PlayMusic(std::string & filename) {
	Mix_Music* music = (this->GetMusic(filename)->music);
	if (music == nullptr)
		return;
	if (Mix_PlayMusic(music, 1) == -1) {
		Log::PrintMIXError();
	}
}

void Sound::ChangeSong(int change) {
	this->curr_track += change;
	this->curr_track %= (int)this->music.size();
	if (this->curr_track == -1)
		this->curr_track = (signed int)this->music.size() - 1;

	try {
		Music& mus = this->music.at(this->curr_track);
		Mix_PlayMusic(mus.music, 1);
		this->music_changed = true;
		this->song_filename = mus.name;
	}
	catch (...) {}
}

void PlayNextSong() {
	Sound.ChangeSong(1);
}

void RemoveChunkPlaying(int channel) {
	for (auto P = Sound.chunks_playing.begin(); P != Sound.chunks_playing.end(); ++P) {
		if (P->channel == channel) {
			Sound.chunks_playing.erase(P);
			return;
		}
	}
}

void Sound::StopMusic() {
	Mix_HaltMusic();
}

void Sound::StopAllSound() {
	this->chunks_playing.clear();
	Mix_HaltChannel(-1);
}

void Sound::PauseAllSound() {
	for (auto S : this->chunks_playing) {
		Mix_Pause(S.channel);
	}
}

void Sound::ResumeAllSound() {
	for (auto S : this->chunks_playing) {
		Mix_Resume(S.channel);
	}
}

int Sound::LoadChunks() {
	std::vector<std::string> files = GetAllFilesInDir(SFX_FOLDER);

	int count = 0;
	for (auto f : files) {
		if (!this->AddChunk(f)) {
			++count;
			std::cout << "Loaded audio chunk \"" << f << "\"" << std::endl;
		}
	}

	return count;
}

int Sound::AddChunk(const std::string & filename) {
	Mix_Chunk * chunk = Mix_LoadWAV(filename.c_str());
	if (chunk == nullptr) {
		Log::PrintMIXError();
		return -1;
	}

	std::string str = filename;
	for (auto c = str.begin(); c != str.end(); ++c)
		if (*c == '/')
			*c = '\\';
	this->chunks[str] = chunk;
	return 0;
}

int Sound::LoadMusic() {
	std::vector<std::string> files = GetAllFilesInDir(MUSIC_FOLDER);

	int count = 0;
	for (auto f : files) {
		if (!this->AddMusic(f)) {
			++count;
			std::cout << "Loaded music track \"" << f << "\"" << std::endl;
		}
	}

	return count;
}

int Sound::AddMusic(const std::string & filename) {
	Mix_Music * music = Mix_LoadMUS(filename.c_str());
	if (music == nullptr) {
		Log::PrintMIXError();
		return -1;
	}

	Music mus;
	mus.music = music;
	mus.name = filename;
	this->music.push_back(mus);
	return 0;
}

void Sound::SortMusicTracks() {
	std::sort(this->music.begin(), this->music.end(), _CompareTrackNumbers);
}

std::string Sound::GetTrackName(std::string & name) {
	bool slash_found = false, dot_found = false;
	int slash_position = 0, dot_position = 0;
	std::string str = name;
	for (auto c = str.begin(); c != str.end(); ++c) {
		if (!slash_found) {
			if (*c == '\\' || *c == '/')
				slash_found = true;
			++slash_position;
		}

		if (*c == '_')
			*c = ' ';
	}

	for (int i = str.size() - 1; i >= 0; --i) {
		if (str.at(i) == '.') {
			dot_found = true;
			dot_position = i;
			break;
		}
	}

	if (dot_found)
		str = str.substr(0, dot_position);
	if (slash_found)
		str.erase(0, slash_position);

	return str;
}

int Sound::GetTrackNumber(std::string track) {
	try {
		// should get the track number at the beginning of a trackname
		// VV
		// 01.song
		std::string str = track.substr(0, 2);
		return std::stoi(str);
	}
	catch (...) {
		return -1;
	}
}

bool _CompareTrackNumbers(Music& a, Music& b) {
	int a_track = Sound.GetTrackNumber(Sound.GetTrackName(a.name));
	int b_track = Sound.GetTrackNumber(Sound.GetTrackName(b.name));

	if (a_track == -1 || b_track == -1)
		return false;
	else return a_track < b_track;
}
