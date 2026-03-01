#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>

namespace PrismaCraft {
    namespace NBT {

        // 前向声明
        class TagReader;
        class TagWriter;

        /**
         * @brief NBT 标签类型 - 对应 Minecraft: net.minecraft.nbt.TagType
         *
         * NBT (Named Binary Tag) 是 Minecraft 使用的数据存储格式。
         */
        enum class TagType : uint8_t {
            END = 0,           // TAG_End
            BYTE = 1,          // TAG_Byte
            SHORT = 2,         // TAG_Short
            INT = 3,           // TAG_Int
            LONG = 4,          // TAG_Long
            FLOAT = 5,         // TAG_Float
            DOUBLE = 6,        // TAG_Double
            BYTE_ARRAY = 7,    // TAG_Byte_Array
            STRING = 8,        // TAG_String
            LIST = 9,          // TAG_List
            COMPOUND = 10,     // TAG_Compound
            INT_ARRAY = 11,    // TAG_Int_Array
            LONG_ARRAY = 12,   // TAG_Long_Array
            ANY_NUMERIC = 99   // 内部使用：任意数值类型
        };

        /**
         * @brief NBT 标签基类 - 对应 Minecraft: net.minecraft.nbt.Tag
         */
        class Tag {
        public:
            virtual ~Tag() = default;

            /**
             * @brief 获取标签类型
             */
            virtual TagType getId() const noexcept = 0;

            /**
             * @brief 深拷贝标签
             */
            virtual std::unique_ptr<Tag> copy() const = 0;

            /**
             * @brief 获取标签的字符串表示（用于调试）
             */
            virtual std::string toString() const = 0;

            /**
             * @brief 获取标签的简化字符串表示
             */
            virtual std::string toShortString() const {
                return toString();
            }

            /**
             * @brief 检查标签是否相等
             */
            virtual bool equals(const Tag& other) const = 0;

            /**
             * @brief 获取标签的哈希码
             */
            virtual size_t hashCode() const noexcept = 0;

            // ========== 类型检查便捷方法 ==========

            bool isByte() const noexcept { return getId() == TagType::BYTE; }
            bool isShort() const noexcept { return getId() == TagType::SHORT; }
            bool isInt() const noexcept { return getId() == TagType::INT; }
            bool isLong() const noexcept { return getId() == TagType::LONG; }
            bool isFloat() const noexcept { return getId() == TagType::FLOAT; }
            bool isDouble() const noexcept { return getId() == TagType::DOUBLE; }
            bool isString() const noexcept { return getId() == TagType::STRING; }
            bool isByteArray() const noexcept { return getId() == TagType::BYTE_ARRAY; }
            bool isIntArray() const noexcept { return getId() == TagType::INT_ARRAY; }
            bool isLongArray() const noexcept { return getId() == TagType::LONG_ARRAY; }
            bool isList() const noexcept { return getId() == TagType::LIST; }
            boolCompound() const noexcept { return getId() == TagType::COMPOUND; }

        protected:
            Tag() = default;
        };

        /**
         * @brief 字节标签 - 对应 Minecraft: net.minecraft.nbt.ByteTag
         */
        class ByteTag : public Tag {
        public:
            static constexpr TagType TYPE = TagType::BYTE;

            explicit ByteTag(int8_t data = 0) noexcept : m_data(data) {}

            TagType getId() const noexcept override { return TYPE; }

            std::unique_ptr<Tag> copy() const override {
                return std::make_unique<ByteTag>(m_data);
            }

            std::string toString() const override {
                return m_data >= 0 ? std::to_string(m_data) : "0x" + std::to_string(static_cast<uint8_t>(m_data));
            }

            bool equals(const Tag& other) const override {
                return other.getId() == TYPE &&
                       static_cast<const ByteTag&>(other).m_data == m_data;
            }

            size_t hashCode() const noexcept override {
                return std::hash<int8_t>{}(m_data);
            }

            int8_t getData() const noexcept { return m_data }
            void setData(int8_t data) noexcept { m_data = data; }

