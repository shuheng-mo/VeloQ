# 第三方库依赖

VeloQ 项目依赖以下第三方库，请按照下述指南手动下载并放置到相应目录。

## 目录结构

```
third_party/
├── ctp/                    # CTP API
│   ├── include/
│   │   ├── ThostFtdcMdApi.h
│   │   └── ThostFtdcUserApiStruct.h
│   └── lib/
│       └── thostmduserapi_se.so (或 .dll/.dylib)
│
├── imgui/                  # Dear ImGui
│   ├── imgui.h
│   ├── imgui.cpp
│   ├── imgui_draw.cpp
│   ├── imgui_tables.cpp
│   ├── imgui_widgets.cpp
│   └── backends/
│       ├── imgui_impl_glfw.cpp
│       └── imgui_impl_opengl3.cpp
│
├── onnxruntime/            # ONNX Runtime
│   ├── include/
│   │   └── onnxruntime_cxx_api.h
│   └── lib/
│       └── libonnxruntime.so (或 .dll/.dylib)
│
└── spdlog/                 # spdlog
    └── include/
        └── spdlog/
```

## 下载指南

### 1. CTP API

**官方网站**：[上海期货交易所](http://www.sfit.com.cn/)

**SimNow 仿真平台**（推荐用于开发测试）：
- 访问 [SimNow](http://www.simnow.com.cn/)
- 注册账号并下载 CTP API
- 版本推荐：CTP 6.6.9 或更高

**安装步骤**：
```bash
# 1. 下载 CTP API 压缩包
# 2. 解压到 third_party/ctp/
cd third_party
mkdir -p ctp/{include,lib}

# 3. 将头文件复制到 ctp/include/
# 4. 将 .so/.dll/.dylib 文件复制到 ctp/lib/
```

### 2. Dear ImGui

**官方仓库**：[ocornut/imgui](https://github.com/ocornut/imgui)

**版本要求**：v1.89 或更高

**安装步骤**：
```bash
cd third_party
git clone https://github.com/ocornut/imgui.git
cd imgui
git checkout v1.90  # 或最新稳定版本

# imgui 是 header-only + source 形式，无需编译
# 项目会直接包含源文件编译
```

**注意**：Dashboard 模块还需要 GLFW 和 OpenGL 库，请通过系统包管理器安装：
```bash
# macOS
brew install glfw

# Ubuntu/Debian
sudo apt-get install libglfw3-dev libgl1-mesa-dev

# Windows (使用 vcpkg)
vcpkg install glfw3 opengl
```

### 3. ONNX Runtime

**官方网站**：[microsoft/onnxruntime](https://github.com/microsoft/onnxruntime)

**版本要求**：v1.16 或更高

**安装步骤**：

**方式一：下载预编译版本（推荐）**
```bash
# 访问 Release 页面
# https://github.com/microsoft/onnxruntime/releases

# 下载对应平台的预编译包，例如：
# onnxruntime-linux-x64-1.16.0.tgz
# onnxruntime-osx-arm64-1.16.0.tgz
# onnxruntime-win-x64-1.16.0.zip

cd third_party
mkdir -p onnxruntime
tar -xzf onnxruntime-*.tgz -C onnxruntime --strip-components=1
```

**方式二：使用包管理器**
```bash
# Ubuntu/Debian (通过 pip 安装后查找库文件)
pip install onnxruntime

# 或从源码编译（耗时较长）
git clone --recursive https://github.com/microsoft/onnxruntime
cd onnxruntime
./build.sh --config Release --build_shared_lib --parallel
```

### 4. spdlog

**官方仓库**：[gabime/spdlog](https://github.com/gabime/spdlog)

**版本要求**：v1.11 或更高

**安装步骤**：

**方式一：Header-only 模式（推荐，无需编译）**
```bash
cd third_party
git clone https://github.com/gabime/spdlog.git
cd spdlog
git checkout v1.12.0  # 或最新稳定版本
```

**方式二：使用包管理器**
```bash
# macOS
brew install spdlog

# Ubuntu/Debian
sudo apt-get install libspdlog-dev

# 如果使用系统安装的 spdlog，请修改 CMakeLists.txt
# 将 include_directories 改为 find_package(spdlog REQUIRED)
```

## 依赖检查

安装完所有依赖后，运行以下脚本检查：

```bash
# 从项目根目录运行
ls -R third_party/

# 预期输出应包含所有必需的头文件和库文件
```

## 可选依赖

以下依赖在某些模块可选：

- **GoogleTest**（用于单元测试）：
  ```bash
  cd third_party
  git clone https://github.com/google/googletest.git
  ```

- **Benchmark**（用于性能测试）：
  ```bash
  cd third_party
  git clone https://github.com/google/benchmark.git
  ```

## 故障排除

### 找不到 CTP 库文件

**症状**：编译时报错 `cannot find -lthostmduserapi_se`

**解决方案**：
1. 确认 `third_party/ctp/lib/` 目录存在
2. 确认库文件名正确（Linux: `.so`, macOS: `.dylib`, Windows: `.dll`）
3. 在 `src/gateway/CMakeLists.txt` 中检查链接路径

### ONNX Runtime 版本不兼容

**症状**：运行时报错 `version mismatch`

**解决方案**：
1. 确保下载的是与你的系统架构匹配的版本（x64/arm64）
2. 检查 C++ 运行时库版本（MSVC/GCC/Clang）
3. 尝试从源码编译以匹配你的编译器

### Dear ImGui 找不到后端实现

**症状**：编译时报错 `imgui_impl_glfw.h: No such file`

**解决方案**：
1. 确认 `third_party/imgui/backends/` 目录存在
2. 安装 GLFW 和 OpenGL 开发库（见上述安装步骤）

## 许可证说明

- **CTP API**：上海期货交易所版权所有，仅限开发使用
- **Dear ImGui**：MIT License
- **ONNX Runtime**：MIT License
- **spdlog**：MIT License

请遵守各依赖库的许可证要求。

---

如有问题，请查阅 [构建文档](../docs/build/) 或提交 Issue。
