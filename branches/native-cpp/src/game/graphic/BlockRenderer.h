#pragma once

#include "ChunkMeshComponent.h"
#include "ChunkMeshGenerator.h"
#include <PrismaEngine/PrismaEngine.h>
#include <memory>
#include <unordered_map>
#include <glm/glm.hpp>

namespace PrismaCraft {

/**
 * @brief 方块渲染器 - 负责渲染区块网格
 *
 * 使用 PrismaEngine 的渲染系统
 */
class BlockRenderer {
public:
    /**
     * @brief 区块渲染数据
     */
    struct ChunkRenderData {
        std::shared_ptr<PrismaEngine::Graphic::IBuffer> vertexBuffer;
        std::shared_ptr<PrismaEngine::Graphic::IBuffer> indexBuffer;
        uint32_t indexCount = 0;
        glm::ivec2 chunkPos;
        bool dirty = true;
    };

    BlockRenderer();
    ~BlockRenderer();

    /**
     * @brief 初始化渲染器
     */
    bool initialize(PrismaEngine::Graphic::IRenderDevice* device);

    /**
     * @brief 关闭渲染器
     */
    void shutdown();

    /**
     * @brief 更新区块网格
     * @param chunkPos 区块位置
     * @param meshData 网格数据
     */
    void updateChunkMesh(const glm::ivec2& chunkPos, const ChunkMeshComponent::MeshData& meshData);

    /**
     * @brief 移除区块网格
     */
    void removeChunkMesh(const glm::ivec2& chunkPos);

    /**
     * @brief 渲染区块
     * @param commandBuffer 命令缓冲区
     * @param camera 相机
     */
    void renderChunks(PrismaEngine::Graphic::ICommandBuffer* commandBuffer,
                     const PrismaEngine::ICamera& camera);

    /**
     * @brief 获取渲染统计
     */
    struct RenderStats {
        uint32_t visibleChunks = 0;
        uint32_t totalVertices = 0;
        uint32_t totalIndices = 0;
    };

    const RenderStats& getRenderStats() const { return stats; }

    /**
     * @brief 清空所有区块
     */
    void clear();

private:
    /**
     * @brief 创建或更新 GPU 缓冲区
     */
    void updateChunkBuffers(const glm::ivec2& chunkPos, ChunkRenderData& renderData);

    PrismaEngine::Graphic::IRenderDevice* renderDevice = nullptr;
    std::shared_ptr<PrismaEngine::Graphic::IPipelineState> pipelineState;

    std::unordered_map<uint64_t, ChunkRenderData> chunkRenderData;
    RenderStats stats;

    // 计算区块哈希
    static uint64_t hashChunkPos(const glm::ivec2& pos) {
        return (static_cast<uint64_t>(pos.x) & 0xFFFFFFFF) |
               (static_cast<uint64_t>(pos.y) << 32);
    }
};

} // namespace PrismaCraft
