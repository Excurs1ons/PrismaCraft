#pragma once

#include "Block.h"
#include "BlockPos.h"
#include <cstdint>
#include <memory>
#include <array>
#include <functional>

namespace PrismaCraft {
    namespace Core {

        // 前向声明
        class BlockStateBase;
        class Level;
        class Entity;
        class RandomSource;

        /**
         * @brief 方块状态属性 - 对应 Minecraft: net.minecraft.world.level.block.state.properties.Property
         *
         * 方块属性用于描述方块的不同状态变体，如朝向、开关状态等。
         */
        class BlockProperty {
        public:
            enum class Type : uint8_t {
                Boolean,    // true/false (如门的开关状态)
                Integer,    // 0-15 整数 (如蛋糕的剩余量)
                Enum,       // 枚举值 (如朝向: north/south/east/west)
            };

            virtual ~BlockProperty() = default;

            virtual Type getType() const noexcept = 0;
            virtual const char* getName() const noexcept = 0;
        };

        /**
         * @brief 布尔属性 - 对应 Minecraft: BooleanProperty
         *
         * 示例：门的开关状态、活塞的延伸状态
         */
        class BooleanProperty : public BlockProperty {
        public:
            explicit constexpr BooleanProperty(const char* name) noexcept
                : m_name(name) {}

            Type getType() const noexcept override { return Type::Boolean; }
            const char* getName() const noexcept override { return m_name; }

            // 预定义的常用布尔属性
            static BooleanProperty POWERED;       // 红石充能状态
            static BooleanProperty OPEN;          // 门/活板门开启状态
            static BooleanProperty OCCUPIED;      // 床被占用状态
            static BooleanProperty LIT;           // 灯点亮起状态
            static BooleanProperty SNOWY;         // 草方块积雪状态
            static BooleanProperty SIGNAL_FIRE;   // 点火器状态
            static BooleanProperty WATERLOGGED;   // 被水淹没状态

        private:
            const char* m_name;
        };

        /**
         * @brief 整数属性 - 对应 Minecraft: IntegerProperty
         *
         * 示例：蛋糕的剩余量 (0-6)、红石线的信号强度 (0-15)
         */
        class IntegerProperty : public BlockProperty {
        public:
            constexpr IntegerProperty(const char* name, int32_t min, int32_t max) noexcept
                : m_name(name), m_min(min), m_max(max) {}

            Type getType() const noexcept override { return Type::Integer; }
            const char* getName() const noexcept override { return m_name; }

            int32_t getMin() const noexcept { return m_min; }
            int32_t getMax() const noexcept { return m_max; }

        private:
            const char* m_name;
            int32_t m_min;
            int32_t m_max;
        };

        /**
         * @brief 枚举属性 - 对应 Minecraft: EnumProperty
         *
         * 示例：朝向 (north/south/east/west/up/down)
         */
        class EnumProperty : public BlockProperty {
        public:
            constexpr EnumProperty(const char* name, const char** values, size_t count) noexcept
                : m_name(name), m_values(values), m_count(count) {}

            Type getType() const noexcept override { return Type::Enum; }
            const char* getName() const noexcept override { return m_name; }

            size_t getValueCount() const noexcept { return m_count; }
            const char* getValue(size_t index) const noexcept { return m_values[index]; }

            // 预定义的常用枚举属性
            static EnumProperty HORIZONTAL_FACING;  // 东西南北
            static EnumProperty FACING;              // 六个方向
            static EnumProperty AXIS;                // 轴 (X/Y/Z)

        private:
            const char* m_name;
            const char** m_values;
            size_t m_count;
        };

        /**
         * @brief 方块状态基类 - 对应 Minecraft: net.minecraft.world.level.block.state.BlockStateBase
         *
         * BlockStateBase 存储方块的属性值。
         * 每个方块位置都有一个 BlockState 实例。
         */
        class BlockStateBase {
        public:
            virtual ~BlockStateBase() = default;

            /**
             * @brief 获取此状态对应的方块
             */
            virtual const Block& getBlock() const noexcept = 0;

            /**
             * @brief 判断是否是空气方块
             * 对应 Minecraft: BlockState.isAir()
             */
            virtual bool isAir() const noexcept;

            /**
             * @brief 获取属性值（通用版本）
             */
            virtual bool getValue(const BooleanProperty& property) const noexcept;
            virtual int32_t getValue(const IntegerProperty& property) const noexcept;
            virtual const char* getValue(const EnumProperty& property) const noexcept;

            /**
             * @brief 设置属性值，返回新的 BlockState
             */
            virtual BlockStateBase* setValue(const BooleanProperty& property, bool value) const = 0;
            virtual BlockStateBase* setValue(const IntegerProperty& property, int32_t value) const = 0;
            virtual BlockStateBase* setValue(const EnumProperty& property, const char* value) const = 0;

            /**
             * @brief 循环枚举值，返回新的 BlockState
             * 对应 Minecraft: BlockState.cycle()
             */
            virtual BlockStateBase* cycle(const EnumProperty& property) const = 0;

            /**
             * @brief 获取方块状态的唯一 ID
             * 用于序列化和哈希
             */
            virtual uint32_t getStateId() const noexcept = 0;

            /**
             * @brief 比较两个方块状态是否相同
             */
            virtual bool operator==(const BlockStateBase& other) const noexcept = 0;
            virtual bool operator!=(const BlockStateBase& other) const noexcept = 0;

        protected:
            BlockStateBase() = default;
        };

