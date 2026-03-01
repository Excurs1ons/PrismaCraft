#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <memory>
#include <vector>
#include <functional>

namespace PrismaCraft {
    namespace Core {

        class Block;
        class BlockState;
        class Biome;
        class EntityType;

        /**
         * @brief 资源位置 - 对应 Minecraft: net.minecraft.resources.ResourceLocation
         *
         * Minecraft 使用命名空间:路径的格式来标识资源。
         * 例如: "minecraft:stone", "minecraft:oak_log"
         */
        class ResourceLocation {
        public:
            std::string namespace_;
            std::string path;

            ResourceLocation() = default;

            /**
             * @brief 从字符串解析资源位置
             * @param location 格式: "namespace:path" 或 "path" (默认命名空间为 minecraft)
             */
            explicit ResourceLocation(const std::string& location) {
                size_t colonPos = location.find(':');
                if (colonPos != std::string::npos) {
                    namespace_ = location.substr(0, colonPos);
                    path = location.substr(colonPos + 1);
                } else {
                    namespace_ = "minecraft";
                    path = location;
                }
            }

            ResourceLocation(const std::string& namespace_, const std::string& path)
                : namespace_(namespace_), path(path) {}

            /**
             * @brief 转换为字符串 "namespace:path"
             */
            std::string toString() const {
                return namespace_ + ":" + path;
            }

            /**
             * @brief 转换为路径 "namespace/path"
             */
            std::string toPath() const {
                return namespace_ + "/" + path;
            }

            bool operator==(const ResourceLocation& other) const {
                return namespace_ == other.namespace_ && path == other.path;
            }

            bool operator!=(const ResourceLocation& other) const {
                return !(*this == other);
            }

            struct Hash {
                std::size_t operator()(const ResourceLocation& loc) const noexcept {
                    return std::hash<std::string>{}(loc.toString());
                }
            };
        };

        /**
         * @brief 注册表 - 对应 Minecraft: net.minecraft.core.Registry
         *
         * Minecraft 使用注册表来管理所有可命名的游戏对象。
         * 包括方块、物品、生物群系、实体类型等。
         *
         * 注册表在游戏启动时初始化，之后是只读的。
         */
        template<typename T>
        class Registry {
        public:
            using ID = uint32_t;
            using FactoryFunc = std::function<std::unique_ptr<T>()>;

            /**
             * @brief 获取注册表单例
             */
            static Registry& getInstance() {
                static Registry instance;
                return instance;
            }

            /**
             * @brief 注册对象
             * @param name 资源位置
             * @param factory 对象工厂函数
             * @return 分配的 ID
             */
            ID registerObject(const ResourceLocation& name, FactoryFunc factory) {
                // 检查是否已注册
                if (m_nameToId.find(name) != m_nameToId.end()) {
                    return m_nameToId[name];
                }

                ID id = static_cast<ID>(m_objects.size());
                m_objects.push_back(factory());
                m_names.push_back(name);
                m_nameToId[name] = id;

                return id;
            }

            /**
             * @brief 从 ID 获取对象
             */
            T* get(ID id) const {
                if (id >= m_objects.size()) {
                    return nullptr;
                }
                return m_objects[id].get();
            }

            /**
             * @brief 从资源位置获取对象
             */
            T* get(const ResourceLocation& name) const {
                auto it = m_nameToId.find(name);
                if (it == m_nameToId.end()) {
                    return nullptr;
                }
                return m_objects[it->second].get();
            }

            /**
             * @brief 获取对象的 ID
             */
            ID getId(const T* object) const {
                for (ID id = 0; id < m_objects.size(); ++id) {
                    if (m_objects[id].get() == object) {
                        return id;
                    }
                }
                return INVALID_ID;
            }

            /**
             * @brief 获取资源位置的 ID
             */
            ID getId(const ResourceLocation& name) const {
                auto it = m_nameToId.find(name);
                return it != m_nameToId.end() ? it->second : INVALID_ID;
            }

            /**
             * @brief 获取对象的资源位置
             */
            const ResourceLocation& getName(ID id) const {
                static const ResourceLocation EMPTY;
                if (id >= m_names.size()) {
                    return EMPTY;
                }
                return m_names[id];
            }

            /**
             * @brief 获取注册对象的数量
             */
            size_t size() const {
                return m_objects.size();
            }

            /**
             * @brief 检查注册表是否已锁定（只读）
             */
            bool isLocked() const {
                return m_locked;
            }

            /**
             * @brief 锁定注册表，禁止后续注册
             */
            void lock() {
                m_locked = true;
            }

            /**
             * @brief 迭代所有对象
             */
            template<typename Func>
            void forEach(Func&& func) const {
                for (size_t i = 0; i < m_objects.size(); ++i) {
                    func(i, m_objects[i].get(), m_names[i]);
                }
            }

