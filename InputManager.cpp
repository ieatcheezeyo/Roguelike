#include "InputManager.h"

InputManager::InputManager() {
    // Initialize the controller subsystem
    if (SDL_Init(SDL_INIT_GAMECONTROLLER) < 0) {
        std::cout << "Error : " << SDL_GetError() << std::endl;
    }

    // Open the first available controller
    if (SDL_NumJoysticks() > 0) {
        controller = SDL_GameControllerOpen(0);
        if (!controller) {
            std::cout << "Error : " << SDL_GetError() << std::endl;
        }
    }
}

InputManager::~InputManager() {
    close();
}

void InputManager::close() {
    if (controller) {
        SDL_GameControllerClose(controller);
        controller = nullptr;
    }
}

float InputManager::normalizeAxis(int value) {
    // SDL axis values range from -32768 to 32767, normalize to [-1.0, 1.0]
    const int MAX_AXIS_VALUE = 32767;
    if (value > MAX_AXIS_VALUE) value = MAX_AXIS_VALUE;  // Prevent overflow
    if (value < -MAX_AXIS_VALUE) value = -MAX_AXIS_VALUE;
    return static_cast<float>(value) / MAX_AXIS_VALUE;
}

void InputManager::update() {
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            quit = true;
        }
        else if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
            bool keyDown = (event.type == SDL_KEYDOWN);
            switch (event.key.keysym.scancode) {
            case SDL_SCANCODE_F1: quit = keyDown; break;
            case SDL_SCANCODE_Z: button_a = keyDown; break;
            case SDL_SCANCODE_X: button_b = keyDown; break;
            case SDL_SCANCODE_A: button_x = keyDown; break;
            case SDL_SCANCODE_S: button_y = keyDown; break;
            case SDL_SCANCODE_RETURN: button_start = keyDown; break;
            case SDL_SCANCODE_ESCAPE: button_select = keyDown; break;
            case SDL_SCANCODE_Q: button_l = keyDown; break;
            case SDL_SCANCODE_W: button_r = keyDown; break;
            case SDL_SCANCODE_UP: dpad_up = keyDown; break;
            case SDL_SCANCODE_DOWN: dpad_down = keyDown; break;
            case SDL_SCANCODE_LEFT: dpad_left = keyDown; break;
            case SDL_SCANCODE_RIGHT: dpad_right = keyDown; break;
            default: break;
            }
        }
        else if (event.type == SDL_MOUSEMOTION) {
            mouse_x = event.motion.x;
            mouse_y = event.motion.y;
        }
        else if (event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP) {
            bool mouseButtonDown = (event.type == SDL_MOUSEBUTTONDOWN);
            switch (event.button.button) {
            case SDL_BUTTON_LEFT: mouse_left = mouseButtonDown; break;
            case SDL_BUTTON_RIGHT: mouse_right = mouseButtonDown; break;
            case SDL_BUTTON_MIDDLE: mouse_middle = mouseButtonDown; break;
            default: break;
            }
        }
        else if (event.type == SDL_CONTROLLERBUTTONDOWN || event.type == SDL_CONTROLLERBUTTONUP) {
            handleControllerEvents(event);
        }
        else if (event.type == SDL_CONTROLLERAXISMOTION) {
            if (event.caxis.axis == SDL_CONTROLLER_AXIS_LEFTX) {
                left_joystick_x = normalizeAxis(event.caxis.value);
            }
            else if (event.caxis.axis == SDL_CONTROLLER_AXIS_LEFTY) {
                left_joystick_y = normalizeAxis(event.caxis.value);
            }
            else if (event.caxis.axis == SDL_CONTROLLER_AXIS_RIGHTX) {
                right_joystick_x = normalizeAxis(event.caxis.value);
            }
            else if (event.caxis.axis == SDL_CONTROLLER_AXIS_RIGHTY) {
                right_joystick_y = normalizeAxis(event.caxis.value);
            }
        }
    }
}

void InputManager::handleControllerEvents(const SDL_Event& event) {
    bool buttonDown = (event.type == SDL_CONTROLLERBUTTONDOWN);
    switch (event.cbutton.button) {
    case SDL_CONTROLLER_BUTTON_A: button_a = buttonDown; break;
    case SDL_CONTROLLER_BUTTON_B: button_b = buttonDown; break;
    case SDL_CONTROLLER_BUTTON_X: button_x = buttonDown; break;
    case SDL_CONTROLLER_BUTTON_Y: button_y = buttonDown; break;
    case SDL_CONTROLLER_BUTTON_START: button_start = buttonDown; break;
    case SDL_CONTROLLER_BUTTON_BACK: button_select = buttonDown; break;
    case SDL_CONTROLLER_BUTTON_DPAD_UP: dpad_up = buttonDown; break;
    case SDL_CONTROLLER_BUTTON_DPAD_DOWN: dpad_down = buttonDown; break;
    case SDL_CONTROLLER_BUTTON_DPAD_LEFT: dpad_left = buttonDown; break;
    case SDL_CONTROLLER_BUTTON_DPAD_RIGHT: dpad_right = buttonDown; break;
    case SDL_CONTROLLER_BUTTON_LEFTSHOULDER: button_l = buttonDown; break;
    case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER: button_r = buttonDown; break;
    default: break;
    }
}

void InputManager::lateUpdate() {
    old_button_a = button_a;
    old_button_b = button_b;
    old_button_x = button_x;
    old_button_y = button_y;
    old_button_start = button_start;
    old_button_select = button_select;
    old_button_l = button_l;
    old_button_r = button_r;
    old_dpad_up = dpad_up;
    old_dpad_down = dpad_down;
    old_dpad_left = dpad_left;
    old_dpad_right = dpad_right;

    old_mouse_left = mouse_left;
    old_mouse_middle = mouse_middle;
    old_mouse_right = mouse_right;
}

