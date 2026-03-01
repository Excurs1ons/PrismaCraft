#pragma once

#include "NBTTag.h"
#include <iostream>
#include <memory>
#include <vector>

namespace PrismaCraft {
    namespace NBT {

        /**
         * @brief NBT 数据输入 - 对应 Minecraft: net.minecraft.nbt.DataInput
         */
        class DataInput {
        public:
            explicit DataInput(std::istream& stream) : m_stream(stream) {}

            // ========== 基础类型读取 ==========

            /**
             * @brief 读取字节
             */
            int8_t readByte() {
                return static_cast<int8_t>(m_stream.get());
            }

            /**
             * @brief 读取无符号字节
             */
            uint8_t readUnsignedByte() {
                return static_cast<uint8_t>(m_stream.get());
            }

            /**
             * @brief 读取短整型（大端序）
             */
            int16_t readShort() {
                int16_t value = 0;
                value |= static_cast<int16_t>(m_stream.get()) << 8;
                value |= static_cast<int16_t>(m_stream.get()) << 0;
                return value;
            }

            /**
             * @brief 读取整型（大端序）
             */
            int32_t readInt() {
                int32_t value = 0;
                value |= static_cast<int32_t>(m_stream.get()) << 24;
                value |= static_cast<int32_t>(m_stream.get()) << 16;
                value |= static_cast<int32_t>(m_stream.get()) << 8;
                value |= static_cast<int32_t>(m_stream.get()) << 0;
                return value;
            }

            /**
             * @brief 读取长整型（大端序）
             */
            int64_t readLong() {
                int64_t value = 0;
                value |= static_cast<int64_t>(m_stream.get()) << 56;
                value |= static_cast<int64_t>(m_stream.get()) << 48;
                value |= static_cast<int64_t>(m_stream.get()) << 40;
                value |= static_cast<int64_t>(m_stream.get()) << 32;
                value |= static_cast<int64_t>(m_stream.get()) << 24;
                value |= static_cast<int64_t>(m_stream.get()) << 16;
                value |= static_cast<int64_t>(m_stream.get()) << 8;
                value |= static_cast<int64_t>(m_stream.get()) << 0;
                return value;
            }

            /**
             * @brief 读取浮点数（大端序）
             */
            float readFloat() {
                int32_t value = readInt();
                float result;
                std::memcpy(&result, &value, sizeof(float));
                return result;
            }

            /**
             * @brief 读取双精度数（大端序）
             */
            double readDouble() {
                int64_t value = readLong();
                double result;
                std::memcpy(&result, &value, sizeof(double));
                return result;
            }

            /**
             * @brief 读取字符串
             * 格式：2 字节长度（无符号短整型）+ UTF-8 字符串
             */
            std::string readUTF() {
                uint16_t length = static_cast<uint16_t>(readShort());
                std::string result(length, '\0');
                m_stream.read(&result[0], length);
                return result;
            }

            /**
             * @brief 读取字节数组
             */
            std::vector<int8_t> readByteArray() {
                int32_t length = readInt();
                std::vector<int8_t> result(length);
                m_stream.read(reinterpret_cast<char*>(result.data()), length);
                return result;
            }

            /**
             * @brief 读取整型数组
             */
            std::vector<int32_t> readIntArray() {
                int32_t length = readInt();
                std::vector<int32_t> result(length);
                for (int32_t i = 0; i < length; ++i) {
                    result[i] = readInt();
                }
                return result;
            }

            /**
             * @brief 读取长整型数组
             */
            std::vector<int64_t> readLongArray() {
                int32_t length = readInt();
                std::vector<int64_t> result(length);
                for (int32_t i = 0; i < length; ++i) {
                    result[i] = readLong();
                }
                return result;
            }

        private:
            std::istream& m_stream;
        };

        /**
         * @brief NBT 数据输出 - 对应 Minecraft: net.minecraft.nbt.DataOutput
         */
        class DataOutput {
        public:
            explicit DataOutput(std::ostream& stream) : m_stream(stream) {}

            // ========== 基础类型写入 ==========

            /**
             * @brief 写入字节
             */
            void writeByte(int8_t value) {
                m_stream.put(static_cast<char>(value));
            }

            /**
             * @brief 写入短整型（大端序）
             */
            void writeShort(int16_t value) {
                m_stream.put(static_cast<char>((value >> 8) & 0xFF));
                m_stream.put(static_cast<char>((value >> 0) & 0xFF));
            }