            /**
             * @brief 作为布尔值使用
             */
            bool getAsBoolean() const noexcept { return m_data != 0; }

        private:
            int8_t m_data;
        };

        /**
         * @brief 短整型标签 - 对应 Minecraft: net.minecraft.nbt.ShortTag
         */
        class ShortTag : public Tag {
        public:
            static constexpr TagType TYPE = TagType::SHORT;

            explicit ShortTag(int16_t data = 0) noexcept : m_data(data) {}

            TagType getId() const noexcept override { return TYPE; }

            std::unique_ptr<Tag> copy() const override {
                return std::make_unique<ShortTag>(m_data);
            }

            std::string toString() const override {
                return std::to_string(m_data) + "s";
            }

            bool equals(const Tag& other) const override {
                return other.getId() == TYPE &&
                       static_cast<const ShortTag&>(other).m_data == m_data;
            }

            size_t hashCode() const noexcept override {
                return std::hash<int16_t>{}(m_data);
            }

            int16_t getData() const noexcept { return m_data; }
            void setData(int16_t data) noexcept { m_data = data; }

        private:
            int16_t m_data;
        };

        /**
         * @brief 整型标签 - 对应 Minecraft: net.minecraft.nbt.IntTag
         */
        class IntTag : public Tag {
        public:
            static constexpr TagType TYPE = TagType::INT;

            explicit IntTag(int32_t data = 0) noexcept : m_data(data) {}

            TagType getId() const noexcept override { return TYPE; }

            std::unique_ptr<Tag> copy() const override {
                return std::make_unique<IntTag>(m_data);
            }

            std::string toString() const override {
                return std::to_string(m_data);
            }

            bool equals(const Tag& other) const override {
                return other.getId() == TYPE &&
                       static_cast<const IntTag&>(other).m_data == m_data;
            }

            size_t hashCode() const noexcept override {
                return std::hash<int32_t>{}(m_data);
            }

            int32_t getData() const noexcept { return m_data; }
            void setData(int32_t data) noexcept { m_data = data; }

        private:
            int32_t m_data;
        };

        /**
         * @brief 长整型标签 - 对应 Minecraft: net.minecraft.nbt.LongTag
         */
        class LongTag : public Tag {
        public:
            static constexpr TagType TYPE = TagType::LONG;

            explicit LongTag(int64_t data = 0) noexcept : m_data(data) {}

            TagType getId() const noexcept override { return TYPE; }

            std::unique_ptr<Tag> copy() const override {
                return std::make_unique<LongTag>(m_data);
            }

            std::string toString() const override {
                return std::to_string(m_data) + "L";
            }

            bool equals(const Tag& other) const override {
                return other.getId() == TYPE &&
                       static_cast<const LongTag&>(other).m_data == m_data;
            }

            size_t hashCode() const noexcept override {
                return std::hash<int64_t>{}(m_data);
            }

            int64_t getData() const noexcept { return m_data; }
            void setData(int64_t data) noexcept { m_data = data; }

        private:
            int64_t m_data;
        };

        /**
         * @brief 浮点标签 - 对应 Minecraft: net.minecraft.nbt.FloatTag
         */
        class FloatTag : public Tag {
        public:
            static constexpr TagType TYPE = TagType::FLOAT;

            explicit FloatTag(float data = 0.0f) noexcept : m_data(data) {}

            TagType getId() const noexcept override { return TYPE; }

            std::unique_ptr<Tag> copy() const override {
                return std::make_unique<FloatTag>(m_data);
            }

            std::string toString() const override {
                return std::to_string(m_data) + "f";
            }

            bool equals(const Tag& other) const override {
                return other.getId() == TYPE &&
                       static_cast<const FloatTag&>(other).m_data == m_data;
            }

            size_t hashCode() const noexcept override {
                return std::hash<float>{}(m_data);
            }

            float getData() const noexcept { return m_data; }
            void setData(float data) noexcept { m_data = data; }

        private:
            float m_data;
        };

