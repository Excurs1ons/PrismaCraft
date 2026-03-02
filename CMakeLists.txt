cmake_minimum_required(VERSION 3.20)
project(PrismaCraft VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 查找 PrismaEngine
# 开发模式：使用 PrismaEngine 构建目录
# SDK 模式：使用已安装的 SDK
if(DEFINED PrismaEngine_DIR)
    set(PRISMA_ENGINE_DIR "${PrismaEngine_DIR}")
elseif(DEFINED ENV{PrismaEngine_DIR})
    set(PRISMA_ENGINE_DIR "$ENV{PrismaEngine_DIR}")
else()
    # 默认：使用相邻的 PrismaEngine 构建目录
    set(PRISMA_ENGINE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../PrismaEngine/build/linux-x64-debug")
endif()

message(STATUS "PrismaEngine 目录: ${PRISMA_ENGINE_DIR}")
find_package(PrismaEngine REQUIRED PATHS "${PRISMA_ENGINE_DIR}" NO_DEFAULT_PATH)

# 选择分支
option(PRISMACRAFT_USE_MONO "使用 Mono C# 分支" OFF)

if(PRISMACRAFT_USE_MONO)
    add_subdirectory(branches/mono-csharp)
    message(STATUS "使用 mono-csharp 分支")
else()
    add_subdirectory(branches/native-cpp)
    message(STATUS "使用 native-cpp 分支")
endif()

# 共享头文件目录
set(PRISMACRAFT_SHARED_DIR "${CMAKE_CURRENT_SOURCE_DIR}/shared/include")
include_directories("${PRISMACRAFT_SHARED_DIR}")

# 游戏可执行文件
add_executable(PrismaCraft
    src/main.cpp
)

# 链接 PrismaEngine
target_link_libraries(PrismaCraft PRIVATE PrismaEngine::Engine)

# 包含目录
target_include_directories(PrismaCraft PRIVATE
    ${PRISMAENGINE_INCLUDE_DIR}
    ${PRISMACRAFT_SHARED_DIR}
)

# 设置输出目录
set_target_properties(PrismaCraft PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# 安装规则
install(TARGETS PrismaCraft
    RUNTIME DESTINATION bin
)
