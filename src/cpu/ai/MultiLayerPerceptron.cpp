#include "MultiLayerPerceptron.h"

#include <cmath>
#include <memory>

#include "core/math/RandomGenerator.h"

#include "loss_functions.h"

MultiLayerPerceptron::MultiLayerPerceptron(std::size_t number_inputs, bool use_bias, float mean) :
	m_number_inputs{ number_inputs }, m_use_bias{ use_bias }, m_mean{ mean }
{
}

bool MultiLayerPerceptron::addLayer(std::size_t number_neurons, const ActivationFunction& activation_function)
{
	if ((m_number_inputs == 0u) || (number_neurons == 0u))
	{
		return false;
	}
	if ((activation_function.function == nullptr) || (activation_function.derivative == nullptr))
	{
		return false;
	}

	std::size_t number_inputs{ m_number_inputs };
	if (!m_layers.empty())
	{
		number_inputs = m_layers.back().neurons.size();
	}

	Layer layer{};
	layer.neurons.resize(number_neurons);
	for (auto& neuron : layer.neurons)
	{
		neuron.weights.resize(number_inputs);
	}
	layer.af = activation_function;

	m_layers.push_back(std::move(layer));

	return true;
}

bool MultiLayerPerceptron::reset(InitializationMethod initialization_method, float bias_value, std::uint32_t seed)
{
	if (m_layers.size() < 2u)
	{
		return false;
	}

	std::uint32_t current_seed = seed;

	for (auto& layer : m_layers)
	{
		for (auto& neuron : layer.neurons)
		{
			std::unique_ptr<ARandomGenerator> rg{ nullptr };

			if (initialization_method == InitializationMethod::ZERO)
			{
				rg = std::make_unique<NoRandomGenerator>(m_mean, current_seed);
			}
			else if (initialization_method == InitializationMethod::KAIMING)
			{
				rg = std::make_unique<NormalRandomGenerator>(m_mean, std::sqrt(2.0f / (float)neuron.weights.size()), current_seed);
			}
			else if (initialization_method == InitializationMethod::UNIFORM_XAVIER)
			{
				float limit = std::sqrt(6.0f / (float)(layer.neurons.size() + neuron.weights.size()));
				rg = std::make_unique<UniformRandomGenerator>(-limit + m_mean, limit + m_mean, current_seed);
			}
			else if (initialization_method == InitializationMethod::NORMAL_XAVIER)
			{
				rg = std::make_unique<NormalRandomGenerator>(m_mean, std::sqrt(2.0f / (float)(layer.neurons.size() + neuron.weights.size())), current_seed);
			}

			if (!rg)
			{
				return false;
			}

			for (auto& weight : neuron.weights)
			{
				weight = rg->generate();
			}

			if (m_use_bias)
			{
				neuron.bias	= bias_value;
			}

			// Change seed every loop.
			current_seed++;
		}
	}

	return true;
}

std::vector<float> MultiLayerPerceptron::forwardPropagation(const std::vector<float>& inputs)
{
	if (m_layers.size() < 2u)
	{
		return {};
	}
	if (inputs.size() != m_number_inputs)
	{
		return {};
	}

	std::vector<float> current_inputs{ inputs };

	for (auto& layer : m_layers)
	{
		std::vector<float> current_outputs(layer.neurons.size());

		for (std::size_t neuron_index = 0u; neuron_index < layer.neurons.size(); neuron_index++)
		{
			auto& neuron{ layer.neurons[neuron_index] };

			float weighted_sum = 0.0f;

			for (std::size_t input_index = 0u; input_index < current_inputs.size(); input_index++)
			{
				weighted_sum += neuron.weights[input_index] * current_inputs[input_index];
			}

			if (m_use_bias)
			{
				weighted_sum += neuron.bias;
			}

			neuron.output = layer.af.function(weighted_sum);

			current_outputs[neuron_index] = neuron.output;
		}

		current_inputs = std::move(current_outputs);
	}

	return current_inputs;
}

std::optional<float> MultiLayerPerceptron::backwardPropagation(const std::vector<float>& inputs, const std::vector<float>& targets, float learning_rate)
{
	// Execute the forward propagation.
	auto result = forwardPropagation(inputs);
	if (result.empty())
	{
		return {};
	}

	// Calculate the gradients.
	if (targets.size() != m_layers.back().neurons.size())
	{
		return {};
	}
	if (learning_rate == 0.0f)
	{
		return {};
	}

	std::vector<float> errors(targets.size());
	std::vector<float> previous_neurons_deltas{};

	auto layer_it = m_layers.end();
	while (layer_it != m_layers.begin())
	{
		layer_it--;
		auto& layer = *layer_it;

		std::vector<float> current_neurons_deltas(layer.neurons.size());

		for (std::size_t neuron_index = 0u; neuron_index < layer.neurons.size(); neuron_index++)
		{
			auto& neuron{ layer.neurons[neuron_index] };

			if ((layer_it + 1u) == m_layers.end())
			{
				// Overall output layer.
				neuron.delta = targets[neuron_index] - neuron.output;

				errors[neuron_index] = neuron.delta;
			}
			else
			{
				// Hidden layers.
				neuron.delta = 0.0f;

				const auto& next_layer = *(layer_it + 1u);
				for (std::size_t next_neuron_index = 0u; next_neuron_index < next_layer.neurons.size(); next_neuron_index++)
				{
					neuron.delta += next_layer.neurons[next_neuron_index].weights[neuron_index] * previous_neurons_deltas[next_neuron_index];
				}
			}

			neuron.delta *= layer.af.derivative(neuron.output);
			
			current_neurons_deltas[neuron_index] = neuron.delta;
		}

		previous_neurons_deltas = std::move(current_neurons_deltas);
	}

	// Update weights and biases.
	std::vector<float> current_inputs{ inputs };

	for (auto& layer : m_layers)
	{
		std::vector<float> current_outputs(layer.neurons.size());
		
		for (std::size_t neuron_index = 0u; neuron_index < layer.neurons.size(); neuron_index++)
		{
			auto& neuron{ layer.neurons[neuron_index] };

			for (std::size_t weight_index = 0u; weight_index < neuron.weights.size(); weight_index++)
			{
				neuron.weights[weight_index] += learning_rate * neuron.delta * current_inputs[weight_index];
			}

			if (m_use_bias)
			{
				neuron.bias += learning_rate * neuron.delta;
			}

			current_outputs[neuron_index] = neuron.output;
		}

		current_inputs = std::move(current_outputs);
	}

	// Calculate and return the error.
	return half_mse(errors);
}
