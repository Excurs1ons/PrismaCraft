#pragma once

#include <PrismaCraft/PrismaCraft.h>
#include "world/level/Level.h"
#include "world/entity/player/Player.h"
#include "physics/Ray.h"
#include "physics/RaycastHit.h"
#include <glm/glm.hpp>

namespace PrismaCraft {

/**
 * Block interaction result
 */
struct BlockInteractionResult {
    bool success;
    BlockPos blockPos;
    const BlockState* oldState;
    const BlockState* newState;

    BlockInteractionResult()
        : success(false)
        , oldState(nullptr)
        , newState(nullptr) {}
};

/**
 * Block interaction handler
 * Handles raycasting and block interaction (break/place/interact)
 */
class PRISMACRAFT_API BlockInteraction {
public:
    BlockInteraction(Level* level, Player* player);
    ~BlockInteraction() = default;

    // Raycast to find block
    bool raycastToBlock(float maxDistance = 5.0f);
    bool raycastToBlock(const Ray& ray, float maxDistance = 5.0f);

    // Get raycast result
    const RaycastHit& getRaycastResult() const { return raycastResult_; }
    bool hasTarget() const { return hasTarget_; }

    // Block actions
    BlockInteractionResult breakBlock();
    BlockInteractionResult placeBlock(const BlockState& block);
    BlockInteractionResult interactBlock();

    // Get targeted block position
    BlockPos getTargetBlockPos() const { return targetBlockPos_; }
    BlockPos getPlaceBlockPos() const { return placeBlockPos_; } // Adjacent face

    // Configuration
    void setReachDistance(float distance) { reachDistance_ = distance; }
    float getReachDistance() const { return reachDistance_; }

private:
    // Calculate ray from player
    Ray getPlayerRay() const;

    // Find block to place on (adjacent to targeted block)
    BlockPos findPlacePosition() const;

    Level* level_;
    Player* player_;

    // Raycast result
    Ray ray_;
    RaycastHit raycastResult_;
    bool hasTarget_;

    // Target positions
    BlockPos targetBlockPos_;
    BlockPos placeBlockPos_;

    // Configuration
    float reachDistance_;
};

} // namespace PrismaCraft
