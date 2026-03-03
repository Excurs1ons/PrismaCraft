#include "InputHandler.h"
#include <PrismaEngine/input/InputManager.h>

namespace PrismaCraft {

InputHandler::InputHandler()
    : inputManager_(nullptr)
    , playerController_(nullptr) {
}

InputHandler::~InputHandler() = default;

void InputHandler::init(PrismaEngine::InputManager* inputManager) {
    inputManager_ = inputManager;
}

void InputHandler::update(float deltaTime) {
    if (!inputManager_) return;

    // Update mouse position
    // Assuming InputManager has getMousePosition() method
    // float x, y;
    // inputManager_->getMousePosition(&x, &y);
    // updateMousePosition(x, y);

    // Update player controller
    if (playerController_) {
        // Movement
        bool moveForward = isKeyDown(Key::W);
        bool moveBackward = isKeyDown(Key::S);
        bool moveLeft = isKeyDown(Key::A);
        bool moveRight = isKeyDown(Key::D);
        bool jump = isKeyDown(Key::Space);
        bool sneak = isKeyDown(Key::Shift) || isKeyDown(Key::Ctrl);

        // Set player movement state
        // playerController_->setMoveForward(moveForward);
        // playerController_->setMoveBackward(moveBackward);
        // playerController_->setMoveLeft(moveLeft);
        // playerController_->setMoveRight(moveRight);

        // Actions
        if (isKeyPressed(Key::Space) && !jump) {
            // playerController_->jump();
        }

        // playerController_->setSneaking(sneak);

        // Mouse look
        // float pitch = getMouseDeltaY() * sensitivity;
        // float yaw = getMouseDeltaX() * sensitivity;
        // playerController_->rotate(pitch, yaw);
    }

    // Clear frame-specific state
    clearFrameState();
}

bool InputHandler::isKeyDown(Key key) const {
    auto it = keyStates_.find(key);
    return it != keyStates_.end() && it->second;
}

bool InputHandler::wasKeyDown(Key key) const {
    auto it = previousKeyStates_.find(key);
    return it != previousKeyStates_.end() && it->second;
}

bool InputHandler::isKeyPressed(Key key) const {
    return isKeyDown(key) && !wasKeyDown(key);
}

bool InputHandler::isKeyReleased(Key key) const {
    return !isKeyDown(key) && wasKeyDown(key);
}

bool InputHandler::isMouseButtonDown(MouseButton button) const {
    auto it = mouseButtonStates_.find(button);
    return it != mouseButtonStates_.end() && it->second;
}

bool InputHandler::isMouseButtonPressed(MouseButton button) const {
    auto it = mouseButtonStates_.find(button);
    auto prevIt = previousMouseButtonStates_.find(button);
    bool current = it != mouseButtonStates_.end() && it->second;
    bool previous = prevIt != previousMouseButtonStates_.end() && prevIt->second;
    return current && !previous;
}

bool InputHandler::isMouseButtonReleased(MouseButton button) const {
    auto it = mouseButtonStates_.find(button);
    auto prevIt = previousMouseButtonStates_.find(button);
    bool current = it != mouseButtonStates_.end() && it->second;
    bool previous = prevIt != previousMouseButtonStates_.end() && prevIt->second;
    return !current && previous;
}

void InputHandler::updateKeyState(Key key, bool down) {
    keyStates_[key] = down;

    if (keyEventCallback_) {
        keyEventCallback_(key, down);
    }
}

void InputHandler::updateMouseButtonState(MouseButton button, bool down) {
    mouseButtonStates_[button] = down;
}

void InputHandler::updateMousePosition(float x, float y) {
    mouseX_ = x;
    mouseY_ = y;
}

void InputHandler::updateScroll(float x, float y) {
    scrollX_ = x;
    scrollY_ = y;
}

void InputHandler::clearFrameState() {
    // Store current states as previous
    previousKeyStates_ = keyStates_;
    previousMouseButtonStates_ = mouseButtonStates_;

    // Reset mouse deltas
    previousMouseX_ = mouseX_;
    previousMouseY_ = mouseY_;
    mouseDeltaX_ = 0.0f;
    mouseDeltaY_ = 0.0f;

    // Reset scroll
    scrollX_ = 0.0f;
    scrollY_ = 0.0f;
}

} // namespace PrismaCraft
