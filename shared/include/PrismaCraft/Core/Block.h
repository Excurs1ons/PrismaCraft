#pragma once

#include "BlockPos.h"
#include <cstdint>
#include <string>
#include <memory>

namespace PrismaCraft {
    namespace Core {

        // 前向声明
        class BlockState;
        class BlockStateBase;

        /**
         * @brief 方块基类 - 对应 Minecraft: net.minecraft.world.level.block.Block
         *
         * 方块是 Minecraft 世界的基本构建单元。
         * 每种方块类型有一个 Block 实例，但世界中的每个方块位置都有一个 BlockState。
         */
        class Block {
        public:
            virtual ~Block() = default;

            // ========== 方块属性 ==========

            /**
             * @brief 获取方块的默认状态
             * 对应 Minecraft: Block.defaultBlockState()
             */
            virtual const BlockState& defaultBlockState() const noexcept = 0;

            /**
             * @brief 检查方块是否是实体的（阻碍移动）
             * 对应 Minecraft: Block.isSolid()
             */
            virtual bool isSolid(const BlockState& state) const noexcept {
                return true;
            }

            /**
             * @brief 检查方块是否完全阻挡光线
             * 对应 Minecraft: Block.propagatesSkylight()
             */
            virtual bool propagatesSkylight() const noexcept {
                return false;
            }

            /**
             * @brief 获取方块的透明度等级 (0-15)
             * 对应 Minecraft: Block.getLightBlock()
             */
            virtual int32_t getLightBlock() const noexcept {
                return isSolid() ? 15 : 0;
            }

            /**
             * @brief 获取方块发出的光照等级 (0-15)
             * 对应 Minecraft: Block.getLightEmission()
             */
            virtual int32_t getLightEmission() const noexcept {
                return 0;
            }

            /**
             * @brief 检查方块是否可以替换
             * 对应 Minecraft: Block.canBeReplaced()
             */
            virtual bool canBeReplaced() const noexcept {
                return false;
            }

            /**
             * @brief 检查方块是否有碰撞箱
             * 对应 Minecraft: Block.isSuffocating()
             */
            virtual bool isSuffocating() const noexcept {
                return isSolid(defaultBlockState());
            }

            /**
             * @brief 检查方块是否是流体
             */
            virtual bool isLiquid() const noexcept {
                return false;
            }

            /**
             * @brief 检查方块是否可燃
             */
            virtual bool isFlammable() const noexcept {
                return false;
            }

            // ========== 方块交互 ==========

            /**
             * @brief 方块被破坏时调用
             * 对应 Minecraft: Block.destroy()
             */
            virtual void onDestroy(class Level& level, const BlockPos& pos, const BlockState& state) const {
                // 默认：无操作
            }

            /**
             * @brief 方块被放置时调用
             * 对应 Minecraft: Block.onPlace()
             */
            virtual void onPlace(class Level& level, const BlockPos& pos, const BlockState& state) const {
                // 默认：无操作
            }

            /**
             * @brief 实体碰撞方块时调用
             * 对应 Minecraft: Block.stepOn()
             */
            virtual void stepOn(class Level& level, const BlockPos& pos, const BlockState& state, class Entity& entity) const {
                // 默认：无操作
            }

            // ========== 方块更新 ==========

            /**
             * @brief 邻居方块更新时调用
             * 对应 Minecraft: Block.neighborChanged()
             */
            virtual void neighborChanged(
                class Level& level,
                const BlockPos& pos,
                const BlockState& state,
                const BlockPos& fromPos,
                bool isMoving
            ) const {
                // 默认：无操作
            }

            // ========== 方块形状 ==========

            /**
             * @brief 获取方块的碰撞箱
             * 对应 Minecraft: Block.getShape()
             */
            virtual class AABB getCollisionShape(
                const BlockState& state,
                class LevelReader& level,
                const BlockPos& pos
            ) const;

            // ========== 方块渲染 ==========

