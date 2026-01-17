#ifndef CORE_MATH_RANDOMGENERATOR_H_
#define CORE_MATH_RANDOMGENERATOR_H_

#include <random>

class ARandomGenerator {

protected:

	std::default_random_engine m_generator{};

public:

	ARandomGenerator();

	explicit ARandomGenerator(std::uint32_t seed);

	virtual ~ARandomGenerator() = default;

	virtual float generate() = 0;

};

class NoRandomGenerator : public ARandomGenerator {

private:

	float m_value{ 0.0f };

public:

	NoRandomGenerator() = default;

	explicit NoRandomGenerator(float value);

	NoRandomGenerator(float value, std::uint32_t seed);

	float generate() override;

};

class UniformRandomGenerator : public ARandomGenerator {

private:

	std::uniform_real_distribution<float> m_uniform{};

public:

	UniformRandomGenerator();

	explicit UniformRandomGenerator(std::uint32_t seed);

	UniformRandomGenerator(float min_value, float max_value);

	UniformRandomGenerator(float min_value, float max_value, std::uint32_t seed);

	float generate() override;

};

class NormalRandomGenerator : public ARandomGenerator {

private:

	std::normal_distribution<float> m_normal{};

public:

	NormalRandomGenerator();

	explicit NormalRandomGenerator(std::uint32_t seed);

	NormalRandomGenerator(float mean, float std_dev);

	NormalRandomGenerator(float mean, float std_dev, std::uint32_t seed);

	float generate() override;

};

#endif /* CORE_MATH_RANDOMGENERATOR_H_ */
