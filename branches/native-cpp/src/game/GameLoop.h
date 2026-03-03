#pragma once

#include <PrismaCraft/PrismaCraft.h>
#include "world/level/Level.h"
#include "world/entity/Entity.h"
#include "world/entity/player/Player.h"
#include <memory>
#include <chrono>
#include <functional>

namespace PrismaCraft {

/**
 * Game loop configuration
 */
struct GameLoopConfig {
    int targetFPS = 60;
    int maxTPS = 20; // Ticks per second (game logic updates)
    float fixedDeltaTime = 0.05f; // 20 ticks per second
    bool vsync = true;
    float maxFrameTime = 0.25f; // 防止螺旋式死亡
};

/**
 * Game loop
 * Manages the main game loop, fixed time step updates, and rendering
 */
class PRISMACRAFT_API GameLoop {
public:
    explicit GameLoop(const GameLoopConfig& config = GameLoopConfig());
    ~GameLoop();

    // Start game loop
    void run();

    // Stop game loop
    void stop();

    // Update functions
    using UpdateCallback = std::function<void(float deltaTime)>;
    void setUpdateCallback(UpdateCallback callback) { updateCallback_ = std::move(callback); }

    // Render function
    using RenderCallback = std::function<void(float alpha)>;
    void setRenderCallback(RenderCallback callback) { renderCallback_ = std::move(callback); }

    // Input handling
    using InputCallback = std::function<void()>;
    void setInputCallback(InputCallback callback) { inputCallback_ = std::move(callback); }

    // Time management
    float getDeltaTime() const { return deltaTime_; }
    float getAlpha() const { return alpha_; }
    int getFPS() const { return fps_; }
    int getTPS() const { return tps_; }
    float get gameTime() const { return gameTime_; }

    // Pause/resume
    void pause() { paused_ = true; }
    void resume() { paused_ = false; }
    bool isPaused() const { return paused_; }

    // Configuration
    const GameLoopConfig& getConfig() const { return config_; }
    void setConfig(const GameLoopConfig& config) { config_ = config; }

private:
    void update(float deltaTime);
    void render(float alpha);
    void handleInput();

    // Time management
    std::chrono::high_resolution_clock::time_point currentTime_;
    std::chrono::high_resolution_clock::time_point lastTime_;
    float deltaTime_ = 0.0f;
    float accumulator_ = 0.0f;
    float alpha_ = 0.0f;
    float gameTime_ = 0.0f;

    // FPS/TPS tracking
    int frameCount_ = 0;
    int tickCount_ = 0;
    float fpsTimer_ = 0.0f;
    float tpsTimer_ = 0.0f;
    int fps_ = 0;
    int tps_ = 0;

    // State
    bool running_ = false;
    bool paused_ = false;

    // Configuration
    GameLoopConfig config_;

    // Callbacks
    UpdateCallback updateCallback_;
    RenderCallback renderCallback_;
    InputCallback inputCallback_;
};

} // namespace PrismaCraft