        /**
         * @brief 双精度浮点标签 - 对应 Minecraft: net.minecraft.nbt.DoubleTag
         */
        class DoubleTag : public Tag {
        public:
            static constexpr TagType TYPE = TagType::DOUBLE;

            explicit DoubleTag(double data = 0.0) noexcept : m_data(data) {}

            TagType getId() const noexcept override { return TYPE; }

            std::unique_ptr<Tag> copy() const override {
                return std::make_unique<DoubleTag>(m_data);
            }

            std::string toString() const override {
                return std::to_string(m_data) + "d";
            }

            bool equals(const Tag& other) const override {
                return other.getId() == TYPE &&
                       static_cast<const DoubleTag&>(other).m_data == m_data;
            }

            size_t hashCode() const noexcept override {
                return std::hash<double>{}(m_data);
            }

            double getData() const noexcept { return m_data; }
            void setData(double data) noexcept { m_data = data; }

        private:
            double m_data;
        };

        /**
         * @brief 字符串标签 - 对应 Minecraft: net.minecraft.nbt.StringTag
         */
        class StringTag : public Tag {
        public:
            static constexpr TagType TYPE = TagType::STRING;

            explicit StringTag(std::string data = "") : m_data(std::move(data)) {}

            TagType getId() const noexcept override { return TYPE; }

            std::unique_ptr<Tag> copy() const override {
                return std::make_unique<StringTag>(m_data);
            }

            std::string toString() const override {
                return quoteAndEscape(m_data);
            }

            std::string toShortString() const override {
                return quoteAndEscape(m_data);
            }

            bool equals(const Tag& other) const override {
                return other.getId() == TYPE &&
                       static_cast<const StringTag&>(other).m_data == m_data;
            }

            size_t hashCode() const noexcept override {
                return std::hash<std::string>{}(m_data);
            }

            const std::string& getData() const noexcept { return m_data; }
            void setData(std::string data) { m_data = std::move(data); }

        private:
            std::string m_data;

            static std::string quoteAndEscape(const std::string& str) {
                // 简化版本：实际应该处理转义字符
                return "\"" + str + "\"";
            }
        };

        /**
         * @brief 字节数组标签 - 对应 Minecraft: net.minecraft.nbt.ByteArrayTag
         */
        class ByteArrayTag : public Tag {
        public:
            static constexpr TagType TYPE = TagType::BYTE_ARRAY;

            ByteArrayTag() = default;
            explicit ByteArrayTag(std::vector<int8_t> data) : m_data(std::move(data)) {}

            TagType getId() const noexcept override { return TYPE; }

            std::unique_ptr<Tag> copy() const override {
                return std::make_unique<ByteArrayTag>(m_data);
            }

            std::string toString() const override {
                return "[B;" + std::to_string(m_data.size()) + " bytes]";
            }

            bool equals(const Tag& other) const override {
                if (other.getId() != TYPE) return false;
                return static_cast<const ByteArrayTag&>(other).m_data == m_data;
            }

            size_t hashCode() const noexcept override {
                size_t hash = 0;
                for (auto b : m_data) {
                    hash = hash * 31 + std::hash<int8_t>{}(b);
                }
                return hash;
            }

            const std::vector<int8_t>& getData() const noexcept { return m_data; }
            std::vector<int8_t>& getData() noexcept { return m_data; }
            size_t size() const noexcept { return m_data.size(); }

        private:
            std::vector<int8_t> m_data;
        };

        /**
         * @brief 整型数组标签 - 对应 Minecraft: net.minecraft.nbt.IntArrayTag
         */
        class IntArrayTag : public Tag {
        public:
            static constexpr TagType TYPE = TagType::INT_ARRAY;

            IntArrayTag() = default;
            explicit IntArrayTag(std::vector<int32_t> data) : m_data(std::move(data)) {}

            TagType getId() const noexcept override { return TYPE; }

            std::unique_ptr<Tag> copy() const override {
                return std::make_unique<IntArrayTag>(m_data);
            }

            std::string toString() const override {
                return "[I;" + std::to_string(m_data.size()) + " ints]";
            }

