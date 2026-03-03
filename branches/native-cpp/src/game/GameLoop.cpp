#include "GameLoop.h"
#include <thread>
#include <iostream>

namespace PrismaCraft {

GameLoop::GameLoop(const GameLoopConfig& config)
    : config_(config)
    , running_(false)
    , paused_(false) {

    lastTime_ = std::chrono::high_resolution_clock::now();
}

GameLoop::~GameLoop() {
    stop();
}

void GameLoop::run() {
    if (running_) return;

    running_ = true;
    lastTime_ = std::chrono::high_resolution_clock::now();
    accumulator_ = 0.0f;

    while (running_) {
        // Calculate delta time
        currentTime_ = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> delta = currentTime_ - lastTime_;
        deltaTime_ = delta.count();

        // Clamp delta time to prevent spiral of death
        if (deltaTime_ > config_.maxFrameTime) {
            deltaTime_ = config_.maxFrameTime;
        }

        lastTime_ = currentTime_;

        if (!paused_) {
            // Fixed time step update
            accumulator_ += deltaTime_;

            while (accumulator_ >= config_.fixedDeltaTime) {
                update(config_.fixedDeltaTime);
                accumulator_ -= config_.fixedDeltaTime;
                tickCount_++;
                gameTime_ += config_.fixedDeltaTime;
            }

            // Calculate interpolation alpha
            alpha_ = accumulator_ / config_.fixedDeltaTime;

            // Render with interpolation
            render(alpha_);
            frameCount_++;

            // Update FPS/TPS counters
            fpsTimer_ += deltaTime_;
            tpsTimer_ += deltaTime_;

            if (fpsTimer_ >= 1.0f) {
                fps_ = frameCount_;
                frameCount_ = 0;
                fpsTimer_ = 0.0f;
            }

            if (tpsTimer_ >= 1.0f) {
                tps_ = tickCount_;
                tickCount_ = 0;
                tpsTimer_ = 0.0f;
            }
        }

        // Handle input
        handleInput();

        // Frame rate limiting
        if (config_.vsync) {
            // VSync is handled by the graphics API
            // Just sleep a bit to avoid 100% CPU
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        } else {
            float targetFrameTime = 1.0f / config_.targetFPS;
            float elapsed = deltaTime_;

            if (elapsed < targetFrameTime) {
                float sleepTime = targetFrameTime - elapsed;
                std::this_thread::sleep_for(
                    std::chrono::duration<float>(sleepTime)
                );
            }
        }
    }
}

void GameLoop::stop() {
    running_ = false;
}

void GameLoop::update(float deltaTime) {
    // Call update callback
    if (updateCallback_) {
        updateCallback_(deltaTime);
    }
}

void GameLoop::render(float alpha) {
    // Call render callback
    if (renderCallback_) {
        renderCallback_(alpha);
    }
}

void GameLoop::handleInput() {
    // Call input callback
    if (inputCallback_) {
        inputCallback_();
    }
}

} // namespace PrismaCraft
