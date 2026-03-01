// PrismaCraft.Core - C# 核心库
// 对应 Minecraft: net.minecraft.* 的核心类

namespace PrismaCraft.Core
{
    /// <summary>
    /// 方块坐标 - 对应 Minecraft: BlockPos
    /// </summary>
    public struct BlockPos
    {
        public int X { get; set; }
        public int Y { get; set; }
        public int Z { get; set; }

        public BlockPos(int x, int y, int z)
        {
            X = x;
            Y = y;
            Z = z;
        }

        // 计算在区块段内的一维索引
        public int GetIndexInSection()
        {
            return (Y & 15) << 8 | (Z & 15) << 4 | (X & 15);
        }

        // 从一维索引创建
        public static BlockPos FromIndexInSection(int index)
        {
            return new BlockPos(
                index & 15,
                (index >> 8) & 15,
                (index >> 4) & 15
            );
        }

        public override bool Equals(object obj)
        {
            if (obj is BlockPos other)
            {
                return X == other.X && Y == other.Y && Z == other.Z;
            }
            return false;
        }

        public override int GetHashCode()
        {
            return X ^ (Y << 8) ^ (Z << 16);
        }

        public static bool operator ==(BlockPos left, BlockPos right)
        {
            return left.Equals(right);
        }

        public static bool operator !=(BlockPos left, BlockPos right)
        {
            return !left.Equals(right);
        }

        public static BlockPos operator +(BlockPos left, BlockPos right)
        {
            return new BlockPos(left.X + right.X, left.Y + right.Y, left.Z + right.Z);
        }

        // 六个面方向
        public BlockPos North() => new BlockPos(X, Y, Z - 1);
        public BlockPos South() => new BlockPos(X, Y, Z + 1);
        public BlockPos East() => new BlockPos(X + 1, Y, Z);
        public BlockPos West() => new BlockPos(X - 1, Y, Z);
        public BlockPos Up() => new BlockPos(X, Y + 1, Z);
        public BlockPos Down() => new BlockPos(X, Y - 1, Z);
    }

    /// <summary>
    /// 区块坐标 - 对应 Minecraft: ChunkPos
    /// </summary>
    public struct ChunkPos
    {
        public int X { get; set; }
        public int Z { get; set; }

        public ChunkPos(int x, int z)
        {
            X = x;
            Z = z;
        }

        public ChunkPos(BlockPos pos)
        {
            X = pos.X >> 4;
            if (pos.X < 0 && (pos.X & 15) != 0) X--;
            Z = pos.Z >> 4;
            if (pos.Z < 0 && (pos.Z & 15) != 0) Z--;
        }

        // 计算区块键值
        public long ToLong()
        {
            return (long)X & 0xFFFFFFFFL | ((long)Z & 0xFFFFFFFFL) << 32;
        }

        public override bool Equals(object obj)
        {
            if (obj is ChunkPos other)
            {
                return X == other.X && Z == other.Z;
            }
            return false;
        }

        public override int GetHashCode()
        {
            return ToLong().GetHashCode();
        }

        public static bool operator ==(ChunkPos left, ChunkPos right)
        {
            return left.Equals(right);
        }

        public static bool operator !=(ChunkPos left, ChunkPos right)
        {
            return !left.Equals(right);
        }
    }

    /// <summary>
    /// 方块类型 - 对应 Minecraft: Block
    /// </summary>
    public enum BlockType : byte
    {
        Air = 0,
        Stone = 1,
        GrassBlock = 2,
        Dirt = 3,
        Cobblestone = 4,
        OakPlanks = 5,
        OakSapling = 6,
        Bedrock = 7,
        Water = 8,
        Lava = 10,
        Sand = 12,
        Gravel = 13,
        GoldOre = 14,
        IronOre = 15,
        CoalOre = 16,
        OakLog = 17,
        OakLeaves = 18,
        Glass = 20,
        LapisOre = 21,
        LapisBlock = 22,
        Sandstone = 24
    }

    /// <summary>
    /// 世界管理器 - 对应 Minecraft: Level
    /// </summary>
    public class WorldManager
    {
        private static WorldManager instance;
        private readonly World nativeWorld;

        private WorldManager()
        {
            // 初始化原生世界
            nativeWorld = new World();
        }

        public static WorldManager Instance
        {
            get
            {
                if (instance == null)
                {
                    instance = new WorldManager();
                }
                return instance;
            }
        }

        /// <summary>
        /// 获取指定位置的方块
        /// </summary>
        public BlockType GetBlock(int x, int y, int z)
        {
            byte blockType;
            uint stateId;
            WorldManager_GetBlock(x, y, z, out blockType, out stateId);
            return (BlockType)blockType;
        }

        /// <summary>
        /// 设置指定位置的方块
        /// </summary>
        public void SetBlock(int x, int y, int z, BlockType blockType)
        {
            WorldManager_SetBlock(x, y, z, (byte)blockType, 0);
        }

        /// <summary>
        /// 加载区块
        /// </summary>
        public void LoadChunk(int chunkX, int chunkZ)
        {
            WorldManager_LoadChunk(chunkX, chunkZ);
        }