            bool equals(const Tag& other) const override {
                if (other.getId() != TYPE) return false;
                return static_cast<const IntArrayTag&>(other).m_data == m_data;
            }

            size_t hashCode() const noexcept override {
                size_t hash = 0;
                for (auto i : m_data) {
                    hash = hash * 31 + std::hash<int32_t>{}(i);
                }
                return hash;
            }

            const std::vector<int32_t>& getData() const noexcept { return m_data; }
            std::vector<int32_t>& getData() noexcept { return m_data; }
            size_t size() const noexcept { return m_data.size(); }

        private:
            std::vector<int32_t> m_data;
        };

        /**
         * @brief 长整型数组标签 - 对应 Minecraft: net.minecraft.nbt.LongArrayTag
         */
        class LongArrayTag : public Tag {
        public:
            static constexpr TagType TYPE = TagType::LONG_ARRAY;

            LongArrayTag() = default;
            explicit LongArrayTag(std::vector<int64_t> data) : m_data(std::move(data)) {}

            TagType getId() const noexcept override { return TYPE; }

            std::unique_ptr<Tag> copy() const override {
                return std::make_unique<LongArrayTag>(m_data);
            }

            std::string toString() const override {
                return "[L;" + std::to_string(m_data.size()) + " longs]";
            }

            bool equals(const Tag& other) const override {
                if (other.getId() != TYPE) return false;
                return static_cast<const LongArrayTag&>(other).m_data == m_data;
            }

            size_t hashCode() const noexcept override {
                size_t hash = 0;
                for (auto l : m_data) {
                    hash = hash * 31 + std::hash<int64_t>{}(l);
                }
                return hash;
            }

            const std::vector<int64_t>& getData() const noexcept { return m_data; }
            std::vector<int64_t>& getData() noexcept { return m_data; }
            size_t size() const noexcept { return m_data.size(); }

        private:
            std::vector<int64_t> m_data;
        };

        /**
         * @brief 列表标签 - 对应 Minecraft: net.minecraft.nbt.ListTag
         *
         * 存储同类型的标签列表
         */
        class ListTag : public Tag {
        public:
            static constexpr TagType TYPE = TagType::LIST;

            ListTag() = default;

            TagType getId() const noexcept override { return TYPE; }

            std::unique_ptr<Tag> copy() const override {
                auto copy = std::make_unique<ListTag>();
                copy->m_listType = m_listType;
                for (const auto& tag : m_data) {
                    copy->m_data.push_back(tag->copy());
                }
                return copy;
            }

            std::string toString() const override {
                std::string result = "[";
                for (size_t i = 0; i < m_data.size(); ++i) {
                    if (i > 0) result += ",";
                    result += m_data[i]->toString();
                }
                result += "]";
                return result;
            }

            std::string toShortString() const override {
                return "[" + std::to_string(m_data.size()) + " entries]";
            }

            bool equals(const Tag& other) const override {
                if (other.getId() != TYPE) return false;
                const auto& otherList = static_cast<const ListTag&>(other);
                if (m_listType != otherList.m_listType) return false;
                if (m_data.size() != otherList.m_data.size()) return false;
                for (size_t i = 0; i < m_data.size(); ++i) {
                    if (!m_data[i]->equals(*otherList.m_data[i])) return false;
                }
                return true;
            }

            size_t hashCode() const noexcept override {
                size_t hash = std::hash<uint8_t>{}(static_cast<uint8_t>(m_listType));
                for (const auto& tag : m_data) {
                    hash = hash * 31 + tag->hashCode();
                }
                return hash;
            }

            // ========== 列表操作 ==========

            /**
             * @brief 获取列表元素类型
             */
            TagType getElementType() const noexcept { return m_listType; }

            /**
             * @brief 设置列表元素类型
             * 添加第一个元素时自动设置
             */
            void setElementType(TagType type) noexcept { m_listType = type; }

