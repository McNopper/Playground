#include "loss_functions.h"

#include <algorithm>
#include <cmath>

float mse(const std::vector<float>& values)
{
	float sum{ 0.0f };

	for (auto v : values)
	{
		sum += v * v;
	}

	return sum / (float)values.size();
}

float half_mse(const std::vector<float>& values)
{
	return 0.5f * mse(values);
}

float mae(const std::vector<float>& values)
{
	float sum{ 0.0f };

	for (auto v : values)
	{
		sum += std::abs(v);
	}

	return sum / static_cast<float>(values.size());
}

float binaryCrossEntropy(const std::vector<float>& predicted, const std::vector<float>& target)
{
	if (predicted.size() != target.size())
	{
		return 0.0f;
	}

	const float epsilon{ 1e-7f };
	float sum{ 0.0f };

	for (std::size_t i{ 0u }; i < predicted.size(); ++i)
	{
		float p{ std::max(epsilon, std::min(1.0f - epsilon, predicted[i])) };
		sum += -(target[i] * std::log(p) + (1.0f - target[i]) * std::log(1.0f - p));
	}

	return sum / static_cast<float>(predicted.size());
}

float categoricalCrossEntropy(const std::vector<float>& predicted, const std::vector<float>& target)
{
	if (predicted.size() != target.size())
	{
		return 0.0f;
	}

	const float epsilon{ 1e-7f };
	float sum{ 0.0f };

	for (std::size_t i{ 0u }; i < predicted.size(); ++i)
	{
		float p{ std::max(epsilon, std::min(1.0f, predicted[i])) };
		sum += -(target[i] * std::log(p));
	}

	return sum / static_cast<float>(predicted.size());
}

float huberLoss(const std::vector<float>& predicted, const std::vector<float>& target, float delta)
{
	if (predicted.size() != target.size())
	{
		return 0.0f;
	}

	float sum{ 0.0f };

	for (std::size_t i{ 0u }; i < predicted.size(); ++i)
	{
		float error{ target[i] - predicted[i] };
		float abs_error{ std::abs(error) };

		if (abs_error <= delta)
		{
			sum += 0.5f * error * error;
		}
		else
		{
			sum += delta * (abs_error - 0.5f * delta);
		}
	}

	return sum / static_cast<float>(predicted.size());
}
