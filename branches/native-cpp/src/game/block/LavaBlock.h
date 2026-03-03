#pragma once

#include "PrismaCraft/Core/Block.h"

namespace PrismaCraft {

/**
 * Lava block (liquid)
 * Corresponds to: net.minecraft.world.level.block.LavaBlock
 */
class LavaBlock : public Block {
public:
    LavaBlock() : name_("minecraft:lava"), id_(0) {}

    const std::string& getName() const override { return name_; }
    int getId() const override { return id_; }

    float getDestroySpeed(const BlockState& state) const override { return 100.0f; }
    float getExplosionResistance() const override { return 0.0f; }

    int getLightEmission(const BlockState& state) const override { return 15; } // 最亮

    bool isSolid(const BlockState& state) const override { return false; }
    bool isAir(const BlockState& state) const override { return false; }

    const BlockState& defaultBlockState() const override;

private:
    std::string name_;
    int id_;
};

} // namespace PrismaCraft
