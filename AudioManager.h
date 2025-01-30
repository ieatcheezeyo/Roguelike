#pragma once

#include <iostream>
#include <unordered_map>

#include <SDL.h>
#include <SDL_mixer.h>

class AudioManager {

public:
	AudioManager();
	~AudioManager();

	Mix_Chunk* loadSFX(const char* name, const char* file);
	void playSFX(const std::string& name);

private:
	std::unordered_map<std::string, Mix_Chunk*> sfxMap;

};