            /**
             * @brief 检查方块是否不应该渲染面（因为相邻方块）
             * 对应 Minecraft: Block.skipRendering()
             */
            virtual bool skipRendering(
                const BlockState& state,
                const BlockState& neighborState
            ) const noexcept {
                return neighborState.isSolid() && state.isSolid();
            }

            /**
             * @brief 检查方块是否全透明（不渲染任何面）
             */
            virtual bool isTransparent() const noexcept {
                return false;
            }

            // ========== 方块刻度 ==========

            /**
             * @brief 随机刻更新
             * 对应 Minecraft: Block.randomTick()
             */
            virtual void randomTick(
                class Level& level,
                const BlockState& state,
                const BlockPos& pos,
                class RandomSource& random
            ) const {
                // 默认：不处理随机刻
            }

            // ========== 方块工具 ==========

            /**
             * @brief 检查工具是否可以正确收获此方块
             * 对应 Minecraft: Block.isCorrectToolForDrops()
             */
            virtual bool isCorrectToolForDrops(const class ItemStack& stack) const noexcept {
                return true;
            }

            /**
             * @brief 获取方块的硬度
             * 对应 Minecraft: Block.getDestroySpeed()
             */
            virtual float getDestroySpeed(const BlockState& state) const noexcept {
                return 1.0f;
            }

            // ========== 方块识别 ==========

            /**
             * @brief 获取方块的语言键
             * 对应 Minecraft: Block.getDescriptionId()
             */
            virtual const std::string& getDescriptionId() const noexcept = 0;

            /**
             * @brief 获取方块 ID（注册表键）
             */
            virtual const std::string& getRegistryName() const noexcept = 0;

            // ========== 基础方块类型 ==========

            /**
             * @brief 空气方块 - 对应 Minecraft: Blocks.AIR
             */
            static Block& AIR();

            /**
             * @brief 石头方块 - 对应 Minecraft: Blocks.STONE
             */
            static Block& STONE();

            /**
             * @brief 草方块 - 对应 Minecraft: Blocks.GRASS_BLOCK
             */
            static Block& GRASS_BLOCK();

            /**
             * @brief 泥土方块 - 对应 Minecraft: Blocks.DIRT
             */
            static Block& DIRT();

        protected:
            Block() = default;
        };

        /**
         * @brief 方块类型 - 对应 Minecraft 方块的基础类型枚举
         *
         * Minecraft 使用方块 ID 来识别方块类型。
         * 这是简化的方块类型枚举，对应注册表中的方块。
         */
        enum class BlockType : uint16_t {
            AIR = 0,
            STONE = 1,
            GRASS_BLOCK = 2,
            DIRT = 3,
            COBBLESTONE = 4,
            PLANKS_OAK = 5,
            SAPLING_OAK = 6,
            BEDROCK = 7,
            WATER_FLOWING = 8,
            WATER_STATIC = 9,
            LAVA_FLOWING = 10,
            LAVA_STATIC = 11,
            SAND = 12,
            GRAVEL = 13,
            GOLD_ORE = 14,
            IRON_ORE = 15,
            COAL_ORE = 16,
            LOG_OAK = 17,
            LEAVES_OAK = 18,
            SPONGE = 19,
            GLASS = 20,
            LAPIS_ORE = 21,
            LAPIS_BLOCK = 22,
            DISPENSER = 23,
            SANDSTONE = 24,
            NOTE_BLOCK = 25,
            BED_RED = 26,
            POWERED_RAIL = 27,
            DETECTOR_RAIL = 28,
            STICKY_PISTON = 29,
            COBWEB = 30,
            GRASS = 31,
            DEAD_BUSH = 32,
            PISTON = 33,
            PISTON_HEAD = 34,
            WHITE_WOOL = 35,
            // ... 更多方块类型
        };

        /**
         * @brief 获取方块类型对应的方块实例
         * 简化版本，实际应该使用注册表
         */
        Block& getBlockByType(BlockType type);

    } // namespace Core
} // namespace PrismaCraft
