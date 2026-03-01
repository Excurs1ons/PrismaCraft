#pragma once

#include "../../../../../../../../shared/include/PrismaCraft/Core/BlockPos.h"
#include "../../../../../../../../shared/include/PrismaCraft/Core/ChunkPos.h"
#include "../../../../../../../../shared/include/PrismaCraft/Core/BlockState.h"
#include "../../../../../../../../shared/include/PrismaCraft/Core/Block.h"
#include <memory>
#include <array>
#include <vector>

namespace PrismaCraft {
    namespace Game {

        // 前向声明
        class Level;
        class ChunkSection;
        class ChunkBiomeContainer;

        /**
         * @brief 区块 - 对应 Minecraft: net.minecraft.world.level.chunk.LevelChunk
         *
         * Chunk 是 16x256x16 的方块组，是 Minecraft 世界的基本存储单位。
         *
         * 结构：
         * - 每个 Chunk 被划分为 16 个 ChunkSection（每个 16x16x16）
         * - 包含方块数据、生物群系数据、高度图等
         */
        class LevelChunk {
        public:
            /**
             * @brief 区块状态
             */
            enum class State {
                EMPTY,       // 空（未生成）
                GENERATING,  // 生成中
                FULL         // 完整（已生成）
            };

            /**
             * @brief 高度图类型
             */
            enum class HeightmapType {
                OCEAN_FLOOR,      // 海底高度图（忽略液体）
                WORLD_SURFACE,    // 世界表面高度图（包含液体）
                MOTION_BLOCKING,  // 移动阻挡高度图
                LIGHT_BLOCKING    // 光线阻挡高度图
            };

            /**
             * @brief 创建空区块
             */
            static std::unique_ptr<LevelChunk> createEmpty(Level& level, const ChunkPos& pos);

            /**
             * @brief 从数据创建区块
             */
            static std::unique_ptr<LevelChunk> create(Level& level, const ChunkPos& pos);

            /**
             * @brief 创建新的客户端区块（用于数据包接收）
             */
            static std::unique_ptr<LevelChunk> createFromPacket(Level& level, const ChunkPos& pos);

            ~LevelChunk();

            // ========== 区块属性 ==========

            /**
             * @brief 获取区块位置
             */
            const ChunkPos& getPos() const noexcept { return m_pos; }

            /**
             * @brief 获取所属世界
             */
            Level& getLevel() const noexcept { return m_level; }

            /**
             * @brief 获取区块状态
             */
            State getState() const noexcept { return m_state; }

            /**
             * @brief 设置区块状态
             */
            void setState(State state) noexcept { m_state = state; }

            /**
             * @brief 检查区块是否已完全生成
             */
            bool isFull() const noexcept { return m_state == State::FULL; }

            // ========== 区块段管理 ==========

            /**
             * @brief 获取指定段的区块段
             * @param sectionY 段 Y 坐标 (0-15)
             * 对应 Minecraft: LevelChunk.getSection(int)
             */
            ChunkSection* getSection(int32_t sectionY) noexcept;
            const ChunkSection* getSection(int32_t sectionY) const noexcept;

            /**
             * @brief 设置区块段
             */
            void setSection(int32_t sectionY, std::unique_ptr<ChunkSection> section);

            /**
             * @brief 获取非空区块段的数量
             */
            int32_t getSectionCount() const noexcept;

            /**
             * @brief 获取最高的非空区块段
             */
            int32_t getHighestSectionIndex() const noexcept;

            // ========== 方块操作 ==========

            /**
             * @brief 获取指定位置的方块状态
             * 对应 Minecraft: LevelChunk.getBlockState(BlockPos)
             */
            const BlockState& getBlockState(const BlockPos& pos) const;

            /**
             * @brief 设置指定位置的方块
             * 对应 Minecraft: LevelChunk.setBlockState(BlockPos, BlockState)
             *
             * @return 之前的方块状态，如果位置无效则返回 nullptr
             */
            const BlockState* setBlockState(const BlockPos& pos, const BlockState& state);

            /**
             * @brief 检查区块是否包含指定位置
             */
            bool isWithinBounds(const BlockPos& pos) const noexcept {
                return (pos.x >> 4) == m_pos.x && (pos.z >> 4) == m_pos.z &&
                       pos.y >= m_level.getMinBuildHeight() &&
                       pos.y < m_level.getMaxBuildHeight();
            }

            /**
             * @brief 获取区块内的一维方块索引
             * @param x, y, z 方块坐标（0-15, 0-255, 0-15）
             */
            static constexpr int32_t getBlockIndex(int32_t x, int32_t y, int32_t z) noexcept {
                return (y & 15) << 8 | (z & 15) << 4 | (x & 15);
            }

