#pragma once

#include "PrismaCraft/Core/Block.h"

namespace PrismaCraft {

/**
 * Glass block
 * Corresponds to: net.minecraft.world.level.block.GlassBlock
 */
class GlassBlock : public Block {
public:
    GlassBlock() : name_("minecraft:glass"), id_(0) {}

    const std::string& getName() const override { return name_; }
    int getId() const override { return id_; }

    float getDestroySpeed(const BlockState& state) const override { return 0.3f; }
    float getExplosionResistance() const override { return 0.3f; }

    // 玻璃不是完全固体
    bool isSolid(const BlockState& state) const override { return false; }
    bool isAir(const BlockState& state) const override { return false; }

    const BlockState& defaultBlockState() const override;

private:
    std::string name_;
    int id_;
};

} // namespace PrismaCraft
