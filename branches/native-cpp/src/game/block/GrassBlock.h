#pragma once

#include "PrismaCraft/Core/Block.h"

namespace PrismaCraft {

/**
 * Grass block
 * Corresponds to: net.minecraft.world.level.block.GrassBlock
 */
class GrassBlock : public Block {
public:
    GrassBlock() : name_("minecraft:grass_block"), id_(0) {}

    const std::string& getName() const override { return name_; }
    int getId() const override { return id_; }

    float getDestroySpeed(const BlockState& state) const override { return 0.6f; }
    float getExplosionResistance() const override { return 0.6f; }

    bool isSolid(const BlockState& state) const override { return true; }
    bool isAir(const BlockState& state) const override { return false; }

    const BlockState& defaultBlockState() const override;

private:
    std::string name_;
    int id_;
};

} // namespace PrismaCraft
