// PrismaCraft.Scripts - C# 游戏逻辑脚本
// 这些脚本继承自引擎的 ScriptBehaviour

using PrismaCraft.Core;
using PrismaEngine;

namespace PrismaCraft.Scripts
{
    /// <summary>
    /// 玩家控制器 - 对应 Minecraft: Player
    ///
    /// 处理玩家输入、移动、方块交互等
    /// </summary>
    public class PlayerController : ScriptBehaviour
    {
        // 移动参数
        private float moveSpeed = 4.5f;
        private float sprintSpeed = 5.6f;
        private float sneakSpeed = 1.3f;
        private float jumpForce = 1.25f; // 约 8 block/s² 的重力下跳 1.25m

        // 玩家状态
        private bool isOnGround = false;
        private bool isSprinting = false;
        private bool isSneaking = false;

        // 视角
        private float yaw = 0f;
        private float pitch = 0f;
        private const float maxPitch = 89f;
        private const float mouseSensitivity = 0.2f;

        // 物理状态
        private Vector3 velocity = Vector3.zero;

        /// <summary>
        /// 脚本启动时调用
        /// </summary>
        public void Start()
        {
            Debug.Log("PlayerController initialized");
        }

        /// <summary>
        /// 每帧调用
        /// </summary>
        public void Update(float deltaTime)
        {
            HandleInput();
            UpdateMovement(deltaTime);
            HandleBlockInteraction();
        }

        /// <summary>
        /// 处理输入
        /// </summary>
        private void HandleInput()
        {
            // 跑步/潜行切换
            if (Input.IsKeyPressed(KeyCode.Control))
            {
                isSprinting = true;
            }
            else if (Input.IsKeyReleased(KeyCode.Control))
            {
                isSprinting = false;
            }

            if (Input.IsKeyPressed(KeyCode.Shift))
            {
                isSneaking = true;
            }
            else if (Input.IsKeyReleased(KeyCode.Shift))
            {
                isSneaking = false;
            }

            // 跳跃
            if (Input.IsKeyPressed(KeyCode.Space) && isOnGround)
            {
                velocity.y = jumpForce;
                isOnGround = false;
            }
        }

        /// <summary>
        /// 更新移动
        /// </summary>
        private void UpdateMovement(float deltaTime)
        {
            // 计算移动方向
            Vector3 moveDirection = Vector3.zero;

            // WASD 移动（相对于视角）
            if (Input.IsKeyDown(KeyCode.W)) moveDirection += GetForwardFlat();
            if (Input.IsKeyDown(KeyCode.S)) moveDirection -= GetForwardFlat();
            if (Input.IsKeyDown(KeyCode.A)) moveDirection -= GetRightFlat();
            if (Input.IsKeyDown(KeyCode.D)) moveDirection += GetRightFlat();

            // 归一化移动方向
            if (moveDirection.magnitude > 0.1f)
            {
                moveDirection = moveDirection.normalized;
            }

            // 应用速度
            float currentSpeed = GetCurrentSpeed();
            velocity.x = moveDirection.x * currentSpeed;
            velocity.z = moveDirection.z * currentSpeed;

            // 应用重力
            velocity.y -= 30.0f * deltaTime; // Minecraft 重力约 30 block/s²

            // 更新位置
            Vector3 newPos = transform.position + velocity * deltaTime;

            // 碰撞检测和响应
            newPos = HandleCollision(newPos);

            // 检查是否着地
            CheckGrounded(newPos);

            // 应用位置
            transform.position = newPos;
        }

        /// <summary>
        /// 获取当前速度
        /// </summary>
        private float GetCurrentSpeed()
        {
            if (isSneaking) return sneakSpeed;
            if (isSprinting) return sprintSpeed;
            return moveSpeed;
        }

        /// <summary>
        /// 获取水平方向的前方向量
        /// </summary>
        private Vector3 GetForwardFlat()
        {
            Vector3 forward = transform.forward;
            forward.y = 0;
            return forward.normalized;
        }

        /// <summary>
        /// 获取水平方向的右方向量
        /// </summary>
        private Vector3 GetRightFlat()
        {
            Vector3 right = transform.right;
            right.y = 0;
            return right.normalized;
        }

