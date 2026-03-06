#include "BlockRenderer.h"
#include <PrismaEngine/graphic/interfaces/IRenderDevice.h>
#include <PrismaEngine/graphic/interfaces/IResourceFactory.h>
#include <PrismaEngine/graphic/interfaces/ICommandBuffer.h>
#include <PrismaEngine/graphic/interfaces/IPipelineState.h>
#include <PrismaEngine/graphic/interfaces/IBuffer.h>
#include <PrismaEngine/graphic/ShaderFactory.h>
#include <PrismaEngine/graphic/RenderTypes.h>
#include <PrismaEngine/graphic/ICamera.h>
#include <PrismaEngine/Logger.h>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

namespace PrismaCraft {

// ============================================================================
// BlockRenderer 实现
// ============================================================================

BlockRenderer::BlockRenderer() = default;

BlockRenderer::~BlockRenderer() {
    shutdown();
}

bool BlockRenderer::initialize(PrismaEngine::Graphic::IRenderDevice* device,
                               PrismaEngine::Graphic::BlockTextureManager* textureMgr) {
    if (!device) {
        return false;
    }

    renderDevice = device;
    textureManager_ = textureMgr;

    auto* factory = renderDevice->GetResourceFactory();
    if (!factory) {
        return false;
    }

    // 确定后端类型
    PrismaEngine::Graphic::RenderAPIType apiType = PrismaEngine::Graphic::RenderAPIType::Vulkan;
    std::string apiName = device->GetAPIName();
    if (apiName == "DirectX 12") {
        apiType = PrismaEngine::Graphic::RenderAPIType::DirectX12;
    } else if (apiName == "Vulkan") {
        apiType = PrismaEngine::Graphic::RenderAPIType::Vulkan;
    } else if (apiName == "OpenGL") {
        apiType = PrismaEngine::Graphic::RenderAPIType::OpenGL;
    }

    // 加载着色器
    // 注意：着色器路径相对于运行目录
    std::string vsPath = "shaders/block.vert";
    std::string psPath = "shaders/block.frag";

    PrismaEngine::Graphic::ShaderDesc vsDesc;
    vsDesc.type = PrismaEngine::Graphic::ShaderType::Vertex;
    vsDesc.name = "BlockVS";

    PrismaEngine::Graphic::ShaderDesc psDesc;
    psDesc.type = PrismaEngine::Graphic::ShaderType::Pixel;
    psDesc.name = "BlockPS";

    auto vs = PrismaEngine::Graphic::ShaderFactory::CreateShaderFromFile(apiType, vsPath, vsDesc);
    auto ps = PrismaEngine::Graphic::ShaderFactory::CreateShaderFromFile(apiType, psPath, psDesc);

    if (!vs || !ps) {
        // 尝试备选路径
        vsPath = "branches/native-cpp/src/game/shaders/block.vert";
        psPath = "branches/native-cpp/src/game/shaders/block.frag";
        vs = PrismaEngine::Graphic::ShaderFactory::CreateShaderFromFile(apiType, vsPath, vsDesc);
        ps = PrismaEngine::Graphic::ShaderFactory::CreateShaderFromFile(apiType, psPath, psDesc);

        if (!vs || !ps) {
             LOG_ERROR("BlockRenderer", "无法加载方块着色器");
             return false;
        }
    }

    // 创建管线状态
    pipelineState = factory->CreatePipelineStateImpl();
    if (!pipelineState) {
        return false;
    }

    pipelineState->SetShader(PrismaEngine::Graphic::ShaderType::Vertex, std::move(vs));
    pipelineState->SetShader(PrismaEngine::Graphic::ShaderType::Pixel, std::move(ps));

    // 顶点布局
    std::vector<PrismaEngine::Graphic::VertexInputAttribute> attributes = {
        {"POSITION", 0, PrismaEngine::Graphic::TextureFormat::RGB32_Float, 0, offsetof(ChunkMeshComponent::Vertex, position), 0, 0},
        {"NORMAL", 0, PrismaEngine::Graphic::TextureFormat::RGB32_Float, 0, offsetof(ChunkMeshComponent::Vertex, normal), 0, 0},
        {"TEXCOORD", 0, PrismaEngine::Graphic::TextureFormat::RG32_Float, 0, offsetof(ChunkMeshComponent::Vertex, uv), 0, 0},
        {"BLOCKID", 0, PrismaEngine::Graphic::TextureFormat::R8_UInt, 0, offsetof(ChunkMeshComponent::Vertex, blockID), 0, 0}
    };
    pipelineState->SetInputLayout(attributes);
    pipelineState->SetPrimitiveTopology(PrismaEngine::Graphic::PrimitiveTopology::TriangleList);

    // 渲染目标格式
    pipelineState->SetRenderTargetFormat(0, PrismaEngine::Graphic::TextureFormat::RGBA8_UNorm);
    pipelineState->SetDepthStencilFormat(PrismaEngine::Graphic::TextureFormat::D32_Float);

    // 创建 PSO
    if (!pipelineState->Create(renderDevice)) {
        LOG_ERROR("BlockRenderer", "无法创建管线状态: {0}", pipelineState->GetErrors());
        return false;
    }

    // 创建 UBO 缓冲
    PrismaEngine::Graphic::BufferDesc uboDesc;
    uboDesc.type = PrismaEngine::Graphic::BufferType::Constant;
    uboDesc.size = sizeof(UniformBufferObject);
    uboDesc.usage = PrismaEngine::Graphic::BufferUsage::Dynamic;

    uboBuffer = factory->CreateBufferImpl(uboDesc);

    LOG_INFO("BlockRenderer", "初始化完成");
    return uboBuffer != nullptr;
}

void BlockRenderer::shutdown() {
    clear();
    renderDevice = nullptr;
    textureManager_ = nullptr;
    pipelineState.reset();
    uboBuffer.reset();
}

void BlockRenderer::updateChunkMesh(const glm::ivec2& chunkPos,
                                   const ChunkMeshComponent::MeshData& meshData) {
    if (!renderDevice) {
        return;
    }

    // 计算区块哈希
    uint64_t hash = hashChunkPos(chunkPos);
    
    // 如果网格为空，移除该区块
    if (meshData.vertices.empty() || meshData.indices.empty()) {
        removeChunkMesh(chunkPos);
        return;
    }

    auto& renderData = chunkRenderData[hash];
    renderData.chunkPos = chunkPos;

    // 获取资源工厂
    auto* factory = renderDevice->GetResourceFactory();
    if (!factory) {
        return;
    }

    // 创建或更新顶点缓冲区
    size_t vertexBufferSize = meshData.vertices.size() * sizeof(ChunkMeshComponent::Vertex);
    if (!renderData.vertexBuffer || renderData.vertexBuffer->GetSize() < vertexBufferSize) {
        PrismaEngine::Graphic::BufferDesc vbDesc;
        vbDesc.type = PrismaEngine::Graphic::BufferType::Vertex;
        vbDesc.size = vertexBufferSize;
        vbDesc.usage = PrismaEngine::Graphic::BufferUsage::Dynamic;
        vbDesc.stride = sizeof(ChunkMeshComponent::Vertex);

        renderData.vertexBuffer = factory->CreateBufferImpl(vbDesc);
    }

    // 上传顶点数据
    if (renderData.vertexBuffer) {
        renderData.vertexBuffer->UpdateData(meshData.vertices.data(), vertexBufferSize);
    }

    // 创建或更新索引缓冲区
    size_t indexBufferSize = meshData.indices.size() * sizeof(uint32_t);
    if (!renderData.indexBuffer || renderData.indexBuffer->GetSize() < indexBufferSize) {
        PrismaEngine::Graphic::BufferDesc ibDesc;
        ibDesc.type = PrismaEngine::Graphic::BufferType::Index;
        ibDesc.size = indexBufferSize;
        ibDesc.usage = PrismaEngine::Graphic::BufferUsage::Dynamic;
        ibDesc.stride = sizeof(uint32_t);

        renderData.indexBuffer = factory->CreateBufferImpl(ibDesc);
    }

    // 上传索引数据
    if (renderData.indexBuffer) {
        renderData.indexBuffer->UpdateData(meshData.indices.data(), indexBufferSize);
        renderData.indexCount = static_cast<uint32_t>(meshData.indices.size());
    }

    renderData.dirty = false;
}

void BlockRenderer::updateChunkBuffers(const glm::ivec2& chunkPos, ChunkRenderData& renderData) {
    // 此方法已在 updateChunkMesh 中实现
    renderData.dirty = false;
}

void BlockRenderer::removeChunkMesh(const glm::ivec2& chunkPos) {
    uint64_t hash = hashChunkPos(chunkPos);
    chunkRenderData.erase(hash);
}

void BlockRenderer::renderChunks(PrismaEngine::Graphic::ICommandBuffer* commandBuffer,
                                 const PrismaEngine::ICamera& camera) {
    if (!commandBuffer || !renderDevice || !pipelineState || !uboBuffer) {
        return;
    }

    // 重置统计
    stats = RenderStats{};
    
    // 绑定管线状态
    commandBuffer->SetPipelineState(pipelineState.get());

    // 绑定纹理图集
    if (textureManager_) {
        commandBuffer->SetTexture(textureManager_->getAtlasTexture(), 1);
    }

    // 准备 UBO
    UniformBufferObject ubo;
    ubo.view = camera.GetViewMatrix();
    ubo.proj = camera.GetProjectionMatrix();

    // 渲染每个区块
    for (auto& [hash, renderData] : chunkRenderData) {
        if (renderData.indexCount == 0 || !renderData.vertexBuffer || !renderData.indexBuffer) {
            continue;
        }

        // 更新模型矩阵
        ubo.model = glm::translate(glm::mat4(1.0f),
            glm::vec3(renderData.chunkPos.x * 16, 0, renderData.chunkPos.y * 16));

        // 更新 UBO 并绑定
        commandBuffer->UpdateBuffer(uboBuffer.get(), &ubo, sizeof(ubo));
        commandBuffer->SetConstantBuffer(uboBuffer.get(), 0);

        // 绑定缓冲区并绘制
        commandBuffer->SetVertexBuffer(renderData.vertexBuffer.get(), 0, 0, sizeof(ChunkMeshComponent::Vertex));
        commandBuffer->SetIndexBuffer(renderData.indexBuffer.get(), true);
        commandBuffer->DrawIndexed(renderData.indexCount);

        stats.visibleChunks++;
        stats.totalVertices += static_cast<uint32_t>(renderData.vertexBuffer->GetSize() / sizeof(ChunkMeshComponent::Vertex));
        stats.totalIndices += renderData.indexCount;
    }
}

void BlockRenderer::clear() {
    chunkRenderData.clear();
    stats = RenderStats{};
}

} // namespace PrismaCraft
