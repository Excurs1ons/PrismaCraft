#pragma once

#include <cstdint>
#include <functional>

namespace PrismaCraft {
    namespace Core {

        /**
         * @brief 区块坐标 - 对应 Minecraft: net.minecraft.core.ChunkPos
         *
         * Minecraft 世界被划分为 16x256x16 的区块。
         * ChunkPos 表示区块在 XZ 平面上的位置。
         */
        class ChunkPos {
        public:
            int32_t x;
            int32_t z;

            // ========== 构造函数 ==========

            constexpr ChunkPos() noexcept : x(0), z(0) {}

            constexpr ChunkPos(int32_t x, int32_t z) noexcept : x(x), z(z) {}

            /**
             * @brief 从 BlockPos 计算对应的 ChunkPos
             * 对应 Minecraft: ChunkPos(BlockPos)
             */
            explicit constexpr ChunkPos(const class BlockPos& pos) noexcept;

            /**
             * @brief 从区块键值创建
             * 对应 Minecraft: ChunkPos(long)
             */
            explicit constexpr ChunkPos(int64_t key) noexcept
                : x(static_cast<int32_t>(key)), z(static_cast<int32_t>(key >> 32)) {}

            // ========== 静态工厂方法 ==========

            /**
             * @brief 从方块坐标计算区块坐标
             * @param blockX 方块 X 坐标
             * @param blockZ 方块 Z 坐标
             * 对应 Minecraft: ChunkPos.getBlockCoord(int)
             */
            static constexpr int32_t getBlockCoord(int32_t blockCoord) noexcept {
                // 右移 4 位 = 除以 16，处理负数时需要特殊处理
                return blockCoord >= 0 ? blockCoord >> 4 : ~(~blockCoord >> 4);
            }

            static constexpr ChunkPos fromBlockPos(int32_t blockX, int32_t blockZ) noexcept {
                return ChunkPos(getBlockCoord(blockX), getBlockCoord(blockZ));
            }

            // ========== 区块键值 ==========

            /**
             * @brief 计算区块的唯一键值
             * 对应 Minecraft: ChunkPos.toLong()
             *
             * 将 x 和 z 压缩到一个 64 位整数中：
             * - 低 32 位：x
             * - 高 32 位：z
             */
            constexpr int64_t toLong() const noexcept {
                return static_cast<int64_t>(x) & 0xFFFFFFFFLL |
                       (static_cast<int64_t>(z) & 0xFFFFFFFFLL) << 32;
            }

            /**
             * @brief 将 x 和 z 组合成 long 键值
             * 对应 Minecraft: ChunkPos.asLong(int, int)
             */
            static constexpr int64_t asLong(int32_t x, int32_t z) noexcept {
                return static_cast<int64_t>(x) & 0xFFFFFFFFLL |
                       (static_cast<int64_t>(z) & 0xFFFFFFFFLL) << 32;
            }

            // ========== 坐标转换 ==========

            /**
             * @brief 获取区块中心的世界 X 坐标
             */
            constexpr int32_t getCenterX() const noexcept {
                return (x << 4) + 8;
            }

            /**
             * @brief 获取区块中心的世界 Z 坐标
             */
            constexpr int32_t getCenterZ() const noexcept {
                return (z << 4) + 8;
            }

            /**
             * @brief 获取区块的最小 X 坐标
             */
            constexpr int32_t getMinBlockX() const noexcept {
                return x << 4;
            }

            /**
             * @brief 获取区块的最大 X 坐标（包含）
             */
            constexpr int32_t getMaxBlockX() const noexcept {
                return (x << 4) + 15;
            }

            /**
             * @brief 获取区块的最小 Z 坐标
             */
            constexpr int32_t getMinBlockZ() const noexcept {
                return z << 4;
            }

            /**
             * @brief 获取区块的最大 Z 坐标（包含）
             */
            constexpr int32_t getMaxBlockZ() const noexcept {
                return (z << 4) + 15;
            }

            // ========== 区块距离计算 ==========

            /**
             * @brief 计算到另一个区块的距离（平方）
             * 用于区块加载优先级判断
             */
            constexpr double distSqr(const ChunkPos& other) const noexcept {
                double dx = x - other.x;
                double dz = z - other.z;
                return dx * dx + dz * dz;
            }

            /**
             * @brief 曼哈顿距离
             */
            constexpr int32_t distManhattan(const ChunkPos& other) const noexcept {
                return std::abs(x - other.x) + std::abs(z - other.z);
            }

            /**
             * @brief 切比雪夫距离（方块距离）
             */
            constexpr int32_t distChessboard(const ChunkPos& other) const noexcept {
                return std::max(std::abs(x - other.x), std::abs(z - other.z));
            }

            // ========== 相邻区块 ==========

            /**
             * @brief 获取 8 个相邻区块的位置
             * 返回包含 9 个元素的数组（中心 + 8 个方向）
             */
            void getNeighbors(class ChunkPos neighbors[9]) const noexcept {
                neighbors[0] = *this;                          // 中心
                neighbors[1] = ChunkPos(x - 1, z - 1);        // 西北
                neighbors[2] = ChunkPos(x, z - 1);            // 北
                neighbors[3] = ChunkPos(x + 1, z - 1);        // 东北
                neighbors[4] = ChunkPos(x - 1, z);            // 西
                neighbors[5] = ChunkPos(x + 1, z);            // 东
                neighbors[6] = ChunkPos(x - 1, z + 1);        // 西南
                neighbors[7] = ChunkPos(x, z + 1);            // 南
                neighbors[8] = ChunkPos(x + 1, z + 1);        // 东南
            }

