#include "PlayerController.h"
#include <PrismaEngine/input/InputManager.h>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

namespace PrismaCraft {

// ============================================================================
// PlayerController 实现
// ============================================================================

PlayerController::PlayerController() {
    // 默认选项
    controlOptions.mouseSensitivity = 0.1f;
    controlOptions.moveSpeed = 4.0f;
    controlOptions.jumpForce = 8.0f;
    controlOptions.gravity = 20.0f;
    controlOptions.enableFlight = false;
    controlOptions.lockMouse = true;
}

bool PlayerController::initialize(PrismaEngine::Input::IInputManager* inputMgr,
                                  PrismaEngine::Physics::CollisionSystem* collisionSys) {
    inputManager = inputMgr;
    collisionSystem = collisionSys;
    return true;
}

void PlayerController::update(float deltaTime) {
    if (!isActive || !controlledPlayer) {
        return;
    }

    // 处理移动
    handleMovement(deltaTime);

    // 处理视角
    handleRotation(deltaTime);

    // 应用重力
    if (!controlOptions.enableFlight) {
        applyGravity(deltaTime);
    }

    // 检查碰撞
    // TODO: 实现完整的碰撞检测
}

void PlayerController::handleKeyboardInput(int key, int scancode, int action, int mods) {
    if (!isActive) return;

    // WASD 移动
    switch (key) {
        case PRISMA_KEY_W:
            moveForward = (action != PRISMA_RELEASE);
            break;
        case PRISMA_KEY_S:
            moveBackward = (action != PRISMA_RELEASE);
            break;
        case PRISMA_KEY_A:
            moveLeft = (action != PRISMA_RELEASE);
            break;
        case PRISMA_KEY_D:
            moveRight = (action != PRISMA_RELEASE);
            break;
        case PRISMA_KEY_SPACE:
            if (action == PRISMA_PRESS) {
                jump();
            }
            break;
        case PRISMA_KEY_LEFT_SHIFT:
            controlledPlayer->setSneaking(action != PRISMA_RELEASE);
            break;
        case PRISMA_KEY_LEFT_CONTROL:
            controlledPlayer->setSprinting(action != PRISMA_RELEASE);
            break;
    }
}

void PlayerController::handleMouseButtonInput(int button, int action, int mods) {
    // 鼠标点击处理（方块交互）
    // TODO: 实现方块交互
}

void PlayerController::handleMousePosition(double xpos, double ypos) {
    if (!isActive || !controlOptions.lockMouse) {
        return;
    }

    // 计算鼠标增量
    static double lastX = xpos;
    static double lastY = ypos;

    double deltaX = xpos - lastX;
    double deltaY = ypos - lastY;

    lastX = xpos;
    lastY = ypos;

    // 更新视角
    yaw += static_cast<float>(deltaX * controlOptions.mouseSensitivity);
    pitch -= static_cast<float>(deltaY * controlOptions.mouseSensitivity);

    // 限制俯仰角（避免翻转）
    pitch = glm::clamp(pitch, -89.0f, 89.0f);
}

void PlayerController::handleMouseScroll(double xoffset, double yoffset) {
    // 滚轮处理（切换物品等）
    // TODO: 实现物品栏切换
}

void PlayerController::handleMovement(float deltaTime) {
    if (!controlledPlayer) return;

    // 计算移动方向
    glm::vec3 moveDirection(0, 0, 0);

    if (moveForward) moveDirection += getForwardDirection();
    if (moveBackward) moveDirection -= getForwardDirection();
    if (moveRight) moveDirection += getRightDirection();
    if (moveLeft) moveDirection -= getRightDirection();

    // 归一化
    if (glm::length(moveDirection) > 0.0f) {
        moveDirection = glm::normalize(moveDirection);
    }

    // 应用速度
    float speed = controlOptions.moveSpeed;
    if (controlledPlayer->isSprinting()) {
        speed *= 1.3f;  // 疾跑加速
    }
    if (controlledPlayer->isSneaking()) {
        speed *= 0.3f;  // 潜行减速
    }

    glm::vec3 movement = moveDirection * speed * deltaTime;

    // 更新玩家位置
    controlledPlayer->move(movement.x, 0, movement.z);
}

void PlayerController::handleRotation(float deltaTime) {
    if (!controlledPlayer) return;

    // 设置玩家朝向（基于 yaw 和 pitch）
    // 实际游戏中的玩家有独立的身体和头部旋转
    // 这里简化为整体旋转
}

void PlayerController::applyGravity(float deltaTime) {
    if (!controlledPlayer) return;

    // 应用重力
    velocity.y -= controlOptions.gravity * deltaTime;

    // 更新垂直位置
    controlledPlayer->move(0, velocity.y * deltaTime, 0);

    // 地面检测（简化版）
    if (controlledPlayer->getPosition().y < 64.0f) {  // 假设 Y=64 是地面
        controlledPlayer->setOnGround(true);
        velocity.y = 0;
        controlledPlayer->setPosition(
            controlledPlayer->getPosition().x,
            64.0f,
            controlledPlayer->getPosition().z
        );
    } else {
        controlledPlayer->setOnGround(false);
    }
}

void PlayerController::jump() {
    if (!controlledPlayer) return;

    // 只能在地面跳跃
    if (controlledPlayer->isOnGround() || controlOptions.enableFlight) {
        velocity.y = controlOptions.jumpForce;
        controlledPlayer->setOnGround(false);
    }
}

glm::vec3 PlayerController::getForwardDirection() const {
    // 根据视角计算前方向量（只考虑水平旋转）
    glm::vec3 forward;
    forward.x = glm::cos(glm::radians(yaw));
    forward.z = glm::sin(glm::radians(yaw));
    forward.y = 0;
    return glm::normalize(forward);
}

glm::vec3 PlayerController::getRightDirection() const {
    // 右方向量 = 前方向量 × 上方向量
    return glm::normalize(glm::cross(getForwardDirection(), getUpDirection()));
}

glm::vec3 PlayerController::getPlayerPosition() const {
    return controlledPlayer ? controlledPlayer->getPosition() : glm::vec3(0);
}

glm::quat PlayerController::getPlayerRotation() const {
    // 将 yaw 和 pitch 转换为四元数
    glm::quat pitchQuat = glm::angleAxis(glm::radians(pitch), glm::vec3(1, 0, 0));
    glm::quat yawQuat = glm::angleAxis(glm::radians(yaw), glm::vec3(0, 1, 0));
    return yawQuat * pitchQuat;
}

void PlayerController::handleCollisions(const glm::vec3& movement) {
    // TODO: 实现完整的碰撞检测
    // 需要检查玩家 AABB 与周围方块 AABB 的碰撞
    if (!collisionSystem || !controlledPlayer) {
        return;
    }

    // 获取玩家 AABB
    PrismaEngine::Physics::AABB playerAABB = controlledPlayer->getBoundingBox();

    // 移动 AABB
    PrismaEngine::Physics::AABB movedAABB = playerAABB.move(movement);

    // 检查碰撞（需要从 Level 获取周围方块）
    // bool onGround;
    // glm::dvec3 adjustedMovement = movement;
    // PrismaEngine::Physics::CollisionSystem::resolveCollisions(
    //     playerAABB, adjustedMovement, worldCollisions, onGround
    // );
}

} // namespace PrismaCraft
