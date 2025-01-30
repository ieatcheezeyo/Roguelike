#include "TextConsole.h"

TextConsole::TextConsole() {}

TextConsole::~TextConsole() {
	for (auto& message : messages) {
		delete message;
	}
	messages.clear();
}

void TextConsole::addMessage(const std::string& msg) {
    if (messages.size() >= 8) {
        delete messages.front();
        messages.erase(messages.begin());
    }

    Message* message = new Message{ msg, 5.0f, 0.0f };
    messages.push_back(message);
}

void TextConsole::addMessage(const char* format, ...) {
    if (messages.size() >= 8) {
        delete messages.front();
        messages.erase(messages.begin());
    }


    char buffer[1024];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    Message* message = new Message{ std::string(buffer), 5.0f, 0.0f };
    messages.push_back(message);

}

void TextConsole::update(double dt) {
    for (auto it = messages.begin(); it != messages.end(); ) {
        Message* message = *it;
        message->timer += static_cast<float>(dt);

        if (message->timer >= message->time) {
            delete message;
            it = messages.erase(it);
        } else {
            ++it;
        }
    }
}