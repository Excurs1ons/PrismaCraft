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
#include "PrismaCraft/Core/BlockState.h"

namespace PrismaCraft {

// 为每个方块类定义静态 BlockState 实例

namespace {
    // 静态 BlockState 实例
    std::unique_ptr<BlockState> g_stoneDefaultState;
    std::unique_ptr<BlockState> g_grassDefaultState;
    std::unique_ptr<BlockState> g_dirtDefaultState;
    std::unique_ptr<BlockState> g_cobblestoneDefaultState;
    std::unique_ptr<BlockState> g_planksDefaultState;
    std::unique_ptr<BlockState> g_bedrockDefaultState;
    std::unique_ptr<BlockState> g_waterDefaultState;
    std::unique_ptr<BlockState> g_lavaDefaultState;
    std::unique_ptr<BlockState> g_sandDefaultState;
    std::unique_ptr<BlockState> g_gravelDefaultState;
    std::unique_ptr<BlockState> g_oreDefaultState;
    std::unique_ptr<BlockState> g_logDefaultState;
    std::unique_ptr<BlockState> g_leavesDefaultState;
    std::unique_ptr<BlockState> g_glassDefaultState;
    std::unique_ptr<BlockState> g_spongeDefaultState;
}

// StoneBlock
const BlockState& StoneBlock::defaultBlockState() const {
    if (!g_stoneDefaultState) {
        g_stoneDefaultState = std::make_unique<BlockState>(*this, 0);
    }
    return *g_stoneDefaultState;
}

// GrassBlock
const BlockState& GrassBlock::defaultBlockState() const {
    if (!g_grassDefaultState) {
        g_grassDefaultState = std::make_unique<BlockState>(*this, 0);
    }
    return *g_grassDefaultState;
}

// DirtBlock
const BlockState& DirtBlock::defaultBlockState() const {
    if (!g_dirtDefaultState) {
        g_dirtDefaultState = std::make_unique<BlockState>(*this, 0);
    }
    return *g_dirtDefaultState;
}

// CobblestoneBlock
const BlockState& CobblestoneBlock::defaultBlockState() const {
    if (!g_cobblestoneDefaultState) {
        g_cobblestoneDefaultState = std::make_unique<BlockState>(*this, 0);
    }
    return *g_cobblestoneDefaultState;
}

// PlanksBlock
const BlockState& PlanksBlock::defaultBlockState() const {
    // 使用 woodType 作为 stateId 的一部分
    size_t stateId = std::hash<std::string>{}(woodType_) % 1000;
    static std::map<std::string, std::unique_ptr<BlockState>> states;
    auto it = states.find(woodType_);
    if (it == states.end()) {
        it = states.emplace(woodType_, std::make_unique<BlockState>(*this, stateId)).first;
    }
    return *it->second;
}

// BedrockBlock
const BlockState& BedrockBlock::defaultBlockState() const {
    if (!g_bedrockDefaultState) {
        g_bedrockDefaultState = std::make_unique<BlockState>(*this, 0);
    }
    return *g_bedrockDefaultState;
}

// WaterBlock
const BlockState& WaterBlock::defaultBlockState() const {
    if (!g_waterDefaultState) {
        g_waterDefaultState = std::make_unique<BlockState>(*this, 0);
    }
    return *g_waterDefaultState;
}

// LavaBlock
const BlockState& LavaBlock::defaultBlockState() const {
    if (!g_lavaDefaultState) {
        g_lavaDefaultState = std::make_unique<BlockState>(*this, 0);
    }
    return *g_lavaDefaultState;
}

// SandBlock
const BlockState& SandBlock::defaultBlockState() const {
    if (!g_sandDefaultState) {
        g_sandDefaultState = std::make_unique<BlockState>(*this, 0);
    }
    return *g_sandDefaultState;
}

// GravelBlock
const BlockState& GravelBlock::defaultBlockState() const {
    if (!g_gravelDefaultState) {
        g_gravelDefaultState = std::make_unique<BlockState>(*this, 0);
    }
    return *g_gravelDefaultState;
}

// OreBlock
const BlockState& OreBlock::defaultBlockState() const {
    if (!g_oreDefaultState) {
        g_oreDefaultState = std::make_unique<BlockState>(*this, 0);
    }
    return *g_oreDefaultState;
}

// LogBlock
const BlockState& LogBlock::defaultBlockState() const {
    size_t stateId = std::hash<std::string>{}(woodType_) % 1000;
    static std::map<std::string, std::unique_ptr<BlockState>> states;
    auto it = states.find(woodType_);
    if (it == states.end()) {
        it = states.emplace(woodType_, std::make_unique<BlockState>(*this, stateId)).first;
    }
    return *it->second;
}

// LeavesBlock
const BlockState& LeavesBlock::defaultBlockState() const {
    size_t stateId = std::hash<std::string>{}(woodType_) % 1000 + 500;
    static std::map<std::string, std::unique_ptr<BlockState>> states;
    auto it = states.find(woodType_);
    if (it == states.end()) {
        it = states.emplace(woodType_, std::make_unique<BlockState>(*this, stateId)).first;
    }
    return *it->second;
}

// GlassBlock
const BlockState& GlassBlock::defaultBlockState() const {
    if (!g_glassDefaultState) {
        g_glassDefaultState = std::make_unique<BlockState>(*this, 0);
    }
    return *g_glassDefaultState;
}

// SpongeBlock
const BlockState& SpongeBlock::defaultBlockState() const {
    if (!g_spongeDefaultState) {
        g_spongeDefaultState = std::make_unique<BlockState>(*this, 0);
    }
    return *g_spongeDefaultState;
}

} // namespace PrismaCraft