            /**
             * @brief 添加标签
             */
            void add(std::unique_ptr<Tag> tag) {
                if (m_data.empty()) {
                    m_listType = tag->getId();
                }
                m_data.push_back(std::move(tag));
            }

            /**
             * @brief 获取标签数量
             */
            size_t size() const noexcept { return m_data.size(); }

            /**
             * @brief 检查是否为空
             */
            bool isEmpty() const noexcept { return m_data.empty(); }

            /**
             * @brief 获取指定索引的标签
             */
            Tag* get(size_t index) noexcept {
                return index < m_data.size() ? m_data[index].get() : nullptr;
            }

            const Tag* get(size_t index) const noexcept {
                return index < m_data.size() ? m_data[index].get() : nullptr;
            }

            /**
             * @brief 获取指定类型的标签
             */
            template<typename T>
            T* get(size_t index) noexcept {
                return static_cast<T*>(get(index));
            }

            /**
             * @brief 清空列表
             */
            void clear() noexcept {
                m_data.clear();
                m_listType = TagType::END;
            }

        private:
            TagType m_listType = TagType::END;
            std::vector<std::unique_ptr<Tag>> m_data;
        };

        /**
         * @brief 复合标签 - 对应 Minecraft: net.minecraft.nbt.CompoundTag
         *
         * 最常用的标签类型，存储键值对
         */
        class CompoundTag : public Tag {
        public:
            static constexpr TagType TYPE = TagType::COMPOUND;

            CompoundTag() = default;

            TagType getId() const noexcept override { return TYPE; }

            std::unique_ptr<Tag> copy() const override {
                auto copy = std::make_unique<CompoundTag>();
                for (const auto& pair : m_data) {
                    copy->m_data[pair.first] = pair.second->copy();
                }
                return copy;
            }

            std::string toString() const override {
                std::string result = "{";
                bool first = true;
                for (const auto& pair : m_data) {
                    if (!first) result += ",";
                    first = false;
                    result += pair.first + ":" + pair.second->toString();
                }
                result += "}";
                return result;
            }

            std::string toShortString() const override {
                return "{" + std::to_string(m_data.size()) + " entries}";
            }

            bool equals(const Tag& other) const override {
                if (other.getId() != TYPE) return false;
                const auto& otherCompound = static_cast<const CompoundTag&>(other);
                if (m_data.size() != otherCompound.m_data.size()) return false;
                for (const auto& pair : m_data) {
                    auto it = otherCompound.m_data.find(pair.first);
                    if (it == otherCompound.m_data.end()) return false;
                    if (!pair.second->equals(*it->second)) return false;
                }
                return true;
            }

            size_t hashCode() const noexcept override {
                size_t hash = 0;
                for (const auto& pair : m_data) {
                    hash = hash * 31 + std::hash<std::string>{}(pair.first);
                    hash = hash * 31 + pair.second->hashCode();
                }
                return hash;
            }

            // ========== 键值对操作 ==========

            /**
             * @brief 检查是否包含指定键
             */
            bool contains(const std::string& key) const noexcept {
                return m_data.find(key) != m_data.end();
            }

            /**
             * @brief 获取所有键
             */
            std::vector<std::string> getAllKeys() const {
                std::vector<std::string> keys;
                keys.reserve(m_data.size());
                for (const auto& pair : m_data) {
                    keys.push_back(pair.first);
                }
                return keys;
            }

            /**
             * @brief 获取键值对数量
             */
            size_t size() const noexcept { return m_data.size(); }

            /**
             * @brief 检查是否为空
             */
            bool isEmpty() const noexcept { return m_data.empty(); }

            /**
             * @brief 移除指定键
             */
            bool remove(const std::string& key) {
                return m_data.erase(key) > 0;
            }

            /**
             * @brief 清空所有数据
             */
            void clear() noexcept {
                m_data.clear();
            }

            // ========== 类型安全的获取方法 ==========

            /**
             * @brief 获取字节值
             * 如果键不存在或类型不匹配，返回默认值
             */
            int8_t getByte(const std::string& key, int8_t defaultValue = 0) const noexcept {
                auto* tag = getTag(key);
                if (tag && tag->getId() == TagType::BYTE) {
                    return static_cast<const ByteTag*>(tag)->getData();
                }
                return defaultValue;
            }

