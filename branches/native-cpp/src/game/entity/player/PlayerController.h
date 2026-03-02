#pragma once

#include "Player.h"
#include <PrismaEngine/PrismaEngine.h>
#include <glm/glm.hpp>
#include <memory>

namespace PrismaCraft {

/**
 * @brief 玩家控制器 - 处理输入和控制玩家
 *
 * 对应 Minecraft 的玩家控制系统
 * 功能：
 * - WASD 移动
 * - 空格跳跃
 * - 鼠标视角控制
 * - 碰撞检测
 * - 方块交互
 */
class PlayerController {
public:
    PlayerController();
    ~PlayerController() = default;

    /**
     * @brief 初始化控制器
     */
    bool initialize(PrismaEngine::Input::IInputManager* inputManager,
                   PrismaEngine::Physics::CollisionSystem* collisionSystem);

    /**
     * @brief 设置控制的玩家
     */
    void setPlayer(Player* player) { controlledPlayer = player; }
    Player* getPlayer() const { return controlledPlayer; }

    /**
     * @brief 更新控制器（每帧调用）
     */
    void update(float deltaTime);

    /**
     * @brief 处理输入事件
     */
    void handleKeyboardInput(int key, int scancode, int action, int mods);
    void handleMouseButtonInput(int button, int action, int mods);
    void handleMousePosition(double xpos, double ypos);
    void handleMouseScroll(double xoffset, double yoffset);

    /**
     * @brief 控制选项
     */
    struct ControlOptions {
        float mouseSensitivity = 0.1f;     // 鼠标灵敏度
        float moveSpeed = 4.0f;            // 移动速度（单位/秒）
        float jumpForce = 8.0f;            // 跳跃力
        float gravity = 20.0f;             // 重力加速度
        bool enableFlight = false;         // 是否启用飞行（创造模式）
        bool lockMouse = true;             // 是否锁定鼠标
    };

    void setControlOptions(const ControlOptions& options) { controlOptions = options; }
    const ControlOptions& getControlOptions() const { return controlOptions; }

    /**
     * @brief 获取相机方向
     */
    glm::vec3 getForwardDirection() const;
    glm::vec3 getRightDirection() const;
    glm::vec3 getUpDirection() const { return glm::vec3(0, 1, 0); }

    /**
     * @brief 获取玩家视角
     */
    glm::vec3 getPlayerPosition() const;
    glm::quat getPlayerRotation() const;

    /**
     * @brief 激活/停用控制器
     */
    void setActive(bool active) { isActive = active; }
    bool is_active() const { return isActive; }

private:
    /**
     * @brief 处理移动输入
     */
    void handleMovement(float deltaTime);

    /**
     * @brief 处理视角旋转
     */
    void handleRotation(float deltaTime);

    /**
     * @brief 应用重力
     */
    void applyGravity(float deltaTime);

    /**
     * @brief 执行跳跃
     */
    void jump();

    /**
     * @brief 碰撞检测和响应
     */
    void handleCollisions(const glm::vec3& movement);

    Player* controlledPlayer = nullptr;
    PrismaEngine::Input::IInputManager* inputManager = nullptr;
    PrismaEngine::Physics::CollisionSystem* collisionSystem = nullptr;

    // 视角
    float yaw = 0.0f;      // 偏航角（左右）
    float pitch = 0.0f;    // 俯仰角（上下）

    // 移动状态
    bool moveForward = false;
    bool moveBackward = false;
    bool moveLeft = false;
    bool moveRight = false;
    bool wantsJump = false;

    // 控制状态
    bool isActive = true;
    ControlOptions controlOptions;

    // 速度
    glm::vec3 velocity{0, 0, 0};
};

} // namespace PrismaCraft