            static constexpr ID INVALID_ID = static_cast<ID>(-1);

        private:
            Registry() = default;

            std::vector<std::unique_ptr<T>> m_objects;
            std::vector<ResourceLocation> m_names;
            std::unordered_map<ResourceLocation, ID, ResourceLocation::Hash> m_nameToId;
            bool m_locked = false;
        };

        /**
         * @brief 内置注册表 - 对应 Minecraft: net.minecraft.core.Registries
         */
        class Registries {
        public:
            /**
             * @brief 初始化所有内置注册表
             * 对应 Minecraft: Bootstrap.register()
             */
            static void initialize();

            /**
             * @brief 获取方块注册表
             */
            static Registry<Block>& getBlockRegistry() {
                return Registry<Block>::getInstance();
            }

            /**
             * @brief 获取生物群系注册表
             */
            static Registry<Biome>& getBiomeRegistry() {
                return Registry<Biome>::getInstance();
            }

            /**
             * @brief 获取实体类型注册表
             */
            static Registry<EntityType>& getEntityTypeRegistry() {
                return Registry<EntityType>::getInstance();
            }

            // ========== 预定义的方块注册快捷方法 ==========

            /**
             * @brief 注册方块
             */
            static Registry<Block>::ID registerBlock(
                const std::string& name,
                Registry<Block>::FactoryFunc factory
            ) {
                return getBlockRegistry().registerObject(
                    ResourceLocation("minecraft", name),
                    std::move(factory)
                );
            }

            /**
             * @brief 从 ID 获取方块
             */
            static Block* getBlock(Registry<Block>::ID id) {
                return getBlockRegistry().get(id);
            }

            /**
             * @brief 从名称获取方块
             */
            static Block* getBlock(const std::string& name) {
                return getBlockRegistry().get(ResourceLocation(name));
            }

        private:
            Registries() = delete;
        };

        /**
         * @brief 默认方块注册
         * 对应 Minecraft: net.minecraft.core.Blocks
         *
         * 这是默认方块实例的便捷访问点。
         * 实际的方块实例存储在注册表中。
         */
        class Blocks {
        public:
            /**
             * @brief 注册所有默认方块
             */
            static void registerDefaultBlocks();

            /**
             * @brief 获取空气方块
             */
            static Block& AIR() {
                return *Registries::getBlock("minecraft:air");
            }

            /**
             * @brief 获取石头方块
             */
            static Block& STONE() {
                return *Registries::getBlock("minecraft:stone");
            }

            /**
             * @brief 获取草方块
             */
            static Block& GRASS_BLOCK() {
                return *Registries::getBlock("minecraft:grass_block");
            }

            /**
             * @brief 获取泥土方块
             */
            static Block& DIRT() {
                return *Registries::getBlock("minecraft:dirt");
            }

            /**
             * @brief 获取圆石方块
             */
            static Block& COBBLESTONE() {
                return *Registries::getBlock("minecraft:cobblestone");
            }

            /**
             * @brief 获取橡木木板
             */
            static Block& OAK_PLANKS() {
                return *Registries::getBlock("minecraft:oak_planks");
            }

            /**
             * @brief 获取橡木原木
             */
            static Block& OAK_LOG() {
                return *Registries::getBlock("minecraft:oak_log");
            }

            /**
             * @brief 获取橡木树叶
             */
            static Block& OAK_LEAVES() {
                return *Registries::getBlock("minecraft:oak_leaves");
            }

            /**
             * @brief 获取基岩
             */
            static Block& BEDROCK() {
                return *Registries::getBlock("minecraft:bedrock");
            }

            /**
             * @brief 获取水（流动）
             */
            static Block& WATER() {
                return *Registries::getBlock("minecraft:water");
            }

            /**
             * @brief 获取岩浆（流动）
             */
            static Block& LAVA() {
                return *Registries::getBlock("minecraft:lava");
            }

            /**
             * @brief 获取沙子
             */
            static Block& SAND() {
                return *Registries::getBlock("minecraft:sand");
            }

            /**
             * @brief 获取沙砾
             */
            static Block& GRAVEL() {
                return *Registries::getBlock("minecraft:gravel");
            }

            /**
             * @brief 获取玻璃
             */
            static Block& GLASS() {
                return *Registries::getBlock("minecraft:glass");
            }

        private:
            Blocks() = delete;
        };

        /**
         * @brief 默认方块状态注册
         * 对应 Minecraft: 方块状态自动注册到 Block
         */
        class BlockStates {
        public:
            /**
             * @brief 注册所有默认方块状态
             */
            static void registerDefaultBlockStates();

        private:
            BlockStates() = delete;
        };

    } // namespace Core
} // namespace PrismaCraft
