#include <cmath>
#include <cstdint>
#include <vector>

#include <gtest/gtest.h>

#include "cpu/cpu.h"

TEST(AI, XOR)
{
    // Use library sigmoid activation
    ActivationFunction activation_function{ sigmoid, sigmoidDerivative };

    // XOR dataset
    std::vector<std::vector<float>> inputs = { { 0.0f, 0.0f }, { 0.0f, 1.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f } };
    std::vector<std::vector<float>> targets = { { 0.0f }, { 1.0f }, { 1.0f }, { 0.0f } };

    // Create perceptron: 2 inputs, use bias
    MultiLayerPerceptron mlp(2u, true);
    mlp.addLayer(2u, activation_function); // hidden layer with 2 neurons
    mlp.addLayer(1u, activation_function); // output layer

    // Initialize weights with fixed seed for deterministic test
    ASSERT_TRUE(mlp.reset(InitializationMethod::NORMAL_XAVIER, 0.0f, 123u));

    const float learning_rate{ 0.5f };

    // Train for some epochs
    for (std::size_t epoch = 0u; epoch < 50000u; epoch++)
    {
        float total_error{ 0.0f };
        for (std::size_t i = 0u; i < inputs.size(); i++)
        {
            auto error = mlp.backwardPropagation(inputs[i], targets[i], learning_rate);
            if (error)
            {
                total_error += *error;
            }
        }
        if (total_error < 0.001f)
        {
            break;
        }
    }

    // Validate
    std::int32_t correct{ 0 };
    for (std::size_t i = 0u; i < inputs.size(); i++)
    {
        auto out = mlp.forwardPropagation(inputs[i]);
        ASSERT_FALSE(out.empty());
        float v = out[0];
        std::int32_t predicted{ v > 0.5f ? 1 : 0 };
        std::int32_t expected{ targets[i][0] > 0.5f ? 1 : 0 };
        if (predicted == expected)
        {
            correct++;
        }
    }

    EXPECT_EQ(correct, static_cast<std::int32_t>(inputs.size()));
}
