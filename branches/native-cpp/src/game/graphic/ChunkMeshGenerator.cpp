#include "ChunkMeshGenerator.h"
#include <algorithm>

namespace PrismaCraft {

// ============================================================================
// ChunkMeshGenerator 实现
// ============================================================================

ChunkMeshComponent::MeshData ChunkMeshGenerator::generateMesh(const Level* level, const LevelChunk& chunk, const GenerationOptions& options) {
    ChunkMeshComponent::MeshData meshData;

    // 遍历所有区块切片
    for (int sectionIndex = 0; sectionIndex < LevelChunk::SECTION_COUNT; ++sectionIndex) {
        const ChunkSection* section = chunk.getSection(sectionIndex);
        if (!section || section->isEmpty()) {
            continue;
        }

        // 生成该切片的网格
        ChunkMeshComponent::MeshData sectionMesh = generateSectionMesh(level, chunk, *section, sectionIndex * 16, options);

        // 合并到总网格
        meshData.vertices.insert(meshData.vertices.end(),
                                 sectionMesh.vertices.begin(),
                                 sectionMesh.vertices.end());
        meshData.indices.insert(meshData.indices.end(),
                                sectionMesh.indices.begin(),
                                sectionMesh.indices.end());
    }

    return meshData;
}

ChunkMeshComponent::MeshData ChunkMeshGenerator::generateSectionMesh(const Level* level, const LevelChunk& chunk, const ChunkSection& section, int chunkYOffset, const GenerationOptions& options) {
    ChunkMeshComponent::MeshData meshData;

    // 计算区块的世界坐标
    int chunkWorldX = chunk.getPos().x * 16;
    int chunkWorldZ = chunk.getPos().z * 16;

    // 遍历切片中的所有方块
    for (int y = 0; y < ChunkSection::SECTION_HEIGHT; ++y) {
        for (int z = 0; z < ChunkSection::SECTION_DEPTH; ++z) {
            for (int x = 0; x < ChunkSection::SECTION_WIDTH; ++x) {
                const BlockState& blockState = section.getBlockState(x, y, z);

                // 跳过空气方块
                if (blockState.isAir()) {
                    continue;
                }

                // 计算世界坐标
                int worldX = chunkWorldX + x;
                int worldY = y + chunkYOffset;
                int worldZ = chunkWorldZ + z;
                glm::vec3 blockPos(worldX, worldY, worldZ);
                uint8_t blockID = static_cast<uint8_t>(blockState.getBlock()->getId());

                // 获取方块名称（用于纹理查找）
                const std::string& blockName = blockState.getBlock()->getName();

                // 检查并添加每个面
                for (int face = 0; face < 6; ++face) {
                    FaceDirection direction = static_cast<FaceDirection>(face);

                    // 如果启用面剔除，检查是否可见
                    if (options.enableFaceCulling && !isFaceVisible(level, worldX, worldY, worldZ, direction)) {
                        continue;
                    }

                    addFace(meshData, blockPos, direction, blockID, blockName, options);
                }
            }
        }
    }

    return meshData;
}

bool ChunkMeshGenerator::isFaceVisible(const Level* level, int x, int y, int z, FaceDirection direction) {
    // 计算相邻方块的世界坐标
    int nx = x, ny = y, nz = z;

    switch (direction) {
        case FaceDirection::TOP:    ny = y + 1; break;
        case FaceDirection::BOTTOM: ny = y - 1; break;
        case FaceDirection::NORTH:  nz = z - 1; break;
        case FaceDirection::SOUTH:  nz = z + 1; break;
        case FaceDirection::WEST:   nx = x - 1; break;
        case FaceDirection::EAST:   nx = x + 1; break;
    }

    // 通过 Level 查询相邻方块（自动处理跨区块边界）
    const BlockState& neighbor = level->getBlockState(BlockPos(nx, ny, nz));

    // 如果相邻方块是空气 OR 不阻挡运动，则当前面可见
    // 修复原 Bug：原逻辑是 && 应该改为 ||
    return neighbor.isAir() || !neighbor.blocksMotion();
}

PrismaEngine::Graphic::BlockTextureManager::FaceDirection ChunkMeshGenerator::toTextureManagerFaceDirection(FaceDirection direction) {
    switch (direction) {
        case FaceDirection::TOP:    return PrismaEngine::Graphic::BlockTextureManager::FaceDirection::TOP;
        case FaceDirection::BOTTOM: return PrismaEngine::Graphic::BlockTextureManager::FaceDirection::BOTTOM;
        case FaceDirection::NORTH:  return PrismaEngine::Graphic::BlockTextureManager::FaceDirection::NORTH;
        case FaceDirection::SOUTH:  return PrismaEngine::Graphic::BlockTextureManager::FaceDirection::SOUTH;
        case FaceDirection::WEST:   return PrismaEngine::Graphic::BlockTextureManager::FaceDirection::WEST;
        case FaceDirection::EAST:   return PrismaEngine::Graphic::BlockTextureManager::FaceDirection::EAST;
    }
    return PrismaEngine::Graphic::BlockTextureManager::FaceDirection::TOP;
}

glm::vec3 ChunkMeshGenerator::getFaceNormal(FaceDirection direction) {
    switch (direction) {
        case FaceDirection::TOP:    return glm::vec3(0, 1, 0);
        case FaceDirection::BOTTOM: return glm::vec3(0, -1, 0);
        case FaceDirection::NORTH:  return glm::vec3(0, 0, -1);
        case FaceDirection::SOUTH:  return glm::vec3(0, 0, 1);
        case FaceDirection::WEST:   return glm::vec3(-1, 0, 0);
        case FaceDirection::EAST:   return glm::vec3(1, 0, 0);
    }
    return glm::vec3(0, 1, 0);
}

std::array<glm::vec2, 4> ChunkMeshGenerator::getFaceUVs(FaceDirection direction, const std::string& blockName) const {
    // 如果没有纹理管理器，返回默认 UV
    if (!blockTextureManager_) {
        return {
            glm::vec2(0, 0),
            glm::vec2(1, 0),
            glm::vec2(1, 1),
            glm::vec2(0, 1)
        };
    }

    // 从纹理管理器获取该方块面的 UV 坐标
    glm::vec4 uv = blockTextureManager_->getFaceUV(blockName, toTextureManagerFaceDirection(direction));

    // 返回四个顶点的 UV 坐标（对应顶点顺序：左上、右上、右下、左下）
    return {
        glm::vec2(uv.x, uv.y),  // 左上 (u0, v0)
        glm::vec2(uv.z, uv.y),  // 右上 (u1, v0)
        glm::vec2(uv.z, uv.w),  // 右下 (u1, v1)
        glm::vec2(uv.x, uv.w)   // 左下 (u0, v1)
    };
}

void ChunkMeshGenerator::addFace(ChunkMeshComponent::MeshData& mesh,
                                 const glm::vec3& pos, FaceDirection direction,
                                 uint8_t blockID, const std::string& blockName,
                                 const GenerationOptions& options) {
    // 获取面的顶点和 UV
    auto vertices = getFaceVertices(pos, direction);
    auto normal = getFaceNormal(direction);
    auto uvs = getFaceUVs(direction, blockName);  // 使用新方法

    // 添加四边形
    addQuad(mesh, vertices, normal, uvs, blockID);
}

std::array<glm::vec3, 4> ChunkMeshGenerator::getFaceVertices(const glm::vec3& pos, FaceDirection direction) {
    // 方块的 8 个角点
    glm::vec3 min = pos;
    glm::vec3 max = pos + glm::vec3(1.0f);

    switch (direction) {
        case FaceDirection::TOP:
            return {
                glm::vec3(min.x, max.y, max.z),  // 左上后
                glm::vec3(max.x, max.y, max.z),  // 右上后
                glm::vec3(max.x, max.y, min.z),  // 右上前
                glm::vec3(min.x, max.y, min.z)   // 左上前
            };
        case FaceDirection::BOTTOM:
            return {
                glm::vec3(min.x, min.y, min.z),  // 左下前
                glm::vec3(max.x, min.y, min.z),  // 右下前
                glm::vec3(max.x, min.y, max.z),  // 右下后
                glm::vec3(min.x, min.y, max.z)   // 左下后
            };
        case FaceDirection::NORTH:
            return {
                glm::vec3(min.x, max.y, min.z),  // 左上前
                glm::vec3(max.x, max.y, min.z),  // 右上前
                glm::vec3(max.x, min.y, min.z),  // 右下前
                glm::vec3(min.x, min.y, min.z)   // 左下前
            };
        case FaceDirection::SOUTH:
            return {
                glm::vec3(max.x, max.y, max.z),  // 右上后
                glm::vec3(min.x, max.y, max.z),  // 左上后
                glm::vec3(min.x, min.y, max.z),  // 左下后
                glm::vec3(max.x, min.y, max.z)   // 右下后
            };
        case FaceDirection::WEST:
            return {
                glm::vec3(min.x, max.y, max.z),  // 左上后
                glm::vec3(min.x, max.y, min.z),  // 左上前
                glm::vec3(min.x, min.y, min.z),  // 左下前
                glm::vec3(min.x, min.y, max.z)   // 左下后
            };
        case FaceDirection::EAST:
            return {
                glm::vec3(max.x, max.y, min.z),  // 右上前
                glm::vec3(max.x, max.y, max.z),  // 右上后
                glm::vec3(max.x, min.y, max.z),  // 右下后
                glm::vec3(max.x, min.y, min.z)   // 右下前
            };
    }

    return {};
}

void ChunkMeshGenerator::addQuad(ChunkMeshComponent::MeshData& mesh,
                                const std::array<glm::vec3, 4>& vertices,
                                const glm::vec3& normal,
                                const std::array<glm::vec2, 4>& uvs,
                                uint8_t blockID) {
    uint32_t baseIndex = mesh.vertices.size();

    // 添加 4 个顶点
    for (int i = 0; i < 4; ++i) {
        ChunkMeshComponent::Vertex vertex;
        vertex.position = vertices[i];
        vertex.normal = normal;
        vertex.uv = uvs[i];
        vertex.blockID = blockID;
        mesh.vertices.push_back(vertex);
    }

    // 添加索引（两个三角形，顺时针）
    mesh.indices.push_back(baseIndex + 0);
    mesh.indices.push_back(baseIndex + 1);
    mesh.indices.push_back(baseIndex + 2);

    mesh.indices.push_back(baseIndex + 0);
    mesh.indices.push_back(baseIndex + 2);
    mesh.indices.push_back(baseIndex + 3);
}

} // namespace PrismaCraft
