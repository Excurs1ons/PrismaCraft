# PrismaCraft 项目说明

## 项目概述
基于 PrismaEngine 的 Minecraft Java Edition 复刻项目。

## 技术栈
- **引擎**: PrismaEngine (Vulkan 渲染)
- **语言**: C++20
- **脚本**: C# (mono-csharp 分支，可选)
- **构建**: CMake 3.20+

## 目录结构
```
PrismaCraft/
├── src/                      # 主程序
│   └── main.cpp             # 入口点
├── branches/                 # 分支实现
│   ├── native-cpp/          # 原生 C++ 分支
│   │   ├── include/         # 公共头文件
│   │   └── src/             # 源文件
│   └── mono-csharp/         # C# 脚本分支
│       ├── src/core/bridge/ # C++ ↔ C# 桥接
│       └── src/scripts/     # C# 脚本
├── shared/                   # 共享代码
│   └── include/PrismaCraft/
│       ├── Core/            # 核心（BlockPos, Block 等）
│       └── NBT/            # NBT 格式
└── docs/                    # 文档
```

## 关键类映射

| PrismaCraft 类 | Minecraft 类 | 说明 |
|----------------|--------------|------|
| `BlockPos` | `net.minecraft.core.BlockPos` | 方块坐标 |
| `BlockState` | `net.minecraft.world.level.block.state.BlockState` | 方块状态 |
| `LevelChunk` | `net.minecraft.world.level.chunk.LevelChunk` | 区块 |
| `Entity` | `net.minecraft.world.entity.Entity` | 实体 |

## 开发流程

### 1. 环境设置
```bash
# 设置 PrismaEngine 路径
export PrismaEngine_DIR=/path/to/prismaengine/build/linux-x64-debug

# 或使用 SDK
export PrismaEngine_DIR=/opt/PrismaEngine-SDK
```

### 2. 构建项目
```bash
mkdir build && cd build
cmake ..
cmake --build .
```

### 3. 运行游戏
```bash
./bin/PrismaCraft
```

## 分支选择

### native-cpp（推荐新手）
- 纯 C++ 实现
- 高性能
- 简单直接

### mono-csharp（高级用户）
- C# 脚本支持
- 游戏逻辑可用 C# 编写
- 支持热重载（TODO）

## 下一步开发
1. ✅ 创建构建系统
2. ⏳ 实现区块加载
3. ⏳ 实现方块渲染
4. ⏳ 实现实体系统
5. ⏳ 实现世界生成
