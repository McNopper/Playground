#include "convert.h"

#include <algorithm>
#include <cmath>
#include <cstdint>

float3x3 toXYZ(const ChromaticityCoordinates& c)
{
    const float counter_a = (((1.0f - c.W.x) / c.W.y) - ((1.0f - c.R.x) / c.R.y)) * (c.G.x / c.G.y - c.R.x / c.R.y);
    const float counter_b = (c.W.x / c.W.y - c.R.x / c.R.y) * (((1.0f - c.G.x) / c.G.y) - ((1.0f - c.R.x) / c.R.y));
    const float denominator_a = (((1.0f - c.B.x) / c.B.y) - ((1.0f - c.R.x) / c.R.y)) * (c.G.x / c.G.y - c.R.x / c.R.y);
    const float denominator_b = (c.B.x / c.B.y - c.R.x / c.R.y) * (((1.0f - c.G.x) / c.G.y) - ((1.0f - c.R.x) / c.R.y));

    const float BY = (counter_a - counter_b) / (denominator_a - denominator_b);

    const float GY = ((c.W.x / c.W.y) - (c.R.x / c.R.y) - BY * ((c.B.x / c.B.y) - (c.R.x / c.R.y))) / ((c.G.x / c.G.y) - (c.R.x / c.R.y));

    const float RY = 1.0f - GY - BY;

    return {
        { (RY / c.R.y) * c.R.x, (GY / c.G.y) * c.G.x, (BY / c.B.y) * c.B.x },
        { RY, GY, BY },
        { (RY / c.R.y) * (1.0f - c.R.x - c.R.y), (GY / c.G.y) * (1.0f - c.G.x - c.G.y), (BY / c.B.y) * (1.0f - c.B.x - c.B.y) }
    };
}

float3 toLinearSRGB(const float3& color)
{
    float3 result{};

    for (uint32_t i = 0u; i < 3u; i++)
    {
        if (color[i] <= 0.04045f)
        {
            result[i] = color[i] / 12.92f;
        }
        else
        {
            result[i] = powf((color[i] + 0.055f) / 1.055f, 2.4f);
        }
    }

    return result;
}

float3 toNonLinearSRGB(const float3& color)
{
    float3 result{};

    for (uint32_t i = 0u; i < 3u; i++)
    {
        if (color[i] <= 0.0031308f)
        {
            result[i] = color[i] * 12.92f;
        }
        else
        {
            result[i] = 1.055f * powf(color[i], 1.0f / 2.4f) - 0.055f;
        }
    }

    return result;
}

float3 toLinearSCRGB(const float3& color)
{
    float3 result{};

    for (uint32_t i = 0u; i < 3u; i++)
    {
        if (color[i] < -0.04045f)
        {
            result[i] = -powf((0.055f - color[i]) / 1.055f, 2.4f);
        }
        else if (color[i] <= 0.04045f)
        {
            result[i] = color[i] / 12.92f;
        }
        else
        {
            result[i] = powf((color[i] + 0.055f) / 1.055f, 2.4f);
        }
    }

    return result;
}

float3 toNonLinearSCRGB(const float3& color)
{
    float3 result{};

    for (uint32_t i = 0u; i < 3u; i++)
    {
        if (color[i] <= -0.0031308f)
        {
            result[i] = -1.055f * powf(-color[i], 1.0f / 2.4f) + 0.055f;
        }
        else if (color[i] < 0.0031308f)
        {
            result[i] = color[i] * 12.92f;
        }
        else
        {
            result[i] = 1.055f * powf(color[i], 1.0f / 2.4f) - 0.055f;
        }
    }

    return result;
}

float3 toLinearBT706(const float3& color)
{
    const float alpha{ 1.099f };
    const float beta{ 0.018f };

    const float delta = alpha * powf(beta, 0.45f) - (alpha - 1.0f);

    float3 result{};

    for (uint32_t i = 0u; i < 3u; i++)
    {
        if (color[i] < delta)
        {
            result[i] = color[i] / 4.5f;
        }
        else
        {
            result[i] = powf((color[i] + (alpha - 1.0f)) / alpha, 1.0f / 0.45f);
        }
    }

    return result;
}

