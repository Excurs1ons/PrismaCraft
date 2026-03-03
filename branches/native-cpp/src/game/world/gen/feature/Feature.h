#pragma once

#include <PrismaCraft/PrismaCraft.h>
#include "../../world/level/chunk/LevelChunk.h"
#include <memory>

namespace PrismaCraft {

/**
 * World feature base class
 * Corresponds to: net.minecraft.world.level.levelgen.feature.Feature
 */
class PRISMACRAFT_API Feature {
public:
    virtual ~Feature() = default;

    // Generate feature in chunk
    virtual bool generate(LevelChunk* chunk, int x, int y, int z) = 0;

    // Get feature name
    virtual const char* getName() const = 0;

protected:
    Feature() = default;
};

/**
 * Ore feature
 * Corresponds to: net.minecraft.world.level.levelgen.feature.OreFeature
 */
class PRISMACRAFT_API OreFeature : public Feature {
public:
    struct OreConfig {
        const Block* oreBlock;
        const Block* stoneBlock;
        int veinSize;
        int minLevel;
        int maxLevel;
        int countPerChunk;
    };

    explicit OreFeature(const OreConfig& config);

    bool generate(LevelChunk* chunk, int x, int y, int z) override;
    const char* getName() const override { return "Ore"; }

private:
    OreConfig config_;
};

/**
 * Tree feature
 * Corresponds to: net.minecraft.world.level.levelgen.feature.TreeFeature
 */
class PRISMACRAFT_API TreeFeature : public Feature {
public:
    struct TreeConfig {
        const Block* logBlock;
        const Block* leavesBlock;
        int trunkHeight;
        int trunkHeightRandom;
        bool canopy;
    };

    explicit TreeFeature(const TreeConfig& config);

    bool generate(LevelChunk* chunk, int x, int y, int z) override;
    const char* getName() const override { return "Tree"; }

private:
    TreeConfig config_;
    std::mt19937 random_;

    void generateCanopy(LevelChunk* chunk, int x, int y, int z);
};

/**
 * Village feature
 * Corresponds to: net.minecraft.world.level.levelgen.feature.VillageFeature
 */
class PRISMACRAFT_API VillageFeature : public Feature {
public:
    bool generate(LevelChunk* chunk, int x, int y, int z) override;
    const char* getName() const override { return "Village"; }

private:
    // TODO: Village generation logic
};

/**
 * Lake feature (lava/water lakes)
 * Corresponds to: net.minecraft.world.level.levelgen.feature.LakeFeature
 */
class PRISMACRAFT_API LakeFeature : public Feature {
public:
    struct LakeConfig {
        const Block* fluidBlock;
        const Block* barrierBlock;
        int radius;
    };

    explicit LakeFeature(const LakeConfig& config);

    bool generate(LevelChunk* chunk, int x, int y, int z) override;
    const char* getName() const override { return "Lake"; }

private:
    LakeConfig config_;
};

} // namespace PrismaCraft
