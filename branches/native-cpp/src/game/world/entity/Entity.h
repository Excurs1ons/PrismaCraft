#pragma once

#include "../../../../../shared/include/PrismaCraft/Core/BlockPos.h"
#include "../../../../../shared/include/PrismaCraft/Core/BlockState.h"
#include <cstdint>
#include <string>
#include <memory>

namespace PrismaCraft {
    namespace Game {

        // 前向声明
        class Level;
        class EntityType;
        class Player;
        class AABB;

        /**
         * @brief 实体基类 - 对应 Minecraft: net.minecraft.world.entity.Entity
         *
         * Entity 是所有游戏对象（玩家、生物、物品、掉落物等）的基类。
         *
         * 继承层次：
         * - Entity (基类)
         *   - LivingEntity (生物实体)
         *     - Player (玩家)
         *     - Mob (怪物/动物)
         *   - ItemEntity (掉落物品)
         *   - ExperienceOrb (经验球)
         */
        class Entity {
        public:
            virtual ~Entity() = default;

            // ========== 实体 ID ==========

            /**
             * @brief 获取实体唯一 ID
             */
            int32_t getId() const noexcept { return m_id; }

            /**
             * @brief 设置实体 ID
             */
            void setId(int32_t id) noexcept { m_id = id; }

            // ========== 实体类型 ==========

            /**
             * @brief 获取实体类型
             */
            const EntityType& getType() const noexcept { return *m_entityType; }

            // ========== 位置和移动 ==========

            /**
             * @brief 获取实体位置
             * 对应 Minecraft: Entity.position()
             */
            class Vec3 getPosition() const noexcept { return m_position; }

            /**
             * @brief 设置实体位置
             * 对应 Minecraft: Entity.setPos(double, double, double)
             */
            void setPos(double x, double y, double z);

            /**
             * @brief 移动实体
             * 对应 Minecraft: Entity.move(Vec3)
             */
            virtual void move(double dx, double dy, double dz);

            /**
             * @brief 获取方块坐标（包含实体的方块）
             */
            BlockPos getBlockPos() const noexcept {
                return BlockPos(
                    static_cast<int32_t>(std::floor(m_position.x)),
                    static_cast<int32_t>(std::floor(m_position.y)),
                    static_cast<int32_t>(std::floor(m_position.z))
                );
            }

            /**
             * @brief 获取方块坐标（包含实体脚部的方块）
             */
            BlockPos getBlockPosBelow() const noexcept {
                return BlockPos(
                    static_cast<int32_t>(std::floor(m_position.x)),
                    static_cast<int32_t>(std::floor(m_position.y - 0.2)),
                    static_cast<int32_t>(std::floor(m_position.z))
                );
            }

            // ========== 旋转 ==========

            /**
             * @brief 获取 Y 轴旋转（偏航角）
             * 对应 Minecraft: Entity.getYRot()
             */
            float getYRot() const noexcept { return m_yRot; }

            /**
             * @brief 设置 Y 轴旋转
             */
            void setYRot(float rot) noexcept { m_yRot = rot; }

            /**
             * @brief 获取 X 轴旋转（俯仰角）
             * 对应 Minecraft: Entity.getXRot()
             */
            float getXRot() const noexcept { return m_xRot; }

            /**
             * @brief 设置 X 轴旋转
             */
            void setXRot(float rot) noexcept { m_xRot = rot; }

            // ========== 速度 ==========

            /**
             * @brief 获取当前速度
             */
            class Vec3 getDeltaMovement() const noexcept { return m_deltaMovement; }

            /**
             * @brief 设置速度
             */
            void setDeltaMovement(double dx, double dy, double dz) {
                m_deltaMovement = Vec3(dx, dy, dz);
            }

            /**
             * @brief 应用速度
             */
            void addDeltaMovement(double dx, double dy, double dz) {
                m_deltaMovement = m_deltaMovement.add(dx, dy, dz);
            }

