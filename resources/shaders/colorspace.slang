// colorspace.slang - Color space conversion module
// Transfer functions and color space transformations for SDR and HDR workflows

module colorspace;

// ============================================================================
// SDR: sRGB
// ============================================================================

// sRGB to Linear (decode)
public float3 sRGBToLinear(float3 srgb)
{
    float3 low = srgb / 12.92;
    float3 high = pow((srgb + 0.055) / 1.055, 2.4);
    return srgb <= 0.04045 ? low : high;
}

// Linear to sRGB (encode)
public float3 linearToSRGB(float3 linear)
{
    float3 low = linear * 12.92;
    float3 high = 1.055 * pow(max(linear, 0.0), 1.0 / 2.4) - 0.055;
    return linear <= 0.0031308 ? low : high;
}

// ============================================================================
// HDR: PQ (Perceptual Quantizer) - SMPTE ST 2084 / ITU-R BT.2100
// ============================================================================

// PQ to Linear (decode)
// Input: PQ-encoded in [0, 1]
// Output: Linear light in [0, 10000] nits
public float3 pqToLinear(float3 pq)
{
    const float m1 = 0.1593017578125;      // 2610 / 16384
    const float m2 = 78.84375;             // 2523 / 32
    const float c1 = 0.8359375;            // 3424 / 4096
    const float c2 = 18.8515625;           // 2413 / 128
    const float c3 = 18.6875;              // 2392 / 128
    
    float3 pq_m2 = pow(max(pq, 0.0), 1.0 / m2);
    float3 num = max(pq_m2 - c1, 0.0);
    float3 den = c2 - c3 * pq_m2;
    
    return 10000.0 * pow(num / den, 1.0 / m1);
}

// Linear to PQ (encode)
// Input: Linear light in [0, 10000] nits
// Output: PQ-encoded in [0, 1]
public float3 linearToPQ(float3 linear)
{
    const float m1 = 0.1593017578125;
    const float m2 = 78.84375;
    const float c1 = 0.8359375;
    const float c2 = 18.8515625;
    const float c3 = 18.6875;
    
    float3 y = clamp(linear / 10000.0, 0.0, 1.0);
    float3 y_m1 = pow(y, m1);
    float3 num = c1 + c2 * y_m1;
    float3 den = 1.0 + c3 * y_m1;
    
    return pow(num / den, m2);
}

// ============================================================================
// HDR: HLG (Hybrid Log-Gamma) - ITU-R BT.2100
// ============================================================================

// HLG to Linear (decode)
// Input: HLG-encoded in [0, 1]
// Output: Linear light, scene-referred
public float3 hlgToLinear(float3 hlg)
{
    const float a = 0.17883277;
    const float b = 0.28466892;
    const float c = 0.55991073;
    
    float3 low = (hlg * hlg) / 3.0;
    float3 high = (exp((hlg - c) / a) + b) / 12.0;
    
    return hlg <= 0.5 ? low : high;
}

// Linear to HLG (encode)
// Input: Linear light, scene-referred
// Output: HLG-encoded in [0, 1]
public float3 linearToHLG(float3 linear)
{
    const float a = 0.17883277;
    const float b = 0.28466892;
    const float c = 0.55991073;
    
    float3 low = sqrt(3.0 * linear);
    float3 high = a * log(12.0 * linear - b) + c;
    
    return linear <= 1.0 / 12.0 ? low : high;
}

// ============================================================================
// HDR: scRGB (Extended Range Linear)
// ============================================================================

// scRGB is already linear - identity functions for API completeness
public float3 scRGBToLinear(float3 scrgb)
{
    return scrgb;  // Already linear, extended range preserved
}

public float3 linearToScRGB(float3 linear)
{
    return linear;  // Already linear, extended range preserved
}

// ============================================================================
// BT.709 Transfer Function (ITU-R BT.709)
// ============================================================================

// BT.709 to Linear (decode)
public float3 bt709ToLinear(float3 bt709)
{
    const float alpha = 1.099;
    const float beta = 0.018;
    const float delta = alpha * pow(beta, 0.45) - (alpha - 1.0);
    
    float3 low = bt709 / 4.5;
    float3 high = pow((bt709 + (alpha - 1.0)) / alpha, 1.0 / 0.45);
    
    return bt709 < delta ? low : high;
}

// Linear to BT.709 (encode)
public float3 linearToBT709(float3 linear)
{
    const float alpha = 1.099;
    const float beta = 0.018;
    
    float3 low = linear * 4.5;
    float3 high = alpha * pow(linear, 0.45) - (alpha - 1.0);
    
    return linear < beta ? low : high;
}

// ============================================================================
// BT.2020 Transfer Function (ITU-R BT.2020)
// ============================================================================

// BT.2020 to Linear (decode)
public float3 bt2020ToLinear(float3 bt2020)
{
    const float alpha = 1.0993;
    const float beta = 0.0181;
    const float delta = alpha * pow(beta, 0.45) - (alpha - 1.0);
    
    float3 low = bt2020 / 4.5;
    float3 high = pow((bt2020 + (alpha - 1.0)) / alpha, 1.0 / 0.45);
    
    return bt2020 < delta ? low : high;
}

// Linear to BT.2020 (encode)
public float3 linearToBT2020(float3 linear)
{
    const float alpha = 1.0993;
    const float beta = 0.0181;
    
    float3 low = linear * 4.5;
    float3 high = alpha * pow(linear, 0.45) - (alpha - 1.0);
    
    return linear < beta ? low : high;
}
