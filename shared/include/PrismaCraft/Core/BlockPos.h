#pragma once

#include <cstdint>
#include <functional>
#include <cmath>

namespace PrismaCraft {
    namespace Core {

        /**
         * @brief 方块坐标 - 对应 Minecraft: net.minecraft.core.BlockPos
         *
         * Minecraft 使用整数坐标系统表示方块位置。
         * 每个方块占用 1x1x1 的空间。
         */
        class BlockPos {
        public:
            int32_t x;
            int32_t y;
            int32_t z;

            // ========== 构造函数 ==========

            constexpr BlockPos() noexcept : x(0), y(0), z(0) {}

            constexpr BlockPos(int32_t x, int32_t y, int32_t z) noexcept
                : x(x), y(y), z(z) {}

            explicit constexpr BlockPos(int32_t value) noexcept
                : x(value), y(value), z(value) {}

            // ========== 静态工厂方法 (Minecraft 风格) ==========

            /**
             * @brief 从容器坐标（float）创建 BlockPos
             * 对应 Minecraft: BlockPos.containing(Vec3)
             */
            static constexpr BlockPos containing(double x, double y, double z) noexcept {
                return BlockPos(
                    static_cast<int32_t>(std::floor(x)),
                    static_cast<int32_t>(std::floor(y)),
                    static_cast<int32_t>(std::floor(z))
                );
            }

            // ========== 坐标操作 ==========

            /**
             * @brief 获取 X 坐标在区块内的本地坐标 (0-15)
             * 对应 Minecraft: getX() & 15
             */
            constexpr int32_t getXInSection() const noexcept {
                return x & 15;
            }

            /**
             * @brief 获取 Z 坐标在区块内的本地坐标 (0-15)
             */
            constexpr int32_t getZInSection() const noexcept {
                return z & 15;
            }

            /**
             * @brief 获取 Y 坐标在区块段内的本地坐标 (0-15)
             */
            constexpr int32_t getYInSection() const noexcept {
                return y & 15;
            }

            /**
             * @brief 计算方块的一维数组索引
             * 用于 16x16x16 的 ChunkSection
             * 对应 Minecraft: BlockPos.getY() << 8 | getZ() << 4 | getX()
             */
            constexpr int32_t getIndexInSection() const noexcept {
                return (y & 15) << 8 | (z & 15) << 4 | (x & 15);
            }

            /**
             * @brief 从一维索引计算 BlockPos
             * @param index 一维索引 (0-4095)
             */
            static constexpr BlockPos fromIndexInSection(int32_t index) noexcept {
                return BlockPos(
                    index & 15,
                    (index >> 8) & 15,
                    (index >> 4) & 15
                );
            }

            /**
             * @brief 计算到另一个 BlockPos 的距离（平方）
             */
            constexpr double distSqr(const BlockPos& other) const noexcept {
                double dx = x - other.x;
                double dy = y - other.y;
                double dz = z - other.z;
                return dx * dx + dy * dy + dz * dz;
            }

            /**
             * @brief 曼哈顿距离
             */
            constexpr int32_t distManhattan(const BlockPos& other) const noexcept {
                return std::abs(x - other.x) +
                       std::abs(y - other.y) +
                       std::abs(z - other.z);
            }

            // ========== 方向偏移 (Minecraft 风格) ==========

            constexpr BlockPos offset(int32_t dx, int32_t dy, int32_t dz) const noexcept {
                return BlockPos(x + dx, y + dy, z + dz);
            }

            // 六个基本面方向
            constexpr BlockPos north() const noexcept { return offset(0, 0, -1); }
            constexpr BlockPos south() const noexcept { return offset(0, 0, 1); }
            constexpr BlockPos east()  const noexcept { return offset(1, 0, 0); }
            constexpr BlockPos west()  const noexcept { return offset(-1, 0, 0); }
            constexpr BlockPos up()    const noexcept { return offset(0, 1, 0); }
            constexpr BlockPos down()  const noexcept { return offset(0, -1, 0); }

            // ========== 运算符重载 ==========

