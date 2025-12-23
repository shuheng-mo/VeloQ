# VeloQ 项目结构说明

本文档描述 VeloQ 项目的目录结构和文件组织。

## 目录树

```
veloq/
├── CMakeLists.txt              # 根 CMake 配置文件
├── LICENSE                      # MIT 开源协议
├── README.md                    # 项目说明文档
├── prd.md                       # 产品需求文档
├── PROJECT_STRUCTURE.md         # 本文件
│
├── config/                      # 配置文件目录
│   └── veloq.example.ini        # 示例配置文件
│
├── docs/                        # 文档目录
│   ├── api/                     # API 文档
│   ├── architecture/            # 架构设计文档
│   ├── build/                   # 构建相关文档
│   │   └── dependencies.md      # 依赖安装指南
│   └── user_guide/              # 用户指南
│
├── examples/                    # 示例程序
│   └── README.md                # 示例说明文档
│
├── include/veloq/               # 公共头文件（对外接口）
│   ├── common/                  # 公共类型和工具
│   │   ├── types.hpp            # 基础类型定义（Timestamp, Price, MarketTick 等）
│   │   └── lockfree_queue.hpp   # 无锁队列实现
│   │
│   ├── gateway/                 # CTP 网关模块
│   │   └── ctp_gateway.hpp      # CTP 网关接口
│   │
│   ├── feature_engine/          # 特征计算模块
│   │   └── features.hpp         # 特征引擎接口（OFI, VWAP 等）
│   │
│   ├── inference/               # AI 推断模块
│   │   └── model.hpp            # ONNX 推断引擎接口
│   │
│   ├── ipc_bridge/              # 进程间通信模块
│   │   └── shared_memory.hpp    # 共享内存接口
│   │
│   └── dashboard/               # 可视化模块
│       └── renderer.hpp         # Dear ImGui 渲染器接口
│
├── src/                         # 源代码实现
│   ├── common/                  # 公共模块实现
│   │   ├── CMakeLists.txt       # 模块构建配置
│   │   ├── include/             # 模块内部头文件
│   │   ├── src/                 # 源文件
│   │   │   └── placeholder.cpp  # 占位符
│   │   └── tests/               # 单元测试
│   │
│   ├── gateway/                 # CTP 网关实现
│   │   ├── CMakeLists.txt
│   │   ├── include/             # 模块内部头文件
│   │   ├── src/
│   │   │   └── ctp_gateway.cpp  # CTP 网关实现
│   │   └── tests/
│   │
│   ├── feature_engine/          # 特征计算实现
│   │   ├── CMakeLists.txt
│   │   ├── include/
│   │   ├── src/
│   │   │   └── features.cpp     # 特征计算实现
│   │   └── tests/
│   │
│   ├── inference/               # AI 推断实现
│   │   ├── CMakeLists.txt
│   │   ├── include/
│   │   ├── src/
│   │   │   └── model.cpp        # ONNX 推断实现
│   │   └── tests/
│   │
│   ├── ipc_bridge/              # IPC 实现
│   │   ├── CMakeLists.txt
│   │   ├── include/
│   │   ├── src/
│   │   │   └── shared_memory.cpp # 共享内存实现
│   │   └── tests/
│   │
│   └── dashboard/               # Dashboard 实现
│       ├── CMakeLists.txt
│       ├── include/
│       ├── src/
│       │   ├── main.cpp         # Dashboard 主程序
│       │   └── renderer.cpp     # 渲染器实现
│       └── tests/
│
└── third_party/                 # 第三方库（需手动下载）
    └── README.md                # 第三方库安装指南
```

## 模块说明

### 1. Common（公共模块）

**职责**：提供跨模块共享的基础设施

**关键文件**：

- `types.hpp` - 基础类型定义（价格、时间戳、行情结构等）
- `lockfree_queue.hpp` - 高性能无锁队列

**依赖**：Boost, 标准库

### 2. Gateway（网关模块）

**职责**：封装 CTP API，接收实时行情数据

**关键文件**：

- `ctp_gateway.hpp/cpp` - CTP 网关实现

**依赖**：Common, CTP API

**性能目标**：行情接收延迟 < 10μs

### 3. Feature Engine（特征引擎模块）

**职责**：实时计算市场微观结构特征

**关键文件**：

- `features.hpp/cpp` - 特征计算引擎

**支持特征**：

- OFI (Order Flow Imbalance)
- 盘口压力
- 买卖价差
- VWAP (Volume Weighted Average Price)

**依赖**：Common

**性能目标**：特征计算延迟 < 100μs

### 4. Inference（推断模块）

**职责**：基于 ONNX Runtime 的 AI 实时推断

**关键文件**：

- `model.hpp/cpp` - 推断引擎

**依赖**：Common, Feature Engine, ONNX Runtime

**性能目标**：推断延迟 < 500μs

### 5. IPC Bridge（进程间通信模块）

**职责**：通过共享内存与 Python 策略通信

**关键文件**：

- `shared_memory.hpp/cpp` - 共享内存实现

**依赖**：Common, Boost.Interprocess

**性能目标**：通信延迟 < 10μs (零拷贝)

### 6. Dashboard（可视化模块）

**职责**：使用 Dear ImGui 实时可视化

**关键文件**：

- `renderer.hpp/cpp` - ImGui 渲染器
- `main.cpp` - Dashboard 主程序

**功能**：

- 盘口热力图
- AI 预测曲线
- 系统延迟监控
- 特征值时间线

**依赖**：所有模块, Dear ImGui, GLFW, OpenGL

## 编译产物

构建后的输出：

```
build/
├── lib/                        # 静态库
│   ├── libveloq_common.a
│   ├── libveloq_gateway.a
│   ├── libveloq_feature_engine.a
│   ├── libveloq_inference.a
│   └── libveloq_ipc_bridge.a
│
├── bin/                        # 可执行文件
│   └── veloq_dashboard
│
└── examples/                   # 示例程序
    ├── gateway_example
    ├── feature_engine_example
    └── ...
```

## 配置文件

### veloq.ini (从 veloq.example.ini 复制)

包含运行时配置：

- CTP 连接参数
- 订阅合约列表
- 特征计算参数
- AI 模型路径
- 共享内存配置
- 日志设置

## 扩展指南

### 添加新的特征

1. 在 `include/veloq/feature_engine/features.hpp` 添加特征字段
2. 在 `src/feature_engine/src/features.cpp` 实现计算逻辑
3. 更新 `MarketFeatures` 结构体
4. 编写单元测试

### 添加新的 AI 模型

1. 训练并导出 ONNX 格式模型
2. 放置到 `models/` 目录
3. 修改 `config/veloq.ini` 中的 `model_path`
4. 确保输入特征与模型输入匹配

### 集成其他数据源

1. 在 `src/` 下创建新模块目录
2. 实现与 Gateway 类似的接口
3. 添加 CMakeLists.txt
4. 更新根 CMakeLists.txt

## 设计原则

1. **模块化**：每个模块独立编译为静态库
2. **低耦合**：模块间通过头文件接口通信
3. **高性能**：关键路径使用无锁数据结构和 SIMD 优化
4. **可测试**：每个模块都有独立的测试目录
5. **可配置**：运行时参数通过配置文件管理

## 开发工作流

1. **修改代码** → 2. **增量编译** → 3. **运行测试** → 4. **性能分析** → 5. **提交代码**

```bash
# 增量编译
cd build
cmake --build . -j8

# 运行测试
ctest --output-on-failure

# 性能分析（可选）
cmake .. -DCMAKE_BUILD_TYPE=RelWithDebInfo
perf record ./bin/veloq_dashboard
perf report
```
