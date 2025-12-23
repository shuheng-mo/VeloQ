#pragma once

#include "veloq/feature_engine/features.hpp"
#include "veloq/inference/model.hpp"
#include <string>
#include <memory>

namespace veloq {
namespace ipc_bridge {

/**
 * @brief Shared data structure for IPC
 *
 * Memory layout is designed for zero-copy access from Python.
 */
struct SharedData {
    // Latest market features
    feature_engine::MarketFeatures features;

    // Latest AI prediction
    inference::Prediction prediction;

    // Sequence number for data consistency check
    uint64_t sequence;

    // Status flags
    bool is_valid;
};

/**
 * @brief Shared Memory Bridge for Python communication
 *
 * Provides zero-copy inter-process communication using Boost.Interprocess.
 * End-to-end latency < 10μs.
 */
class SharedMemoryBridge {
public:
    explicit SharedMemoryBridge(const std::string& shm_name);
    ~SharedMemoryBridge();

    /**
     * @brief Initialize shared memory segment
     * @param size Size of shared memory in bytes
     * @return true if initialization successful
     */
    bool initialize(size_t size = sizeof(SharedData));

    /**
     * @brief Write data to shared memory
     * @param data Data to write
     * @return true if write successful
     */
    bool write(const SharedData& data);

    /**
     * @brief Read data from shared memory
     * @param data Output parameter for read data
     * @return true if read successful
     */
    bool read(SharedData& data);

    /**
     * @brief Cleanup shared memory
     */
    void cleanup();

    /**
     * @brief Check if bridge is initialized
     */
    bool is_initialized() const { return initialized_; }

private:
    std::string shm_name_;
    bool initialized_;
    // Boost shared memory objects will be stored here
};

} // namespace ipc_bridge
} // namespace veloq
