#pragma once

#include "veloq/feature_engine/features.hpp"
#include <string>
#include <vector>
#include <memory>

namespace veloq {
namespace inference {

/**
 * @brief Prediction result from AI model
 */
struct Prediction {
    // Probability of price going up in next 5-10 seconds
    float up_probability;

    // Probability of price going down
    float down_probability;

    // Probability of price staying flat
    float flat_probability;

    // Model inference latency (microseconds)
    int64_t latency_us;

    // Timestamp of prediction
    common::Timestamp timestamp;
};

/**
 * @brief AI Inference Engine using ONNX Runtime
 *
 * Loads and runs lightweight deep learning models for price prediction.
 * Optimized for real-time inference with <100μs latency.
 */
class InferenceEngine {
public:
    InferenceEngine();
    ~InferenceEngine();

    /**
     * @brief Load ONNX model from file
     * @param model_path Path to ONNX model file
     * @return true if model loaded successfully
     */
    bool load_model(const std::string& model_path);

    /**
     * @brief Run inference on features
     * @param features Input market features
     * @return Prediction result
     */
    Prediction predict(const feature_engine::MarketFeatures& features);

    /**
     * @brief Check if model is loaded
     */
    bool is_loaded() const { return model_loaded_; }

    /**
     * @brief Get model metadata
     */
    std::string get_model_info() const;

private:
    bool model_loaded_;
    // ONNX Runtime session will be stored here
    // std::unique_ptr<Ort::Session> session_;
};

} // namespace inference
} // namespace veloq
