#include "Level.h"
#include "chunk/LevelChunk.h"
#include "../../Core/Registry.h"
#include "../../Core/Block.h"
#include <unordered_map>
#include <mutex>
#include <algorithm>

namespace PrismaCraft {

/**
 * Server level implementation
 * Corresponds to: net.minecraft.server.level.ServerLevel
 */
class ServerLevel : public Level {
public:
    ServerLevel(const std::string& dimension, int64_t seed)
        : dimension_(dimension)
        , dayTime_(0)
        , seed_(seed)
        , nextEntityId_(1) {
        // Initialize default block state (air)
        auto& registry = Registries::BLOCK();
        auto* airBlock = registry.get("minecraft:air");
        if (airBlock) {
            airState_ = &airBlock->defaultBlockState();
        }
    }

    ~ServerLevel() override = default;

    // Chunk access
    LevelChunk* getChunk(int x, int z) const override {
        std::lock_guard<std::mutex> lock(chunkMutex_);
        ChunkPos pos(x, z);
        auto it = chunks_.find(pos);
        return (it != chunks_.end()) ? it->second.get() : nullptr;
    }

    bool isLoaded(int x, int z) const override {
        std::lock_guard<std::mutex> lock(chunkMutex_);
        ChunkPos pos(x, z);
        return chunks_.find(pos) != chunks_.end();
    }

    // Block access
    const BlockState& getBlockState(const BlockPos& pos) const override {
        // 获取区块
        int chunkX = pos.x >> 4;
        int chunkZ = pos.z >> 4;

        LevelChunk* chunk = const_cast<ServerLevel*>(this)->getChunk(chunkX, chunkZ);
        if (!chunk) {
            return *airState_; // 区块未加载，返回空气
        }

        return chunk->getBlockState(pos);
    }

    void setBlock(const BlockPos& pos, const BlockState& state) override {
        // 获取区块
        int chunkX = pos.x >> 4;
        int chunkZ = pos.z >> 4;

        LevelChunk* chunk = const_cast<ServerLevel*>(this)->getChunk(chunkX, chunkZ);
        if (!chunk) {
            // 区块未加载，忽略或创建新区块
            return;
        }

        chunk->setBlockState(pos, state);
        blockChanged(pos);
    }

    // Light
    int getBrightness(const BlockPos& pos) const override {
        return getRawBrightness(pos, 0);
    }

    int getRawBrightness(const BlockPos& pos, int amount) const override {
        // 简化实现：基于高度计算亮度
        // TODO: 实现完整的光照系统
        int height = getHeight(pos.x, pos.z);
        if (pos.y > height) {
            return 15 - std::max(0, pos.y - height);
        }
        return std::max(0, amount);
    }

    // Time
    int64_t getDayTime() const override {
        return dayTime_;
    }

    void setDayTime(int64_t time) override {
        dayTime_ = time;
    }

    // Dimension
    const std::string& getDimension() const override {
        return dimension_;
    }

    // Entity management
    void addEntity(std::unique_ptr<Entity> entity) override {
        if (!entity) return;

        int id = nextEntityId_++;
        entity->setId(id);
        entities_[id] = std::move(entity);
    }

    void removeEntity(Entity* entity) override {
        if (!entity) return;

        auto it = entities_.find(entity->getId());
        if (it != entities_.end()) {
            entities_.erase(it);
        }
    }

    Entity* getEntity(int id) const override {
        auto it = entities_.find(id);
        return (it != entities_.end()) ? it->second.get() : nullptr;
    }

    // Height map
    int getHeight(int x, int z) const override {
        int chunkX = x >> 4;
        int chunkZ = z >> 4;

        const LevelChunk* chunk = const_cast<ServerLevel*>(this)->getChunk(chunkX, chunkZ);
        if (!chunk) {
            return 64; // 默认高度
        }

        return chunk->getHeight(x & 0xF, z & 0xF);
    }

    void setHeight(int x, int z, int height) override {
        int chunkX = x >> 4;
        int chunkZ = z >> 4;

        LevelChunk* chunk = const_cast<ServerLevel*>(this)->getChunk(chunkX, chunkZ);
        if (chunk) {
            chunk->setHeight(x & 0xF, z & 0xF, height);
        }
    }

    // Game rules
    bool getGameRuleBoolean(const std::string& rule) const override {
        auto it = gameRules_.find(rule);
        if (it != gameRules_.end()) {
            return std::get<bool>(it->second);
        }
        return false;
    }

    int getGameRuleInt(const std::string& rule) const override {
        auto it = gameRules_.find(rule);
        if (it != gameRules_.end()) {
            return std::get<int>(it->second);
        }
        return 0;
    }

    // Block update
    void blockChanged(const BlockPos& pos) override {
        // TODO: 实现方块更新通知
        // 通知邻近方块，触发更新等
    }

    // Chunk management
    void loadChunk(std::unique_ptr<LevelChunk> chunk) {
        if (!chunk) return;

        std::lock_guard<std::mutex> lock(chunkMutex_);
        ChunkPos pos = chunk->getPos();
        chunk->setLoaded(true);
        chunks_[pos] = std::move(chunk);
    }

    void unloadChunk(int x, int z) {
        std::lock_guard<std::mutex> lock(chunkMutex_);
        ChunkPos pos(x, z);
        chunks_.erase(pos);
    }

    std::vector<LevelChunk*> getLoadedChunks() const {
        std::lock_guard<std::mutex> lock(chunkMutex_);
        std::vector<LevelChunk*> result;
        result.reserve(chunks_.size());
        for (const auto& pair : chunks_) {
            result.push_back(pair.second.get());
        }
        return result;
    }

