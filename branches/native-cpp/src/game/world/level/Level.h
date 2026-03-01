#pragma once

#include "../../../../../../shared/include/PrismaCraft/Core/BlockPos.h"
#include "../../../../../../shared/include/PrismaCraft/Core/ChunkPos.h"
#include "../../../../../../shared/include/PrismaCraft/Core/BlockState.h"
#include <memory>
#include <unordered_map>
#include <functional>

namespace PrismaCraft {
    namespace Game {

        // 前向声明
        class LevelChunk;
        class Entity;
        class Player;
        class BlockGetter;

        /**
         * @brief 世界/维度 - 对应 Minecraft: net.minecraft.world.level.Level
         *
         * Level 是 Minecraft 世界的核心类，管理所有的区块、实体和游戏逻辑。
         *
         * 继承层次：
         * - Level (基类)
         *   - ServerLevel (服务端世界)
         *   - ClientLevel (客户端世界)
         */
        class Level : public BlockGetter {
        public:
            /**
             * @brief 世界类型
             */
            enum class Type {
                OVERWORLD,    // 主世界
                NETHER,       // 下界
                THE_END       // 末地
            };

            virtual ~Level() = default;

            // ========== 世界访问 ==========

            /**
             * @brief 获取世界类型
             */
            Type getWorldType() const noexcept { return m_type; }

            /**
             * @brief 获取世界种子
             */
            int64_t getSeed() const noexcept { return m_seed; }

            /**
             * @brief 获取世界名称
             */
            const std::string& getWorldName() const noexcept { return m_worldName; }

            // ========== 区块管理 ==========

            /**
             * @brief 获取指定位置的区块
             * 如果区块不存在，返回 nullptr（客户端）或加载区块（服务端）
             * 对应 Minecraft: Level.getChunk(int, int)
             */
            virtual LevelChunk* getChunk(int32_t chunkX, int32_t chunkZ) = 0;

            /**
             * @brief 获取指定位置的区块，不触发加载
             * 对应 Minecraft: Level.getChunkIfLoaded(int, int)
             */
            virtual LevelChunk* getChunkIfLoaded(int32_t chunkX, int32_t chunkZ) noexcept = 0;

            /**
             * @brief 加载区块
             * 对应 Minecraft: Level.setChunk(ChunkAccess)
             */
            virtual void setChunk(int32_t chunkX, int32_t chunkZ, std::unique_ptr<LevelChunk> chunk) = 0;

            /**
             * @brief 卸载区块
             */
            virtual void unloadChunk(int32_t chunkX, int32_t chunkZ) = 0;

            /**
             * @brief 检查区块是否已加载
             */
            virtual bool isChunkLoaded(int32_t chunkX, int32_t chunkZ) const noexcept = 0;

            // ========== 方块操作 (实现 BlockGetter) ==========

            /**
             * @brief 获取指定位置的方块状态
             * 对应 Minecraft: Level.getBlockState(BlockPos)
             */
            const BlockState& getBlockState(const BlockPos& pos) override = 0;

            /**
             * @brief 设置指定位置的方块
             * 对应 Minecraft: Level.setBlock(BlockPos, BlockState)
             */
            virtual bool setBlock(const BlockPos& pos, const BlockState& state) = 0;

            /**
             * @brief 设置方块并触发更新
             * 对应 Minecraft: Level.setBlock(BlockPos, BlockState, int)
             */
            virtual bool setBlockAndUpdate(const BlockPos& pos, const BlockState& state) = 0;

            /**
             * @brief 移除方块（设置为空气）
             * 对应 Minecraft: Level.removeBlock(BlockPos)
             */
            virtual bool removeBlock(const BlockPos& pos) = 0;

            /**
             * @brief 销毁方块，触发掉落物和粒子效果
             * 对应 Minecraft: Level.destroyBlock(BlockPos)
             */
            virtual bool destroyBlock(const BlockPos& pos) = 0;

            /**
             * @brief 检查指定位置是否在界限内
             * 对应 Minecraft: Level.isInWorldBounds(BlockPos)
             */
            virtual bool isInWorldBounds(const BlockPos& pos) const noexcept = 0;

            // ========== 实体管理 ==========

            /**
             * @brief 添加实体到世界
             * 对应 Minecraft: Level.addFreshEntity(Entity)
             */
            virtual bool addEntity(std::unique_ptr<Entity> entity) = 0;

            /**
             * @brief 从世界移除实体
             * 对应 Minecraft: Level.remove(Entity)
             */
            virtual bool removeEntity(Entity& entity) = 0;

            /**
             * @brief 获取指定 ID 的实体
             */
            virtual Entity* getEntity(int32_t entityId) noexcept = 0;

            /**
             * @brief 获取指定位置的玩家
             */
            virtual Player* getNearestPlayer(const BlockPos& pos, double distance) noexcept = 0;

            // ========== 世界更新 ==========

            /**
             * @brief 更新世界（每帧调用）
             * 对应 Minecraft: Level.tick()
             */
            virtual void tick() = 0;

            /**
             * @brief 更新区块（每帧调用）
             */
            virtual void tickChunk(LevelChunk* chunk) = 0;