float3 toNonLinearBT706(const float3& color)
{
    const float alpha{ 1.099f };
    const float beta{ 0.018f };

    float3 result{};

    for (uint32_t i = 0u; i < 3u; i++)
    {
        if (color[i] < beta)
        {
            result[i] = color[i] * 4.5f;
        }
        else
        {
            result[i] = alpha * powf(color[i], 0.45f) - (alpha - 1.0f);
        }
    }

    return result;
}

float3 toLinearBT2020(const float3& color)
{
    const float alpha{ 1.0993f };
    const float beta{ 0.0181f };

    const float delta = alpha * powf(beta, 0.45f) - (alpha - 1.0f);

    float3 result{};

    for (uint32_t i = 0u; i < 3u; i++)
    {
        if (color[i] < delta)
        {
            result[i] = color[i] / 4.5f;
        }
        else
        {
            result[i] = powf((color[i] + (alpha - 1.0f)) / alpha, 1.0f / 0.45f);
        }
    }

    return result;
}

float3 toNonLinearBT2020(const float3& color)
{
    const float alpha{ 1.0993f };
    const float beta{ 0.0181f };

    float3 result{};

    for (uint32_t i = 0u; i < 3u; i++)
    {
        if (color[i] < beta)
        {
            result[i] = color[i] * 4.5f;
        }
        else
        {
            result[i] = alpha * powf(color[i], 0.45f) - (alpha - 1.0f);
        }
    }

    return result;
}

float3 toLinearPQ(const float3& color)
{
    const float m1 = 2610.0f / 16384.0f;
    const float m2 = 2523.0f / 4096.0f * 128.0f;
    const float c1 = 3424.0f / 4096.0f;
    const float c2 = 2413.0f / 4096.0f * 32.0f;
    const float c3 = 2392.0f / 4096.0f * 32.0f;

    float3 result{};

    for (uint32_t i = 0u; i < 3u; i++)
    {
        float E = powf(color[i], 1.0f / m2);
        float counter = std::max(E - c1, 0.0f);
        float denominator = c2 - c3 * E;

        result[i] = 10000.0f * powf(counter / denominator, 1.0f / m1);
    }

    return result;
}

float3 toNonLinearPQ(const float3& color)
{
    const float m1 = 2610.0f / 16384.0f;
    const float m2 = 2523.0f / 4096.0f * 128.0f;
    const float c1 = 3424.0f / 4096.0f;
    const float c2 = 2413.0f / 4096.0f * 32.0f;
    const float c3 = 2392.0f / 4096.0f * 32.0f;

    float3 result{};

    for (uint32_t i = 0u; i < 3u; i++)
    {
        float Y = color[i] / 10000.0f;
        float Ym1 = powf(Y, m1);

        result[i] = powf((c1 + c2 * Ym1) / (1.0f + c3 * Ym1), m2);
    }

    return result;
}

float3 toLinearHLG(const float3& color)
{
    const float a = 0.17883277f;
    const float b = 1.0f - 4.0f * a;
    const float c = 0.5f - a * logf(4.0f * a);

    float3 result{};

    for (uint32_t i = 0u; i < 3u; i++)
    {
        if (color[i] <= 0.5f)
        {
            result[i] = color[i] * color[i] / 3.0f;
        }
        else
        {
            result[i] = (1.0f / 12.0f) * (b + expf((color[i] - c) / a));
        }
    }

    return result;
}

float3 toNonLinearHLG(const float3& color)
{
    const float a = 0.17883277f;
    const float b = 1.0f - 4.0f * a;
    const float c = 0.5f - a * logf(4.0f * a);

    float3 result{};

    for (uint32_t i = 0u; i < 3u; i++)
    {
        if (color[i] <= 1.0f / 12.0f)
        {
            result[i] = sqrtf(3.0f * color[i]);
        }
        else
        {
            result[i] = a * logf(12.0f * color[i] - b) + c;
        }
    }

    return result;
}
