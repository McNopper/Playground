#ifndef CORE_MATH_REAL_SPHERICAL_HARMONICS_H_
#define CORE_MATH_REAL_SPHERICAL_HARMONICS_H_

#include <cstdint>
#include <vector>

struct rsh
{

    // Real spherical harmonic normalization constants
    // With Condon-Shortley phase (-1)^m baked in
    // Organized by l, then m from -l to +l for easy indexing
    std::vector<float> constants{
        // l=0: 1 term (index 0)
        0.2820947917738781f, // m=0: (1/2)*sqrt(1/π)

        // l=1: 3 terms (index 1-3)
        -0.4886025119029199f, // m=-1: -sqrt(3/(4π))
        0.4886025119029199f,  // m=0: sqrt(3/(4π))
        -0.4886025119029199f, // m=1: -sqrt(3/(4π))

        // l=2: 5 terms (index 4-8)
        1.092548430592079f,  // m=-2: (1/2)*sqrt(15/π)
        -1.092548430592079f, // m=-1: -(1/2)*sqrt(15/π)
        0.3153915652525200f, // m=0: (1/4)*sqrt(5/π)
        -1.092548430592079f, // m=1: -(1/2)*sqrt(15/π)
        0.5462742152960395f, // m=2: (1/4)*sqrt(15/π)

        // l=3: 7 terms (index 9-15)
        -0.5900435899266435f, // m=-3: -(1/4)*sqrt(35/(2π))
        2.890611442640554f,   // m=-2: (1/2)*sqrt(105/π)
        -0.4570457994644657f, // m=-1: -(1/4)*sqrt(21/(2π))
        0.3731763325901154f,  // m=0: (1/4)*sqrt(7/π)
        -0.4570457994644657f, // m=1: -(1/4)*sqrt(21/(2π))
        1.445305721320277f,   // m=2: (1/4)*sqrt(105/π)
        -0.5900435899266435f  // m=3: -(1/4)*sqrt(35/(2π))
    };

    float Clm(std::uint32_t l, std::int32_t m) const;

    float Cln(std::uint32_t l, std::uint32_t n) const;

    // Evaluate real spherical harmonic Y_l^m at direction (x, y, z)
    // Direction should be normalized (unit vector)
    float Ylm(std::uint32_t l, std::int32_t m, float x, float y, float z) const;

    // Evaluate real spherical harmonic Y_l^n at direction (x, y, z) using linear index n
    // Direction should be normalized (unit vector)
    float Yln(std::uint32_t l, std::uint32_t n, float x, float y, float z) const;
};

#endif /* CORE_MATH_REAL_SPHERICAL_HARMONICS_H_*/
