#include "real_spherical_harmonics.h"
#include <cmath>

float rsh::Cln(std::uint32_t l, std::uint32_t n) const
{
    return constants[l * l + n];
}

float rsh::Clm(std::uint32_t l, std::int32_t m) const
{
    return Cln(l, static_cast<std::uint32_t>(m + static_cast<std::int32_t>(l)));
}

float rsh::Yln(std::uint32_t l, std::uint32_t n, float x, float y, float z) const
{
    const float C = Cln(l, n);
    const std::int32_t m = static_cast<std::int32_t>(n) - static_cast<std::int32_t>(l);
    
    // l=0
    if (l == 0)
    {
        return C;
    }
    
    // l=1
    if (l == 1)
    {
        if (m == -1)
        {
            return C * y;
        }
        if (m == 0)
        {
            return C * z;
        }
        if (m == 1)
        {
            return C * x;
        }
    }
    
    // l=2
    if (l == 2)
    {
        if (m == -2)
        {
            return C * x * y;
        }
        if (m == -1)
        {
            return C * y * z;
        }
        if (m == 0)
        {
            return C * (3.0f * z * z - 1.0f);
        }
        if (m == 1)
        {
            return C * z * x;
        }
        if (m == 2)
        {
            return C * (x * x - y * y);
        }
    }
    
    // l=3
    if (l == 3)
    {
        if (m == -3)
        {
            return C * y * (3.0f * x * x - y * y);
        }
        if (m == -2)
        {
            return C * x * y * z;
        }
        if (m == -1)
        {
            return C * y * (5.0f * z * z - 1.0f);
        }
        if (m == 0)
        {
            return C * z * (5.0f * z * z - 3.0f);
        }
        if (m == 1)
        {
            return C * x * (5.0f * z * z - 1.0f);
        }
        if (m == 2)
        {
            return C * z * (x * x - y * y);
        }
        if (m == 3)
        {
            return C * x * (x * x - 3.0f * y * y);
        }
    }
    
    return 0.0f;
}

float rsh::Ylm(std::uint32_t l, std::int32_t m, float x, float y, float z) const
{
    const std::uint32_t n = static_cast<std::uint32_t>(m + static_cast<std::int32_t>(l));
    return Yln(l, n, x, y, z);
}
