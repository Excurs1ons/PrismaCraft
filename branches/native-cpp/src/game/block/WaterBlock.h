#pragma once

#include "PrismaCraft/Core/Block.h"

namespace PrismaCraft {

/**
 * Water block (liquid)
 * Corresponds to: net.minecraft.world.level.block.WaterBlock
 */
class WaterBlock : public Block {
public:
    WaterBlock() : name_("minecraft:water"), id_(0) {}

    const std::string& getName() const override { return name_; }
    int getId() const override { return id_; }

    float getDestroySpeed(const BlockState& state) const override { return 100.0f; }
    float getExplosionResistance() const override { return 0.0f; }

    bool isSolid(const BlockState& state) const override { return false; }
    bool isAir(const BlockState& state) const override { return false; }

    const BlockState& defaultBlockState() const override;

private:
    std::string name_;
    int id_;
};

} // namespace PrismaCraft
