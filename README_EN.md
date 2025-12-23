<div align="center">

# VeloQ

> Turbocharge Python Quant Frameworks with C++ - Eliminate High-Frequency Market Data Processing Latency Bottlenecks

[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)
[![Version](https://img.shields.io/badge/version-1.0.0--alpha-orange.svg)](https://github.com/yourusername/veloq/releases)
[![Build Status](https://img.shields.io/badge/build-in%20progress-yellow.svg)](https://github.com/yourusername/veloq/actions)
[![C++](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://en.cppreference.com/w/cpp/17)
[![PRs Welcome](https://img.shields.io/badge/PRs-welcome-brightgreen.svg)](CONTRIBUTING.md)

[English](README_EN.md) | [简体中文](README.md)

</div>

---

## Table of Contents

- [Introduction](#introduction)
- [Key Features](#key-features)
- [Tech Stack](#tech-stack)
- [System Architecture](#system-architecture)
- [Quick Start](#quick-start)
  - [Requirements](#requirements)
  - [Installation](#installation)
- [Usage Guide](#usage-guide)
- [Project Structure](#project-structure)
- [Configuration](#configuration)
- [API Documentation](#api-documentation)
- [Development Guide](#development-guide)
- [Testing](#testing)
- [Deployment](#deployment)
- [Performance Metrics](#performance-metrics)
- [FAQ](#faq)
- [Changelog](#changelog)
- [Contributing](#contributing)
- [License](#license)
- [Contact](#contact)
- [Acknowledgments](#acknowledgments)

---

## Introduction

**VeloQ** (Velocity + Quant) is a **high-performance market data acceleration middleware** designed specifically for quantitative trading.

### What is it?

VeloQ provides microsecond-level (μs) market data parsing, feature engineering, and real-time AI inference through C++, offering "turbo boost" capabilities to existing Python quantitative frameworks (such as vn.py, MiniQMT). It adopts a "sidecar" architecture and seamlessly integrates with Python strategy layers through shared memory, achieving:

- 📡 **Ultra-fast CTP Market Data Reception**: Lock-free queue design ensures network threads are not blocked by computation
- 🧮 **Real-time Feature Computation**: Microstructure features like OFI, book pressure, VWAP completed in sub-milliseconds
- 🤖 **Real-time AI Model Inference**: Based on ONNX Runtime, end-to-end latency < 500μs
- 🔗 **Zero-copy IPC Communication**: Communicate with Python strategies via shared memory, latency < 10μs
- 📊 **Real-time Visualization Monitoring**: Geek-style dashboard built with Dear ImGui

### Why build this?

Quantitative retail traders and small-to-medium institutions in China commonly use Python frameworks to process high-frequency market data. However, Python's GIL (Global Interpreter Lock) and dynamic language characteristics cause significant latency (typically >100ms) in **feature computation** and **real-time AI model inference**, making it difficult to handle volatile market scenarios.

VeloQ aims to solve this pain point by leveraging C++'s performance advantages to reduce critical path latency to the microsecond level while maintaining compatibility with the existing Python ecosystem.

### Use Cases

- **Quantitative Traders**: Need feature computation and AI prediction in high-frequency scenarios
- **HFT Developers**: Pursue extreme low-latency market data processing
- **vn.py / MiniQMT Users**: Want to improve performance without changing existing frameworks
- **Quantitative Researchers**: Need to validate AI model performance in production environments in real-time

---

## Key Features

- ⚡ **Ultra-low Latency** - Total latency from receiving raw data packets to generating AI predictions controlled within 500μs
- 🔒 **Lock-free Design** - Dual-buffer lock-free queue completely decouples producer-consumer
- 🎯 **SIMD Optimization** - Feature computation accelerated with AVX2 instruction set, memory alignment optimizes cache hit rate
- 🧠 **Plug-and-play AI** - Supports ONNX format lightweight deep learning models without recompilation
- 🖥️ **Geek Aesthetics** - Dear ImGui real-time monitoring dashboard displaying order book heatmaps, AI prediction curves, and system latency
- 🐍 **Python Friendly** - Seamlessly integrates with existing Python strategies through shared memory, zero-copy read
- 🛡️ **Production-grade Reliability** - Supports 7×24 hours continuous operation, equipped with Valgrind/ASan memory detection

---

## Tech Stack

### Core Technologies

- [C++ 17/20](https://en.cppreference.com/w/cpp/17) - Modern C++ standards supporting advanced features
- [CMake](https://cmake.org/) - 3.20+ - Cross-platform build system
- [CTP API](http://www.sfit.com.cn/) - Official Shanghai Futures Exchange market data interface
- [Dear ImGui](https://github.com/ocornut/imgui) - 1.89+ - High-performance immediate mode GUI framework
- [ONNX Runtime](https://github.com/microsoft/onnxruntime) - 1.16+ - AI inference engine
- [Boost](https://www.boost.org/) - 1.70+ - Interprocess, System, Thread components
- [spdlog](https://github.com/gabime/spdlog) - 1.11+ - Asynchronous high-performance logging library

### Development Tools

- [Valgrind](https://valgrind.org/) - Memory leak detection
- [AddressSanitizer](https://github.com/google/sanitizers) - Runtime memory error detection
- [Google Test](https://github.com/google/googletest) - Unit testing framework (optional)
- [Google Benchmark](https://github.com/google/benchmark) - Performance testing framework (optional)

---

## System Architecture

```text
┌─────────────────────────────────────────────────────────┐
│                    VeloQ Core (C++)                     │
├──────────────┬──────────────┬──────────────┬────────────┤
│   Gateway    │Feature Engine│  Inference   │ Dashboard  │
│ (CTP API)    │ (OFI/VWAP)   │(ONNX Runtime)│(Dear ImGui)│
└──────┬───────┴──────┬───────┴──────┬───────┴────────────┘
       │              │              │
       └──────────────┴──────────────┘
                      │
              ┌───────▼────────┐
              │  IPC Bridge    │  (Shared Memory, <10μs)
              │ (Boost.IPC)    │
              └───────┬────────┘
                      │
       ┌──────────────▼──────────────┐
       │   Python Strategy Layer     │
       │  (vn.py / MiniQMT / ...)    │
       └─────────────────────────────┘
```

For detailed architecture design, see [PROJECT_STRUCTURE.md](PROJECT_STRUCTURE.md)

---

## Quick Start

### Requirements

Before getting started, ensure your development environment meets the following requirements:

- **Operating System**: Linux (Ubuntu 20.04+) / macOS 11+ / Windows 10+
- **Compiler**: C++17 compatible compiler
  - GCC 7+ / Clang 6+ (Linux/macOS)
  - MSVC 2019+ (Windows)
- **CMake**: >= 3.20
- **Git**: >= 2.0
- **Boost**: >= 1.70 (system, thread, interprocess)
- **CTP API**: Download from [SHFE official website](http://www.sfit.com.cn/) or [SimNow](http://www.simnow.com.cn/)
- **Optional Dependencies**:
  - ONNX Runtime (for AI inference)
  - Dear ImGui + GLFW + OpenGL (for Dashboard)

### Installation

**1. Clone the repository**

```bash
git clone https://github.com/yourusername/veloq.git
cd veloq
```

**2. Install dependencies**

For detailed dependency installation guide, see [docs/build/dependencies.md](docs/build/dependencies.md)

```bash
# Ubuntu/Debian example
sudo apt-get update
sudo apt-get install build-essential cmake libboost-all-dev

# macOS example
brew install cmake boost
```

**3. Prepare third-party libraries**

```bash
# Follow instructions in third_party/README.md to download third-party libraries
mkdir -p third_party
# Place CTP API, Dear ImGui, ONNX Runtime, etc. into third_party/ directory
```

**4. Configure environment**

```bash
# Copy configuration template
cp config/veloq.example.ini config/veloq.ini
# Edit configuration file, fill in SimNow account information
nano config/veloq.ini
```

**5. Build the project**

```bash
mkdir build && cd build
cmake .. \
  -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_TESTS=ON \
  -DBUILD_DASHBOARD=ON
cmake --build . -j8
```

**6. Run tests**

```bash
ctest --output-on-failure
```

**7. Start Dashboard (optional)**

```bash
./bin/veloq_dashboard
```

---

## Usage Guide

### Basic Usage - Gateway Connecting to CTP

```cpp
#include "veloq/gateway/ctp_gateway.hpp"
#include <iostream>

int main() {
    using namespace veloq::gateway;

    // Create CTP Gateway
    CtpGateway gateway;

    // Connect to SimNow 7x24 simulation environment
    bool connected = gateway.connect(
        "tcp://180.168.146.187:10131",  // Front address
        "9999",                          // BrokerID
        "YOUR_USER_ID",                  // Username
        "YOUR_PASSWORD"                  // Password
    );

    if (!connected) {
        std::cerr << "Failed to connect to CTP" << std::endl;
        return 1;
    }

    // Subscribe to instruments
    std::vector<std::string> instruments = {"rb2510", "cu2506"};
    gateway.subscribe(instruments);

    // Start receiving market data with callback
    gateway.start([](const auto& tick) {
        std::cout << "Received tick: " << tick.instrument_id
                  << " price=" << tick.last_price << std::endl;
    });

    // Run main loop...

    return 0;
}
```

### Advanced Usage - Complete Pipeline Example

```cpp
#include "veloq/gateway/ctp_gateway.hpp"
#include "veloq/feature_engine/features.hpp"
#include "veloq/inference/model.hpp"
#include "veloq/ipc_bridge/shared_memory.hpp"

int main() {
    using namespace veloq;

    // 1. Initialize modules
    gateway::CtpGateway gateway;
    feature_engine::FeatureEngine feature_engine;
    inference::InferenceEngine inference_engine;
    ipc_bridge::SharedMemoryBridge ipc_bridge("veloq_shm");

    // 2. Load AI model
    inference_engine.load_model("models/price_predictor.onnx");

    // 3. Initialize shared memory
    ipc_bridge.initialize();

    // 4. Connect to CTP and subscribe
    gateway.connect(/* ... */);
    gateway.subscribe({"rb2510"});

    // 5. Start data processing pipeline
    gateway.start([&](const auto& tick) {
        // Compute features
        auto features = feature_engine.compute(tick);

        // AI inference
        auto prediction = inference_engine.predict(features);

        // Pass to Python via shared memory
        ipc_bridge::SharedData data{features, prediction, /* ... */};
        ipc_bridge.write(data);
    });

    return 0;
}
```

### Python Side Read Example

```python
import mmap
import struct

# Open shared memory
shm = mmap.mmap(-1, 1024, "veloq_shm")

# Read data (zero-copy)
data = struct.unpack('ddddf', shm[:32])
ofi, book_pressure, spread, vwap, up_prob = data

print(f"OFI: {ofi}, Up Probability: {up_prob}")
```

For detailed examples, see [examples/](examples/) directory.

---

## Project Structure

```text
veloq/
├── include/veloq/          # Public headers (external API)
│   ├── common/             # Basic types, lock-free queue
│   ├── gateway/            # CTP gateway interface
│   ├── feature_engine/     # Feature computation engine
│   ├── inference/          # AI inference engine
│   ├── ipc_bridge/         # Inter-process communication
│   └── dashboard/          # Visualization renderer
├── src/                    # Source code implementation
│   ├── common/
│   ├── gateway/
│   ├── feature_engine/
│   ├── inference/
│   ├── ipc_bridge/
│   └── dashboard/
├── third_party/            # Third-party libraries (manual placement required)
│   ├── ctp/                # CTP API
│   ├── imgui/              # Dear ImGui
│   ├── onnxruntime/        # ONNX Runtime
│   └── spdlog/             # spdlog
├── examples/               # Example code
├── docs/                   # Documentation
├── config/                 # Configuration files
├── CMakeLists.txt          # Root build configuration
├── prd.md                  # Product Requirements Document
└── PROJECT_STRUCTURE.md    # Detailed project structure description
```

For complete structure description, see [PROJECT_STRUCTURE.md](PROJECT_STRUCTURE.md)

---

## Configuration

### Configuration File: `config/veloq.ini`

| Configuration Item | Description | Default Value | Required |
|-------------------|-------------|---------------|----------|
| `[Gateway].front_address` | CTP front address | tcp://180.168.146.187:10131 | Yes |
| `[Gateway].broker_id` | BrokerID | 9999 | Yes |
| `[Gateway].user_id` | Username | - | Yes |
| `[Gateway].password` | Password | - | Yes |
| `[Gateway].instruments` | Subscribed instrument list | - | Yes |
| `[FeatureEngine].window_size` | VWAP window size | 100 | No |
| `[Inference].model_path` | ONNX model path | models/price_predictor.onnx | Yes |
| `[IPC].shm_name` | Shared memory name | veloq_shm | No |
| `[Logging].log_level` | Log level | info | No |

For complete configuration description, see `config/veloq.example.ini`

---

## API Documentation

### Core Interfaces

| Module | Class Name | Main Methods | Description |
|--------|------------|--------------|-------------|
| Gateway | `CtpGateway` | `connect()`, `subscribe()`, `start()` | CTP market data reception |
| Feature Engine | `FeatureEngine` | `compute()`, `reset()` | Feature computation |
| Inference | `InferenceEngine` | `load_model()`, `predict()` | AI inference |
| IPC Bridge | `SharedMemoryBridge` | `initialize()`, `write()`, `read()` | Inter-process communication |
| Dashboard | `DashboardRenderer` | `initialize()`, `update()`, `render()` | Visualization |

For detailed API documentation, see [docs/api/](docs/api/) (in development)

---

## Development Guide

### Development Workflow

1. Fork this repository
2. Create a feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit changes (`git commit -m 'feat: add some amazing feature'`)
4. Push to branch (`git push origin feature/AmazingFeature`)
5. Submit Pull Request

### Code Standards

- Follow [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html)
- Use `clang-format` to format code
- Commit messages follow [Conventional Commits](https://www.conventionalcommits.org/)

### Commit Convention

```text
<type>(<scope>): <subject>

<body>

<footer>
```

**Type Categories:**

- `feat`: New feature
- `fix`: Bug fix
- `perf`: Performance optimization
- `refactor`: Refactoring
- `docs`: Documentation update
- `test`: Test related
- `chore`: Build/toolchain update

---

## Testing

### Running Tests

```bash
# Enter build directory
cd build

# Run all tests
ctest --output-on-failure

# Run individual module tests
./src/gateway/veloq_gateway_tests
./src/feature_engine/veloq_feature_engine_tests

# Detect memory issues with AddressSanitizer
cmake .. -DENABLE_SANITIZERS=ON
cmake --build .
ctest
```

### Performance Testing

```bash
# Use Google Benchmark (requires installation first)
cmake .. -DBUILD_BENCHMARKS=ON
cmake --build .
./benchmarks/feature_engine_benchmark
```

### Test Coverage

Current test coverage: In development

Target: Maintain > 80% test coverage for core code

---

## Deployment

### Production Build

```bash
mkdir build-release && cd build-release
cmake .. \
  -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_TESTS=OFF \
  -DBUILD_EXAMPLES=OFF
cmake --build . -j8
sudo cmake --install .
```

### Deploy to Linux Server

<details>
<summary>Expand to view deployment steps</summary>

**1. Install dependencies**

```bash
sudo apt-get update
sudo apt-get install libboost-all-dev
```

**2. Deploy binaries**

```bash
# Copy build artifacts
scp -r build/bin/* user@server:/opt/veloq/bin/
scp -r build/lib/* user@server:/opt/veloq/lib/

# Set library path
echo "export LD_LIBRARY_PATH=/opt/veloq/lib:\$LD_LIBRARY_PATH" >> ~/.bashrc
```

**3. Configure systemd service (optional)**

```ini
[Unit]
Description=VeloQ Market Data Service
After=network.target

[Service]
Type=simple
User=trader
WorkingDirectory=/opt/veloq
ExecStart=/opt/veloq/bin/veloq_service
Restart=on-failure

[Install]
WantedBy=multi-user.target
```

</details>

### Using Docker

<details>
<summary>Expand to view Docker deployment</summary>

**1. Build image**

```bash
docker build -t veloq:latest .
```

**2. Run container**

```bash
docker run -d \
  --name veloq \
  -v $(pwd)/config:/app/config \
  -v $(pwd)/models:/app/models \
  veloq:latest
```

</details>

---

## Performance Metrics

### Latency Performance

| Stage | Target Latency | Measured Latency (In Development) |
|-------|---------------|----------------------------------|
| Gateway Reception | < 10μs | - |
| Feature Computation | < 100μs | - |
| AI Inference | < 500μs | - |
| IPC Communication | < 10μs | - |
| **End-to-end Total Latency** | **< 500μs** | **To be tested** |

### Throughput

- Theoretical maximum processing capacity: >10,000 ticks/s (to be verified)

---

## FAQ

### Q: How to connect to CTP SimNow test environment?

A: First register an account at [SimNow official website](http://www.simnow.com.cn/), then modify `config/veloq.ini`:

```ini
[Gateway]
front_address = tcp://180.168.146.187:10131  # 7x24 market data front
broker_id = 9999
user_id = YOUR_SIMNOW_USER_ID
password = YOUR_SIMNOW_PASSWORD
```

### Q: What to do when CTP library is not found during compilation?

A: Ensure CTP API has been downloaded according to [third_party/README.md](third_party/README.md) and correctly placed in `third_party/ctp/` directory. Check if the link path in `src/gateway/CMakeLists.txt` is correct.

### Q: Dashboard won't start?

A: Dashboard requires Dear ImGui, GLFW and OpenGL support. If visualization is not needed, you can disable it during CMake configuration:

```bash
cmake .. -DBUILD_DASHBOARD=OFF
```

### Q: How to integrate with Python vn.py?

A: VeloQ communicates with Python through shared memory. Python side uses `mmap` module to read data. See example code in [examples/python/](examples/python/) directory for details.

### Q: How to get more help?

A: You can get help through the following channels:

- View [complete documentation](docs/)
- Submit an [Issue](https://github.com/yourusername/veloq/issues)
- Read [PRD document](prd.md) to understand design philosophy

---

## Changelog

For detailed version history, see [CHANGELOG.md](CHANGELOG.md)

### Recent Updates

**v1.0.0-alpha** (2025-06-18)

- ✅ Project skeleton completed
- ✅ Core module interface definitions
- ⏳ Gateway module in development (Week 1-2)
- ⏳ Feature Engine in development (Week 3-6)
- ⏳ AI Inference integration in development (Week 7-8)

### Development Roadmap

See [prd.md - Three-Month Roadmap](prd.md#5-三个月路线图与里程碑-roadmap):

- **Month 1**: CTP Gateway + Basic UI
- **Month 2**: Feature Engineering + AI Model Integration
- **Month 3**: IPC Bridge + Performance Optimization + Open Source Release

---

## Contributing

Welcome any form of contribution! Whether it's reporting bugs, suggesting new features, or directly submitting code.

Please read [CONTRIBUTING.md](CONTRIBUTING.md) for detailed contribution guidelines.

### Contributors

Thanks to all developers who have contributed to this project!

<!-- ALL-CONTRIBUTORS-LIST:START -->
<!-- Contributors list will be auto-generated here -->
<!-- ALL-CONTRIBUTORS-LIST:END -->

---

## License

This project is open-sourced under **MIT License** - see [LICENSE](LICENSE) file for details.

---

## Contact

- **GitHub**: [@shuheng-mo](https://github.com/shuheng-mo)

---

## Acknowledgments

Thanks to the following projects/resources for inspiration and help:

- [vn.py](https://github.com/vnpy/vnpy) - Excellent Python quantitative framework in China, inspiration for this project
- [Dear ImGui](https://github.com/ocornut/imgui) - Simple and efficient immediate mode GUI framework
- [ONNX Runtime](https://github.com/microsoft/onnxruntime) - High-performance AI inference engine
- [Boost C++ Libraries](https://www.boost.org/) - Powerful C++ toolkit
- [spdlog](https://github.com/gabime/spdlog) - Fast, asynchronous C++ logging library
- CTP API - Market data interface provided by Shanghai Futures Exchange

---

## Star History

If this project helps you, please give it a Star! ⭐

[![Star History Chart](https://api.star-history.com/svg?repos=yourusername/veloq&type=Date)](https://star-history.com/#yourusername/veloq&Date)

---

<div align="center">

**[⬆ Back to top](#veloq)**

Made with ♥ by [shuheng-mo](https://github.com/shuheng-mo)

**Note**: This project is for learning and research purposes only. Users bear their own risks for live trading.

</div>
