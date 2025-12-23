# 贡献指南

首先，感谢你考虑为 VeloQ 做出贡献！正是像你这样的人让 VeloQ 成为如此优秀的工具。

以下是一套指导方针，帮助你为 VeloQ 做出贡献。这些主要是指导方针，而不是规则。请根据自己的判断行事，并随时提出对本文档的改进建议。

## 目录

- [行为准则](#行为准则)
- [我能做什么贡献？](#我能做什么贡献)
  - [报告 Bug](#报告-bug)
  - [建议功能增强](#建议功能增强)
  - [代码贡献](#代码贡献)
  - [改进文档](#改进文档)
- [开发环境设置](#开发环境设置)
- [开发流程](#开发流程)
- [代码规范](#代码规范)
- [提交信息规范](#提交信息规范)
- [Pull Request 流程](#pull-request-流程)
- [测试要求](#测试要求)
- [社区](#社区)

---

## 行为准则

本项目及其所有参与者均受 [行为准则](CODE_OF_CONDUCT.md) 的约束。参与本项目即表示你同意遵守其条款。如果你发现不可接受的行为，请报告至项目维护者。

### 我们的承诺

为了营造开放和友好的环境，我们作为贡献者和维护者承诺：无论年龄、体型、残疾、种族、性别认同和表达、经验水平、国籍、个人外貌、种族、宗教或性取向如何，参与我们的项目和社区对每个人来说都是无骚扰的体验。

---

## 我能做什么贡献？

### 报告 Bug

如果你发现了一个 Bug，请通过 [GitHub Issues](https://github.com/yourusername/veloq/issues) 提交报告。

**在提交 Bug 报告之前：**

1. 检查 [现有 Issues](https://github.com/yourusername/veloq/issues) 确保该问题尚未被报告
2. 确定这是一个 Bug 而不是预期行为（可以先在文档中查找）
3. 收集关于该问题的详细信息

**提交高质量的 Bug 报告：**

Bug 报告应该包含以下信息：

```markdown
**描述 Bug**
清晰简洁地描述这个 Bug 是什么。

**重现步骤**
重现该行为的步骤：
1. 执行命令 '...'
2. 使用配置 '....'
3. 观察到错误

**预期行为**
清晰简洁地描述你期望发生的事情。

**实际行为**
清晰简洁地描述实际发生的事情。

**环境信息**
- OS: [例如 Ubuntu 22.04, macOS 13.0]
- 编译器: [例如 GCC 11.3, Clang 14.0]
- CMake 版本: [例如 3.25.0]
- VeloQ 版本: [例如 1.0.0-alpha]

**日志输出**
如果适用，请粘贴相关的日志输出（使用代码块）。

**额外信息**
添加任何其他关于该问题的信息。
```

### 建议功能增强

如果你有新功能的想法，我们很乐意听到！请通过 [GitHub Issues](https://github.com/yourusername/veloq/issues) 提交功能请求。

**提交功能请求时：**

```markdown
**问题陈述**
描述你想要解决的问题。例如："我在使用 [...] 时总是很沮丧，因为 [...]"

**建议的解决方案**
清晰简洁地描述你希望发生的事情。

**替代方案**
清晰简洁地描述你考虑过的任何替代解决方案或功能。

**使用场景**
描述这个功能将如何使用的具体场景。

**额外信息**
添加任何其他关于功能请求的信息或截图。
```

### 代码贡献

我们欢迎代码贡献！无论是修复 Bug、实现新功能还是改进性能。

**第一次贡献？**

寻找标记为 `good first issue` 或 `help wanted` 的 Issues。这些是特别适合新贡献者的任务。

**贡献类型：**

- 🐛 Bug 修复
- ✨ 新功能
- ⚡ 性能优化
- 📝 文档改进
- 🎨 代码重构
- ✅ 测试增强

### 改进文档

文档和代码一样重要！你可以通过以下方式改进文档：

- 修复拼写错误或语法错误
- 改进现有文档的清晰度
- 添加缺失的文档
- 添加代码示例
- 翻译文档

---

## 开发环境设置

### 前置要求

确保你的系统满足以下要求：

- C++17 兼容的编译器（GCC 7+, Clang 6+, MSVC 2019+）
- CMake 3.20+
- Git 2.0+
- Boost 1.70+

详细依赖安装请参考 [docs/build/dependencies.md](docs/build/dependencies.md)

### Fork 和 Clone 仓库

1. 在 GitHub 上 Fork 本仓库
2. Clone 你的 Fork：

```bash
git clone https://github.com/YOUR_USERNAME/veloq.git
cd veloq
```

3. 添加上游仓库：

```bash
git remote add upstream https://github.com/yourusername/veloq.git
```

### 构建项目

```bash
# 创建构建目录
mkdir build && cd build

# 配置 CMake（开启所有检查）
cmake .. \
  -DCMAKE_BUILD_TYPE=Debug \
  -DBUILD_TESTS=ON \
  -DENABLE_SANITIZERS=ON

# 编译
cmake --build . -j8

# 运行测试
ctest --output-on-failure
```

### 开发工具推荐

- **IDE**: CLion, VSCode with C++ extensions, Visual Studio
- **代码格式化**: clang-format
- **静态分析**: clang-tidy, cppcheck
- **调试器**: gdb, lldb
- **性能分析**: valgrind, perf, google-perftools

---

## 开发流程

### 1. 创建分支

始终在新分支上工作，不要直接在 `main` 分支上提交：

```bash
# 更新 main 分支
git checkout main
git pull upstream main

# 创建新分支
git checkout -b feature/my-awesome-feature
# 或
git checkout -b fix/issue-123
```

分支命名约定：
- `feature/` - 新功能
- `fix/` - Bug 修复
- `perf/` - 性能优化
- `refactor/` - 代码重构
- `docs/` - 文档更新
- `test/` - 测试相关

### 2. 进行更改

- 保持提交的原子性（一个提交只做一件事）
- 编写清晰的提交信息（见下文）
- 经常提交，不要积累太多更改

### 3. 测试你的更改

```bash
# 运行所有测试
cd build
ctest --output-on-failure

# 运行特定模块的测试
./src/gateway/veloq_gateway_tests

# 使用内存检查工具
cmake .. -DENABLE_SANITIZERS=ON
cmake --build .
ctest

# 性能测试（如果相关）
./benchmarks/feature_engine_benchmark
```

### 4. 保持同步

在提交 PR 之前，确保你的分支与上游 `main` 分支同步：

```bash
git fetch upstream
git rebase upstream/main
```

如果有冲突，解决它们并继续 rebase。

---

## 代码规范

### C++ 风格指南

VeloQ 遵循 [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html)，但有一些调整：

#### 命名约定

```cpp
// 类名：大驼峰
class MarketDataGateway {};

// 函数名：小驼峰或蛇形命名
void processMarketTick();
void process_market_tick();  // 也可接受

// 变量名：蛇形命名
int order_count;
double last_price;

// 常量：大写蛇形命名
constexpr int MAX_QUEUE_SIZE = 1024;

// 命名空间：小写
namespace veloq {
namespace gateway {
}
}

// 私有成员变量：下划线后缀
class MyClass {
private:
    int value_;
};
```

#### 代码格式化

使用 `clang-format` 格式化代码：

```bash
# 格式化单个文件
clang-format -i src/gateway/ctp_gateway.cpp

# 格式化所有源文件
find src include -name "*.cpp" -o -name "*.hpp" | xargs clang-format -i
```

`.clang-format` 配置已包含在项目根目录。

#### 代码质量

- **避免过早优化**：首先编写清晰正确的代码，然后在必要时优化
- **RAII**：使用 RAII 管理资源（智能指针、RAII 包装器）
- **const 正确性**：尽可能使用 const
- **避免原始指针**：优先使用智能指针（`std::unique_ptr`, `std::shared_ptr`）
- **异常安全**：确保代码在异常情况下也能正确清理资源
- **头文件保护**：使用 `#pragma once`

#### 性能关键代码

对于性能关键路径（如 Gateway、Feature Engine）：

- 避免不必要的内存分配
- 使用内存对齐优化 Cache 访问
- 考虑使用 SIMD 指令
- 避免虚函数调用的开销（如果可能）
- 使用 inline 关键字提示编译器

#### 注释

```cpp
// 好的注释解释"为什么"，而不是"做什么"
// Bad: Increment counter
counter++;

// Good:
// Reset counter after every 100 ticks to prevent overflow
if (counter >= 100) {
    counter = 0;
}
```

对于公共 API，使用 Doxygen 风格的注释：

```cpp
/**
 * @brief Connects to CTP server and authenticates
 *
 * @param front_addr CTP front server address (e.g., "tcp://127.0.0.1:10131")
 * @param broker_id Broker ID
 * @param user_id User ID for authentication
 * @param password User password
 * @return true if connection successful, false otherwise
 *
 * @note This function is blocking and may take several seconds
 * @throws std::runtime_error if CTP API initialization fails
 */
bool connect(const std::string& front_addr,
             const std::string& broker_id,
             const std::string& user_id,
             const std::string& password);
```

---

## 提交信息规范

VeloQ 遵循 [Conventional Commits](https://www.conventionalcommits.org/) 规范。

### 格式

```
<type>(<scope>): <subject>

<body>

<footer>
```

### Type（类型）

- `feat`: 新功能
- `fix`: Bug 修复
- `perf`: 性能优化
- `refactor`: 重构（既不修复 Bug 也不添加功能）
- `docs`: 文档更新
- `test`: 添加或修改测试
- `build`: 影响构建系统或外部依赖的更改
- `ci`: CI 配置文件和脚本的更改
- `chore`: 其他不修改 src 或 test 文件的更改
- `style`: 不影响代码含义的更改（空格、格式化等）

### Scope（范围）

可选，指明更改影响的模块：

- `gateway`
- `feature-engine`
- `inference`
- `ipc-bridge`
- `dashboard`
- `common`
- `build`
- `deps`

### Subject（主题）

- 使用祈使句，现在时："add" 而不是 "added" 或 "adds"
- 不要大写首字母
- 不要以句号结尾
- 限制在 50 个字符以内

### Body（正文）

- 可选，提供更详细的说明
- 使用祈使句，现在时
- 解释"是什么"和"为什么"，而不是"怎么做"
- 每行限制在 72 个字符以内

### Footer（页脚）

- 可选，用于引用 Issues 或说明破坏性更改
- 格式：`Closes #123` 或 `Fixes #456`
- 破坏性更改：`BREAKING CHANGE: description`

### 示例

```
feat(gateway): add support for multiple CTP connections

Allow users to connect to multiple CTP servers simultaneously
for redundancy and load balancing.

- Add ConnectionPool class to manage multiple connections
- Update CtpGateway to use connection pool
- Add configuration options for connection pooling

Closes #42
```

```
fix(feature-engine): correct OFI calculation for edge cases

The OFI calculation was incorrect when bid/ask volumes were zero,
leading to division by zero errors.

This fix adds checks for zero volumes and returns NaN in such cases,
which is the expected behavior according to the OFI specification.

Fixes #89
```

```
perf(inference): optimize ONNX model inference with batch processing

Reduce inference latency by 40% by batching multiple predictions
and leveraging ONNX Runtime's batch processing capabilities.

Benchmark results:
- Before: 120μs per prediction
- After: 72μs per prediction

BREAKING CHANGE: InferenceEngine::predict() now requires a batch size parameter
```

---

## Pull Request 流程

### 1. 准备你的 PR

在提交 PR 之前：

- ✅ 确保所有测试通过
- ✅ 添加新测试覆盖你的更改
- ✅ 更新相关文档
- ✅ 代码已格式化（使用 clang-format）
- ✅ 提交信息遵循规范
- ✅ 与上游 main 分支同步

### 2. 创建 Pull Request

```bash
# 推送你的分支到 Fork
git push origin feature/my-awesome-feature
```

然后在 GitHub 上创建 Pull Request。

### 3. PR 描述模板

```markdown
## 描述

简要描述这个 PR 做了什么。

## 动机和背景

解释为什么需要这个更改。如果它修复了一个已有的 Issue，请链接该 Issue。

Closes #123

## 更改类型

- [ ] Bug 修复（非破坏性更改，修复一个 Issue）
- [ ] 新功能（非破坏性更改，添加功能）
- [ ] 破坏性更改（修复或功能会导致现有功能不按预期工作）
- [ ] 文档更新
- [ ] 性能优化
- [ ] 代码重构

## 测试

描述你进行的测试以验证你的更改。

- [ ] 单元测试通过
- [ ] 集成测试通过
- [ ] 性能测试通过（如果相关）
- [ ] 内存检查通过（AddressSanitizer/Valgrind）

## 检查清单

- [ ] 我的代码遵循项目的代码风格
- [ ] 我已进行自我审查
- [ ] 我已注释了难以理解的代码
- [ ] 我已更新了相关文档
- [ ] 我的更改没有产生新的警告
- [ ] 我已添加测试证明我的修复有效或我的功能工作正常
- [ ] 新的和现有的单元测试在本地通过
- [ ] 任何依赖的更改已被合并和发布

## 截图（如果适用）

添加截图以帮助解释你的更改。

## 其他信息

添加关于 PR 的任何其他信息。
```

### 4. 代码审查

- 响应审查意见并进行必要的更改
- 将新的提交推送到同一分支，它们会自动添加到 PR
- 感谢审查者的时间和反馈

### 5. 合并

一旦你的 PR 被批准：

- 维护者将合并你的 PR
- 你的更改将出现在下一个版本中
- 你将被添加到贡献者列表中！

---

## 测试要求

### 单元测试

- 所有新代码都应该有相应的单元测试
- 测试应该快速、独立且可重复
- 使用有意义的测试名称

```cpp
TEST(FeatureEngineTest, ComputeOFI_ZeroVolumes_ReturnsNaN) {
    FeatureEngine engine;
    MarketTick tick{};
    // ... setup tick with zero volumes ...

    auto features = engine.compute(tick);

    EXPECT_TRUE(std::isnan(features.ofi));
}
```

### 集成测试

- 测试模块之间的交互
- 放在 `tests/integration/` 目录

### 性能测试

- 对于性能关键代码，添加性能基准测试
- 使用 Google Benchmark 框架
- 在 PR 描述中包含性能对比

```cpp
static void BM_FeatureComputation(benchmark::State& state) {
    FeatureEngine engine;
    MarketTick tick = GenerateTestTick();

    for (auto _ : state) {
        auto features = engine.compute(tick);
        benchmark::DoNotOptimize(features);
    }
}
BENCHMARK(BM_FeatureComputation);
```

### 测试覆盖率

- 目标：核心代码 >80% 覆盖率
- 使用 gcov/lcov 生成覆盖率报告

```bash
cmake .. -DCMAKE_BUILD_TYPE=Debug -DENABLE_COVERAGE=ON
cmake --build .
ctest
gcovr -r .. --html --html-details -o coverage.html
```

---

## 社区

### 获取帮助

- 📖 阅读 [文档](docs/)
- 💬 提交 [Issue](https://github.com/yourusername/veloq/issues)
- 📧 联系维护者：[@shuheng-mo](https://github.com/shuheng-mo)

### 认可贡献者

我们使用 [All Contributors](https://allcontributors.org/) 规范来认可所有类型的贡献。

贡献类型包括但不限于：
- 💻 代码
- 📖 文档
- 🐛 Bug 报告
- 💡 想法和建议
- 🤔 回答问题
- 📢 宣传
- 🎨 设计

---

## 许可证

通过贡献代码，你同意你的贡献将在 [MIT License](LICENSE) 下授权。

---

## 问题？

如果你有任何问题或需要澄清，请随时：

- 提交 Issue
- 在 PR 中提问
- 联系维护者

**再次感谢你的贡献！** 🎉

---

<div align="center">

**[返回顶部](#贡献指南)**

Made with ♥ by the VeloQ community

</div>
