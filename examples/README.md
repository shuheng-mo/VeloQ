# VeloQ 示例程序

本目录包含 VeloQ 各模块的示例代码，帮助您快速上手。

## 示例列表

### 1. Gateway 示例 (Coming Soon)

`gateway_example.cpp` - 演示如何使用 CTP Gateway 连接 SimNow 并接收行情

**预期功能**：
- 连接到 SimNow 7x24 仿真环境
- 订阅指定合约
- 接收并打印实时行情数据

### 2. Feature Engine 示例 (Coming Soon)

`feature_engine_example.cpp` - 演示特征计算引擎的使用

**预期功能**：
- 从行情数据计算 OFI
- 计算盘口压力和价差
- 实时更新 VWAP

### 3. Inference 示例 (Coming Soon)

`inference_example.cpp` - 演示 AI 推断引擎的使用

**预期功能**：
- 加载 ONNX 模型
- 对实时特征进行推断
- 输出价格趋势预测

### 4. IPC Bridge 示例 (Coming Soon)

`ipc_bridge_example.cpp` - 演示与 Python 的进程间通信

**预期功能**：
- 创建共享内存段
- 写入特征和预测数据
- Python 端读取示例

### 5. 完整流程示例 (Coming Soon)

`full_pipeline_example.cpp` - 演示完整的数据处理流程

**预期功能**：
- Gateway -> Feature Engine -> Inference -> IPC Bridge
- 端到端延迟测量
- 性能指标输出

## 构建示例

示例程序在主项目构建时自动编译：

```bash
cd build
cmake .. -DBUILD_EXAMPLES=ON
cmake --build .

# 示例可执行文件位于 build/examples/ 目录
```

## 运行示例

### 准备工作

1. 注册 SimNow 账号：http://www.simnow.com.cn/
2. 修改配置文件：
   ```bash
   cp ../config/veloq.example.ini ../config/veloq.ini
   # 编辑 veloq.ini，填入你的 SimNow 账号信息
   ```

### 运行

```bash
# 从 build 目录运行
cd examples

# 运行 Gateway 示例
./gateway_example

# 运行完整流程示例
./full_pipeline_example --config ../config/veloq.ini
```

## 示例说明

所有示例都包含详细的注释，说明每个步骤的作用。建议按以下顺序学习：

1. **gateway_example** - 了解行情接收基础
2. **feature_engine_example** - 学习特征计算
3. **inference_example** - 掌握 AI 推断
4. **ipc_bridge_example** - 理解进程间通信
5. **full_pipeline_example** - 整合所有模块

## Python 集成示例

`python/` 子目录包含 Python 端示例：

```bash
cd python
pip install -r requirements.txt
python read_shared_memory.py
```

## 故障排除

### 连接 CTP 失败

**症状**：`gateway_example` 报错 "Failed to connect to CTP"

**可能原因**：
1. SimNow 账号信息错误
2. 网络连接问题
3. CTP 库版本不兼容

**解决方案**：
1. 检查 `config/veloq.ini` 中的账号信息
2. 确认可以访问 SimNow 前置地址
3. 下载最新版本的 CTP API

### 找不到 ONNX 模型

**症状**：`inference_example` 报错 "Model file not found"

**解决方案**：
1. 确保 `models/` 目录存在
2. 下载或训练一个 ONNX 模型并放入该目录
3. 修改配置文件中的 `model_path`

### 共享内存访问失败

**症状**：`ipc_bridge_example` 报错 "Failed to create shared memory"

**可能原因**：
1. 权限不足
2. 共享内存名称冲突

**解决方案**：
```bash
# 清理残留的共享内存（Linux）
ipcs -m | grep veloq
ipcrm -m <shmid>

# 或修改配置中的 shm_name
```

---

更多信息请参考 [API 文档](../docs/api/)。