            // ========== 碰撞箱 ==========

            /**
             * @brief 获取实体碰撞箱
             * 对应 Minecraft: Entity.getBoundingBox()
             */
            virtual const AABB& getBoundingBox() const noexcept {
                return m_boundingBox;
            }

            /**
             * @brief 设置碰撞箱大小
             * 对应 Minecraft: Entity.setDimensions(float, float)
             */
            void setDimensions(float width, float height);

            /**
             * @brief 检查实体是否与方块碰撞
             */
            virtual bool isColliding(const AABB& box) const;

            // ========== 世界交互 ==========

            /**
             * @brief 获取实体所在的世界
             */
            Level& getLevel() const noexcept { return *m_level; }

            /**
             * @brief 检查实体是否在水中
             */
            bool isInWater() const noexcept;

            /**
             * @brief 检查实体是否在岩浆中
             */
            bool isInLava() const noexcept;

            /**
             * @brief 检查实体是否在流体中
             */
            virtual bool isInFluid() const;

            /**
             * @brief 检查实体是否在地面上
             */
            virtual bool isOnGround() const noexcept { return m_onGround; }

            /**
             * @brief 设置是否在地面上
             */
            void setOnGround(bool onGround) noexcept { m_onGround = onGround; }

            // ========== 实体更新 ==========

            /**
             * @brief 每帧更新实体
             * 对应 Minecraft: Entity.tick()
             */
            virtual void tick();

            /**
             * @brief 基础更新（移动、物理等）
             * 对应 Minecraft: Entity.baseTick()
             */
            virtual void baseTick();

            /**
             * @brief 游戏刻度更新后调用
             */
            virtual void postTick();

            // ========== 实体生命周期 ==========

            /**
             * @brief 实体被召唤时调用
             * 对应 Minecraft: Entity.finalizeSpawn()
             */
            virtual bool finalizeSpawn(Level& level);

            /**
             * @brief 实体死亡时调用
             * 对应 Minecraft: Entity.die()
             */
            virtual void die(const DamageSource& source);

            /**
             * @brief 实体被移除时调用
             * 对应 Minecraft: Entity.remove()
             */
            virtual void remove();

            /**
             * @brief 检查实体是否还存活
             */
            bool isAlive() const noexcept { return m_isAlive; }

            /**
             * @brief 检查实体是否已被移除
             */
            bool isRemoved() const noexcept { return m_isRemoved; }

            // ========== 伤害和生命值 ==========

            /**
             * @brief 对实体造成伤害
             * 对应 Minecraft: Entity.hurt()
             */
            virtual bool hurt(const DamageSource& source, float amount);

            /**
             * @brief 获取实体生命值
             */
            virtual float getHealth() const noexcept { return m_health; }

            /**
             * @brief 设置实体生命值
             */
            virtual void setHealth(float health) noexcept { m_health = health; }

            /**
             * @brief 获取最大生命值
             */
            virtual float getMaxHealth() const noexcept { return m_maxHealth; }

            // ========== 实体属性 ==========

            /**
             * @brief 检查实体是否是玩家
             */
            virtual bool isPlayer() const { return false; }

            /**
             * @brief 转换为玩家（如果是）
             */
            virtual Player* asPlayer() { return nullptr; }
            virtual const Player* asPlayer() const { return nullptr; }

            /**
             * @brief 检查实体是否在服务器端
             */
            bool isServerSide() const noexcept { return m_isServerSide; }

            // ========== 渲染 ==========

            /**
             * @brief 获取渲染距离
             */
            float getRenderDistance() const noexcept { return m_renderDistance; }

            /**
             * @brief 设置渲染距离
             */
            void setRenderDistance(float distance) noexcept { m_renderDistance = distance; }

            /**
             * @brief 检查实体是否应该被渲染
             */
            bool shouldRenderAtSqrDistance(double distanceSqr) const;

            // ========== 伤害来源 ==========

