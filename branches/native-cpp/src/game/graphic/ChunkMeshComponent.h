#pragma once

#include <PrismaEngine/PrismaEngine.h>
#include <glm/glm.hpp>
#include <vector>
#include <memory>

namespace PrismaCraft {

/**
 * @brief 区块网格组件 - 存储区块的渲染数据
 *
 * 对应 Minecraft 的区块网格系统
 */
class ChunkMeshComponent {
public:
    /**
     * @brief 顶点格式
     */
    struct Vertex {
        glm::vec3 position;     // 位置
        glm::vec3 normal;       // 法线
        glm::vec2 uv;           // 纹理坐标
        uint8_t blockID;        // 方块 ID（用于纹理查找）

        Vertex() = default;
        Vertex(const glm::vec3& pos, const glm::vec3& norm, const glm::vec2& tex, uint8_t bid)
            : position(pos), normal(norm), uv(tex), blockID(bid) {}
    };

    /**
     * @brief 网格数据
     */
    struct MeshData {
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;

        void clear() {
            vertices.clear();
            indices.clear();
        }

        size_t getVertexCount() const { return vertices.size(); }
        size_t getIndexCount() const { return indices.size(); }
        bool isEmpty() const { return vertices.empty(); }
    };

    ChunkMeshComponent() = default;
    ~ChunkMeshComponent() = default;

    /**
     * @brief 获取网格数据
     */
    const MeshData& getMeshData() const { return meshData; }
    MeshData& getMeshData() { return meshData; }

    /**
     * @brief 设置网格数据
     */
    void setMeshData(const MeshData& data) { meshData = data; }

    /**
     * @brief 检查是否需要更新
     */
    bool isDirty() const { return dirty; }
    void markDirty() { dirty = true; }
    void clearDirty() { dirty = false; }

    /**
     * @brief 获取区块位置
     */
    const glm::ivec2& getChunkPos() const { return chunkPos; }
    void setChunkPos(const glm::ivec2& pos) { chunkPos = pos; }

    /**
     * @brief 释放资源
     */
    void release() {
        meshData.clear();
    }

private:
    MeshData meshData;
    glm::ivec2 chunkPos{0, 0};
    bool dirty = true;
};

} // namespace PrismaCraft
