#pragma once

#include "PrismaCraft/Core/Block.h"

namespace PrismaCraft {

/**
 * Bedrock block (indestructible)
 * Corresponds to: net.minecraft.world.level.block.Blocks.BEDROCK
 */
class BedrockBlock : public Block {
public:
    BedrockBlock() : name_("minecraft:bedrock"), id_(0) {}

    const std::string& getName() const override { return name_; }
    int getId() const override { return id_; }

    float getDestroySpeed(const BlockState& state) const override { return -1.0f; } // 不可破坏
    float getExplosionResistance() const override { return 3600000.0f; }

    bool isSolid(const BlockState& state) const override { return true; }
    bool isAir(const BlockState& state) const override { return false; }

    const BlockState& defaultBlockState() const override;

private:
    std::string name_;
    int id_;
};

} // namespace PrismaCraft
