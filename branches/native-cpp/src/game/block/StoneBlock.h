#pragma once

#include "PrismaCraft/Core/Block.h"

namespace PrismaCraft {

/**
 * Stone block
 * Corresponds to: net.minecraft.world.level.block.StoneBlock
 */
class StoneBlock : public Block {
public:
    StoneBlock() : name_("minecraft:stone"), id_(0) {}

    const std::string& getName() const override { return name_; }
    int getId() const override { return id_; }

    float getDestroySpeed(const BlockState& state) const override { return 1.5f; }
    float getExplosionResistance() const override { return 6.0f; }

    bool isSolid(const BlockState& state) const override { return true; }
    bool isAir(const BlockState& state) const override { return false; }

    const BlockState& defaultBlockState() const override;

private:
    std::string name_;
    int id_;
};

} // namespace PrismaCraft