        /// <summary>
        /// 处理碰撞
        /// </summary>
        private Vector3 HandleCollision(Vector3 newPos)
        {
            // 简化的 AABB 碰撞检测
            // 实际实现需要检查周围的方块

            // X 轴碰撞
            if (CheckCollisionAt(new Vector3(newPos.x, transform.position.y, transform.position.z)))
            {
                newPos.x = transform.position.x;
                velocity.x = 0;
            }

            // Y 轴碰撞
            if (CheckCollisionAt(new Vector3(newPos.x, newPos.y, transform.position.z)))
            {
                newPos.y = transform.position.y;
                velocity.y = 0;
            }

            // Z 轴碰撞
            if (CheckCollisionAt(new Vector3(newPos.x, newPos.y, newPos.z)))
            {
                newPos.z = transform.position.z;
                velocity.z = 0;
            }

            return newPos;
        }

        /// <summary>
        /// 检查指定位置是否碰撞
        /// </summary>
        private bool CheckCollisionAt(Vector3 pos)
        {
            // 检查玩家包围盒与方块的碰撞
            BlockPos blockPos = new BlockPos(
                Mathf.FloorToInt(pos.x),
                Mathf.FloorToInt(pos.y),
                Mathf.FloorToInt(pos.z)
            );

            BlockType block = WorldManager.Instance.GetBlock(blockPos.X, blockPos.Y, blockPos.Z);
            return block != BlockType.Air && IsSolid(block);
        }

        /// <summary>
        /// 检查方块是否是实体的
        /// </summary>
        private bool IsSolid(BlockType block)
        {
            // 简化版本，实际应该查询方块属性
            return block != BlockType.Air &&
                   block != BlockType.Water &&
                   block != BlockType.Lava;
        }

        /// <summary>
        /// 检查是否着地
        /// </summary>
        private void CheckGrounded(Vector3 pos)
        {
            BlockPos feetPos = new BlockPos(
                Mathf.FloorToInt(pos.x),
                Mathf.FloorToInt(pos.y - 0.2f),
                Mathf.FloorToInt(pos.z)
            );

            BlockType blockBelow = WorldManager.Instance.GetBlock(feetPos.X, feetPos.Y, feetPos.Z);
            isOnGround = IsSolid(blockBelow);
        }

        /// <summary>
        /// 处理方块交互
        /// </summary>
        private void HandleBlockInteraction()
        {
            // 左键破坏方块
            if (Input.IsMouseButtonPressed(0))
            {
                BlockPos targetBlock = GetTargetBlock();
                if (targetBlock.X != 0 || targetBlock.Y != 0 || targetBlock.Z != 0)
                {
                    BreakBlock(targetBlock);
                }
            }

            // 右键放置方块
            if (Input.IsMouseButtonPressed(1))
            {
                BlockPos targetBlock = GetTargetBlock();
                BlockPos placePos = targetBlock + GetTargetFaceNormal();
                PlaceBlock(placePos, BlockType.Stone);
            }
        }

        /// <summary>
        /// 获取玩家注视的方块（简化版射线检测）
        /// </summary>
        private BlockPos GetTargetBlock()
        {
            // 简化版本：直接返回玩家前方的方块
            Vector3 rayOrigin = transform.position;
            Vector3 rayDirection = transform.forward;

            // 简单的步进式射线检测
            const float maxDistance = 5f;
            const float stepSize = 0.1f;
            int steps = Mathf.FloorToInt(maxDistance / stepSize);

            for (int i = 0; i < steps; i++)
            {
                Vector3 currentPos = rayOrigin + rayDirection * (i * stepSize);
                BlockPos blockPos = new BlockPos(
                    Mathf.FloorToInt(currentPos.x),
                    Mathf.FloorToInt(currentPos.y),
                    Mathf.FloorToInt(currentPos.z)
                );

                BlockType block = WorldManager.Instance.GetBlock(blockPos.X, blockPos.Y, blockPos.Z);
                if (block != BlockType.Air)
                {
                    return blockPos;
                }
            }

            return new BlockPos(0, -1, 0); // 无效位置
        }

        /// <summary>
        /// 获取目标面的法线方向（简化版）
        /// </summary>
        private BlockPos GetTargetFaceNormal()
        {
            // 简化版本：返回上方向
            return new BlockPos(0, 1, 0);
        }

        /// <summary>
        /// 破坏方块
        /// </summary>
        private void BreakBlock(BlockPos pos)
        {
            WorldManager.Instance.SetBlock(pos.X, pos.Y, pos.Z, BlockType.Air);
            Debug.Log($"Broke block at ({pos.X}, {pos.Y}, {pos.Z})");
        }