            /**
             * @brief 触发方块更新
             * 对应 Minecraft: Level.blockUpdated(BlockPos, Block)
             */
            virtual void blockUpdated(const BlockPos& pos, const Block& block) = 0;

            // ========== 方块邻居更新 ==========

            /**
             * @brief 通知方块邻居已更新
             * 对应 Minecraft: Level.updateNeighborsAt(BlockPos, Block)
             */
            virtual void updateNeighborsAt(const BlockPos& pos, const Block& block) = 0;

            /**
             * @brief 通知指定方向的邻居已更新
             * 对应 Minecraft: Level.updateNeighbor(BlockPos, Block, Direction)
             */
            virtual void updateNeighbor(const BlockPos& pos, const Block& block, const Direction& direction) = 0;

            // ========== 方块刻度 ==========

            /**
             * @brief 调度方块刻度
             * 对应 Minecraft: Level.scheduleTick(BlockPos, Block, int)
             */
            virtual void scheduleBlockTick(const BlockPos& pos, const Block& block, int32_t delay) = 0;

            /**
             * @brief 调度液体刻度
             */
            virtual void scheduleFluidTick(const BlockPos& pos, const class Fluid& fluid, int32_t delay) = 0;

            // ========== 世界属性 ==========

            /**
             * @brief 获取世界高度（最低 Y 坐标）
             */
            int32_t getMinBuildHeight() const noexcept { return m_minBuildHeight; }

            /**
             * @brief 获取世界高度（最高 Y 坐标 + 1）
             */
            int32_t getMaxBuildHeight() const noexcept { return m_maxBuildHeight; }

            /**
             * @brief 获取世界总高度
             */
            int32_t getHeight() const noexcept { return m_maxBuildHeight - m_minBuildHeight; }

            /**
             * @brief 获取海平面高度
             */
            int32_t getSeaLevel() const noexcept { return m_seaLevel; }

            // ========== 世界时间 ==========

            /**
             * @brief 获取世界时间（游戏内天数）
             */
            int64_t getDayTime() const noexcept { return m_dayTime; }

            /**
             * @brief 设置世界时间
             */
            void setDayTime(int64_t time) noexcept { m_dayTime = time; }

            /**
             * @brief 获取游戏总时间（刻度数）
             */
            int64_t getGameTime() const noexcept { return m_gameTime; }

            // ========== 游戏规则 ==========

            /**
             * @brief 游戏规则 - 对应 Minecraft: GameRules
             */
            class GameRules {
            public:
                bool doFireTick = true;        // 火是否蔓延
                bool mobGriefing = true;       // 生物是否破坏方块
                bool keepInventory = false;    // 死亡是否保留物品
                bool doDaylightCycle = true;   // 昼夜循环
                bool doWeatherCycle = true;    // 天气循环
                bool doMobSpawning = true;     // 生物生成
            };

            GameRules& getGameRules() noexcept { return m_gameRules; }
            const GameRules& getGameRules() const noexcept { return m_gameRules; }

        protected:
            Level(Type type, const std::string& worldName, int64_t seed)
                : m_type(type)
                , m_worldName(worldName)
                , m_seed(seed)
                , m_minBuildHeight(-64)
                , m_maxBuildHeight(320)
                , m_seaLevel(62)
                , m_dayTime(0)
                , m_gameTime(0)
            {}

            Type m_type;
            std::string m_worldName;
            int64_t m_seed;

            // 世界边界
            int32_t m_minBuildHeight;
            int32_t m_maxBuildHeight;
            int32_t m_seaLevel;

            // 时间
            int64_t m_dayTime;
            int64_t m_gameTime;

            // 游戏规则
            GameRules m_gameRules;

            // 已加载的区块
            std::unordered_map<ChunkPos, std::unique_ptr<LevelChunk>, ChunkPos::Hash> m_chunks;
        };

        /**
         * @brief 方块获取器接口 - 对应 Minecraft: net.minecraft.world.level.BlockGetter
         *
         * 只读的方块访问接口
         */
        class BlockGetter {
        public:
            virtual ~BlockGetter() = default;

            /**
             * @brief 获取方块状态
             */
            virtual const BlockState& getBlockState(const BlockPos& pos) = 0;

            /**
             * @brief 获取方块亮度
             */
            virtual int32_t getBrightness(const BlockPos& pos) {
                return 0;
            }

            /**
             * @brief 获取方块发出的光照等级
             */
            virtual int32_t getDirectSignal(const BlockPos& pos) {
                return 0;
            }
        };

        /**
         * @brief 方块修改器接口 - 对应 Minecraft: net.minecraft.world.level.LevelWriter
         *
         * 可写的方块访问接口
         */
        class LevelWriter : public BlockGetter {
        public:
            /**
             * @brief 设置方块
             */
            virtual bool setBlock(const BlockPos& pos, const BlockState& state) = 0;

            /**
             * @brief 移除方块
             */
            virtual bool removeBlock(const BlockPos& pos) = 0;
        };

    } // namespace Game
} // namespace PrismaCraft
