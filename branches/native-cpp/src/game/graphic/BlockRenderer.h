#pragma once

#include "ChunkMeshComponent.h"
#include "ChunkMeshGenerator.h"
#include <PrismaEngine/PrismaEngine.h>
#include <PrismaEngine/graphic/TextureAtlas.h>
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
     * @param device 渲染设备
     * @param textureMgr 方块纹理管理器（可选）
     */
    bool initialize(PrismaEngine::Graphic::IRenderDevice* device,
                   PrismaEngine::Graphic::BlockTextureManager* textureMgr = nullptr);

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

    /**
     * @brief 设置方块纹理管理器
     */
    void setBlockTextureManager(PrismaEngine::Graphic::BlockTextureManager* manager) {
        textureManager_ = manager;
    }

    /**
     * @brief 获取方块纹理管理器
     */
    PrismaEngine::Graphic::BlockTextureManager* getBlockTextureManager() const {
        return textureManager_;
    }

private:
    /**
     * @brief 统一缓冲对象 (UBO) 结构
     */
    struct UniformBufferObject {
        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 proj;
    };

    /**
     * @brief 创建或更新 GPU 缓冲区
     */
    void updateChunkBuffers(const glm::ivec2& chunkPos, ChunkRenderData& renderData);

    PrismaEngine::Graphic::IRenderDevice* renderDevice = nullptr;
    PrismaEngine::Graphic::BlockTextureManager* textureManager_ = nullptr;
    std::shared_ptr<PrismaEngine::Graphic::IPipelineState> pipelineState;
    std::shared_ptr<PrismaEngine::Graphic::IBuffer> uboBuffer;

    std::unordered_map<uint64_t, ChunkRenderData> chunkRenderData;
    RenderStats stats;

    // 计算区块哈希
    static uint64_t hashChunkPos(const glm::ivec2& pos) {
        return (static_cast<uint64_t>(pos.x) & 0xFFFFFFFF) |
               (static_cast<uint64_t>(pos.y) << 32);
    }
};

} // namespace PrismaCraft