            // ========== 运算符重载 ==========

            constexpr bool operator==(const ChunkPos& other) const noexcept {
                return x == other.x && z == other.z;
            }

            constexpr bool operator!=(const ChunkPos& other) const noexcept {
                return !(*this == other);
            }

            constexpr ChunkPos operator+(const ChunkPos& other) const noexcept {
                return ChunkPos(x + other.x, z + other.z);
            }

            constexpr ChunkPos operator-(const ChunkPos& other) const noexcept {
                return ChunkPos(x - other.x, z - other.z);
            }

            // ========== 哈希函数 ==========

            struct Hash {
                std::size_t operator()(const ChunkPos& pos) const noexcept {
                    // 使用 toLong() 的结果作为哈希值
                    return static_cast<std::size_t>(pos.toLong());
                }
            };
        };

        // ========== SectionPos - 区块段坐标 ==========

        /**
         * @brief 区块段坐标 - 对应 Minecraft: net.minecraft.core.SectionPos
         *
         * ChunkSection 是 16x16x16 的方块组。
         * SectionPos 表示 ChunkSection 的位置。
         */
        class SectionPos {
        public:
            int32_t x;
            int32_t y;
            int32_t z;

            constexpr SectionPos() noexcept : x(0), y(0), z(0) {}

            constexpr SectionPos(int32_t x, int32_t y, int32_t z) noexcept
                : x(x), y(y), z(z) {}

            /**
             * @brief 从 BlockPos 创建 SectionPos
             */
            explicit constexpr SectionPos(const class BlockPos& pos) noexcept
                : x(pos.x >> 4), y(pos.y >> 4), z(pos.z >> 4) {}

            /**
             * @brief 从 ChunkPos 和段 Y 坐标创建
             */
            constexpr SectionPos(const ChunkPos& chunkPos, int32_t sectionY) noexcept
                : x(chunkPos.x), y(sectionY), z(chunkPos.z) {}

            /**
             * @brief 计算区块段的一维索引
             * 用于 Y=0-15 的段数组索引
             */
            constexpr int32_t getYIndex() const noexcept {
                return y;
            }

            /**
             * @brief 转换为区块坐标
             */
            constexpr ChunkPos toChunkPos() const noexcept {
                return ChunkPos(x, z);
            }

            /**
             * @brief 计算区块段的最小方块坐标
             */
            constexpr class BlockPos getMinBlockPos() const noexcept {
                return class BlockPos(x << 4, y << 4, z << 4);
            }

            /**
             * @brief 从 BlockPos 计算段坐标
             */
            static constexpr int32_t blockToSectionCoord(int32_t blockCoord) noexcept {
                return blockCoord >= 0 ? blockCoord >> 4 : ~(~blockCoord >> 4);
            }

            // ========== 区块段键值 ==========

            /**
             * @brief 计算区块段的唯一键值
             * 对应 Minecraft: SectionPos.asLong()
             */
            constexpr int64_t asLong() const noexcept {
                return (static_cast<int64_t>(x) & 0xFFFFFFF) |
                       ((static_cast<int64_t>(y) & 0xFFFFFFF) << 42) |
                       ((static_cast<int64_t>(z) & 0xFFFFFFF) << 21);
            }

            static constexpr int64_t asLong(int32_t x, int32_t y, int32_t z) noexcept {
                return (static_cast<int64_t>(x) & 0xFFFFFFF) |
                       ((static_cast<int64_t>(y) & 0xFFFFFFF) << 42) |
                       ((static_cast<int64_t>(z) & 0xFFFFFFF) << 21);
            }

            // ========== 运算符重载 ==========

            constexpr bool operator==(const SectionPos& other) const noexcept {
                return x == other.x && y == other.y && z == other.z;
            }

            constexpr bool operator!=(const SectionPos& other) const noexcept {
                return !(*this == other);
            }

            // ========== 哈希函数 ==========

            struct Hash {
                std::size_t operator()(const SectionPos& pos) const noexcept {
                    return static_cast<std::size_t>(pos.asLong());
                }
            };
        };

        // ========== inline 实现 ==========

        constexpr ChunkPos::ChunkPos(const BlockPos& pos) noexcept
            : x(getBlockCoord(pos.x)), z(getBlockCoord(pos.z)) {}

    } // namespace Core
} // namespace PrismaCraft

// 前向声明
namespace PrismaCraft { namespace Core { class BlockPos; } }

// 特化 std::hash
template <>
struct std::hash<PrismaCraft::Core::ChunkPos> {
    std::size_t operator()(const PrismaCraft::Core::ChunkPos& pos) const noexcept {
        return PrismaCraft::Core::ChunkPos::Hash{}(pos);
    }
};

template <>
struct std::hash<PrismaCraft::Core::SectionPos> {
    std::size_t operator()(const PrismaCraft::Core::SectionPos& pos) const noexcept {
        return PrismaCraft::Core::SectionPos::Hash{}(pos);
    }
};