        /// <summary>
        /// 卸载区块
        /// </summary>
        public void UnloadChunk(int chunkX, int chunkZ)
        {
            WorldManager_UnloadChunk(chunkX, chunkZ);
        }

        // ========== P/Invoke 到原生代码 ==========

        [System.Runtime.InteropServices.DllImport("PrismaCraftNative")]
        private static extern void WorldManager_GetBlock(
            int x, int y, int z,
            out byte blockType,
            out uint stateId
        );

        [System.Runtime.InteropServices.DllImport("PrismaCraftNative")]
        private static extern void WorldManager_SetBlock(
            int x, int y, int z,
            byte blockType,
            uint stateId
        );

        [System.Runtime.InteropServices.DllImport("PrismaCraftNative")]
        private static extern void WorldManager_LoadChunk(int chunkX, int chunkZ);

        [System.Runtime.InteropServices.DllImport("PrismaCraftNative")]
        private static extern void WorldManager_UnloadChunk(int chunkX, int chunkZ);

        // 原生世界类（内部类）
        private class World
        {
            // 这个类只是占位符，实际实现在 C++ 中
        }
    }

    /// <summary>
    /// 输入系统 - 对应 Minecraft: Options / Input
    /// </summary>
    public static class Input
    {
        /// <summary>
        /// 检查键是否按下
        /// </summary>
        public static bool IsKeyDown(KeyCode keyCode)
        {
            return Input_IsKeyDown((int)keyCode);
        }

        /// <summary>
        /// 检查键是否刚刚按下
        /// </summary>
        public static bool IsKeyPressed(KeyCode keyCode)
        {
            return Input_IsKeyPressed((int)keyCode);
        }

        /// <summary>
        /// 检查键是否刚刚释放
        /// </summary>
        public static bool IsKeyReleased(KeyCode keyCode)
        {
            return Input_IsKeyReleased((int)keyCode);
        }

        /// <summary>
        /// 检查鼠标按钮是否按下
        /// </summary>
        public static bool IsMouseButtonPressed(int button)
        {
            return Input_IsMouseButtonPressed(button);
        }

        // ========== P/Invoke ==========

        [System.Runtime.InteropServices.DllImport("PrismaCraftNative")]
        private static extern bool Input_IsKeyDown(int keyCode);

        [System.Runtime.InteropServices.DllImport("PrismaCraftNative")]
        private static extern bool Input_IsKeyPressed(int keyCode);

        [System.Runtime.InteropServices.DllImport("PrismaCraftNative")]
        private static extern bool Input_IsKeyReleased(int keyCode);

        [System.Runtime.InteropServices.DllImport("PrismaCraftNative")]
        private static extern bool Input_IsMouseButtonPressed(int button);
    }

    /// <summary>
    /// 键码定义
    /// </summary>
    public enum KeyCode
    {
        // 字母键
        A = 0x41,
        B, C, D, E, F, G, H, I, J, K, L, M,
        N, O, P, Q, R, S, T, U, V, W, X, Y, Z,

        // 数字键
        D0 = 0x30, D1, D2, D3, D4, D5, D6, D7, D8, D9,

        // 功能键
        F1 = 0x70, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,

        // 特殊键
        Space = 0x20,
        Enter = 0x0D,
        Escape = 0x1B,
        Tab = 0x09,
        Backspace = 0x08,
        Shift = 0x10,
        Control = 0x11,
        Alt = 0x12,

        // 方向键
        Left = 0x25,
        Up = 0x26,
        Right = 0x27,
        Down = 0x28
    }

    /// <summary>
    /// 时间系统 - 对应 Minecraft: MinecraftClient.timer
    /// </summary>
    public static class Time
    {
        /// <summary>
        /// 获取两帧之间的时间（秒）
        /// </summary>
        public static float DeltaTime
        {
            get { return Time_GetDeltaTime(); }
        }

        /// <summary>
        /// 获取游戏运行的总时间（秒）
        /// </summary>
        public static float TimeSinceStartup
        {
            get { return Time_GetTime(); }
        }

        // ========== P/Invoke ==========

        [System.Runtime.InteropServices.DllImport("PrismaCraftNative")]
        private static extern float Time_GetDeltaTime();

        [System.Runtime.InteropServices.DllImport("PrismaCraftNative")]
        private static extern float Time_GetTime();
    }

    /// <summary>
    /// 调试日志 - 对应 Minecraft: Logger
    /// </summary>
    public static class Debug
    {
        public static void Log(object message)
        {
            Debug_Log(message?.ToString() ?? "null");
        }

        public static void LogWarning(object message)
        {
            Debug_LogWarning(message?.ToString() ?? "null");
        }

        public static void LogError(object message)
        {
            Debug_LogError(message?.ToString() ?? "null");
        }

        // ========== P/Invoke ==========

        [System.Runtime.InteropServices.DllImport("PrismaCraftNative")]
        private static extern void Debug_Log(string message);

        [System.Runtime.InteropServices.DllImport("PrismaCraftNative")]
        private static extern void Debug_LogWarning(string message);

        [System.Runtime.InteropServices.DllImport("PrismaCraftNative")]
        private static extern void Debug_LogError(string message);
    }
}
