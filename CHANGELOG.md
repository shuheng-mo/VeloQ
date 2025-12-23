# Changelog

All notable changes to VeloQ will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Planned

- CTP Gateway implementation with SimNow support
- Feature Engine with OFI, book pressure, VWAP calculations
- ONNX Runtime integration for AI inference
- Dear ImGui dashboard for real-time visualization
- IPC Bridge with shared memory for Python integration

---

## [1.0.0-alpha] - 2025-06-18

### Added

- **Project Structure**: Complete C++ project skeleton with CMake build system
- **Core Modules**: Directory structure for 6 main modules
  - `gateway` - CTP Market Data Gateway
  - `feature_engine` - Real-time feature computation
  - `inference` - AI inference with ONNX Runtime
  - `ipc_bridge` - Inter-process communication
  - `dashboard` - Real-time visualization
  - `common` - Shared utilities and data structures
- **Header Files**: Public API interfaces for all modules
  - `veloq/common/types.hpp` - Basic type definitions
  - `veloq/common/lockfree_queue.hpp` - Lock-free SPSC queue template
  - `veloq/gateway/ctp_gateway.hpp` - CTP gateway interface
  - `veloq/feature_engine/features.hpp` - Feature computation engine
  - `veloq/inference/model.hpp` - ONNX inference engine
  - `veloq/ipc_bridge/shared_memory.hpp` - Shared memory bridge
  - `veloq/dashboard/renderer.hpp` - ImGui renderer interface
- **Build System**: CMake configuration with modern C++17 support
  - Root `CMakeLists.txt` with module organization
  - Per-module `CMakeLists.txt` for independent compilation
  - Build options: `BUILD_TESTS`, `BUILD_DASHBOARD`, `BUILD_EXAMPLES`, `ENABLE_SANITIZERS`
  - Compiler flags for performance optimization (`-O3`, `-march=native`)
- **Documentation**
  - `README.md` - Complete project documentation (Chinese)
  - `README_EN.md` - Complete project documentation (English)
  - `PROJECT_STRUCTURE.md` - Detailed architecture description
  - `prd.md` - Product Requirements Document
  - `third_party/README.md` - Third-party library installation guide
  - `docs/build/dependencies.md` - Dependency installation instructions
  - `examples/README.md` - Example programs guide
  - `CONTRIBUTING.md` - Contribution guidelines
  - `CHANGELOG.md` - This file
- **Configuration**
  - `config/veloq.example.ini` - Example configuration file template
  - `.gitignore` - Comprehensive ignore rules for C++ projects
- **Placeholder Implementations**: Basic source files for all modules
  - Gateway, Feature Engine, Inference, IPC Bridge, Dashboard stubs
  - Compilation-ready but not functionally implemented

### Infrastructure

- **CI/CD**: Ready for GitHub Actions integration
- **Testing**: Test directory structure in place for all modules
- **Examples**: Examples directory with README
- **Third-party Integration**: Prepared structure for CTP API, Dear ImGui, ONNX Runtime, Boost, spdlog

### Development Tools

- Support for AddressSanitizer (`-DENABLE_SANITIZERS=ON`)
- Memory leak detection preparation (Valgrind integration ready)
- SIMD optimization flags for feature computation

---

## Release Schedule

### Phase 1: Core Infrastructure (Month 1)

**Target Date**: Week 1-4

**Milestone 1 (Week 2)**: CTP Gateway MVP
- [ ] CTP API integration
- [ ] SimNow connection and authentication
- [ ] Market data subscription
- [ ] Real-time tick reception
- [ ] Lock-free queue implementation
- [ ] Console-based tick printing (microsecond latency)

**Milestone 2 (Week 4)**: Dashboard MVP
- [ ] Dear ImGui integration with GLFW/OpenGL
- [ ] Basic window and layout
- [ ] Real-time orderbook display
- [ ] Price/volume charts
- [ ] System latency monitoring

### Phase 2: Feature Engineering & AI (Month 2)

**Target Date**: Week 5-8

**Milestone 3 (Week 6)**: Feature Engine
- [ ] OFI (Order Flow Imbalance) calculation
- [ ] Book pressure computation
- [ ] Bid-ask spread tracking
- [ ] Real-time VWAP calculation
- [ ] SIMD optimization for vector operations
- [ ] Sub-millisecond latency validation

**Milestone 4 (Week 8)**: AI Inference Integration
- [ ] ONNX Runtime integration
- [ ] Model loading and validation
- [ ] Real-time prediction pipeline
- [ ] Feature → Inference integration
- [ ] Dashboard prediction visualization
- [ ] End-to-end latency < 500μs validation

### Phase 3: Integration & Performance (Month 3)

**Target Date**: Week 9-12

**Milestone 5 (Week 10)**: IPC Bridge
- [ ] Boost.Interprocess shared memory implementation
- [ ] Zero-copy data structure design
- [ ] Python reader library
- [ ] vn.py integration example
- [ ] IPC latency < 10μs validation

**Milestone 6 (Week 12)**: Release 1.0.0
- [ ] Complete system integration testing
- [ ] Performance benchmarking and white paper
- [ ] Production-ready documentation
- [ ] Example strategies and tutorials
- [ ] Open source release on GitHub
- [ ] Community building and promotion

---

## Version History Summary

| Version | Date | Type | Description |
|---------|------|------|-------------|
| [Unreleased] | - | Development | Planned features for 1.0.0 release |
| [1.0.0-alpha] | 2025-06-18 | Alpha | Project skeleton and architecture |

---

## Version Numbering

VeloQ follows [Semantic Versioning](https://semver.org/):

- **MAJOR** version for incompatible API changes
- **MINOR** version for backwards-compatible functionality additions
- **PATCH** version for backwards-compatible bug fixes

Pre-release versions:
- **alpha**: Early development, may be unstable
- **beta**: Feature-complete, testing phase
- **rc** (release candidate): Final testing before stable release

Example: `1.2.3-beta.1` = Version 1.2.3, beta pre-release 1

---

## Migration Guides

### From 0.x to 1.0

*To be written when 1.0.0 is released*

---

## Deprecation Notices

No deprecations at this time.

---

## Security Updates

No security updates at this time. For security issues, please see [SECURITY.md](SECURITY.md).

---

## Notes

- This changelog is automatically generated from Git commit messages following [Conventional Commits](https://www.conventionalcommits.org/)
- For detailed commit history, see the [commit log](https://github.com/yourusername/veloq/commits/main)
- Release dates are approximate and subject to change

---

<div align="center">

**[⬆ Back to Top](#changelog)**

For more information, see the [full documentation](README.md)

</div>
