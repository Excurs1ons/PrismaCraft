#pragma once

#include "noise/PerlinNoise.h"
#include "noise/SimplexNoise.h"
#include "noise/OctaveNoise.h"
#include "feature/Feature.h"
#include <PrismaCraft/PrismaCraft.h>
#include <vector>
#include <memory>
#include <random>

namespace PrismaCraft {

class LevelChunk;

/**
 * World generator configuration
 */
struct WorldGeneratorConfig {
    int64_t seed;
    int seaLevel = 64;
    int baseHeight = 64;
    double heightScale = 20.0;
    double heightVariation = 10.0;
    bool generateCaves = true;
    bool generateRavines = false;
    bool generateStructures = true;
};

/**
 * World generator
 * Corresponds to: net.minecraft.world.level.levelgen.WorldGenSettings
 */
class PRISMACRAFT_API WorldGenerator {
public:
    explicit WorldGenerator(const WorldGeneratorConfig& config);
    explicit WorldGenerator(int64_t seed);

    // Generate chunk
    void generateChunk(LevelChunk* chunk);

    // Set seed
    void setSeed(int64_t seed);

    // Get seed
    int64_t getSeed() const { return config_.seed; }

    // Add feature
    void addFeature(std::unique_ptr<Feature> feature);

    // Get terrain height at position
    int getTerrainHeight(int x, int z) const;

private:
    WorldGeneratorConfig config_;

    // Noise generators
    std::unique_ptr<PerlinNoise> terrainNoise_;
    std::unique_ptr<SimplexNoise> caveNoise_;
    std::unique_ptr<OctaveNoise> detailNoise_;

    // Features
    std::vector<std::unique_ptr<Feature>> features_;

    // Random
    std::mt19937_64 random_;

    // Internal methods
    void generateTerrain(LevelChunk* chunk);
    void generateCaves(LevelChunk* chunk);
    void generateStructures(LevelChunk* chunk);
    void generateOres(LevelChunk* chunk);

    // Helper methods
    double getTerrainNoise(int x, int z) const;
    bool shouldGenerateCave(int x, int y, int z) const;
};

} // namespace PrismaCraft
