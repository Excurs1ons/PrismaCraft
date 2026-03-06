#include "Player.h"
#include "../../world/entity/Entity.h"
#include "../../world/level/Level.h"
#include <algorithm>
#include <cmath>

namespace PrismaCraft {

// ============================================================================
// Player 实现
// ============================================================================

Player::Player(Level* level, const glm::dvec3& position)
    : Entity(nullptr, level) {

    // 设置玩家位置
    pos = Vec3(position.x, position.y, position.z);

    // 设置玩家尺寸（Minecraft 玩家：宽 0.6，高 1.8）
    width = 0.6f;
    height = 1.8f;

    // 初始化碰撞盒
    refreshDimensions();

    // 设置玩家名称
    name = "Player";
}

void Player::tick() {
    // 基础实体更新
    baseTick();

    // TODO: 处理玩家输入、跳跃、重力等
}

void Player::move(double dx, double dy, double dz) {
    // 使用带碰撞检测的移动
    moveWithCollision(dx, dy, dz);
}

void Player::moveWithCollision(double dx, double dy, double dz) {
    if (!level) {
        // 如果没有世界，直接移动
        pos = pos.add(dx, dy, dz);
        refreshDimensions();
        return;
    }

    // 分轴处理碰撞（允许贴墙滑动）

    // === X 轴移动 ===
    AABB newBoxX = boundingBox.move(dx, 0, 0);
    auto collisionsX = getCollidingBoxes(newBoxX);

    if (collisionsX.empty()) {
        // X 轴无碰撞，应用移动
        pos = pos.add(dx, 0, 0);
        boundingBox = newBoxX;
    } else {
        // X 轴有碰撞，计算允许的移动距离
        double allowedX = 0.0;
        if (dx > 0) {
            // 向右移动
            double maxAllowedX = collisionsX[0].minX;
            for (const auto& box : collisionsX) {
                if (box.minX < maxAllowedX) {
                    maxAllowedX = box.minX;
                }
            }
            allowedX = maxAllowedX - boundingBox.maxX - 0.001;
        } else if (dx < 0) {
            // 向左移动
            double minAllowedX = collisionsX[0].maxX;
            for (const auto& box : collisionsX) {
                if (box.maxX > minAllowedX) {
                    minAllowedX = box.maxX;
                }
            }
            allowedX = minAllowedX - boundingBox.minX + 0.001;
        }

        // 应用允许的移动
        pos = pos.add(allowedX, 0, 0);
        boundingBox = boundingBox.move(allowedX, 0, 0);
    }

    // === Y 轴移动 ===
    AABB newBoxY = boundingBox.move(0, dy, 0);
    auto collisionsY = getCollidingBoxes(newBoxY);

    if (collisionsY.empty()) {
        // Y 轴无碰撞，应用移动
        pos = pos.add(0, dy, 0);
        boundingBox = newBoxY;
        onGround = false;
    } else {
        // Y 轴有碰撞
        if (dy < 0) {
            // 向下移动，检测是否落地
            onGround = true;

            // 计算地面位置
            double maxGroundY = collisionsY[0].maxY;
            for (const auto& box : collisionsY) {
                if (box.maxY > maxGroundY) {
                    maxGroundY = box.maxY;
                }
            }

            // 设置玩家位置到地面
            double newY = maxGroundY - boundingBox.minY;
            pos = pos.add(0, newY, 0);
            boundingBox = boundingBox.move(0, newY, 0);

            // 清除向下的速度
            delta.y = 0;
        } else if (dy > 0) {
            // 向上移动（碰撞天花板）
            double minCeilingY = collisionsY[0].minY;
            for (const auto& box : collisionsY) {
                if (box.minY < minCeilingY) {
                    minCeilingY = box.minY;
                }
            }

            // 设置玩家位置到天花板下方
            double newY = minCeilingY - boundingBox.maxY - 0.001;
            pos = pos.add(0, newY, 0);
            boundingBox = boundingBox.move(0, newY, 0);

            // 清除向上的速度
            delta.y = 0;
        }
    }

    // === Z 轴移动 ===
    AABB newBoxZ = boundingBox.move(0, 0, dz);
    auto collisionsZ = getCollidingBoxes(newBoxZ);

    if (collisionsZ.empty()) {
        // Z 轴无碰撞，应用移动
        pos = pos.add(0, 0, dz);
        boundingBox = newBoxZ;
    } else {
        // Z 轴有碰撞，计算允许的移动距离
        double allowedZ = 0.0;
        if (dz > 0) {
            // 向前移动
            double maxAllowedZ = collisionsZ[0].minZ;
            for (const auto& box : collisionsZ) {
                if (box.minZ < maxAllowedZ) {
                    maxAllowedZ = box.minZ;
                }
            }
            allowedZ = maxAllowedZ - boundingBox.maxZ - 0.001;
        } else if (dz < 0) {
            // 向后移动
            double minAllowedZ = collisionsZ[0].maxZ;
            for (const auto& box : collisionsZ) {
                if (box.maxZ > minAllowedZ) {
                    minAllowedZ = box.maxZ;
                }
            }
            allowedZ = minAllowedZ - boundingBox.minZ + 0.001;
        }

        // 应用允许的移动
        pos = pos.add(0, 0, allowedZ);
        boundingBox = boundingBox.move(0, 0, allowedZ);
    }
}

std::vector<AABB> Player::getCollidingBoxes(const AABB& box) const {
    std::vector<AABB> boxes;

    if (!level) {
        return boxes;
    }

    // 计算实体 AABB 影响的方块范围
    int minX = static_cast<int>(std::floor(box.minX));
    int maxX = static_cast<int>(std::floor(box.maxX));
    int minY = static_cast<int>(std::floor(box.minY));
    int maxY = static_cast<int>(std::floor(box.maxY));
    int minZ = static_cast<int>(std::floor(box.minZ));
    int maxZ = static_cast<int>(std::floor(box.maxZ));

    // 遍历范围内的所有方块
    for (int x = minX; x <= maxX; ++x) {
        for (int y = minY; y <= maxY; ++y) {
            for (int z = minZ; z <= maxZ; ++z) {
                // 通过 Level 查询方块（支持跨区块）
                const auto& blockState = level->getBlockState(BlockPos(x, y, z));

                // 跳过非固体方块
                if (!blockState.blocksMotion()) {
                    continue;
                }

                // 添加方块 AABB
                boxes.emplace_back(
                    x, y, z,
                    x + 1, y + 1, z + 1
                );
            }
        }
    }

    return boxes;
}

void Player::refreshDimensions() {
    // 根据当前位置和尺寸更新碰撞盒
    // 玩家的碰撞盒以位置为中心
    double halfWidth = width * 0.5;
    boundingBox = AABB(
        pos.x - halfWidth,
        pos.y,
        pos.z - halfWidth,
        pos.x + halfWidth,
        pos.y + height,
        pos.z + halfWidth
    );
}

} // namespace PrismaCraft