            /**
             * @brief 写入整型（大端序）
             */
            void writeInt(int32_t value) {
                m_stream.put(static_cast<char>((value >> 24) & 0xFF));
                m_stream.put(static_cast<char>((value >> 16) & 0xFF));
                m_stream.put(static_cast<char>((value >> 8) & 0xFF));
                m_stream.put(static_cast<char>((value >> 0) & 0xFF));
            }

            /**
             * @brief 写入长整型（大端序）
             */
            void writeLong(int64_t value) {
                m_stream.put(static_cast<char>((value >> 56) & 0xFF));
                m_stream.put(static_cast<char>((value >> 48) & 0xFF));
                m_stream.put(static_cast<char>((value >> 40) & 0xFF));
                m_stream.put(static_cast<char>((value >> 32) & 0xFF));
                m_stream.put(static_cast<char>((value >> 24) & 0xFF));
                m_stream.put(static_cast<char>((value >> 16) & 0xFF));
                m_stream.put(static_cast<char>((value >> 8) & 0xFF));
                m_stream.put(static_cast<char>((value >> 0) & 0xFF));
            }

            /**
             * @brief 写入浮点数（大端序）
             */
            void writeFloat(float value) {
                int32_t intValue;
                std::memcpy(&intValue, &value, sizeof(float));
                writeInt(intValue);
            }

            /**
             * @brief 写入双精度数（大端序）
             */
            void writeDouble(double value) {
                int64_t longValue;
                std::memcpy(&longValue, &value, sizeof(double));
                writeLong(longValue);
            }

            /**
             * @brief 写入字符串
             * 格式：2 字节长度（无符号短整型）+ UTF-8 字符串
             */
            void writeUTF(const std::string& value) {
                writeShort(static_cast<int16_t>(value.size()));
                m_stream.write(value.data(), value.size());
            }

            /**
             * @brief 写入字节数组
             */
            void writeByteArray(const std::vector<int8_t>& value) {
                writeInt(static_cast<int32_t>(value.size()));
                m_stream.write(reinterpret_cast<const char*>(value.data()), value.size());
            }

            /**
             * @brief 写入整型数组
             */
            void writeIntArray(const std::vector<int32_t>& value) {
                writeInt(static_cast<int32_t>(value.size()));
                for (int32_t item : value) {
                    writeInt(item);
                }
            }

            /**
             * @brief 写入长整型数组
             */
            void writeLongArray(const std::vector<int64_t>& value) {
                writeInt(static_cast<int32_t>(value.size()));
                for (int64_t item : value) {
                    writeLong(item);
                }
            }

        private:
            std::ostream& m_stream;
        };

        /**
         * @brief NBT 读取器 - 对应 Minecraft: net.minecraft.nbt.NbtIo
         */
        class NbtReader {
        public:
            /**
             * @brief 从流读取命名标签
             * 格式：标签类型 + 名称 + 标签数据
             * 对应 Minecraft: NbtIo.read(DataInput)
             */
            static std::unique_ptr<Tag> read(DataInput& input) {
                TagType type = static_cast<TagType>(input.readUnsignedByte());
                if (type == TagType::END) {
                    return nullptr;
                }

                // 读取名称
                std::string name = input.readUTF();

                // 读取标签数据
                return readTag(input, type);
            }

            /**
             * @brief 从流读取未命名标签
             */
            static std::unique_ptr<Tag> readUnnamedTag(DataInput& input) {
                TagType type = static_cast<TagType>(input.readUnsignedByte());
                if (type == TagType::END) {
                    return nullptr;
                }
                return readTag(input, type);
            }

            /**
             * @brief 从流读取复合标签（最常用）
             */
            static std::unique_ptr<CompoundTag> readCompoundTag(DataInput& input) {
                auto tag = read(input);
                if (tag && tag->getId() == TagType::COMPOUND) {
                    return std::unique_ptr<CompoundTag>(
                        static_cast<CompoundTag*>(tag.release())
                    );
                }
                return nullptr;
            }

            /**
             * @brief 从字节数组读取复合标签
             */
            static std::unique_ptr<CompoundTag> readCompoundTag(const std::vector<uint8_t>& data) {
                // TODO: 实现内存流读取
                return nullptr;
            }

