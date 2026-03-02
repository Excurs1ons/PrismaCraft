#include <PrismaEngine/PrismaEngine.h>
#include <PrismaCraft/Core/BlockPos.h>
#include <PrismaCraft/Core/Registry.h>
#include <iostream>

using namespace PrismaEngine;

class PrismaCraftGame : public IApplication<PrismaCraftGame> {
public:
    bool Initialize() override {
        LOG_INFO("PrismaCraft", "初始化游戏");

        // 初始化平台
        if (!Platform::Initialize()) {
            LOG_ERROR("PrismaCraft", "平台初始化失败");
            return false;
        }

        // 创建窗口
        WindowProps props("PrismaCraft - Minecraft Recreation", 1280, 720);
        m_window = Platform::CreateWindow(props);

        if (!m_window) {
            LOG_ERROR("PrismaCraft", "窗口创建失败");
            return false;
        }

        // 初始化游戏系统
        InitializeGameSystems();

        LOG_INFO("PrismaCraft", "初始化完成");
        return true;
    }

    int Run() override {
        LOG_INFO("PrismaCraft", "开始游戏循环");

        auto renderSystem = Graphic::RenderSystem::GetInstance();

        if (!renderSystem->Initialize()) {
            LOG_ERROR("PrismaCraft", "渲染系统初始化失败");
            return 1;
        }

        SetRunning(true);

        while (IsRunning()) {
            Platform::PumpEvents();

            if (Platform::ShouldClose(m_window)) {
                SetRunning(false);
                break;
            }

            // 游戏更新
            UpdateGame();

            // 渲染一帧
            renderSystem->BeginFrame();

            RenderGame();

            renderSystem->EndFrame();
            renderSystem->Present();
        }

        LOG_INFO("PrismaCraft", "游戏循环结束");
        return 0;
    }

    void Shutdown() override {
        LOG_INFO("PrismaCraft", "关闭游戏");

        if (m_window) {
            Platform::DestroyWindow(m_window);
        }

        Platform::Shutdown();
    }

private:
    void InitializeGameSystems() {
        // TODO: 初始化游戏系统
        LOG_INFO("PrismaCraft", "初始化游戏系统");

        // 测试：创建方块位置
        PrismaCraft::BlockPos pos(10, 64, 10);
        LOG_INFO("PrismaCraft", "测试 BlockPos: ({}, {}, {})", pos.getX(), pos.getY(), pos.getZ());
    }

    void UpdateGame() {
        // TODO: 游戏逻辑更新
    }

    void RenderGame() {
        // TODO: 游戏渲染
    }

    WindowHandle m_window = nullptr;
};

int main(int argc, char* argv[]) {
    Logger::GetInstance().Initialize();

    LOG_INFO("Main", "PrismaCraft v0.1.0");
    LOG_INFO("Main", "基于 PrismaEngine");

    PrismaCraftGame game;
    if (!game.Initialize()) {
        LOG_FATAL("Main", "游戏初始化失败");
        return 1;
    }

    int exitCode = game.Run();
    game.Shutdown();

    return exitCode;
}
