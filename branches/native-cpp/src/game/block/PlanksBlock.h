#pragma once

#include "PrismaCraft/Core/Block.h"
#include <string>

namespace PrismaCraft {

/**
 * Wooden planks block
 * Corresponds to: net.minecraft.world.level.block.PlanksBlock
 */
class PlanksBlock : public Block {
public:
    explicit PlanksBlock(const std::string& woodType)
        : name_("minecraft:" + woodType + "_planks"), id_(0), woodType_(woodType) {}

    const std::string& getName() const override { return name_; }
    int getId() const override { return id_; }

    float getDestroySpeed(const BlockState& state) const override { return 2.0f; }
    float getExplosionResistance() const override { return 3.0f; }

    bool isSolid(const BlockState& state) const override { return true; }
    bool isAir(const BlockState& state) const override { return false; }

    const std::string& getWoodType() const { return woodType_; }
    const BlockState& defaultBlockState() const override;

private:
    std::string name_;
    int id_;
    std::string woodType_;
};

} // namespace PrismaCraft
