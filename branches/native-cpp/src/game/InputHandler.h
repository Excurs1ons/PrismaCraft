#pragma once

#include <PrismaCraft/PrismaCraft.h>
#include "world/entity/player/PlayerController.h"
#include <memory>
#include <unordered_map>
#include <functional>

namespace PrismaCraft {

// Forward declarations
namespace PrismaEngine {
    class InputManager;
}

/**
 * Input handler
 * Maps input events to game actions
 */
class PRISMACRAFT_API InputHandler {
public:
    // Key codes
    enum class Key {
        // Movement
        W = 0x57,
        A = 0x41,
        S = 0x53,
        D = 0x44,
        Space = 0x20,
        Shift = 0x10,
        Ctrl = 0x11,

        // Actions
        E = 0x45,     // Use/Interact
        Q = 0x51,     // Drop item
        F = 0x46,     // Swap item
        ESC = 0x1B,   // Pause/Menu

        // Inventory
        I = 0x49,     // Inventory
        B = 0x42,     // Creative mode search

        // Debug
        F3 = 0x72,    // Debug info
        F1 = 0x70,    // Hide HUD
    };

    // Mouse buttons
    enum class MouseButton {
        Left = 0,
        Right = 1,
        Middle = 2
    };

    InputHandler();
    ~InputHandler();

    // Initialize with input manager
    void init(PrismaEngine::InputManager* inputManager);

    // Update input state (call each frame)
    void update(float deltaTime);

    // Key events
    bool isKeyDown(Key key) const;
    bool wasKeyDown(Key key) const;
    bool isKeyPressed(Key key) const; // Just pressed
    bool isKeyReleased(Key key) const; // Just released

    // Mouse events
    bool isMouseButtonDown(MouseButton button) const;
    bool isMouseButtonPressed(MouseButton button) const;
    bool isMouseButtonReleased(MouseButton button) const;

    // Mouse position
    float getMouseX() const { return mouseX_; }
    float getMouseY() const { return mouseY_; }
    float getMouseDeltaX() const { return mouseDeltaX_; }
    float getMouseDeltaY() const { return mouseDeltaY_; }

    // Mouse scroll
    float getScrollX() const { return scrollX_; }
    float getScrollY() const { return scrollY_; }

    // Player controller
    void setPlayerController(PlayerController* controller) {
        playerController_ = controller;
    }

    // Input callbacks
    using KeyEventCallback = std::function<void(Key, bool)>;
    void setKeyEventCallback(KeyEventCallback callback) {
        keyEventCallback_ = std::move(callback);
    }

    using MouseEventCallback = std::function<void(float, float)>;
    void setMouseMoveCallback(MouseEventCallback callback) {
        mouseMoveCallback_ = std::move(callback);
    }

    // State management
    void updateKeyState(Key key, bool down);
    void updateMouseButtonState(MouseButton button, bool down);
    void updateMousePosition(float x, float y);
    void updateScroll(float x, float y);

    // Clear frame-specific state
    void clearFrameState();

private:
    PrismaEngine::InputManager* inputManager_;
    PlayerController* playerController_;

    // Key state
    std::unordered_map<Key, bool> keyStates_;
    std::unordered_map<Key, bool> previousKeyStates_;

    // Mouse button state
    std::unordered_map<MouseButton, bool> mouseButtonStates_;
    std::unordered_map<MouseButton, bool> previousMouseButtonStates_;

    // Mouse position
    float mouseX_ = 0.0f;
    float mouseY_ = 0.0f;
    float previousMouseX_ = 0.0f;
    float previousMouseY_ = 0.0f;
    float mouseDeltaX_ = 0.0f;
    float mouseDeltaY_ = 0.0f;

    // Mouse scroll
    float scrollX_ = 0.0f;
    float scrollY_ = 0.0f;

    // Callbacks
    KeyEventCallback keyEventCallback_;
    MouseEventCallback mouseMoveCallback_;
};

} // namespace PrismaCraft
