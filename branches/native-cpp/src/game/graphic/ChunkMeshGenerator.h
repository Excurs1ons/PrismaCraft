#pragma once

#include "ChunkMeshComponent.h"
#include "../world/level/chunk/LevelChunk.h"
#include "../world/level/Level.h"
#include <PrismaEngine/graphic/TextureAtlas.h>
#include <glm/glm.hpp>
#include <array>
#include <functional>
#include <string>

namespace PrismaCraft {

/**
 * @brief 区块网格生成器 - 从区块数据生成渲染网格
 *
 * 对应 Minecraft 的 ChunkMeshBuilder
 * 功能：
 * - 方块可见性剔除（不渲染被遮挡的面）
 * - Greedy Meshing 优化（合并相邻面）
 * - 纹理 UV 坐标计算
 * - 环境光遮蔽（可选）
 */
class ChunkMeshGenerator {
public:
    /**
     * @brief 方块面方向
     */
    enum class FaceDirection {
        TOP = 0,     // +Y
        BOTTOM = 1,  // -Y
        NORTH = 2,   // -Z
        SOUTH = 3,   // +Z
        WEST = 4,    // -X
        EAST = 5     // +X
    };

    /**
     * @brief 网格生成选项
     */
    struct GenerationOptions {
        bool enableFaceCulling = true;       // 启用面剔除
        bool enableGreedyMeshing = true;     // 启用 Greedy Meshing
        bool enableAmbientOcclusion = false; // 启用环境光遮蔽（高级）
        bool smoothLighting = false;         // 启用平滑光照
    };

    ChunkMeshGenerator() = default;
    ~ChunkMeshGenerator() = default;

    /**
     * @brief 设置方块纹理管理器
     * @param manager 纹理管理器指针
     */
    void setBlockTextureManager(PrismaEngine::Graphic::BlockTextureManager* manager) {
        blockTextureManager_ = manager;
    }

    /**
     * @brief 获取方块纹理管理器
     */
    PrismaEngine::Graphic::BlockTextureManager* getBlockTextureManager() const {
        return blockTextureManager_;
    }

    /**
     * @brief 生成区块网格
     * @param level 世界级别（用于跨区块查询）
     * @param chunk 区块数据
     * @param options 生成选项
     * @return 网格数据
     */
    ChunkMeshComponent::MeshData generateMesh(const Level* level, const LevelChunk& chunk, const GenerationOptions& options = GenerationOptions());

    /**
     * @brief 生成单个区块切片的网格
     * @param level 世界级别（用于跨区块查询）
     * @param chunk 当前区块
     * @param section 区块切片
     * @param chunkYOffset 区块 Y 偏移
     * @param options 生成选项
     * @return 网格数据
     */
    ChunkMeshComponent::MeshData generateSectionMesh(const Level* level, const LevelChunk& chunk, const ChunkSection& section, int chunkYOffset, const GenerationOptions& options = GenerationOptions());

    /**
     * @brief 检查方块面是否可见
     * @param level 世界级别（用于跨区块查询）
     * @param chunk 当前区块
     * @param x, y, z 方块世界坐标
     * @param direction 面方向
     * @return 是否可见
     */
    static bool isFaceVisible(const Level* level, int x, int y, int z, FaceDirection direction);

    /**
     * @brief 获取面法线
     */
    static glm::vec3 getFaceNormal(FaceDirection direction);

    /**
     * @brief 获取面的 UV 坐标
     * @param direction 面方向
     * @param blockName 方块名称
     * @return UV 坐标数组（4个顶点）
     */
    std::array<glm::vec2, 4> getFaceUVs(FaceDirection direction, const std::string& blockName) const;

private:
    /**
     * @brief 添加方块面到网格
     * @param mesh 网格数据
     * @param pos 方块位置
     * @param direction 面方向
     * @param blockID 方块 ID
     * @param blockName 方块名称（用于获取纹理）
     * @param options 生成选项
     */
    void addFace(ChunkMeshComponent::MeshData& mesh,
                 const glm::vec3& pos, FaceDirection direction,
                 uint8_t blockID, const std::string& blockName,
                 const GenerationOptions& options);

    /**
     * @brief 检查相邻方块是否遮挡
     */
    static bool isBlockSolid(const LevelChunk& chunk, int x, int y, int z);

    /**
     * @brief 计算面的顶点
     */
    static std::array<glm::vec3, 4> getFaceVertices(const glm::vec3& pos, FaceDirection direction);

    /**
     * @brief 添加四边形（两个三角形）
     */
    void addQuad(ChunkMeshComponent::MeshData& mesh,
                const std::array<glm::vec3, 4>& vertices,
                const glm::vec3& normal,
                const std::array<glm::vec2, 4>& uvs,
                uint8_t blockID);

    /**
     * @brief 将面方向转换为 BlockTextureManager 的 FaceDirection
     */
    static PrismaEngine::Graphic::BlockTextureManager::FaceDirection toTextureManagerFaceDirection(FaceDirection direction);

private:
    /**
     * @brief 方块纹理管理器（不持有所有权）
     */
    PrismaEngine::Graphic::BlockTextureManager* blockTextureManager_ = nullptr;
};

} // namespace PrismaCraft