            // ========== 生物群系 ==========

            /**
             * @brief 获取指定位置的生物群系
             * 对应 Minecraft: LevelChunk.getBiome(int, int, int)
             */
            class Biome* getBiome(int32_t x, int32_t y, int32_t z);

            /**
             * @brief 获取生物群系容器
             */
            ChunkBiomeContainer& getBiomes() noexcept { return *m_biomes; }
            const ChunkBiomeContainer& getBiomes() const noexcept { return *m_biomes; }

            // ========== 高度图 ==========

            /**
             * @brief 获取指定类型的高度图
             */
            const std::array<int16_t, 256>& getHeightmap(HeightmapType type) const noexcept;

            /**
             * @brief 设置高度图数据
             */
            void setHeightmap(HeightmapType type, const std::array<int16_t, 256>& data);

            /**
             * @brief 更新高度图
             * 当方块改变时调用
             */
            void updateHeightmap(HeightmapType type, int32_t x, int32_t z, int16_t height);

            /**
             * @brief 获取指定位置的高度
             * @param x, z 区块内坐标 (0-15)
             */
            int16_t getHeight(HeightmapType type, int32_t x, int32_t z) const noexcept;

            // ========== 区块标记 ==========

            /**
             * @brief 检查区块是否为空（不包含任何非空气方块）
             */
            bool isEmpty() const noexcept;

            /**
             * @brief 标记区块为脏（需要保存）
             */
            void setDirty(bool dirty = true) noexcept { m_isDirty = dirty; }

            /**
             * @brief 检查区块是否需要保存
             */
            bool isDirty() const noexcept { return m_isDirty; }

            /**
             * @brief 标记区块需要重新生成网格
             */
            void setMeshDirty(bool dirty = true) noexcept { m_isMeshDirty = dirty; }

            /**
             * @brief 检查区块网格是否需要更新
             */
            bool isMeshDirty() const noexcept { return m_isMeshDirty; }

            /**
             * @brief 标记区块为已加载
             */
            void setLoaded(bool loaded = true) noexcept { m_isLoaded = loaded; }

            /**
             * @brief 检查区块是否已加载
             */
            bool isLoaded() const noexcept { return m_isLoaded; }

            // ========== 区块邻居 ==========

            /**
             * @brief 检查边缘方块（用于光照更新）
             * @param direction 方向
             * @return 需要更新的边缘方块列表
             */
            std::vector<BlockPos> getEdgeBlocks(const Direction& direction) const;

            /**
             * @brief 通知邻居方块已更新
             */
            void onNeighborChanged(const BlockPos& pos, const Block& block);

            // ========== 刻度管理 ==========

            /**
             * @brief 调度方块刻度
             */
            void scheduleBlockTick(const BlockPos& pos, const Block& block, int32_t delay);

            /**
             * @brief 处理所有刻度
             */
            void processTicks();

            // ========== 随机刻度 ==========

            /**
             * @brief 获取随机刻度的方块位置
             * 对应 Minecraft: LevelChunk.getPosForRandom(int)
             */
            BlockPos getPosForRandom(int32_t index) const noexcept;

            /**
             * @brief 处理随机刻度
             */
            void processRandomTicks();

            // ========== 区块实体 ==========

            /**
             * @brief 区块实体 - 对应 Minecraft: BlockEntity
             *
             * 如箱子、熔炉、告示牌等有额外数据的方块
             */
            class BlockEntity {
            public:
                const BlockPos pos;
                const Block* block;

                BlockEntity(const BlockPos& pos, const Block& block)
                    : pos(pos), block(&block) {}

                virtual ~BlockEntity() = default;
            };

            /**
             * @brief 添加区块实体
             */
            void addBlockEntity(std::unique_ptr<BlockEntity> blockEntity);

            /**
             * @brief 移除区块实体
             */
            void removeBlockEntity(const BlockPos& pos);

            /**
             * @brief 获取区块实体
             */
            BlockEntity* getBlockEntity(const BlockPos& pos) noexcept;

            // ========== 保存和加载 ==========

            /**
             * @brief 序列化区块数据
             * 用于保存到磁盘
             */
            std::vector<uint8_t> serialize() const;

            /**
             * @brief 反序列化区块数据
             */
            static std::unique_ptr<LevelChunk> deserialize(
                Level& level,
                const ChunkPos& pos,
                const std::vector<uint8_t>& data
            );

        private:
            LevelChunk(Level& level, const ChunkPos& pos);
            LevelChunk(Level& level, const ChunkPos& pos, State state);

            Level& m_level;
            ChunkPos m_pos;
            State m_state;