            /**
             * @brief 获取短整型值
             */
            int16_t getShort(const std::string& key, int16_t defaultValue = 0) const noexcept {
                auto* tag = getTag(key);
                if (tag && tag->getId() == TagType::SHORT) {
                    return static_cast<const ShortTag*>(tag)->getData();
                }
                return defaultValue;
            }

            /**
             * @brief 获取整型值
             */
            int32_t getInt(const std::string& key, int32_t defaultValue = 0) const noexcept {
                auto* tag = getTag(key);
                if (tag && tag->getId() == TagType::INT) {
                    return static_cast<const IntTag*>(tag)->getData();
                }
                return defaultValue;
            }

            /**
             * @brief 获取长整型值
             */
            int64_t getLong(const std::string& key, int64_t defaultValue = 0) const noexcept {
                auto* tag = getTag(key);
                if (tag && tag->getId() == TagType::LONG) {
                    return static_cast<const LongTag*>(tag)->getData();
                }
                return defaultValue;
            }

            /**
             * @brief 获取浮点值
             */
            float getFloat(const std::string& key, float defaultValue = 0.0f) const noexcept {
                auto* tag = getTag(key);
                if (tag && tag->getId() == TagType::FLOAT) {
                    return static_cast<const FloatTag*>(tag)->getData();
                }
                return defaultValue;
            }

            /**
             * @brief 获取双精度值
             */
            double getDouble(const std::string& key, double defaultValue = 0.0) const noexcept {
                auto* tag = getTag(key);
                if (tag && tag->getId() == TagType::DOUBLE) {
                    return static_cast<const DoubleTag*>(tag)->getData();
                }
                return defaultValue;
            }

            /**
             * @brief 获取字符串值
             */
            const std::string& getString(const std::string& key, const std::string& defaultValue = "") const noexcept {
                auto* tag = getTag(key);
                if (tag && tag->getId() == TagType::STRING) {
                    return static_cast<const StringTag*>(tag)->getData();
                }
                return defaultValue;
            }

            /**
             * @brief 获取字节数组
             */
            const std::vector<int8_t>* getByteArray(const std::string& key) const noexcept {
                auto* tag = getTag(key);
                if (tag && tag->getId() == TagType::BYTE_ARRAY) {
                    return &static_cast<const ByteArrayTag*>(tag)->getData();
                }
                return nullptr;
            }

            /**
             * @brief 获取整型数组
             */
            const std::vector<int32_t>* getIntArray(const std::string& key) const noexcept {
                auto* tag = getTag(key);
                if (tag && tag->getId() == TagType::INT_ARRAY) {
                    return &static_cast<const IntArrayTag*>(tag)->getData();
                }
                return nullptr;
            }

            /**
             * @brief 获取长整型数组
             */
            const std::vector<int64_t>* getLongArray(const std::string& key) const noexcept {
                auto* tag = getTag(key);
                if (tag && tag->getId() == TagType::LONG_ARRAY) {
                    return &static_cast<const LongArrayTag*>(tag)->getData();
                }
                return nullptr;
            }

            /**
             * @brief 获取列表
             */
            ListTag* getList(const std::string& key) noexcept {
                auto* tag = getTag(key);
                if (tag && tag->getId() == TagType::LIST) {
                    return static_cast<ListTag*>(tag);
                }
                return nullptr;
            }

            const ListTag* getList(const std::string& key) const noexcept {
                auto* tag = getTag(key);
                if (tag && tag->getId() == TagType::LIST) {
                    return static_cast<const ListTag*>(tag);
                }
                return nullptr;
            }

            /**
             * @brief 获取复合标签
             */
            CompoundTag* getCompound(const std::string& key) noexcept {
                auto* tag = getTag(key);
                if (tag && tag->getId() == TagType::COMPOUND) {
                    return static_cast<CompoundTag*>(tag);
                }
                return nullptr;
            }

