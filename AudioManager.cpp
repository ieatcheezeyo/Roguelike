#include "AudioManager.h"

AudioManager::AudioManager() {}

AudioManager::~AudioManager() {
	for (auto& sfxPair : sfxMap) {
		std::cout << "<AudioManager> Destroying SFX: " << sfxPair.first << std::endl;
		Mix_FreeChunk(sfxPair.second);
	}
	sfxMap.clear();
}

Mix_Chunk* AudioManager::loadSFX(const char* name, const char* file) {
	Mix_Chunk* tempSFX = Mix_LoadWAV(file);
	if (!tempSFX) {
		std::cout << "Mix_LoadWAV() Failed: " << Mix_GetError() << std::endl;
		return nullptr;
	}
	sfxMap[name] = tempSFX;
	return sfxMap[name];
}

void AudioManager::playSFX(const std::string& soundName) {
	if (sfxMap.find(soundName) != sfxMap.end()) {
		Mix_PlayChannel(-1, sfxMap[soundName], 0);
	} else {
		std::cerr << "Error: Sound effect '" << soundName << "' not found in AudioManager!\n";
	}
}