            // 区块段数组（16 个段，每个 16x16x16）
            std::array<std::unique_ptr<ChunkSection>, 16> m_sections;

            // 生物群系数据
            std::unique_ptr<ChunkBiomeContainer> m_biomes;

            // 高度图（4 种类型，每种 16x16 = 256 个值）
            std::array<std::array<int16_t, 256>, 4> m_heightmaps;

            // 区块实体
            std::unordered_map<BlockPos, std::unique_ptr<BlockEntity>, BlockPos::Hash> m_blockEntities;

            // 标记
            bool m_isDirty : 1 = false;      // 需要保存
            bool m_isMeshDirty : 1 = false; // 需要更新网格
            bool m_isLoaded : 1 = false;    // 已加载

            // 刻度
            struct ScheduledTick {
                BlockPos pos;
                const Block* block;
                int32_t scheduledTime;
            };
            std::vector<ScheduledTick> m_scheduledTicks;
        };

        /**
         * @brief 区块段 - 对应 Minecraft: net.minecraft.world.level.chunk.ChunkSection
         *
         * ChunkSection 是 16x16x16 的方块组（4096 个方块）。
         */
        class ChunkSection {
        public:
            /**
             * @brief 创建空区块段
             */
            static std::unique_ptr<ChunkSection> createEmpty();

            /**
             * @brief 从数据创建区块段
             */
            static std::unique_ptr<ChunkSection> create(
                int32_t sectionY,
                const std::vector<const BlockState*>& blockStates
            );

            ~ChunkSection();

            // ========== 区块段属性 ==========

            /**
             * @brief 获取区块段的 Y 坐标
             */
            int32_t getY() const noexcept { return m_sectionY; }

            /**
             * @brief 检查区块段是否为空
             */
            bool isEmpty() const noexcept;

            /**
             * @brief 获取非空气方块的数量
             */
            int32_t getNonEmptyBlockCount() const noexcept { return m_nonEmptyBlockCount; }

            // ========== 方块操作 ==========

            /**
             * @brief 获取方块状态
             * @param index 一维索引 (0-4095)
             */
            const BlockState& getBlockState(int32_t index) const;

            /**
             * @brief 设置方块状态
             * @return 之前的方块状态
             */
            const BlockState* setBlockState(int32_t index, const BlockState& state);

            /**
             * @brief 检查是否只有空气
             */
            bool hasOnlyAir() const noexcept;

            // ========== 刻度优化 ==========

            /**
             * @brief 获取随机刻度的位置列表
             * 对应 Minecraft: ChunkSection.getRandomPos()
             */
            const std::vector<int32_t>& getRandomPositions() const noexcept {
                return m_randomPositions;
            }

            /**
             * @brief 重新计算随机刻位置
             * 当方块状态改变时调用
             */
            void recalculateRandomPositions();

        private:
            ChunkSection(int32_t sectionY);

            int32_t m_sectionY;
            int32_t m_nonEmptyBlockCount = 0;

            // 方块状态存储（使用 Palette 压缩）
            std::unique_ptr<class PalettedContainer> m_blockStates;

            // 随机刻度优化：只对可能随机刻的方块进行采样
            std::vector<int32_t> m_randomPositions;
        };

        /**
         * @brief 调色板容器 - 对应 Minecraft: net.minecraft.world.level.chunk.PalettedContainer
         *
         * 用于压缩存储方块数据。
         * 如果区块中只有少量方块类型，使用调色板可以大幅节省内存。
         */
        class PalettedContainer {
        public:
            /**
             * @brief 数据配置
             */
            struct Configuration {
                int32_t bits;           // 每个元素的位数
                int32_t maxStates;      // 最大状态数
                int32_t paletteSize;    // 调色板大小
            };

            PalettedContainer(const Configuration& config);
            ~PalettedContainer();

            /**
             * @brief 获取指定索引的方块状态
             */
            const BlockState& get(int32_t index) const;

            /**
             * @brief 设置指定索引的方块状态
             */
            void set(int32_t index, const BlockState& state);

            /**
             * @brief 获取所有方块状态（用于序列化）
             */
            const std::vector<const BlockState*>& getAll() const;

            /**
             * @brief 从数组批量设置
             */
            void setAll(const std::vector<const BlockState*>& states);

            /**
             * @brief 计算内存大小
             */
            size_t getMemorySize() const noexcept;

        private:
            Configuration m_config;

            // 数据存储（紧凑格式）
            std::unique_ptr<uint8_t[]> m_data;

            // 调色板（实际使用的方块状态）
            std::vector<const BlockState*> m_palette;

            // 单值优化：如果所有方块相同，只存储一个值
            const BlockState* m_singleValue = nullptr;
        };

    } // namespace Game
} // namespace PrismaCraft
