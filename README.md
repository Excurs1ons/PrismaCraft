# PrismaCraft

![License](https://img.shields.io/badge/license-MIT-blue.svg)
![C++20](https://img.shields.io/badge/C%2B%2B-20-blue.svg)
![CMake](https://img.shields.io/badge/CMake-3.20%2B-blue.svg)
![Vulkan](https://img.shields.io/badge/Vulkan-1.3%2B-purple.svg)
![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20Windows%20%7C%20Android-lightgrey.svg)

A Minecraft recreation powered by PrismaEngine

## 快速开始

### 环境要求
- CMake 3.20+
- C++20 编译器
- PrismaEngine SDK 或构建目录
- Vulkan 驱动（推荐）

### 构建和运行

#### 使用 PrismaEngine 源码开发
```bash
# 假设 PrismaEngine 和 PrismaCraft 在同一目录
cd /path/to/repos

# 构建 PrismaEngine
cd PrismaEngine
./scripts/build-linux.sh linux-x64-debug

# 构建 PrismaCraft
cd ../PrismaCraft
mkdir build && cd build
cmake .. -DPrismaEngine_DIR=../PrismaEngine/build/linux-x64-debug
cmake --build .
./bin/PrismaCraft
```

#### 使用 PrismaEngine SDK
```bash
# 假设 SDK 已安装到 /opt/PrismaEngine-SDK
cd /path/to/PrismaCraft
mkdir build && cd build
cmake .. -DPrismaEngine_DIR=/opt/PrismaEngine-SDK
cmake --build .
./bin/PrismaCraft
```

## 分支说明

### native-cpp
原生 C++ 实现，高性能，无脚本开销。

**启用方式**：
```bash
cmake .. -DPRISMACRAFT_USE_MONO=OFF
```

### mono-csharp
使用 C# 脚本开发游戏逻辑，支持热重载。

**启用方式**：
```bash
cmake .. -DPRISMACRAFT_USE_MONO=ON
```

**安装 Mono**（Ubuntu/Debian）：
```bash
sudo apt install mono-devel
```

## 项目结构
```
PrismaCraft/
├── src/                    # 主程序源码
├── branches/              # 分支实现
│   ├── native-cpp/       # 原生 C++ 实现
│   └── mono-csharp/      # C# 脚本实现
├── shared/               # 共享头文件
│   └── include/PrismaCraft/
│       ├── Core/        # 核心数据结构
│       └── NBT/        # NBT 格式支持
└── docs/                # 文档
```

## 开发指南

### 添加新的方块类型
1. 在 `shared/include/PrismaCraft/Core/Block.h` 中定义
2. 在 `Registry.h` 中注册
3. 在游戏逻辑中使用

### 渲染自定义方块
参考 PrismaEngine 的渲染系统文档。

## 架构设计

本项目严格遵循 Minecraft Java Edition 的真实架构：

| PrismaCraft 类 | Minecraft 类 | 说明 |
|----------------|--------------|------|
| `BlockPos` | `net.minecraft.core.BlockPos` | 方块坐标 |
| `BlockState` | `net.minecraft.world.level.block.state.BlockState` | 方块状态 |
| `LevelChunk` | `net.minecraft.world.level.chunk.LevelChunk` | 区块 |
| `Entity` | `net.minecraft.world.entity.Entity` | 实体 |
| `Registry<T>` | `net.minecraft.core.Registry` | 注册表 |

## 技术栈

- **引擎**: PrismaEngine (Vulkan 渲染)
- **语言**: C++20
- **脚本**: C# (mono-csharp 分支，可选)
- **构建**: CMake 3.20+
- **数据格式**: NBT (Minecraft 原生格式)

## 许可证
MIT License

## 贡献
欢迎贡献！请查看贡献指南。
