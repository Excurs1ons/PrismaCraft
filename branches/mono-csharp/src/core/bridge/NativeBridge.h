#pragma once

#include "../../../../../../shared/include/PrismaCraft/Core/BlockPos.h"
#include "../../../../../../shared/include/PrismaCraft/Core/BlockState.h"
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/mono-config.h>
#include <string>
#include <unordered_map>
#include <functional>

namespace PrismaCraft {
    namespace Mono {

        // 前向声明
        class MonoRuntime;

        /**
         * @brief C# 到 C++ 桥接层
         *
         * 这个模块提供了 C# 脚本与 C++ 引擎之间的互操作接口。
         * 对应 Minecraft 的脚本系统，但使用 Mono 运行时。
         */
        class NativeBridge {
        public:
            /**
             * @brief 初始化桥接层
             * 注册所有内部调用（Internal Call）
             */
            static void initialize(MonoRuntime* runtime);

            /**
             * @brief 关闭桥接层
             */
            static void shutdown();

            // ========== 世界管理桥接 ==========

            /**
             * @brief 获取方块状态 - C# 调用
             * 对应 C#: WorldManager.GetBlock(int x, int y, int z)
             */
            static void WorldManager_GetBlock(
                int32_t x, int32_t y, int32_t z,
                uint8_t* blockType,
                uint32_t* stateId
            );

            /**
             * @brief 设置方块状态 - C# 调用
             */
            static void WorldManager_SetBlock(
                int32_t x, int32_t y, int32_t z,
                uint8_t blockType,
                uint32_t stateId
            );

            /**
             * @brief 加载区块 - C# 调用
             */
            static void WorldManager_LoadChunk(int32_t chunkX, int32_t chunkZ);

            /**
             * @brief 卸载区块 - C# 调用
             */
            static void WorldManager_UnloadChunk(int32_t chunkX, int32_t chunkZ);

            // ========== 实体管理桥接 ==========

            /**
             * @brief 创建玩家实体 - C# 调用
             */
            static uint32_t EntityManager_CreatePlayer(const char* name);

            /**
             * @brief 获取实体位置 - C# 调用
             */
            static void Entity_GetPosition(uint32_t entityId, float* x, float* y, float* z);

            /**
             * @brief 设置实体位置 - C# 调用
             */
            static void Entity_SetPosition(uint32_t entityId, float x, float y, float z);

            /**
             * @brief 获取实体旋转 - C# 调用
             */
            static void Entity_GetRotation(uint32_t entityId, float* yaw, float* pitch);

            /**
             * @brief 设置实体旋转 - C# 调用
             */
            static void Entity_SetRotation(uint32_t entityId, float yaw, float pitch);

            // ========== 输入系统桥接 ==========

            /**
             * @brief 检查键是否按下 - C# 调用
             */
            static bool Input_IsKeyDown(int32_t keyCode);

            /**
             * @brief 检查键是否刚刚按下 - C# 调用
             */
            static bool Input_IsKeyPressed(int32_t keyCode);

            /**
             * @brief 检查键是否刚刚释放 - C# 调用
             */
            static bool Input_IsKeyReleased(int32_t keyCode);

            /**
             * @brief 获取鼠标位置 - C# 调用
             */
            static void Input_GetMousePosition(float* x, float* y);

            /**
             * @brief 检查鼠标按钮是否按下 - C# 调用
             */
            static bool Input_IsMouseButtonPressed(int32_t button);

            // ========== 时间系统桥接 ==========

            /**
             * @brief 获取 Delta Time - C# 调用
             */
            static float Time_GetDeltaTime();

            /**
             * @brief 获取游戏总时间 - C# 调用
             */
            static float Time_GetTime();

            // ========== 日志系统桥接 ==========

            /**
             * @brief 输出日志 - C# 调用
             */
            static void Debug_Log(const char* message);
            static void Debug_LogWarning(const char* message);
            static void Debug_LogError(const char* message);

