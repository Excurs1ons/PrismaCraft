# Native-C++ 分支的构建配置

# 收集头文件
file(GLOB_RECURSE PRISMACRAFT_NATIVE_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
)

# 创建接口库（头文件Only）
add_library(PrismaCraftNative INTERFACE)

# 包含目录
target_include_directories(PrismaCraftNative INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/../shared/include
)

# 链接 PrismaEngine
target_link_libraries(PrismaCraftNative INTERFACE PrismaEngine::Engine)