    // Seed
    int64_t getSeed() const { return seed_; }

private:
    std::string dimension_;
    int64_t dayTime_;
    int64_t seed_;

    // Chunks
    mutable std::mutex chunkMutex_;
    std::unordered_map<ChunkPos, std::unique_ptr<LevelChunk>, ChunkPos::Hash> chunks_;

    // Entities
    int nextEntityId_;
    std::unordered_map<int, std::unique_ptr<Entity>> entities_;

    // Game rules
    std::unordered_map<std::string, std::variant<bool, int>> gameRules_;

    // Default air state
    const BlockState* airState_;
};

/**
 * Client level implementation
 * Corresponds to: net.minecraft.client.multiplayer.ClientLevel
 */
class ClientLevel : public Level {
public:
    ClientLevel(const std::string& dimension)
        : dimension_(dimension)
        , dayTime_(0) {
        // Initialize default block state (air)
        auto& registry = Registries::BLOCK();
        auto* airBlock = registry.get("minecraft:air");
        if (airBlock) {
            airState_ = &airBlock->defaultBlockState();
        }
    }

    ~ClientLevel() override = default;

    // Chunk access
    LevelChunk* getChunk(int x, int z) const override {
        std::lock_guard<std::mutex> lock(chunkMutex_);
        ChunkPos pos(x, z);
        auto it = chunks_.find(pos);
        return (it != chunks_.end()) ? it->second.get() : nullptr;
    }

    bool isLoaded(int x, int z) const override {
        std::lock_guard<std::mutex> lock(chunkMutex_);
        ChunkPos pos(x, z);
        return chunks_.find(pos) != chunks_.end();
    }

    // Block access
    const BlockState& getBlockState(const BlockPos& pos) const override {
        int chunkX = pos.x >> 4;
        int chunkZ = pos.z >> 4;

        LevelChunk* chunk = const_cast<ClientLevel*>(this)->getChunk(chunkX, chunkZ);
        if (!chunk) {
            return *airState_;
        }

        return chunk->getBlockState(pos);
    }

    void setBlock(const BlockPos& pos, const BlockState& state) override {
        // 客户端通常不能直接修改方块
        // 需要通过服务器发送数据包
    }

    // Light
    int getBrightness(const BlockPos& pos) const override {
        return getRawBrightness(pos, 0);
    }

    int getRawBrightness(const BlockPos& pos, int amount) const override {
        int height = getHeight(pos.x, pos.z);
        if (pos.y > height) {
            return 15 - std::max(0, pos.y - height);
        }
        return std::max(0, amount);
    }

    // Time
    int64_t getDayTime() const override {
        return dayTime_;
    }

    void setDayTime(int64_t time) override {
        dayTime_ = time;
    }

    // Dimension
    const std::string& getDimension() const override {
        return dimension_;
    }

    // Entity management
    void addEntity(std::unique_ptr<Entity> entity) override {
        if (!entity) return;

        int id = entity->getId();
        entities_[id] = std::move(entity);
    }

    void removeEntity(Entity* entity) override {
        if (!entity) return;

        auto it = entities_.find(entity->getId());
        if (it != entities_.end()) {
            entities_.erase(it);
        }
    }

    Entity* getEntity(int id) const override {
        auto it = entities_.find(id);
        return (it != entities_.end()) ? it->second.get() : nullptr;
    }

    // Height map
    int getHeight(int x, int z) const override {
        int chunkX = x >> 4;
        int chunkZ = z >> 4;

        const LevelChunk* chunk = const_cast<ClientLevel*>(this)->getChunk(chunkX, chunkZ);
        if (!chunk) {
            return 64;
        }

        return chunk->getHeight(x & 0xF, z & 0xF);
    }

    void setHeight(int x, int z, int height) override {
        // 客户端通常不直接修改高度
    }

    // Game rules
    bool getGameRuleBoolean(const std::string& rule) const override {
        auto it = gameRules_.find(rule);
        if (it != gameRules_.end()) {
            return std::get<bool>(it->second);
        }
        return false;
    }

    int getGameRuleInt(const std::string& rule) const override {
        auto it = gameRules_.find(rule);
        if (it != gameRules_.end()) {
            return std::get<int>(it->second);
        }
        return 0;
    }

    // Block update
    void blockChanged(const BlockPos& pos) override {
        // 客户端接收更新
    }

    // Chunk management
    void loadChunk(std::unique_ptr<LevelChunk> chunk) {
        if (!chunk) return;

        std::lock_guard<std::mutex> lock(chunkMutex_);
        ChunkPos pos = chunk->getPos();
        chunk->setLoaded(true);
        chunks_[pos] = std::move(chunk);
    }

    void unloadChunk(int x, int z) {
        std::lock_guard<std::mutex> lock(chunkMutex_);
        ChunkPos pos(x, z);
        chunks_.erase(pos);
    }

private:
    std::string dimension_;
    int64_t dayTime_;

    // Chunks
    mutable std::mutex chunkMutex_;
    std::unordered_map<ChunkPos, std::unique_ptr<LevelChunk>, ChunkPos::Hash> chunks_;

    // Entities
    std::unordered_map<int, std::unique_ptr<Entity>> entities_;

    // Game rules
    std::unordered_map<std::string, std::variant<bool, int>> gameRules_;

    // Default air state
    const BlockState* airState_;
};

} // namespace PrismaCraft