        /// <summary>
        /// 放置方块
        /// </summary>
        private void PlaceBlock(BlockPos pos, BlockType blockType)
        {
            // 检查位置是否有效
            BlockType currentBlock = WorldManager.Instance.GetBlock(pos.X, pos.Y, pos.Z);
            if (currentBlock == BlockType.Air)
            {
                WorldManager.Instance.SetBlock(pos.X, pos.Y, pos.Z, blockType);
                Debug.Log($"Placed block at ({pos.X}, {pos.Y}, {pos.Z})");
            }
        }
    }

    /// <summary>
    /// 世界生成器 - 对应 Minecraft: ChunkGenerator
    ///
    /// 使用多线程生成区块数据
    /// </summary>
    public class WorldGenerator : ScriptBehaviour
    {
        private int seed = 42;
        private System.Threading.Thread[] generationThreads;
        private System.Collections.Concurrent.ConcurrentQueue<ChunkGenerationTask> taskQueue;
        private bool isRunning = false;

        /// <summary>
        /// 区块生成任务
        /// </summary>
        public struct ChunkGenerationTask
        {
            public int ChunkX;
            public int ChunkZ;
            public System.Action<bool> Callback;
        }

        public void Start()
        {
            Debug.Log("WorldGenerator initialized with seed: " + seed);

            // 启动多线程生成
            int threadCount = System.Environment.ProcessorCount;
            generationThreads = new System.Threading.Thread[threadCount];
            taskQueue = new System.Collections.Concurrent.ConcurrentQueue<ChunkGenerationTask>();
            isRunning = true;

            for (int i = 0; i < threadCount; i++)
            {
                generationThreads[i] = new System.Threading.Thread(GenerationWorker);
                generationThreads[i].IsBackground = true;
                generationThreads[i].Start();
            }

            Debug.Log($"Started {threadCount} chunk generation threads");
        }

        public void Update(float deltaTime)
        {
            // 检查任务队列并处理完成的任务
        }

        public void OnDestroy()
        {
            isRunning = false;

            // 等待所有线程结束
            foreach (var thread in generationThreads)
            {
                if (thread.IsAlive)
                {
                    thread.Join(1000);
                }
            }
        }

        /// <summary>
        /// 添加区块生成任务
        /// </summary>
        public void QueueChunkGeneration(int chunkX, int chunkZ, System.Action<bool> callback)
        {
            ChunkGenerationTask task = new ChunkGenerationTask
            {
                ChunkX = chunkX,
                ChunkZ = chunkZ,
                Callback = callback
            };

            taskQueue.Enqueue(task);
        }

        /// <summary>
        /// 生成工作线程
        /// </summary>
        private void GenerationWorker()
        {
            while (isRunning)
            {
                ChunkGenerationTask task;
                if (taskQueue.TryDequeue(out task))
                {
                    bool success = GenerateChunk(task.ChunkX, task.ChunkZ);
                    task.Callback?.Invoke(success);
                }
                else
                {
                    // 队列为空，等待一小段时间
                    System.Threading.Thread.Sleep(10);
                }
            }
        }

        /// <summary>
        /// 生成区块数据（在后台线程执行）
        /// </summary>
        private bool GenerateChunk(int chunkX, int chunkZ)
        {
            try
            {
                // 16x16x256 的区块
                const int sectionCount = 16;
                const int sectionHeight = 16;

                for (int sectionY = 0; sectionY < sectionCount; sectionY++)
                {
                    for (int y = 0; y < sectionHeight; y++)
                    {
                        int worldY = sectionY * sectionHeight + y;

                        for (int z = 0; z < 16; z++)
                        {
                            for (int x = 0; x < 16; x++)
                            {
                                int worldX = chunkX * 16 + x;
                                int worldZ = chunkZ * 16 + z;

                                BlockType block = GenerateBlockAt(worldX, worldY, worldZ);

                                // 将方块数据写入区块（需要调用原生函数）
                                // SetBlockInChunk(chunkX, chunkZ, worldY, x, y, z, block);
                            }
                        }
                    }
                }

                return true;
            }
            catch (System.Exception e)
            {
                Debug.LogError($"Chunk generation failed: {e.Message}");
                return false;
            }
        }

        /// <summary>
        /// 生成指定位置的方块类型
        /// </summary>
        private BlockType GenerateBlockAt(int x, int y, int z)
        {
            // 简单的地形生成
            if (y < 0) return BlockType.Air;
            if (y > 60) return BlockType.Air;

            // 简单的平面地形（实际应使用噪声）
            if (y == 60) return BlockType.GrassBlock;
            if (y > 50 && y < 60) return BlockType.Dirt;
            if (y <= 50) return BlockType.Stone;

            return BlockType.Air;
        }
    }
}