            const CompoundTag* getCompound(const std::string& key) const noexcept {
                auto* tag = getTag(key);
                if (tag && tag->getId() == TagType::COMPOUND) {
                    return static_cast<const CompoundTag*>(tag);
                }
                return nullptr;
            }

            /**
             * @brief 获取原始标签
             */
            Tag* getTag(const std::string& key) noexcept {
                auto it = m_data.find(key);
                return it != m_data.end() ? it->second.get() : nullptr;
            }

            const Tag* getTag(const std::string& key) const noexcept {
                auto it = m_data.find(key);
                return it != m_data.end() ? it->second.get() : nullptr;
            }

            // ========== 类型安全的设置方法 ==========

            /**
             * @brief 设置字节值
             */
            void putByte(const std::string& key, int8_t value) {
                m_data[key] = std::make_unique<ByteTag>(value);
            }

            /**
             * @brief 设置短整型值
             */
            void putShort(const std::string& key, int16_t value) {
                m_data[key] = std::make_unique<ShortTag>(value);
            }

            /**
             * @brief 设置整型值
             */
            void putInt(const std::string& key, int32_t value) {
                m_data[key] = std::make_unique<IntTag>(value);
            }

            /**
             * @brief 设置长整型值
             */
            void putLong(const std::string& key, int64_t value) {
                m_data[key] = std::make_unique<LongTag>(value);
            }

            /**
             * @brief 设置浮点值
             */
            void putFloat(const std::string& key, float value) {
                m_data[key] = std::make_unique<FloatTag>(value);
            }

            /**
             * @brief 设置双精度值
             */
            void putDouble(const std::string& key, double value) {
                m_data[key] = std::make_unique<DoubleTag>(value);
            }

            /**
             * @brief 设置字符串值
             */
            void putString(const std::string& key, std::string value) {
                m_data[key] = std::make_unique<StringTag>(std::move(value));
            }

            /**
             * @brief 设置字节数组
             */
            void putByteArray(const std::string& key, std::vector<int8_t> value) {
                m_data[key] = std::make_unique<ByteArrayTag>(std::move(value));
            }

            /**
             * @brief 设置整型数组
             */
            void putIntArray(const std::string& key, std::vector<int32_t> value) {
                m_data[key] = std::make_unique<IntArrayTag>(std::move(value));
            }

            /**
             * @brief 设置长整型数组
             */
            void putLongArray(const std::string& key, std::vector<int64_t> value) {
                m_data[key] = std::make_unique<LongArrayTag>(std::move(value));
            }

            /**
             * @brief 设置标签
             */
            void put(const std::string& key, std::unique_ptr<Tag> tag) {
                m_data[key] = std::move(tag);
            }

            /**
             * @brief 合并另一个复合标签
             */
            void merge(const CompoundTag& other) {
                for (const auto& pair : other.m_data) {
                    m_data[pair.first] = pair.second->copy();
                }
            }

        private:
            std::map<std::string, std::unique_ptr<Tag>> m_data;
        };

        // ========== 便捷的静态工厂函数 ==========

        inline std::unique_ptr<ByteTag> of(int8_t value) {
            return std::make_unique<ByteTag>(value);
        }

        inline std::unique_ptr<ShortTag> of(int16_t value) {
            return std::make_unique<ShortTag>(value);
        }

        inline std::unique_ptr<IntTag> of(int32_t value) {
            return std::make_unique<IntTag>(value);
        }

        inline std::unique_ptr<LongTag> of(int64_t value) {
            return std::make_unique<LongTag>(value);
        }

        inline std::unique_ptr<FloatTag> of(float value) {
            return std::make_unique<FloatTag>(value);
        }

        inline std::unique_ptr<DoubleTag> of(double value) {
            return std::make_unique<DoubleTag>(value);
        }

        inline std::unique_ptr<StringTag> of(std::string value) {
            return std::make_unique<StringTag>(std::move(value));
        }

    } // namespace NBT
} // namespace PrismaCraft
