#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <cstdarg>

#include <SDL.h>
#include <SDL_image.h>

struct Message {
	std::string message;
	float time = 10.0f;
	float timer = 0.0f;
};

class TextConsole {

public:
	TextConsole();
	~TextConsole();

	void addMessage(const std::string& msg);
	void addMessage(const char* format, ...);
	void update(double dt);

	std::vector<Message*> messages;

};

