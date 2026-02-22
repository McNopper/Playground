#include "RandomGenerator.h"

ARandomGenerator::ARandomGenerator() : 
    m_generator{ std::default_random_engine{ std::random_device{}() } }
{
}

ARandomGenerator::ARandomGenerator(std::uint32_t seed) :
    m_generator{ std::default_random_engine{ seed } }
{
}

NoRandomGenerator::NoRandomGenerator(float value) :
    m_value{ value }
{
}

NoRandomGenerator::NoRandomGenerator(float value, std::uint32_t seed) :
    ARandomGenerator(seed), m_value{ value }
{
}

float NoRandomGenerator::generate()
{
    return m_value;
}

UniformRandomGenerator::UniformRandomGenerator() :
    m_uniform{ 0.0f, 1.0f }
{
}

UniformRandomGenerator::UniformRandomGenerator(std::uint32_t seed) :
    ARandomGenerator(seed), m_uniform{ 0.0f, 1.0f }
{
}

UniformRandomGenerator::UniformRandomGenerator(float min_value, float max_value) :
    m_uniform{ min_value, max_value }
{
}

UniformRandomGenerator::UniformRandomGenerator(float min_value, float max_value, std::uint32_t seed) :
    ARandomGenerator(seed), m_uniform{ min_value, max_value }
{
}

float UniformRandomGenerator::generate()
{
    return m_uniform(m_generator);
}

NormalRandomGenerator::NormalRandomGenerator() :
    m_normal{ 0.0f, 1.0f }
{
}

NormalRandomGenerator::NormalRandomGenerator(std::uint32_t seed) :
    ARandomGenerator(seed), m_normal{ 0.0f, 1.0f }
{
}

NormalRandomGenerator::NormalRandomGenerator(float mean, float std_dev) :
    m_normal{ mean, std_dev }
{
}

NormalRandomGenerator::NormalRandomGenerator(float mean, float std_dev, std::uint32_t seed) :
    ARandomGenerator(seed), m_normal{ mean, std_dev }
{
}

float NormalRandomGenerator::generate()
{
    return m_normal(m_generator);
}