            class DamageSource {
            public:
                enum class Type {
                    GENERIC,        // 通用伤害
                    ATTACK,         // 攻击伤害
                    MAGIC,          // 魔法伤害
                    FIRE,           // 火焰伤害
                    FALL,           // 跌落伤害
                    DROWN,          // 溺水伤害
                    STARVE,         // 饥饿伤害
                    EXPLOSION,      // 爆炸伤害
                };

                DamageSource(Type type) : m_type(type) {}

                Type getType() const noexcept { return m_type; }
                const Entity* getEntity() const noexcept { return m_entity; }
                const std::string& getMsgId() const noexcept { return m_msgId; }

            private:
                Type m_type;
                const Entity* m_entity = nullptr;
                std::string m_msgId;
            };

        protected:
            Entity(EntityType& entityType, Level& level);

            // 实体属性
            EntityType* m_entityType;
            Level* m_level;

            // 位置和旋转
            Vec3 m_position;
            Vec3 m_deltaMovement;
            float m_yRot = 0.0f;
            float m_xRot = 0.0f;

            // 碰撞
            AABB m_boundingBox;
            bool m_onGround = false;

            // 生命值
            float m_health = 20.0f;
            float m_maxHealth = 20.0f;

            // 状态
            bool m_isAlive : 1 = true;
            bool m_isRemoved : 1 = false;
            bool m_isServerSide : 1 = true;

            // 渲染
            float m_renderDistance = 64.0f;

            // 实体 ID（由世界分配）
            int32_t m_id = 0;
        };

        /**
         * @brief 三维向量 - 对应 Minecraft: net.minecraft.world.phys.Vec3
         */
        class Vec3 {
        public:
            double x;
            double y;
            double z;

            constexpr Vec3() noexcept : x(0), y(0), z(0) {}

            constexpr Vec3(double x, double y, double z) noexcept
                : x(x), y(y), z(z) {}

            /**
             * @brief 从 BlockPos 创建 Vec3
             */
            constexpr Vec3(const BlockPos& pos) noexcept
                : x(pos.x), y(pos.y), z(pos.z) {}

            // ========== 向量运算 ==========

            Vec3 add(double dx, double dy, double dz) const {
                return Vec3(x + dx, y + dy, z + dz);
            }

            Vec3 add(const Vec3& other) const {
                return Vec3(x + other.x, y + other.y, z + other.z);
            }

            Vec3 subtract(const Vec3& other) const {
                return Vec3(x - other.x, y - other.y, z - other.z);
            }

            Vec3 multiply(double scalar) const {
                return Vec3(x * scalar, y * scalar, z * scalar);
            }

            Vec3 normalize() const;

            double length() const noexcept {
                return std::sqrt(x * x + y * y + z * z);
            }

            double lengthSqr() const noexcept {
                return x * x + y * y + z * z;
            }

            double distanceTo(const Vec3& other) const noexcept {
                double dx = x - other.x;
                double dy = y - other.y;
                double dz = z - other.z;
                return std::sqrt(dx * dx + dy * dy + dz * dz);
            }

            double distanceToSqr(const Vec3& other) const noexcept {
                double dx = x - other.x;
                double dy = y - other.y;
                double dz = z - other.z;
                return dx * dx + dy * dy + dz * dz;
            }

            // ========== 运算符 ==========

            Vec3 operator+(const Vec3& other) const {
                return Vec3(x + other.x, y + other.y, z + other.z);
            }

            Vec3 operator-(const Vec3& other) const {
                return Vec3(x - other.x, y - other.y, z - other.z);
            }

            Vec3 operator*(double scalar) const {
                return Vec3(x * scalar, y * scalar, z * scalar);
            }

            Vec3 operator/(double scalar) const {
                return Vec3(x / scalar, y / scalar, z / scalar);
            }
        };

        /**
         * @brief 轴对齐包围盒 - 对应 Minecraft: net.minecraft.world.phys.AABB
         */
        class AABB {
        public:
            double minX, minY, minZ;
            double maxX, maxY, maxZ;

