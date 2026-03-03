#include "chunk/LevelChunk.h"
#include "../Level.h"
#include "../../../Core/Block.h"
#include "../../../Core/Registry.h"
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>

namespace PrismaCraft {

/**
 * Chunk loading task
 */
struct ChunkLoadTask {
    int x;
    int z;
    int priority;
    std::function<void(LevelChunk*)> callback;

    ChunkLoadTask(int cx, int cz, int p, std::function<void(LevelChunk*)> cb)
        : x(cx), z(cz), priority(p), callback(std::move(cb)) {}

    bool operator<(const ChunkLoadTask& other) const {
        return priority < other.priority; // 优先级高的先执行
    }
};

/**
 * Chunk manager - manages chunk loading and unloading
 * Corresponds to: net.minecraft.server.level.ChunkMap
 */
class ChunkManager {
public:
    ChunkManager(Level* level, size_t loadThreads = 4)
        : level_(level)
        , running_(false)
        , loadThreads_(loadThreads) {

        // Initialize default block state
        auto& registry = Registries::BLOCK();
        auto* airBlock = registry.get("minecraft:air");
        if (airBlock) {
            airState_ = &airBlock->defaultBlockState();
        }
    }

    ~ChunkManager() {
        stop();
    }

    // Start chunk loading threads
    void start() {
        if (running_) return;

        running_ = true;
        for (size_t i = 0; i < loadThreads_; ++i) {
            workers_.emplace_back([this]() {
                workerLoop();
            });
        }
    }

    // Stop chunk loading threads
    void stop() {
        if (!running_) return;

        running_ = false;
        condition_.notify_all();

        for (auto& worker : workers_) {
            if (worker.joinable()) {
                worker.join();
            }
        }
        workers_.clear();
    }

    // Request chunk load
    void loadChunk(int x, int z, int priority = 0, std::function<void(LevelChunk*)> callback = nullptr) {
        std::lock_guard<std::mutex> lock(queueMutex_);

        // 检查是否已加载
        if (level_->isLoaded(x, z)) {
            if (callback) {
                callback(level_->getChunk(x, z));
            }
            return;
        }

        // 添加到加载队列
        tasks_.emplace(ChunkLoadTask(x, z, priority, std::move(callback)));
        condition_.notify_one();
    }

    // Load chunks around a position
    void loadChunksAround(int centerX, int centerZ, int radius) {
        for (int dx = -radius; dx <= radius; ++dx) {
            for (int dz = -radius; dz <= radius; ++dz) {
                int dist = std::abs(dx) + std::abs(dz);
                if (dist <= radius) {
                    int priority = radius - dist; // 距离越近优先级越高
                    loadChunk(centerX + dx, centerZ + dz, priority);
                }
            }
        }
    }

    // Unload far chunks
    void unloadFarChunks(int centerX, int centerZ, int keepRadius) {
        // TODO: 实现远距离区块卸载
        // 遍历已加载的区块，如果距离超过 keepRadius，则卸载
    }

    // Update chunk manager (call each frame)
    void update() {
        // 处理完成队列中的区块
        std::lock_guard<std::mutex> lock(completeMutex_);

        while (!completedChunks_.empty()) {
            auto& chunk = completedChunks_.front();
            // TODO: 处理已加载的区块
            completedChunks_.pop();
        }
    }

    // Get loading statistics
    size_t getPendingCount() const {
        std::lock_guard<std::mutex> lock(queueMutex_);
        return tasks_.size();
    }

    size_t getLoadedCount() const {
        // TODO: 返回已加载区块数量
        return 0;
    }

private:
    void workerLoop() {
        while (running_) {
            ChunkLoadTask task(0, 0, 0, nullptr);

            {
                std::unique_lock<std::mutex> lock(queueMutex_);
                condition_.wait(lock, [this]() {
                    return !running_ || !tasks_.empty();
                });

                if (!running_) break;

                if (!tasks_.empty()) {
                    task = std::move(const_cast<ChunkLoadTask&>(tasks_.top()));
                    tasks_.pop();
                } else {
                    continue;
                }
            }

            // 执行区块加载
            LevelChunk* chunk = loadChunkImpl(task.x, task.z);

            // 调用回调
            if (task.callback) {
                task.callback(chunk);
            }

            // 添加到完成队列
            {
                std::lock_guard<std::mutex> lock(completeMutex_);
                completedChunks_.push(chunk);
            }
        }
    }

    LevelChunk* loadChunkImpl(int x, int z) {
        // TODO: 实现实际的区块加载逻辑
        // 1. 从磁盘加载或生成新区块
        // 2. 初始化区块数据
        // 3. 添加到世界

        auto chunk = std::make_unique<LevelChunk>(level_, ChunkPos(x, z));

        // 初始化为空气方块
        for (int cy = 0; cy < LevelChunk::SECTION_COUNT; ++cy) {
            auto* section = chunk->getSection(cy);
            if (section && !section->isEmpty()) {
                // Section exists
            }
        }

        LevelChunk* chunkPtr = chunk.get();
        // TODO: 将区块添加到世界
        // level_->loadChunk(std::move(chunk));

        return chunkPtr;
    }

    Level* level_;
    const BlockState* airState_;

    // Worker threads
    std::vector<std::thread> workers_;
    size_t loadThreads_;
    bool running_;

    // Task queue
    std::priority_queue<ChunkLoadTask> tasks_;
    mutable std::mutex queueMutex_;
    std::condition_variable condition_;

    // Completed chunks
    std::queue<LevelChunk*> completedChunks_;
    mutable std::mutex completeMutex_;
};

} // namespace PrismaCraft