        private:
            /**
             * @brief 读取指定类型的标签
             */
            static std::unique_ptr<Tag> readTag(DataInput& input, TagType type) {
                switch (type) {
                    case TagType::END:
                        return nullptr;

                    case TagType::BYTE:
                        return std::make_unique<ByteTag>(input.readByte());

                    case TagType::SHORT:
                        return std::make_unique<ShortTag>(input.readShort());

                    case TagType::INT:
                        return std::make_unique<IntTag>(input.readInt());

                    case TagType::LONG:
                        return std::make_unique<LongTag>(input.readLong());

                    case TagType::FLOAT:
                        return std::make_unique<FloatTag>(input.readFloat());

                    case TagType::DOUBLE:
                        return std::make_unique<DoubleTag>(input.readDouble());

                    case TagType::BYTE_ARRAY:
                        return std::make_unique<ByteArrayTag>(input.readByteArray());

                    case TagType::STRING:
                        return std::make_unique<StringTag>(input.readUTF());

                    case TagType::LIST:
                        return readListTag(input);

                    case TagType::COMPOUND:
                        return readCompoundTagData(input);

                    case TagType::INT_ARRAY:
                        return std::make_unique<IntArrayTag>(input.readIntArray());

                    case TagType::LONG_ARRAY:
                        return std::make_unique<LongArrayTag>(input.readLongArray());

                    default:
                        // 未知类型，抛出异常
                        throw std::runtime_error("Unknown NBT tag type: " + std::to_string(static_cast<int>(type)));
                }
            }

            /**
             * @brief 读取列表标签
             */
            static std::unique_ptr<ListTag> readListTag(DataInput& input) {
                auto listTag = std::make_unique<ListTag>();
                TagType elementType = static_cast<TagType>(input.readUnsignedByte());
                int32_t length = input.readInt();

                listTag->setElementType(elementType);

                for (int32_t i = 0; i < length; ++i) {
                    auto tag = readTag(input, elementType);
                    if (tag) {
                        listTag->add(std::move(tag));
                    }
                }

                return listTag;
            }

            /**
             * @brief 读取复合标签数据
             */
            static std::unique_ptr<CompoundTag> readCompoundTagData(DataInput& input) {
                auto compoundTag = std::make_unique<CompoundTag>();

                while (true) {
                    TagType type = static_cast<TagType>(input.readUnsignedByte());
                    if (type == TagType::END) {
                        break;
                    }

                    // 读取键名
                    std::string key = input.readUTF();

                    // 读取标签值
                    auto tag = readTag(input, type);
                    if (tag) {
                        compoundTag->put(key, std::move(tag));
                    }
                }

                return compoundTag;
            }
        };

        /**
         * @brief NBT 写入器 - 对应 Minecraft: net.minecraft.nbt.NbtIo
         */
        class NbtWriter {
        public:
            /**
             * @brief 将标签写入流
             * 对应 Minecraft: NbtIo.write(Tag, DataOutput)
             */
            static void write(const Tag& tag, DataOutput& output) {
                // 写入标签类型
                output.writeByte(static_cast<int8_t>(tag.getId()));

                // 对于非 END 标签，写入名称
                if (tag.getId() != TagType::END) {
                    output.writeUTF("");  // 命名标签通常有名称，这里简化为空
                }

                // 写入标签数据
                writeTagData(tag, output);
            }

            /**
             * @brief 写入复合标签
             */
            static void writeCompoundTag(const CompoundTag& tag, DataOutput& output) {
                write(tag, output);
            }

            /**
             * @brief 写入未命名标签
             */
            static void writeUnnamedTag(const Tag& tag, DataOutput& output) {
                output.writeByte(static_cast<int8_t>(tag.getId()));
                writeTagData(tag, output);
            }

            /**
             * @brief 将复合标签序列化为字节数组
             */
            static std::vector<uint8_t> writeCompoundTagToBytes(const CompoundTag& tag) {
                std::vector<uint8_t> result;
                // TODO: 实现内存流写入
                return result;
            }

