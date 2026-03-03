#pragma once

#include "PrismaCraft/Core/Block.h"
#include <string>

namespace PrismaCraft {

/**
 * Ore block
 * Corresponds to: net.minecraft.world.level.block.OreBlock
 */
class OreBlock : public Block {
public:
    OreBlock(const std::string& oreType, float hardness)
        : name_("minecraft:" + oreType + "_ore"), id_(0), hardness_(hardness) {}

    const std::string& getName() const override { return name_; }
    int getId() const override { return id_; }

    float getDestroySpeed(const BlockState& state) const override { return hardness_; }
    float getExplosionResistance() const override { return 3.0f; }

    bool isSolid(const BlockState& state) const override { return true; }
    bool isAir(const BlockState& state) const override { return false; }

    const BlockState& defaultBlockState() const override;

private:
    std::string name_;
    int id_;
    float hardness_;
};

} // namespace PrismaCraft
