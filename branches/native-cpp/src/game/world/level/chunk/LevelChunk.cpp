#include "LevelChunk.h"
#include "../level/Level.h"
#include "../../../../shared/include/PrismaCraft/NBT/NBTTag.h"
#include <algorithm>

namespace PrismaCraft {

// ============================================================================
// PalettedContainer 实现
// ============================================================================

PalettedContainer::PalettedContainer()
    : defaultState(nullptr) {
    data.resize(SECTION_SIZE);
}

PalettedContainer::PalettedContainer(const class BlockState& defaultState)
    : defaultState(&defaultState) {
    data.resize(SECTION_SIZE);
    fill(defaultState);
}

const class BlockState& PalettedContainer::get(int x, int y, int z) const {
    int index = getIndex(x, y, z);
    int paletteIndex = data[index];

    if (paletteIndex < palette.size()) {
        return *palette[paletteIndex];
    }

    // 返回默认状态
    return defaultState ? *defaultState : BlockState::AIR();
}

void PalettedContainer::set(int x, int y, int z, const class BlockState& state) {
    int index = getIndex(x, y, z);

    // 检查调色板中是否已存在该状态
    auto it = std::find(palette.begin(), palette.end(), &state);
    int paletteIndex;

    if (it != palette.end()) {
        paletteIndex = std::distance(palette.begin(), it);
    } else {
        // 添加到调色板
        if (palette.size() >= (1 << BITS_PER_BLOCK)) {
            // 调色板已满，需要扩展（简化实现：直接覆盖第一个）
            paletteIndex = 0;
        } else {
            paletteIndex = palette.size();
            palette.push_back(&state);
        }
    }

    data[index] = paletteIndex;
}

void PalettedContainer::fill(const class BlockState& state) {
    // 清空调色板并添加单个状态
    palette.clear();
    palette.push_back(&state);

    // 填充数据
    std::fill(data.begin(), data.end(), 0);
}

void PalettedContainer::read(class NBT::CompoundTag* tag) {
    // 简化实现：从 NBT 读取
    // 实际需要解析 palette 和 data 标签
    if (!tag) return;

    // TODO: 实现 NBT 读取
}

std::unique_ptr<class NBT::CompoundTag> PalettedContainer::write() const {
    auto tag = std::make_unique<NBT::CompoundTag>();

    // TODO: 实现 NBT 写入
    // 需要写入 palette 和 data

    return tag;
}

// ============================================================================
// ChunkSection 实现
// ============================================================================

ChunkSection::ChunkSection(int yIndex)
    : yIndex(yIndex)
    , empty(true)
    , level(nullptr) {
    // 初始化为空气方块
    blocks.fill(BlockState::AIR());
}

ChunkSection::~ChunkSection() = default;

const class BlockState& ChunkSection::getBlockState(int x, int y, int z) const {
    // 坐标检查
    if (x < 0 || x >= SECTION_WIDTH ||
        y < 0 || y >= SECTION_HEIGHT ||
        z < 0 || z >= SECTION_DEPTH) {
        return BlockState::AIR();
    }

    return blocks.get(x, y, z);
}

void ChunkSection::setBlockState(int x, int y, int z, const class BlockState& state) {
    // 坐标检查
    if (x < 0 || x >= SECTION_WIDTH ||
        y < 0 || y >= SECTION_HEIGHT ||
        z < 0 || z >= SECTION_DEPTH) {
        return;
    }

    // 如果设置为非空气方块，标记为非空
    if (!state.isAir()) {
        empty = false;
    }

    blocks.set(x, y, z, state);
}

void ChunkSection::recalculate() {
    // 检查是否全部为空气
    empty = true;

    for (int y = 0; y < SECTION_HEIGHT; ++y) {
        for (int z = 0; z < SECTION_DEPTH; ++z) {
            for (int x = 0; x < SECTION_WIDTH; ++x) {
                if (!getBlockState(x, y, z).isAir()) {
                    empty = false;
                    return;
                }
            }
        }
    }
}

void ChunkSection::read(class NBT::CompoundTag* tag) {
    // TODO: 实现 NBT 读取
}

std::unique_ptr<class NBT::CompoundTag> ChunkSection::write() const {
    auto tag = std::make_unique<NBT::CompoundTag>();

    // TODO: 实现 NBT 写入

    return tag;
}

// ============================================================================
// LevelChunk 实现
// ============================================================================

LevelChunk::LevelChunk(Level* level, const ChunkPos& pos)
    : level(level)
    , pos(pos)
    , loaded(false) {
    init();
}

LevelChunk::~LevelChunk() = default;

void LevelChunk::init() {
    // 初始化所有区块切片
    for (int i = 0; i < SECTION_COUNT; ++i) {
        sections[i] = std::make_unique<ChunkSection>(i);
    }

    // 初始化高度映射
    heightMap.fill(0);
}

const class BlockState& LevelChunk::getBlockState(const BlockPos& pos) const {
    return getBlockState(pos.getX(), pos.getY(), pos.getZ());
}

const class BlockState& LevelChunk::getBlockState(int x, int y, int z) const {
    // 坐标转换为本地坐标
    int localX = x & 0xF;  // x % 16
    int localZ = z & 0xF;  // z % 16

    // 获取对应的切片
    int sectionIndex = getSectionIndex(y);
    if (sectionIndex < 0 || sectionIndex >= SECTION_COUNT) {
        return BlockState::AIR();
    }

    const ChunkSection* section = sections[sectionIndex].get();
    if (!section) {
        return BlockState::AIR();
    }

    // 获取切片内的 Y 坐标
    int localY = y & 0xF;  // y % 16

    return section->getBlockState(localX, localY, localZ);
}

void LevelChunk::setBlockState(const BlockPos& pos, const class BlockState& state) {
    int x = pos.getX();
    int y = pos.getY();
    int z = pos.getZ();

    // 坐标转换为本地坐标
    int localX = x & 0xF;
    int localZ = z & 0xF;

    // 获取对应的切片
    int sectionIndex = getSectionIndex(y);
    if (sectionIndex < 0 || sectionIndex >= SECTION_COUNT) {
        return;
    }

    ChunkSection* section = sections[sectionIndex].get();
    if (!section) {
        return;
    }

    // 设置方块状态
    int localY = y & 0xF;
    section->setBlockState(localX, localY, localZ, state);

    // 更新高度映射（如果需要）
    int heightIndex = localX + localZ * WIDTH;
    if (y > heightMap[heightIndex]) {
        heightMap[heightIndex] = y;
    }

    // 标记为未保存
    markUnsaved();
}

ChunkSection* LevelChunk::getSection(int yIndex) {
    if (yIndex < 0 || yIndex >= SECTION_COUNT) {
        return nullptr;
    }
    return sections[yIndex].get();
}

const ChunkSection* LevelChunk::getSection(int yIndex) const {
    if (yIndex < 0 || yIndex >= SECTION_COUNT) {
        return nullptr;
    }
    return sections[yIndex].get();
}

int LevelChunk::getHeight(int x, int z) const {
    // 转换为本地坐标
    int localX = x & 0xF;
    int localZ = z & 0xF;

    int index = localX + localZ * WIDTH;
    return heightMap[index];
}

void LevelChunk::setHeight(int x, int z, int height) {
    // 转换为本地坐标
    int localX = x & 0xF;
    int localZ = z & 0xF;

    int index = localX + localZ * WIDTH;
    heightMap[index] = height;
}

void LevelChunk::markUnsaved() {
    // TODO: 实现保存标记
}

void LevelChunk::markSaved() {
    // TODO: 实现保存标记
}

void LevelChunk::read(class NBT::CompoundTag* tag) {
    // TODO: 实现 NBT 读取
    // 需要读取：
    // - LevelChunk Sections
    // - HeightMap
    // - Entities
    // - BlockEntities
}

std::unique_ptr<class NBT::CompoundTag> LevelChunk::write() const {
    auto tag = std::make_unique<NBT::CompoundTag>();

    // TODO: 实现 NBT 写入
    // 需要写入：
    // - LevelChunk Sections
    // - HeightMap
    // - Entities
    // - BlockEntities

    return tag;
}

} // namespace PrismaCraft
