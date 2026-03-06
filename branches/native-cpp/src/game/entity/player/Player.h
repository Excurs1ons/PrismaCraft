#pragma once

#include "../Entity.h"
#include <PrismaEngine/PrismaEngine.h>
#include <glm/glm.hpp>

namespace PrismaCraft {

/**
 * @brief 玩家实体
 *
 * 对应 Minecraft: net.minecraft.world.entity.player.Player
 */
class Player : public Entity {
public:
    Player(Level* level, const glm::dvec3& position);
    ~Player() override = default;

    /**
     * @brief 玩家类型枚举
     */
    enum class PlayerType {
        SURVIVAL,    // 生存模式
        CREATIVE,    // 创造模式
        ADVENTURE,   // 冒险模式
        SPECTATOR    // 旁观模式
    };

    /**
     * @brief 游戏模式
     */
    void setGameMode(PlayerType type) { gameMode = type; }
    PlayerType getGameMode() const { return gameMode; }

    /**
     * @brief 速度属性
     */
    double getSpeed() const { return speed; }
    void setSpeed(double value) { speed = value; }

    /**
     * @brief 跳跃能力
     */
    double getJumpPower() const { return jumpPower; }
    void setJumpPower(double value) { jumpPower = value; }

    /**
     * @brief 玩家状态
     */
    bool isOnGround() const { return onGround; }
    void setOnGround(bool value) { onGround = value; }

    bool isSneaking() const { return sneaking; }
    void setSneaking(bool value) { sneaking = value; }

    bool isSprinting() const { return sprinting; }
    void setSprinting(bool value) { sprinting = value; }

    /**
     * @brief 玩家属性
     */
    float getHealth() const { return health; }
    void setHealth(float value) { health = value; }

    float getFoodLevel() const { return foodLevel; }
    void setFoodLevel(float value) { foodLevel = value; }

    /**
     * @brief 更新玩家
     */
    void tick() override;

    /**
     * @brief 玩家移动
     */
    void move(double dx, double dy, double dz);

    /**
     * @brief 获取碰撞检测盒列表
     * @param box 要检测的 AABB
     * @return 碰撞的方块 AABB 列表
     */
    std::vector<AABB> getCollidingBoxes(const AABB& box) const;

    /**
     * @brief 处理玩家移动（带碰撞检测）
     */
    void moveWithCollision(double dx, double dy, double dz);

    /**
     * @brief 刷新碰撞盒尺寸
     */
    void refreshDimensions();

private:
    PlayerType gameMode = PlayerType::SURVIVAL;

    // 移动属性
    double speed = 0.1;          // 移动速度
    double jumpPower = 0.42;     // 跳跃高度（Minecraft 默认）
    bool onGround = false;       // 是否在地面

    // 玩家状态
    bool sneaking = false;       // 是否潜行
    bool sprinting = false;      // 是否疾跑

    // 生存属性
    float health = 20.0f;        // 生命值
    float foodLevel = 20.0f;     // 饥饿值
};

} // namespace PrismaCraft
