#pragma once

#include "PrismaCraft/Core/Block.h"
#include <string>

namespace PrismaCraft {

/**
 * Leaves block
 * Corresponds to: net.minecraft.world.level.block.LeavesBlock
 */
class LeavesBlock : public Block {
public:
    explicit LeavesBlock(const std::string& woodType)
        : name_("minecraft:" + woodType + "_leaves"), id_(0), woodType_(woodType) {}

    const std::string& getName() const override { return name_; }
    int getId() const override { return id_; }

    float getDestroySpeed(const BlockState& state) const override { return 0.2f; }
    float getExplosionResistance() const override { return 0.2f; }

    // 树叶不是完全固体（可以穿过）
    bool isSolid(const BlockState& state) const override { return false; }
    bool isAir(const BlockState& state) const override { return false; }

    const std::string& getWoodType() const { return woodType_; }
    const BlockState& defaultBlockState() const override;

private:
    std::string name_;
    int id_;
    std::string woodType_;
};

} // namespace PrismaCraft