        /**
         * @brief 方块状态 - 对应 Minecraft: net.minecraft.world.level.block.state.BlockState
         *
         * BlockState 是 BlockStateBase 的子类，增加了特定方块的功能。
         * 它是不可变的，修改属性会返回新的 BlockState 实例。
         */
        class BlockState : public BlockStateBase {
        public:
            // ========== 委托给 Block 的方法 ==========

            /**
             * @brief 获取方块
             */
            const Block& getBlock() const noexcept override {
                return *m_block;
            }

            /**
             * @brief 检查是否是实体方块
             * 对应 Minecraft: BlockState.isSolid()
             */
            bool isSolid() const noexcept {
                return m_block->isSolid(*this);
            }

            /**
             * @brief 获取光线阻挡等级
             */
            int32_t getLightBlock() const noexcept {
                return m_block->getLightBlock();
            }

            /**
             * @brief 获取发光等级
             */
            int32_t getLightEmission() const noexcept {
                return m_block->getLightEmission();
            }

            /**
             * @brief 检查是否是流体
             */
            bool isLiquid() const noexcept {
                return m_block->isLiquid();
            }

            /**
             * @brief 检查是否透明
             */
            bool isTransparent() const noexcept {
                return m_block->isTransparent();
            }

            /**
             * @brief 获取碰撞箱
             */
            class AABB getCollisionShape(class LevelReader& level, const BlockPos& pos) const;

            /**
             * @brief 检查是否应该跳过渲染（与邻居方块相同）
             */
            bool skipRendering(const BlockState& neighbor) const noexcept {
                return m_block->skipRendering(*this, neighbor);
            }

            // ========== 方块交互 ==========

            void onDestroy(class Level& level, const BlockPos& pos) const {
                m_block->onDestroy(level, pos, *this);
            }

            void onPlace(class Level& level, const BlockPos& pos) const {
                m_block->onPlace(level, pos, *this);
            }

            void stepOn(class Level& level, const BlockPos& pos, class Entity& entity) const {
                m_block->stepOn(level, pos, *this, entity);
            }

            void neighborChanged(
                class Level& level,
                const BlockPos& pos,
                const BlockPos& fromPos,
                bool isMoving
            ) const {
                m_block->neighborChanged(level, pos, *this, fromPos, isMoving);
            }

            void randomTick(class Level& level, const BlockPos& pos, class RandomSource& random) const {
                m_block->randomTick(level, *this, pos, random);
            }

            // ========== 属性访问便捷方法 ==========

            /**
             * @brief 获取布尔属性值
             */
            bool getValue(const BooleanProperty& property) const noexcept override;

            /**
             * @brief 设置布尔属性值
             */
            BlockState* setValue(const BooleanProperty& property, bool value) const override;

            /**
             * @brief 获取整数属性值
             */
            int32_t getValue(const IntegerProperty& property) const noexcept override;

            /**
             * @brief 设置整数属性值
             */
            BlockState* setValue(const IntegerProperty& property, int32_t value) const override;

            /**
             * @brief 获取枚举属性值
             */
            const char* getValue(const EnumProperty& property) const noexcept override;

            /**
             * @brief 设置枚举属性值
             */
            BlockState* setValue(const EnumProperty& property, const char* value) const override;

            /**
             * @brief 循环枚举值
             */
            BlockState* cycle(const EnumProperty& property) const override;

            // ========== 状态 ID 和比较 ==========

            uint32_t getStateId() const noexcept override {
                return m_stateId;
            }

            bool operator==(const BlockStateBase& other) const noexcept override;
            bool operator!=(const BlockStateBase& other) const noexcept override;

            // ========== 创建新状态 ==========

            /**
             * @brief 创建一个新的 BlockState 实例
             * 这是内部使用的工厂方法
             */
            static BlockState* create(const Block& block, uint32_t stateId);

        private:
            const Block* m_block;
            uint32_t m_stateId;

            // 构造函数是私有的，使用 create() 创建
            constexpr BlockState(const Block& block, uint32_t stateId) noexcept
                : m_block(&block), m_stateId(stateId) {}

            friend class BlockStateCache;
        };

        /**
         * @brief 方块状态缓存 - 存储所有方块状态
         *
         * Minecraft 使用状态 ID 来快速查找方块状态。
         * 状态 ID 是一个紧凑的整数，前几位存储方块类型，剩余位存储属性值。
         */
        class BlockStateCache {
        public:
            static BlockStateCache& getInstance();

            /**
             * @brief 从状态 ID 获取方块状态
             */
            const BlockState& getBlockState(uint32_t stateId) const noexcept;

            /**
             * @brief 获取空气方块状态
             */
            const BlockState& getAirState() const noexcept;

            /**
             * @brief 初始化缓存
             */
            void initialize();

        private:
            BlockStateCache() = default;

            // 存储 16 位状态 ID 的方块状态（最大 65536 个状态）
            std::array<std::unique_ptr<BlockState>, 65536> m_states;
            uint32_t m_stateCount = 0;
        };

        // ========== 全局便捷函数 ==========

        /**
         * @brief 获取空气方块状态
         */
        inline const BlockState& AIR_STATE() {
            return BlockStateCache::getInstance().getAirState();
        }

    } // namespace Core
} // namespace PrismaCraft

// 特化 std::hash 以支持 unordered_map
template <>
struct std::hash<PrismaCraft::Core::BlockState> {
    std::size_t operator()(const PrismaCraft::Core::BlockState& state) const noexcept {
        return static_cast<std::size_t>(state.getStateId());
    }
};

template <>
struct std::hash<PrismaCraft::Core::BlockState*> {
    std::size_t operator()(PrismaCraft::Core::BlockState* state) const noexcept {
        return static_cast<std::size_t>(state->getStateId());
    }
};
