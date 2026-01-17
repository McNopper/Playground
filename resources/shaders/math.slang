// math.slang - Math utility module
// Common mathematical operations and matrix utilities

module math;

// Matrix multiplication operators
public float4x4 operator*(float4x4 a, float4x4 b)
{
    return mul(a, b);
}

public float4 operator*(float4x4 a, float4 v)
{
    return mul(a, v);
}

public float3 operator*(float3x3 a, float3 v)
{
    return mul(a, v);
}

public float3x3 operator*(float3x3 a, float3x3 b)
{
    return mul(a, b);
}

public float2 operator *(float2x2 a, float2 v)
{
    return mul(a, v);
}

public float2x2 operator *(float2x2 a, float2x2 b)
{
    return mul(a, b);
}
