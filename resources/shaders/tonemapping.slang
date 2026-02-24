// tonemapping.slang - Tonemapping module
// Color-space-independent tonemapping operators for HDR to SDR mapping
// All functions expect linear RGB input and produce linear RGB output in [0, 1]

module tonemapping;

// ============================================================================
// Reinhard
// ============================================================================

// Maps HDR luminance to [0, 1] using a simple rational curve
public float3 reinhardTonemap(float3 color)
{
    return color / (color + 1.0);
}

// ============================================================================
// Reinhard Extended
// ============================================================================

// White-point aware Reinhard; values at white_point map to 1
public float3 reinhardExtendedTonemap(float3 color, float white_point)
{
    float white_point_sq = white_point * white_point;
    return (color * (1.0 + color / white_point_sq)) / (color + 1.0);
}

// ============================================================================
// Uncharted 2 (Hable)
// ============================================================================

static float3 hablePartial(float3 x)
{
    const float a = 0.15;
    const float b = 0.50;
    const float c = 0.10;
    const float d = 0.20;
    const float e = 0.02;
    const float f = 0.30;
    return ((x * (a * x + c * b) + d * e) / (x * (a * x + b) + d * f)) - e / f;
}

// Filmic curve by John Hable; exposure_bias typically 2.0
public float3 hableTonemap(float3 color, float exposure_bias)
{
    const float white = 11.2;
    float3 current = hablePartial(color * exposure_bias);
    float3 white_scale = 1.0 / hablePartial(float3(white, white, white));
    return current * white_scale;
}

// ============================================================================
// Lottes
// ============================================================================

// Configurable filmic curve by Timothy Lottes
// contrast: steepness (try 1.5), shoulder: rolloff (try 0.97)
// hdr_max: scene max luminance (try 8.0), mid_in/mid_out: midpoint mapping (try 0.18)
public float3 lottesTonemap(float3 color, float contrast, float shoulder, float hdr_max, float mid_in, float mid_out)
{
    float a = contrast;
    float d = shoulder;
    float hdr_max_a = pow(hdr_max, a);
    float mid_in_a  = pow(mid_in,  a);

    float b = (-pow(mid_in_a, d) + pow(hdr_max, a * d) * mid_out) /
              ((pow(hdr_max_a, d) - pow(mid_in_a, d)) * mid_out);

    float c_coeff = (pow(hdr_max_a, d) * pow(mid_in_a, 1.0) - pow(hdr_max, a) * pow(mid_in_a, d) * mid_out) /
                   ((pow(hdr_max_a, d) - pow(mid_in_a, d)) * mid_out);

    float3 color_a = pow(abs(color), a);
    return color_a / (pow(color_a, d) * b + c_coeff);
}

// ============================================================================
// Uchimura (Gran Turismo)
// ============================================================================

// Uchimura 2017 - "HDR theory and practice"
// max_brightness: scene max (try 1.0), contrast: slope (try 1.0)
// linear_start: start of linear section (try 0.22), linear_length: length (try 0.4)
// black_tightness_shape: black point shape (try 1.33), black_tightness_offset (try 0.0)
public float3 uchimuraTonemap(float3 color,
                               float max_brightness,
                               float contrast,
                               float linear_start,
                               float linear_length,
                               float black_tightness_shape,
                               float black_tightness_offset)
{
    float p = max_brightness;
    float a = contrast;
    float m = linear_start;
    float l = linear_length;
    float c = black_tightness_shape;
    float b = black_tightness_offset;

    float l0 = ((p - m) * l) / a;
    float s0 = m + l0;
    float s1 = m + a * l0;
    float c2 = (a * p) / (p - s1);
    float cp = -c2 / p;

    float3 w0 = 1.0 - smoothstep(float3(0.0, 0.0, 0.0), float3(m, m, m), color);
    float3 w2 = step(float3(m + l0, m + l0, m + l0), color);
    float3 w1 = 1.0 - w0 - w2;

    float3 t = m * pow(max(color / m, 0.0), float3(c, c, c)) + b;
    float3 s = p - (p - s1) * exp(cp * (color - s0));
    float3 li = m + a * (color - m);

    return t * w0 + li * w1 + s * w2;
}

// ============================================================================
// Clamp (no tonemapping)
// ============================================================================

// Linear clamp to [0, 1] - no curve applied
public float3 clampTonemap(float3 color)
{
    return clamp(color, 0.0, 1.0);
}
