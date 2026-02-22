#ifndef CPU_AI_MULTILAYERPERCEPTRON_H_
#define CPU_AI_MULTILAYERPERCEPTRON_H_

#include <cstddef>
#include <functional>
#include <optional>
#include <vector>

#include "activation_functions.h"

enum class InitializationMethod
{
    ZERO,
    KAIMING, // also known as He initialization
    UNIFORM_XAVIER,
    NORMAL_XAVIER
};

struct ActivationFunction
{
    std::function<float(float)> function{ nullptr };
    std::function<float(float)> derivative{ nullptr };
};

struct Neuron
{
    std::vector<float> weights{};
    float bias{ 0.0f };
    float output{ 0.0f };
    float delta{ 0.0f };
};

struct Layer
{
    std::vector<Neuron> neurons{};
    ActivationFunction af{};
};

class MultiLayerPerceptron
{

private:

    std::size_t m_number_inputs{ 0u };

    bool m_use_bias{ false };

    float m_mean{ 0.0f };

    std::vector<Layer> m_layers{};

public:

    MultiLayerPerceptron() = default;

    explicit MultiLayerPerceptron(std::size_t number_inputs, bool use_bias, float mean = 0.0f);

    bool addLayer(std::size_t number_neurons, const ActivationFunction& activation_function);

    bool reset(InitializationMethod initialization_method, float bias_value, std::uint32_t seed);

    std::vector<float> forwardPropagation(const std::vector<float>& inputs);

    std::optional<float> backwardPropagation(const std::vector<float>& inputs, const std::vector<float>& targets, float learning_rate);
};

#endif /* CPU_AI_MULTILAYERPERCEPTRON_H_ */