            constexpr bool operator==(const BlockPos& other) const noexcept {
                return x == other.x && y == other.y && z == other.z;
            }

            constexpr bool operator!=(const BlockPos& other) const noexcept {
                return !(*this == other);
            }

            constexpr BlockPos operator+(const BlockPos& other) const noexcept {
                return BlockPos(x + other.x, y + other.y, z + other.z);
            }

            constexpr BlockPos operator-(const BlockPos& other) const noexcept {
                return BlockPos(x - other.x, y - other.y, z - other.z);
            }

            // ========== 哈希函数（用于 unordered_map） ==========

            struct Hash {
                std::size_t operator()(const BlockPos& pos) const noexcept {
                    // Minecraft 风格的哈希函数
                    std::size_t h1 = std::hash<int32_t>{}(pos.x);
                    std::size_t h2 = std::hash<int32_t>{}(pos.y);
                    std::size_t h3 = std::hash<int32_t>{}(pos.z);
                    return h1 ^ (h2 << 4) ^ (h3 << 8);
                }
            };
        };

        /**
         * @brief 可变的 BlockPos - 对应 Minecraft: BlockPos.MutableBlockPos
         *
         * 用于需要频繁修改坐标的场景，避免频繁分配内存
         */
        class MutableBlockPos {
        public:
            int32_t x;
            int32_t y;
            int32_t z;

            constexpr MutableBlockPos() noexcept : x(0), y(0), z(0) {}

            constexpr MutableBlockPos(int32_t x, int32_t y, int32_t z) noexcept
                : x(x), y(y), z(z) {}

            constexpr MutableBlockPos(const BlockPos& pos) noexcept
                : x(pos.x), y(pos.y), z(pos.z) {}

            // 链式设置方法
            MutableBlockPos& set(int32_t x, int32_t y, int32_t z) noexcept {
                this->x = x;
                this->y = y;
                this->z = z;
                return *this;
            }

            MutableBlockPos& setX(int32_t x) noexcept {
                this->x = x;
                return *this;
            }

            MutableBlockPos& setY(int32_t y) noexcept {
                this->y = y;
                return *this;
            }

            MutableBlockPos& setZ(int32_t z) noexcept {
                this->z = z;
                return *this;
            }

            MutableBlockPos& move(int32_t dx, int32_t dy, int32_t dz) noexcept {
                x += dx;
                y += dy;
                z += dz;
                return *this;
            }

            // 转换为不可变的 BlockPos
            constexpr BlockPos toImmutable() const noexcept {
                return BlockPos(x, y, z);
            }

            constexpr operator BlockPos() const noexcept {
                return BlockPos(x, y, z);
            }
        };

        /**
         * @brief 方块坐标的容器版本 - 对应 Minecraft: Vec3i (部分功能)
         */
        class Vec3i {
        public:
            int32_t x;
            int32_t y;
            int32_t z;

            constexpr Vec3i() noexcept : x(0), y(0), z(0) {}

            constexpr Vec3i(int32_t x, int32_t y, int32_t z) noexcept
                : x(x), y(y), z(z) {}

            // 从 BlockPos 转换
            constexpr Vec3i(const BlockPos& pos) noexcept
                : x(pos.x), y(pos.y), z(pos.z) {}

            constexpr Vec3i operator+(const Vec3i& other) const noexcept {
                return Vec3i(x + other.x, y + other.y, z + other.z);
            }

            constexpr Vec3i operator-(const Vec3i& other) const noexcept {
                return Vec3i(x - other.x, y - other.y, z - other.z);
            }

            constexpr Vec3i operator*(int32_t scalar) const noexcept {
                return Vec3i(x * scalar, y * scalar, z * scalar);
            }
        };

    } // namespace Core
} // namespace PrismaCraft

// 特化 std::hash 以支持 unordered_map
template <>
struct std::hash<PrismaCraft::Core::BlockPos> {
    std::size_t operator()(const PrismaCraft::Core::BlockPos& pos) const noexcept {
        return PrismaCraft::Core::BlockPos::Hash{}(pos);
    }
};