            constexpr AABB() noexcept
                : minX(0), minY(0), minZ(0), maxX(0), maxY(0), maxZ(0) {}

            constexpr AABB(double x1, double y1, double z1, double x2, double y2, double z2) noexcept
                : minX(x1), minY(y1), minZ(z1), maxX(x2), maxY(y2), maxZ(z2) {}

            /**
             * @brief 创建方块大小的 AABB
             */
            static constexpr AABB ofSize(double x, double y, double z, double size) noexcept {
                return AABB(x, y, z, x + size, y + size, z + size);
            }

            // ========== 碰撞检测 ==========

            /**
             * @brief 检查是否与另一个 AABB 相交
             */
            bool intersects(const AABB& other) const noexcept {
                return minX < other.maxX && maxX > other.minX &&
                       minY < other.maxY && maxY > other.minY &&
                       minZ < other.maxZ && maxZ > other.minZ;
            }

            /**
             * @brief 检查是否包含点
             */
            bool contains(double x, double y, double z) const noexcept {
                return x >= minX && x <= maxX &&
                       y >= minY && y <= maxY &&
                       z >= minZ && z <= maxZ;
            }

            /**
             * @brief 获取两个 AABB 的交集
             */
            AABB intersection(const AABB& other) const noexcept {
                return AABB(
                    std::max(minX, other.minX),
                    std::max(minY, other.minY),
                    std::max(minZ, other.minZ),
                    std::min(maxX, other.maxX),
                    std::min(maxY, other.maxY),
                    std::min(maxZ, other.maxZ)
                );
            }

            // ========== 属性 ==========

            /**
             * @brief 获取 X 大小
             */
            double getXsize() const noexcept { return maxX - minX; }

            /**
             * @brief 获取 Y 大小
             */
            double getYsize() const noexcept { return maxY - minY; }

            /**
             * @brief 获取 Z 大小
             */
            double getZsize() const noexcept { return maxZ - minZ; }

            /**
             * @brief 获取中心点
             */
            Vec3 getCenter() const noexcept {
                return Vec3(
                    (minX + maxX) * 0.5,
                    (minY + maxY) * 0.5,
                    (minZ + maxZ) * 0.5
                );
            }

            // ========== 操作 ==========

            /**
             * @brief 移动 AABB
             */
            AABB move(double dx, double dy, double dz) const noexcept {
                return AABB(minX + dx, minY + dy, minZ + dz,
                           maxX + dx, maxY + dy, maxZ + dz);
            }

            /**
             * @brief 缩小 AABB
             */
            AABB shrink(double x, double y, double z) const noexcept {
                return AABB(minX + x, minY + y, minZ + z,
                           maxX - x, maxY - y, maxZ - z);
            }

            /**
             * @brief 扩展 AABB
             */
            AABB expand(double x, double y, double z) const noexcept {
                return AABB(minX - x, minY - y, minZ - z,
                           maxX + x, maxY + y, maxZ + z);
            }
        };

        /**
         * @brief 实体类型 - 对应 Minecraft: net.minecraft.world.entity.EntityType
         */
        class EntityType {
        public:
            /**
             * @brief 内置实体类型
             */
            enum class BuiltInType : uint16_t {
                PLAYER = 1,
                PIG = 2,
                ZOMBIE = 3,
                SKELETON = 4,
                CREEPER = 5,
                SPIDER = 6,
                ENDERMAN = 7,
                ITEM = 100,
                EXPERIENCE_ORB = 101,
            };

            EntityType(BuiltInType type, const std::string& name)
                : m_type(static_cast<uint16_t>(type)), m_name(name) {}

            uint16_t getId() const noexcept { return m_type; }
            const std::string& getName() const noexcept { return m_name; }

        private:
            uint16_t m_type;
            std::string m_name;
        };

    } // namespace Game
} // namespace PrismaCraft
