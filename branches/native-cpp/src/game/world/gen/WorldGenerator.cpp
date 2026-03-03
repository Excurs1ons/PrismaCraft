#include "WorldGenerator.h"
#include "../world/level/chunk/LevelChunk.h"
#include "../../Core/Registry.h"
#include "../../Core/Block.h"
#include <cmath>
#include <algorithm>

namespace PrismaCraft {

WorldGenerator::WorldGenerator(const WorldGeneratorConfig& config)
    : config_(config) {
    random_.seed(config_.seed);

    // Initialize noise generators
    terrainNoise_ = std::make_unique<PerlinNoise>(config_.seed);
    caveNoise_ = std::make_unique<SimplexNoise>(config_.seed + 1);
    detailNoise_ = std::make_unique<OctaveNoise>(OctaveNoise::NoiseType::PERLIN, config_.seed + 2);
}

WorldGenerator::WorldGenerator(int64_t seed) {
    WorldGeneratorConfig config;
    config.seed = seed;
    *this = WorldGenerator(config);
}

void WorldGenerator::setSeed(int64_t seed) {
    config_.seed = seed;
    random_.seed(seed);

    if (terrainNoise_) terrainNoise_->setSeed(seed);
    if (caveNoise_) caveNoise_->setSeed(seed + 1);
    if (detailNoise_) detailNoise_->setSeed(seed + 2);
}

void WorldGenerator::addFeature(std::unique_ptr<Feature> feature) {
    features_.push_back(std::move(feature));
}

void WorldGenerator::generateChunk(LevelChunk* chunk) {
    if (!chunk) return;

    // 生成地形
    generateTerrain(chunk);

    // 生成洞穴（如果启用）
    if (config_.generateCaves) {
        generateCaves(chunk);
    }

    // 生成结构（如果启用）
    if (config_.generateStructures) {
        generateStructures(chunk);
    }

    // 生成矿石
    generateOres(chunk);
}

void WorldGenerator::generateTerrain(LevelChunk* chunk) {
    auto& registry = Registries::BLOCK();
    auto* air = registry.get("minecraft:air");
    auto* stone = registry.get("minecraft:stone");
    auto* grass = registry.get("minecraft:grass_block");
    auto* dirt = registry.get("minecraft:dirt");
    auto* water = registry.get("minecraft:water");
    auto* bedrock = registry.get("minecraft:bedrock");
    auto* sand = registry.get("minecraft:sand");

    if (!air || !stone || !grass || !dirt || !water || !bedrock || !sand) return;

    const BlockState* airState = &air->defaultBlockState();
    const BlockState* stoneState = &stone->defaultBlockState();
    const BlockState* grassState = &grass->defaultBlockState();
    const BlockState* dirtState = &dirt->defaultBlockState();
    const BlockState* waterState = &water->defaultBlockState();
    const BlockState* bedrockState = &bedrock->defaultBlockState();
    const BlockState* sandState = &sand->defaultBlockState();

    int cx = chunk->getX();
    int cz = chunk->getZ();

    for (int x = 0; x < 16; ++x) {
        for (int z = 0; z < 16; ++z) {
            int worldX = cx * 16 + x;
            int worldZ = cz * 16 + z;

            // 计算地形高度
            double noise = getTerrainNoise(worldX, worldZ);
            int height = config_.baseHeight + static_cast<int>(noise * config_.heightScale);

            // 限制高度范围
            height = std::max(10, std::min(120, height));

            // 更新高度图
            chunk->setHeight(x, z, height);

            // 填充方块
            for (int y = 0; y < LevelChunk::HEIGHT; ++y) {
                BlockPos pos(worldX, y, worldZ);

                if (y == 0) {
                    // 基岩层
                    chunk->setBlockState(pos, *bedrockState);
                } else if (y < height - 4) {
                    // 石头层
                    chunk->setBlockState(pos, *stoneState);
                } else if (y < height - 1) {
                    // 泥土层
                    chunk->setBlockState(pos, *dirtState);
                } else if (y < height) {
                    // 表面层
                    if (height <= config_.seaLevel + 1) {
                        // 沙滩
                        chunk->setBlockState(pos, *sandState);
                    } else {
                        // 草地
                        chunk->setBlockState(pos, *grassState);
                    }
                } else if (y <= config_.seaLevel) {
                    // 水
                    chunk->setBlockState(pos, *waterState);
                } else {
                    // 空气
                    chunk->setBlockState(pos, *airState);
                }
            }
        }
    }
}

void WorldGenerator::generateCaves(LevelChunk* chunk) {
    auto& registry = Registries::BLOCK();
    auto* air = registry.get("minecraft:air");
    if (!air) return;

    const BlockState* airState = &air->defaultBlockState();

    int cx = chunk->getX();
    int cz = chunk->getZ();

    // 简化的洞穴生成
    for (int x = 0; x < 16; ++x) {
        for (int z = 0; z < 16; ++z) {
            int worldX = cx * 16 + x;
            int worldZ = cz * 16 + z;

            for (int y = 10; y < 50; ++y) {
                if (shouldGenerateCave(worldX, y, worldZ)) {
                    BlockPos pos(worldX, y, worldZ);
                    chunk->setBlockState(pos, *airState);
                }
            }
        }
    }
}

void WorldGenerator::generateStructures(LevelChunk* chunk) {
    // 生成特征（树木、矿石等）
    for (const auto& feature : features_) {
        // 随机选择位置尝试生成
        std::uniform_int_distribution<int> dist(0, 15);

        for (int i = 0; i < 3; ++i) { // 每个特征尝试3次
            int x = dist(random_);
            int z = dist(random_);
            int y = chunk->getHeight(x, z) + 1;

            int worldX = chunk->getX() * 16 + x;
            int worldZ = chunk->getZ() * 16 + z;

            feature->generate(chunk, worldX, y, worldZ);
        }
    }
}

void WorldGenerator::generateOres(LevelChunk* chunk) {
    // 矿石生成已在结构生成中通过 OreFeature 处理
    // 这里可以添加额外的矿石生成逻辑
}

double WorldGenerator::getTerrainNoise(int x, int z) const {
    if (!terrainNoise_) return 0.0;

    // 使用多个八度的噪声组合
    double scale = 0.01;
    double noise = 0.0;

    // 基础地形（低频）
    noise += terrainNoise_->octaveNoise2D(x * scale * 0.5, z * scale * 0.5, 4, 0.5) * 1.0;

    // 细节（中频）
    noise += terrainNoise_->octaveNoise2D(x * scale, z * scale, 6, 0.5) * 0.5;

    // 微观细节（高频）
    noise += terrainNoise_->octaveNoise2D(x * scale * 2.0, z * scale * 2.0, 4, 0.5) * 0.25;

    return noise;
}

bool WorldGenerator::shouldGenerateCave(int x, int y, int z) const {
    if (!caveNoise_) return false;

    double noise = caveNoise_->noise3D(x * 0.05, y * 0.05, z * 0.05);
    double threshold = 0.4;

    return noise > threshold;
}

int WorldGenerator::getTerrainHeight(int x, int z) const {
    double noise = getTerrainNoise(x, z);
    int height = config_.baseHeight + static_cast<int>(noise * config_.heightScale);
    return std::max(10, std::min(120, height));
}

} // namespace PrismaCraft
