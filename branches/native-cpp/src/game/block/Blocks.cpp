#include "PrismaCraft/Core/Registry.h"
#include "PrismaCraft/Core/Block.h"
#include "StoneBlock.h"
#include "GrassBlock.h"
#include "DirtBlock.h"
#include "CobblestoneBlock.h"
#include "PlanksBlock.h"
#include "BedrockBlock.h"
#include "WaterBlock.h"
#include "LavaBlock.h"
#include "SandBlock.h"
#include "GravelBlock.h"
#include "OreBlock.h"
#include "LogBlock.h"
#include "LeavesBlock.h"
#include "GlassBlock.h"
#include "SpongeBlock.h"

namespace PrismaCraft {

// 定义静态成员
Block* Blocks::AIR = nullptr;
Block* Blocks::STONE = nullptr;
Block* Blocks::GRASS_BLOCK = nullptr;
Block* Blocks::DIRT = nullptr;
Block* Blocks::COBBLESTONE = nullptr;
Block* Blocks::PLANKS_OAK = nullptr;
Block* Blocks::PLANKS_SPRUCE = nullptr;
Block* Blocks::PLANKS_BIRCH = nullptr;
Block* Blocks::PLANKS_JUNGLE = nullptr;
Block* Blocks::PLANKS_ACACIA = nullptr;
Block* Blocks::PLANKS_DARK_OAK = nullptr;
Block* Blocks::BEDROCK = nullptr;
Block* Blocks::WATER = nullptr;
Block* Blocks::LAVA = nullptr;
Block* Blocks::SAND = nullptr;
Block* Blocks::GRAVEL = nullptr;
Block* Blocks::GOLD_ORE = nullptr;
Block* Blocks::IRON_ORE = nullptr;
Block* Blocks::COAL_ORE = nullptr;
Block* Blocks::LOG_OAK = nullptr;
Block* Blocks::LOG_SPRUCE = nullptr;
Block* Blocks::LOG_BIRCH = nullptr;
Block* Blocks::LOG_JUNGLE = nullptr;
Block* Blocks::LOG_ACACIA = nullptr;
Block* Blocks::LOG_DARK_OAK = nullptr;
Block* Blocks::LEAVES_OAK = nullptr;
Block* Blocks::LEAVES_SPRUCE = nullptr;
Block* Blocks::LEAVES_BIRCH = nullptr;
Block* Blocks::LEAVES_JUNGLE = nullptr;
Block* Blocks::LEAVES_ACACIA = nullptr;
Block* Blocks::LEAVES_DARK_OAK = nullptr;
Block* Blocks::SPONGE = nullptr;
Block* Blocks::GLASS = nullptr;
Block* Blocks::LAPIS_ORE = nullptr;
Block* Blocks::LAPIS_BLOCK = nullptr;
Block* Blocks::DISPENSER = nullptr;
Block* Blocks::SANDSTONE = nullptr;
Block* Blocks::NOTE_BLOCK = nullptr;
Block* Blocks::BED_RED = nullptr;
Block* Blocks::POWERED_RAIL = nullptr;
Block* Blocks::DETECTOR_RAIL = nullptr;
Block* Blocks::STICKY_PISTON = nullptr;
Block* Blocks::COBWEB = nullptr;
Block* Blocks::GRASS = nullptr;
Block* Blocks::FERN = nullptr;
Block* Blocks::DEAD_BUSH = nullptr;
Block* Blocks::SEAGRASS = nullptr;
Block* Blocks::SEA_PICKLE = nullptr;

void Blocks::init() {
    auto& registry = Registries::BLOCK();

    // 注册空气方块
    AIR = registry.registerObject("minecraft:air",
        std::make_unique<Block>());

    // 基础方块
    STONE = registry.registerObject("minecraft:stone",
        std::make_unique<StoneBlock>());
    GRASS_BLOCK = registry.registerObject("minecraft:grass_block",
        std::make_unique<GrassBlock>());
    DIRT = registry.registerObject("minecraft:dirt",
        std::make_unique<DirtBlock>());
    COBBLESTONE = registry.registerObject("minecraft:cobblestone",
        std::make_unique<CobblestoneBlock>());

    // 木板
    PLANKS_OAK = registry.registerObject("minecraft:oak_planks",
        std::make_unique<PlanksBlock>("oak"));
    PLANKS_SPRUCE = registry.registerObject("minecraft:spruce_planks",
        std::make_unique<PlanksBlock>("spruce"));
    PLANKS_BIRCH = registry.registerObject("minecraft:birch_planks",
        std::make_unique<PlanksBlock>("birch"));
    PLANKS_JUNGLE = registry.registerObject("minecraft:jungle_planks",
        std::make_unique<PlanksBlock>("jungle"));
    PLANKS_ACACIA = registry.registerObject("minecraft:acacia_planks",
        std::make_unique<PlanksBlock>("acacia"));
    PLANKS_DARK_OAK = registry.registerObject("minecraft:dark_oak_planks",
        std::make_unique<PlanksBlock>("dark_oak"));

    // 基岩
    BEDROCK = registry.registerObject("minecraft:bedrock",
        std::make_unique<BedrockBlock>());

    // 液体
    WATER = registry.registerObject("minecraft:water",
        std::make_unique<WaterBlock>());
    LAVA = registry.registerObject("minecraft:lava",
        std::make_unique<LavaBlock>());

    // 沙子和砾石
    SAND = registry.registerObject("minecraft:sand",
        std::make_unique<SandBlock>());
    GRAVEL = registry.registerObject("minecraft:gravel",
        std::make_unique<GravelBlock>());

    // 矿石
    GOLD_ORE = registry.registerObject("minecraft:gold_ore",
        std::make_unique<OreBlock>("gold", 3.0f));
    IRON_ORE = registry.registerObject("minecraft:iron_ore",
        std::make_unique<OreBlock>("iron", 3.0f));
    COAL_ORE = registry.registerObject("minecraft:coal_ore",
        std::make_unique<OreBlock>("coal", 3.0f));
    LAPIS_ORE = registry.registerObject("minecraft:lapis_ore",
        std::make_unique<OreBlock>("lapis", 3.0f));

    // 原木
    LOG_OAK = registry.registerObject("minecraft:oak_log",
        std::make_unique<LogBlock>("oak"));
    LOG_SPRUCE = registry.registerObject("minecraft:spruce_log",
        std::make_unique<LogBlock>("spruce"));
    LOG_BIRCH = registry.registerObject("minecraft:birch_log",
        std::make_unique<LogBlock>("birch"));
    LOG_JUNGLE = registry.registerObject("minecraft:jungle_log",
        std::make_unique<LogBlock>("jungle"));
    LOG_ACACIA = registry.registerObject("minecraft:acacia_log",
        std::make_unique<LogBlock>("acacia"));
    LOG_DARK_OAK = registry.registerObject("minecraft:dark_oak_log",
        std::make_unique<LogBlock>("dark_oak"));

    // 树叶
    LEAVES_OAK = registry.registerObject("minecraft:oak_leaves",
        std::make_unique<LeavesBlock>("oak"));
    LEAVES_SPRUCE = registry.registerObject("minecraft:spruce_leaves",
        std::make_unique<LeavesBlock>("spruce"));
    LEAVES_BIRCH = registry.registerObject("minecraft:birch_leaves",
        std::make_unique<LeavesBlock>("birch"));
    LEAVES_JUNGLE = registry.registerObject("minecraft:jungle_leaves",
        std::make_unique<LeavesBlock>("jungle"));
    LEAVES_ACACIA = registry.registerObject("minecraft:acacia_leaves",
        std::make_unique<LeavesBlock>("acacia"));
    LEAVES_DARK_OAK = registry.registerObject("minecraft:dark_oak_leaves",
        std::make_unique<LeavesBlock>("dark_oak"));

    // 其他方块
    SPONGE = registry.registerObject("minecraft:sponge",
        std::make_unique<SpongeBlock>());
    GLASS = registry.registerObject("minecraft:glass",
        std::make_unique<GlassBlock>());
    LAPIS_BLOCK = registry.registerObject("minecraft:lapis_block",
        std::make_unique<Block>());
    SANDSTONE = registry.registerObject("minecraft:sandstone",
        std::make_unique<Block>());
    NOTE_BLOCK = registry.registerObject("minecraft:note_block",
        std::make_unique<Block>());
    BED_RED = registry.registerObject("minecraft:red_bed",
        std::make_unique<Block>());
    POWERED_RAIL = registry.registerObject("minecraft:powered_rail",
        std::make_unique<Block>());
    DETECTOR_RAIL = registry.registerObject("minecraft:detector_rail",
        std::make_unique<Block>());
    STICKY_PISTON = registry.registerObject("minecraft:sticky_piston",
        std::make_unique<Block>());
    COBWEB = registry.registerObject("minecraft:cobweb",
        std::make_unique<Block>());
    GRASS = registry.registerObject("minecraft:grass",
        std::make_unique<Block>());
    FERN = registry.registerObject("minecraft:fern",
        std::make_unique<Block>());
    DEAD_BUSH = registry.registerObject("minecraft:dead_bush",
        std::make_unique<Block>());
    SEAGRASS = registry.registerObject("minecraft:seagrass",
        std::make_unique<Block>());
    SEA_PICKLE = registry.registerObject("minecraft:sea_pickle",
        std::make_unique<Block>());

    // TODO: 添加更多方块以匹配完整的 Minecraft 方块列表
}

} // namespace PrismaCraft
