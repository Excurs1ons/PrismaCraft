# Mono-C# 分支的构建配置

# 查找 Mono
find_package(Mono QUIET)

if(Mono_FOUND)
    message(STATUS "找到 Mono: ${Mono_VERSION}")

    # 收集 C++ 桥接代码
    file(GLOB_RECURSE PRISMACRAFT_BRIDGE_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/src/core/bridge/*.cpp"
    )

    file(GLOB_RECURSE PRISMACRAFT_BRIDGE_HEADERS
        "${CMAKE_CURRENT_SOURCE_DIR}/src/core/bridge/*.h"
    )

    # 创建桥接库
    add_library(PrismaCraftBridge STATIC
        ${PRISMACRAFT_BRIDGE_SOURCES}
        ${PRISMACRAFT_BRIDGE_HEADERS}
    )

    # 包含 Mono 头文件
    target_include_directories(PrismaCraftBridge PRIVATE
        ${Mono_INCLUDE_DIRS}
        ${CMAKE_CURRENT_SOURCE_DIR}/src/core/bridge
        ${CMAKE_CURRENT_SOURCE_DIR}/../shared/include
    )

    # 链接 Mono 和 PrismaEngine
    target_link_libraries(PrismaCraftBridge PUBLIC
        PrismaEngine::Engine
        ${Mono_LIBRARIES}
    )

    # C# 脚本目录
    set(PRISMACRAFT_SCRIPT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/scripts")

    # 复制 C# 脚本到输出目录
    file(COPY ${PRISMACRAFT_SCRIPT_DIR}/
         DESTINATION ${CMAKE_BINARY_DIR}/scripts
         FILES_MATCHING PATTERN "*.cs"
    )

    # 配置脚本加载路径
    target_compile_definitions(PrismaCraftBridge PRIVATE
        PRISMACRAFT_SCRIPT_PATH="${CMAKE_BINARY_DIR}/scripts"
    )
else()
    message(WARNING "Mono 未找到，mono-csharp 分支将不可用")
    message(WARNING "要启用 C# 脚本支持，请安装 Mono: sudo apt install mono-devel")

    # 创建空接口库
    add_library(PrismaCraftBridge INTERFACE)
endif()