        private:
            /**
             * @brief 写入标签数据
             */
            static void writeTagData(const Tag& tag, DataOutput& output) {
                switch (tag.getId()) {
                    case TagType::END:
                        // END 标签没有数据
                        break;

                    case TagType::BYTE:
                        output.writeByte(static_cast<const ByteTag&>(tag).getData());
                        break;

                    case TagType::SHORT:
                        output.writeShort(static_cast<const ShortTag&>(tag).getData());
                        break;

                    case TagType::INT:
                        output.writeInt(static_cast<const IntTag&>(tag).getData());
                        break;

                    case TagType::LONG:
                        output.writeLong(static_cast<const LongTag&>(tag).getData());
                        break;

                    case TagType::FLOAT:
                        output.writeFloat(static_cast<const FloatTag&>(tag).getData());
                        break;

                    case TagType::DOUBLE:
                        output.writeDouble(static_cast<const DoubleTag&>(tag).getData());
                        break;

                    case TagType::BYTE_ARRAY:
                        output.writeByteArray(static_cast<const ByteArrayTag&>(tag).getData());
                        break;

                    case TagType::STRING:
                        output.writeUTF(static_cast<const StringTag&>(tag).getData());
                        break;

                    case TagType::LIST:
                        writeListTag(static_cast<const ListTag&>(tag), output);
                        break;

                    case TagType::COMPOUND:
                        writeCompoundTagData(static_cast<const CompoundTag&>(tag), output);
                        break;

                    case TagType::INT_ARRAY:
                        output.writeIntArray(static_cast<const IntArrayTag&>(tag).getData());
                        break;

                    case TagType::LONG_ARRAY:
                        output.writeLongArray(static_cast<const LongArrayTag&>(tag).getData());
                        break;

                    default:
                        throw std::runtime_error("Unknown NBT tag type: " + std::to_string(static_cast<int>(tag.getId())));
                }
            }

            /**
             * @brief 写入列表标签
             */
            static void writeListTag(const ListTag& tag, DataOutput& output) {
                output.writeByte(static_cast<int8_t>(tag.getElementType()));
                output.writeInt(static_cast<int32_t>(tag.size()));

                for (size_t i = 0; i < tag.size(); ++i) {
                    const Tag* element = tag.get(i);
                    if (element) {
                        writeTagData(*element, output);
                    }
                }
            }

            /**
             * @brief 写入复合标签数据
             */
            static void writeCompoundTagData(const CompoundTag& tag, DataOutput& output) {
                for (const auto& key : tag.getAllKeys()) {
                    const Tag* childTag = tag.getTag(key);
                    if (childTag) {
                        // 写入标签类型
                        output.writeByte(static_cast<int8_t>(childTag->getId()));

                        // 写入键名
                        output.writeUTF(key);

                        // 写入标签数据
                        writeTagData(*childTag, output);
                    }
                }

                // 写入 END 标签
                output.writeByte(static_cast<int8_t>(TagType::END));
            }
        };

        /**
         * @brief 压缩 NBT IO - 对应 Minecraft: net.minecraft.nbt.NbtIo
         *
         * 支持使用 GZIP 或 ZLIB 压缩的 NBT 数据
         */
        class CompressedNbtIO {
        public:
            /**
             * @brief GZIP 压缩级别
             */
            enum class CompressionLevel {
                NO_COMPRESSION = 0,
                FAST = 1,
                NORMAL = 6,
                MAX = 9
            };

            /**
             * @brief 从 GZIP 压缩的流读取复合标签
             * 对应 Minecraft: NbtIo.readCompressed(InputStream)
             */
            static std::unique_ptr<CompoundTag> readGzipCompressed(std::istream& stream);

            /**
             * @brief 将复合标签写入 GZIP 压缩的流
             * 对应 Minecraft: NbtIo.writeCompressed(CompoundTag, OutputStream)
             */
            static void writeGzipCompressed(const CompoundTag& tag, std::ostream& stream,
                                           CompressionLevel level = CompressionLevel::NORMAL);

            /**
             * @brief 从 ZLIB 压缩的流读取复合标签
             */
            static std::unique_ptr<CompoundTag> readZlibCompressed(std::istream& stream);

            /**
             * @brief 将复合标签写入 ZLIB 压缩的流
             */
            static void writeZlibCompressed(const CompoundTag& tag, std::ostream& stream,
                                           CompressionLevel level = CompressionLevel::NORMAL);

            /**
             * @brief 从字节数组读取 GZIP 压缩的复合标签
             */
            static std::unique_ptr<CompoundTag> readGzipCompressedFromBytes(const std::vector<uint8_t>& data);

            /**
             * @brief 将复合标签序列化为 GZIP 压缩的字节数组
             */
            static std::vector<uint8_t> writeGzipCompressedToBytes(
                const CompoundTag& tag,
                CompressionLevel level = CompressionLevel::NORMAL
            );
        };

    } // namespace NBT
} // namespace PrismaCraft