        private:
            NativeBridge() = default;

            /**
             * @brief 注册内部调用
             */
            static void registerInternalCalls(MonoRuntime* runtime);

            /**
             * @brief 注册 WorldManager 相关内部调用
             */
            static void registerWorldManagerInternalCalls(MonoRuntime* runtime);

            /**
             * @brief 注册 EntityManager 相关内部调用
             */
            static void registerEntityManagerInternalCalls(MonoRuntime* runtime);

            /**
             * @brief 注册 Input 相关内部调用
             */
            static void registerInputInternalCalls(MonoRuntime* runtime);

            /**
             * @brief 注册 Time 相关内部调用
             */
            static void registerTimeInternalCalls(MonoRuntime* runtime);

            /**
             * @brief 注册 Debug 相关内部调用
             */
            static void registerDebugInternalCalls(MonoRuntime* runtime);

            static MonoRuntime* s_runtime;
        };

        /**
         * @brief C# 对象包装器
         *
         * 用于在 C++ 中持有和操作 C# 对象
         */
        class CSharpObject {
        public:
            CSharpObject() = default;

            /**
             * @brief 从 MonoObject 创建
             */
            explicit CSharpObject(MonoObject* obj) : m_object(obj) {}

            /**
             * @brief 获取底层的 MonoObject
             */
            MonoObject* get() const noexcept { return m_object; }

            /**
             * @brief 检查是否为空
             */
            bool isEmpty() const noexcept { return m_object == nullptr; }

            /**
             * @brief 调用 C# 方法
             */
            MonoObject* callMethod(const char* methodName, int paramCount, MonoObject** params);

            /**
             * @brief 获取字段值
             */
            MonoObject* getField(const char* fieldName);

            /**
             * @brief 设置字段值
             */
            void setField(const char* fieldName, MonoObject* value);

            /**
             * @brief 转换为特定类型
             */
            template<typename T>
            T* as() const {
                return static_cast<T*>(m_object);
            }

        protected:
            MonoObject* m_object = nullptr;
            MonoClass* m_class = nullptr;
        };

        /**
         * @brief C# 字符串包装器
         */
        class CSharpString : public CSharpObject {
        public:
            /**
             * @brief 从 std::string 创建 C# 字符串
             */
            static CSharpString fromUTF8(const std::string& str);

            /**
             * @brief 转换为 std::string
             */
            std::string toUTF8() const;

        private:
            CSharpString(MonoString* str) : CSharpString(reinterpret_cast<MonoObject*>(str)) {}
        };

        /**
         * @brief C# 数组包装器
         */
        template<typename T>
        class CSharpArray : public CSharpObject {
        public:
            CSharpArray() = default;

            /**
             * @brief 创建指定大小的数组
             */
            static CSharpArray create(size_t length);

            /**
             * @brief 从指针创建数组
             */
            static CSharpArray fromPointer(const T* data, size_t length);

            /**
             * @brief 获取数组长度
             */
            size_t length() const;

            /**
             * @brief 获取数组元素
             */
            T get(size_t index) const;

            /**
             * @brief 设置数组元素
             */
            void set(size_t index, T value);

            /**
             * @brief 转换为 std::vector
             */
            std::vector<T> toVector() const;
        };

        /**
         * @brief C# 托管字符串包装器（用于指针传递）
         *
         * 注意：这只是辅助类型，实际的字符串传递使用 MonoString
         */
        struct MonoStringWrapper {
            /**
             * @brief 从 C# 字符串获取 UTF-8 字符串
             */
            static std::string toUTF8(MonoString* monoString);

            /**
             * @brief 从 UTF-8 字符串创建 C# 字符串
             */
            static MonoString* fromUTF8(const std::string& str);

            /**
             * @brief 释放 C# 字符串（如果需要）
             */
            static void free(MonoString* monoString);
        };

    } // namespace Mono
} // namespace PrismaCraft
