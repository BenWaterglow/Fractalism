#ifndef _SPECTRAL_COLOR_H_
#define _SPECTRAL_COLOR_H_

#if defined(__cplusplus) || defined(__OPENCL_CPP_VERSION__)
extern "C" {
#endif

typedef struct range {
  float low;
  float high;
} range;

typedef struct spectral_color_term {
  range range;
  float second_order;
  float first_order;
  float zero_order;
} spectral_color_term;

// Modifier to use for a smooth approximation of the Heaviside step function: H(x) = lim(k->inf) 1/(1+exp(-2kx)).
__constant float negative_2_k = -200.0;

// Uses a smooth approximation of the Heaviside step function to return approximately 1.0 if the value is in the range, and approximately 0.0f otherwise.
static inline float is_in_range(float value, range range) {
  return 1.0f / ((1.0f + exp(negative_2_k * (value - range.low))) * (1.0f + exp((negative_2_k * (range.high - value)))));
}

__constant struct {
  spectral_color_term r[5];
  spectral_color_term g[3];
  spectral_color_term b[2];
} spectral_color_terms = {
    {
      {{ 0.0 / 60.0,  2.0 / 60.0}, -180.0, 9.9, 0.0},
      {{ 2.0 / 60.0, 15.0 / 60.0}, -2.76923, 0.184632, 0.136938},
      {{29.0 / 60.0, 39.0 / 60.0}, -36.0, 46.68, 14.152},
      {{39.0 / 60.0, 50.0 / 60.0}, -11.9008, 15.7984, -4.26086},
      {{50.0 / 60.0, 52.0 / 60.0}, 7.2, -17.04, 9.85}
    },
    {
      {{ 3.0 / 60.0, 15.0 / 60.0}, 20.0, -1.99992, 0.05012},
      {{15.0 / 60.0, 38.0 / 60.0}, -5.44424, 4.70472, -0.0359},
      {{38.0 / 60.0, 48.0 / 60.0}, 0.0, -4.66668, 3.71776}
    },
    {
      {{ 0.0 / 60.0, 15.0 / 60.0}, -24.0, 8.80002, 0.00008},
      {{15.0 / 60.0, 32.0 / 60.0}, 3.73703, -5.39793, 1.81586}
    }
};

static inline void process_spectral_color_term(float* coefficients, float value, spectral_color_term term) {
  // Effectively 1.0 for values outside the rage (low, high), and 1.0 within it.
  float in_range = is_in_range(value, term.range);
  coefficients[0] += in_range * term.second_order;
  coefficients[1] += in_range * term.first_order;
  coefficients[2] += in_range * term.zero_order;
}

static float4 spectral_color(float value) {
  // value (0,1) <=> wavelength in nm (400,700). 
  float r[3] = { 0.0 };
  process_spectral_color_term(r, value, spectral_color_terms.r[0]);
  process_spectral_color_term(r, value, spectral_color_terms.r[1]);
  process_spectral_color_term(r, value, spectral_color_terms.r[2]);
  process_spectral_color_term(r, value, spectral_color_terms.r[3]);
  process_spectral_color_term(r, value, spectral_color_terms.r[4]);

  float g[3] = { 0.0 };
  process_spectral_color_term(g, value, spectral_color_terms.g[0]);
  process_spectral_color_term(g, value, spectral_color_terms.g[1]);
  process_spectral_color_term(g, value, spectral_color_terms.g[2]);

  float b[3] = { 0.0 };
  process_spectral_color_term(b, value, spectral_color_terms.b[0]);
  process_spectral_color_term(b, value, spectral_color_terms.b[1]);

  float value_sqr = value * value;

  // Use alpha to store the normalized value, squared for increased transparency.
  return (float4)(
    (r[0] * value_sqr) + (r[1] * value) + r[2],
    (g[0] * value_sqr) + (g[1] * value) + g[2],
    (b[0] * value_sqr) + (b[1] * value) + b[2],
    value_sqr);
}

#if defined(__cplusplus) || defined(__OPENCL_CPP_VERSION__)
}
#endif

#endif // !_SPECTRAL_COLOR_H_
