#include "BlockInteraction.h"
#include "world/entity/player/Player.h"
#include <PrismaEngine/physics/CollisionSystem.h>
#include <PrismaEngine/math/Vec3.h>
#include <algorithm>
#include <cmath>

namespace PrismaCraft {

BlockInteraction::BlockInteraction(Level* level, Player* player)
    : level_(level)
    , player_(player)
    , hasTarget_(false)
    , reachDistance_(5.0f) {

    targetBlockPos_ = BlockPos(0, 0, 0);
    placeBlockPos_ = BlockPos(0, 0, 0);
}

bool BlockInteraction::raycastToBlock(float maxDistance) {
    if (!player_) return false;

    Ray ray = getPlayerRay();
    return raycastToBlock(ray, maxDistance);
}

bool BlockInteraction::raycastToBlock(const Ray& ray, float maxDistance) {
    if (!level_) return false;

    ray_ = ray;
    hasTarget_ = false;

    // DDA (Digital Differential Analyzer) algorithm for voxel raycasting
    // This is more efficient than physics-based raycasting for blocks

    glm::vec3 origin = ray.origin;
    glm::vec3 direction = glm::normalize(ray.direction);

    // Current voxel position
    int x = static_cast<int>(std::floor(origin.x));
    int y = static_cast<int>(std::floor(origin.y));
    int z = static_cast<int>(std::floor(origin.z));

    // Step direction
    int stepX = (direction.x >= 0) ? 1 : -1;
    int stepY = (direction.y >= 0) ? 1 : -1;
    int stepZ = (direction.z >= 0) ? 1 : -1;

    // tMax - distance to next voxel boundary in each axis
    float tMaxX = (direction.x >= 0)
        ? (static_cast<float>(x + 1) - origin.x) / direction.x
        : (origin.x - static_cast<float>(x)) / -direction.x;

    float tMaxY = (direction.y >= 0)
        ? (static_cast<float>(y + 1) - origin.y) / direction.y
        : (origin.y - static_cast<float>(y)) / -direction.y;

    float tMaxZ = (direction.z >= 0)
        ? (static_cast<float>(z + 1) - origin.z) / direction.z
        : (origin.z - static_cast<float>(z)) / -direction.z;

    // tDelta - distance to traverse one voxel in each axis
    float tDeltaX = (direction.x != 0)
        ? static_cast<float>(stepX) / direction.x
        : std::numeric_limits<float>::max();

    float tDeltaY = (direction.y != 0)
        ? static_cast<float>(stepY) / direction.y
        : std::numeric_limits<float>::max();

    float tDeltaZ = (direction.z != 0)
        ? static_cast<float>(stepZ) / direction.z
        : std::numeric_limits<float>::max();

    // Step through voxels
    float distance = 0.0f;
    BlockPos previousPos(x, y, z);

    while (distance < maxDistance) {
        // Check if current block is solid
        BlockPos currentPos(x, y, z);
        const BlockState& state = level_->getBlockState(currentPos);

        if (state.isSolid(state)) {
            // Found solid block
            targetBlockPos_ = currentPos;
            placeBlockPos_ = previousPos;

            raycastResult_.point = origin + direction * distance;
            raycastResult_.distance = distance;
            raycastResult_.normal = glm::vec3(0, 1, 0); // TODO: Calculate actual normal

            hasTarget_ = true;
            return true;
        }

        // Move to next voxel
        previousPos = currentPos;

        if (tMaxX < tMaxY) {
            if (tMaxX < tMaxZ) {
                x += stepX;
                distance = tMaxX;
                tMaxX += tDeltaX;
            } else {
                z += stepZ;
                distance = tMaxZ;
                tMaxZ += tDeltaZ;
            }
        } else {
            if (tMaxY < tMaxZ) {
                y += stepY;
                distance = tMaxY;
                tMaxY += tDeltaY;
            } else {
                z += stepZ;
                distance = tMaxZ;
                tMaxZ += tDeltaZ;
            }
        }
    }

    hasTarget_ = false;
    return false;
}

Ray BlockInteraction::getPlayerRay() const {
    if (!player_) return Ray();

    Vec3 position = player_->getPosition();
    Vec3 rotation = player_->getRotation(); // pitch, yaw

    // Convert rotation to direction vector
    float pitch = rotation.x;
    float yaw = rotation.y;

    glm::vec3 direction;
    direction.x = -std::cos(glm::radians(pitch)) * std::sin(glm::radians(yaw));
    direction.y = -std::sin(glm::radians(pitch));
    direction.z = std::cos(glm::radians(pitch)) * std::cos(glm::radians(yaw));

    Ray ray;
    ray.origin = glm::vec3(position.x, position.y + player_->getEyeHeight(), position.z);
    ray.direction = glm::normalize(direction);

    return ray;
}

BlockPos BlockInteraction::findPlacePosition() const {
    return placeBlockPos_;
}

BlockInteractionResult BlockInteraction::breakBlock() {
    BlockInteractionResult result;
    result.success = false;

    if (!hasTarget_ || !level_) return result;

    targetBlockPos_ = BlockPos(
        static_cast<int>(std::floor(raycastResult_.point.x)),
        static_cast<int>(std::floor(raycastResult_.point.y)),
        static_cast<int>(std::floor(raycastResult_.point.z))
    );

    // Get current block state
    result.oldState = &level_->getBlockState(targetBlockPos_);
    result.blockPos = targetBlockPos_;

    // Check if can break
    if (result.oldState->isAir(*result.oldState)) {
        return result;
    }

    // Get air block state
    auto& registry = Registries::BLOCK();
    auto* airBlock = registry.get("minecraft:air");
    if (!airBlock) return result;

    result.newState = &airBlock->defaultBlockState();

    // Set to air
    level_->setBlock(targetBlockPos_, *result.newState);
    result.success = true;

    return result;
}

BlockInteractionResult BlockInteraction::placeBlock(const BlockState& block) {
    BlockInteractionResult result;
    result.success = false;

    if (!hasTarget_ || !level_) return result;

    BlockPos placePos = findPlacePosition();

    // Get current block state
    result.oldState = &level_->getBlockState(placePos);
    result.blockPos = placePos;

    // Check if can place (target must be replaceable)
    if (!result.oldState->isAir(*result.oldState)) {
        return result;
    }

    // Check if player is in the way
    // TODO: Add player AABB check

    result.newState = &block;

    // Place block
    level_->setBlock(placePos, block);
    result.success = true;

    return result;
}

BlockInteractionResult BlockInteraction::interactBlock() {
    BlockInteractionResult result;
    result.success = false;

    if (!hasTarget_ || !level_) return result;

    targetBlockPos_ = BlockPos(
        static_cast<int>(std::floor(raycastResult_.point.x)),
        static_cast<int>(std::floor(raycastResult_.point.y)),
        static_cast<int>(std::floor(raycastResult_.point.z))
    );

    result.oldState = &level_->getBlockState(targetBlockPos_);
    result.blockPos = targetBlockPos_;

    // TODO: Implement block interaction (open doors, press buttons, etc.)

    return result;
}

} // namespace PrismaCraft
