#pragma once

#include <iostream>

#include <SDL.h>

class InputManager {

public:
    InputManager();
    ~InputManager();

    void update();
    void lateUpdate();
    void close();

    bool quit = false;

    bool button_a = false;
    bool button_b = false;
    bool button_x = false;
    bool button_y = false;

    bool button_start = false;
    bool button_select = false;

    bool button_l = false;
    bool button_r = false;

    bool dpad_up = false;
    bool dpad_down = false;
    bool dpad_left = false;
    bool dpad_right = false;

    bool old_button_a = false;
    bool old_button_b = false;
    bool old_button_x = false;
    bool old_button_y = false;

    bool old_button_start = false;
    bool old_button_select = false;

    bool old_button_l = false;
    bool old_button_r = false;

    bool old_dpad_up = false;
    bool old_dpad_down = false;
    bool old_dpad_left = false;
    bool old_dpad_right = false;

    int mouse_x = 0, mouse_y = 0;
    bool mouse_left = false;
    bool mouse_right = false;
    bool mouse_middle = false;

    bool old_mouse_left = false;
    bool old_mouse_middle = false;
    bool old_mouse_right = false;

    float left_joystick_x = 0.0f, left_joystick_y = 0.0f;
    float right_joystick_x = 0.0f, right_joystick_y = 0.0f;

private:
    SDL_Event event;
    SDL_GameController* controller = nullptr;

    float normalizeAxis(int value);
    void handleControllerEvents(const SDL_Event& event);
};

