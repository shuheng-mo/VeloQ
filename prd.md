
# 产品需求文档 (PRD): VeloQ 高性能量化行情加速中间件

| 项目名称 | VeloQ (Velocity + Quant) | 版本 | v1.0.0-Draft |
| --- | --- | --- | --- |
| **状态** | 规划中 / Build in Public | **负责人** | shuheng-mo |
| **目标受众** | 量化交易员、HFT 开发者、vn.py 用户 | **核心价值** | 消除 Python 在高频行情处理中的延迟瓶颈 |

---

## 1. 项目背景与目标 (Executive Summary)

### 1.1 背景

当前国内量化散户及中小型机构多采用 `vn.py` 或 `MiniQMT` 等 Python 框架。然而，在处理 CTP (期货) 或证券 Level 2 等高频逐笔行情时，Python 的全局解释器锁 (GIL) 和动态语言特性导致其在**特征计算**与**AI 模型实时推断**上存在显著延迟（通常 >100ms），难以应对行情剧烈波动。

### 1.2 目标

**VeloQ** 旨在构建一个 C++ 编写的“涡轮增压”中间件，通过“侧边栏”架构挂载于现有 Python 框架旁，实现微秒级（μs）的行情解析、特征工程及 AI 预测，并通过共享内存将结果推送至策略层。

---

## 2. 业务需求与功能模块 (Functional Requirements)

### 2.1 模块 A：CTP 极速网关 (Market Data Gateway)

* **功能：** 封装 CTP API，实现 SimNow 环境下的账户登录、合约订阅及行情接收。
* **技术要求：** 采用双缓冲区无锁队列（Lock-free Queue）进行生产消费解耦，确保网络线程不被计算逻辑阻塞。

### 2.2 模块 B：计算内核 (Feature Engine)

* **功能：** 实时计算盘口微观特征。
* **指标清单：** OFI (Order Flow Imbalance)、盘口压力、买卖价差（Spread）、实时 VWAP。

* **技术要求：** 内存对齐（Cache-friendly）、利用 SIMD 指令集优化向量化计算。

### 2.3 模块 C：AI 实时推断补丁 (Inference Patch)

* **功能：** 集成 **ONNX Runtime**，载入轻量级深度学习模型。
* **输入：** 模块 B 产生的实时特征流。
* **输出：** 未来 5-10 秒的价格趋势预测值（Probability Stream）。

### 2.4 模块 D：可视化看板 (Dashboard)

* **功能：** 使用 **Dear ImGui** 渲染实时盘口热力图、AI 预测曲线及系统延迟监控。
* **特性：** 支持 Windows/macOS 桌面独立运行，具备“极客审美”视觉效果。

### 2.5 模块 E：跨进程通信 (IPC Bridge)

* **功能：** 基于共享内存（Shared Memory）与 Python 策略通讯。
* **技术要求：** 零拷贝（Zero-copy）读取，确保端到端通信延迟低于 10μs。

---

## 3. 技术架构设计 (Technical Architecture)

### 3.1 技术栈 (Tech Stack)

* **核心语言：** 现代 C++ (C++ 17/20)
* **构建系统：** CMake 3.20+
* **关键库：**
* `CTP API`: 官方上期所接口
* `Dear ImGui`: 高性能图形界面
* `ONNX Runtime`: AI 推理引擎
* `Boost.Interprocess`: 进程间共享内存
* `spdlog`: 异步高性能日志

---

## 4. 非功能性需求 (Non-Functional Requirements)

* **延迟指标：** 从接收原始数据包到产生 AI 预测值，总延迟控制在 500μs 以内。
* **稳定性：** 支持 7×24 小时连续运行，具备完善的内存泄漏检测（使用 Valgrind/ASan）。
* **兼容性：** 核心逻辑支持 Linux 服务器部署，可视化前端支持 macOS/Windows 监控。

---

## 5. 前三个月路线图与里程碑 (Roadmap)

### 阶段一：内核构建与“数据哨兵” (Month 1)

* **目标：** 实现 C++ 实时解析 CTP 行情，完成基础 UI 展示。
* **Milestone 1 (W2):** 实现 `VeloQ-Gateway` 稳定登录并能在控制台微秒级打印 L1 行情。
* **Milestone 2 (W4):** `VeloQ-GUI` 初版完成，实现实时跳动的盘口数字及行情波形图。

### 阶段二：特征工程与 AI 注入 (Month 2)

* **目标：** 完成 AI 模型挂载，开启直播验证。
* **Milestone 3 (W6):** 特征计算模块完成，实现 OFI 特征的亚毫秒级计算。
* **Milestone 4 (W8):** **AI 补丁上线。** 看板展示实时预测概率条，开始直播 `Vibe Coding`。

### 阶段三：集成、压测与发布 (Month 3)

* **目标：** 打通 Python 链路，完成开源交付，助力求职。
* **Milestone 5 (W10):** `IPC-Bridge` 联调成功，Python 策略可直接读取 C++ 计算好的 Alpha 信号。
* **Milestone 6 (W12):** 系统压力测试完成，生成性能白皮书，发布 GitHub 仓库。

---

## 6. 风险评估与应对 (Risk Management)

1. **数据延迟：** 若 SimNow 服务器波动，可能导致测试数据异常。应对：增加本地数据回放（Data Replay）模块进行离线调试。
2. **模型准确率：** 初期模型可能存在过拟合。应对：在文档中明确“架构优于算法”，强调 C++ 部署能力而非模型盈利能力。
