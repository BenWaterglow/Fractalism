----------------------------------------------------------------------------

# Ray-Marching algorithm

----------------------------------------------------------------------------

## Overview

----------------------------------------------------------------------------

We want to find the correct ray-marching path through the render
volume in a way that avoids non-uniform flow control and the resulting
elimiation of derivatives for accurate texture sampling.

----------------------------------------------------------------------------

## Definitions and parameters

----------------------------------------------------------------------------

The render volume is set as an axis-aligned bounding cube, centered at
the origin, with side length of 1.0. This means the vertices are all
(±0.5, ±0.5, ±0.5).

----------------------------------------------------------------------------

The parameter worldspacePostion is equivalent to gl_FragCoords in
worldspace coordinates. This is the point where the ray "hits" the
render volume. It is supplied as an out param from the vertex shader.

----------------------------------------------------------------------------

The eyePosition uniform is the viewing point in worldspace coordinates.
This is the "base" of the viewing vector.

----------------------------------------------------------------------------

## The algorithm

----------------------------------------------------------------------------

Let:  
* 𝘋 be the rendering volume,  
* 𝘤₊ be the maximum vertices of 𝘋,  
* 𝘤₋ be the minimum vertices of 𝘋,  
* 𝙑 = eyePosition be the point of view,  
* 𝙋 = worldspacePostion be a point on interest on 𝘋.

Let 𝙙 = 𝙋-𝙑 be the unnormalized viewing direction vector. To "march"
along this direction, we parameterize 𝙙 using the vector-valued
function 𝙧(𝜆) = 𝙑+𝜆𝙙 = 𝙑+𝜆(𝙋-𝙑). Note that 𝙧(0) = 𝙑, 𝙧(1) = 𝙋. Also
note that, component-wise, 𝙧ₙ(𝜆)=𝙑ₙ+𝜆(𝙋ₙ-𝙑ₙ) ∀𝘯∈(𝘹, 𝘺, 𝘻).

Let 𝘍ₛₙ : 𝒗 ↦ 𝒗ₙ=𝘤ₛ, 𝘴∈(+, -), 𝘯∈(𝘹, 𝘺, 𝘻) be equations defining the
infinite planes collinear with the faces of 𝘋.

In the special case where 𝙑 is within 𝘋:  
𝘤₋ - ≤ 𝙑ₙ ≤ 𝘤₊ + ∀𝘯∈(𝘹, 𝘺, 𝘻) ⇒ 𝜆∈[0, 1] ⇒ 𝙧(𝜆)∈[𝙑, 𝙋],  
since 𝙋 will be the "exit" point from 𝘋.
Otherwise:  
∃𝘯∈(𝘹, 𝘺, 𝘻)∋|𝙑ₙ| > 𝘤 ⇒ 𝜆∈[1, 𝛾], 𝛾 = min(𝛽), 𝛽∈(1, +∞)∋𝙧(𝛽)∈𝘍ₛₙ.

Given that point along the viewing vector that intersects the faces
of therendering volume will necessarily lie within one of the planes
𝘍ₛₙ, we can restrict our search of points to six possibilites given by  
𝙧ₙ(𝜆)∈𝘍₊ₙ ⇒ 𝙑ₙ+𝜆𝙙ₙ=𝘤₊ ⇒ 𝜆=(𝘤₊-𝙑ₙ)/𝙙ₙ and  
𝙧ₙ(𝜆)∈𝘍₋ₙ ⇒ 𝙑ₙ+𝜆𝙙ₙ=𝘤₋ ⇒ 𝜆=(𝘤₋-𝙑ₙ)/𝙙ₙ ∀𝘯∈(𝘹, 𝘺, 𝘻).  
Note that 𝙙ₙ=0 ⇒ 𝙧ₙ(𝜆)∉𝘍ₛₙ.

Let 𝞴₊ = (𝘤₊-𝙑)/𝙙 and 𝞴₋ = (𝘤₋-𝙑)/𝙙 be vectors containing the values
of 𝜆 ∋ 𝙧(𝞴₊ₙ)∈𝘍₊ₙ and 𝙧(𝞴₊ₙ)∈𝘍₋ₙ ∀𝘯∈(𝘹, 𝘺, 𝘻), respectively.

Since 𝞴₊ and 𝞴₋ will contain values in the range (-∞, +∞), and we
need min(𝞴 > 1), we need to eliminate candidates ≤ 1. To achieve this
without entering non-uniform flow control, we look to the GLSL 4.60.8
specifications document, available
[here](https://registry.khronos.org/OpenGL/specs/gl/GLSLangSpec.4.60.pdf).

Of particular intrest are the following:

From Section 8.3:  
> ***genFType step(float edge, genFType x)***  
Returns 0.0 if x < edge; otherwise it returns 1.0.

> ***genFType abs(genFType x)***  
Returns x if x ≥ 0; otherwise it returns -x.

> ***genFType min(genFType x, genFType y)***  
Returns y if y < x; otherwise it returns x.


> ***genFType frexp(highp genFType x, out highp genIType exp)***  
Splits x into a floating-point significand in the
range [0.5,1.0], and an integral exponent of two,
such that x = significant * 2^exponent.

> ***genFType ldexp(highp genFType x, highp genIType exp)***  
Builds a floating-point number from x and the
corresponding integral exponent of two in exp, returning:
significand * 2^exponent.

From Section 4.7.1:  
> The following rules apply to highp for both single and double-
precision operations: Signed infinities and zeros are generated as
dictated by IEEE, but subject to the precisions allowed in the 
following table.
>
>| Operation             | Precision                                       |
>|-----------------------|-------------------------------------------------|
>| a + b, a - b, a * b   |  Correctly rounded.                             |
>| <, <=, ==, >, >=      |  Correct result.                                |
>| a / b, 1.0 / b        |  2.5 ULP for \|b\| in the range [2^-126, 2^126] |

From the IEEE standard 754-2019, Section 7.3:  
> The divideByZero exception shall be signaled if and only if an
exact infinite result is defined for an operation on finite operands.
The default result of divideByZero shall be an ∞ correctly signed
according to the operation:  
― For division, when the divisor is zero and the dividend is a finite
non-zero number, the sign of the infinity is the exclusive OR of the
operands’ signs.

----------------------------------------------------------------------------

Given this information, we can now eliminate the aforementioned
candidate values in 𝞴₊ and 𝞴₋ by "sending" values that lie outside the
desired range to +∞ thusly:  
𝞴₊′ = abs(𝞴₊/step(𝜀₊, 𝞴₊ - 1.0));  
𝞴₋′ = abs(𝞴₋/step(𝜀₋, 𝞴₋ - 1.0));

The values of 𝜀 must be chosen carefully. If 𝜀 is slightly too large,
and ||𝙙|| ≫ ||𝙧(𝛾)-𝙋||, then 𝛾 will be "sent" to +∞ and the next
smallest candidate will be chosen (potentially +∞). If 𝜀 is slightly
too small, the value that should be 1.0 may be spurriously included,
considering the potential floating-point error of up to 2.5 ULP.
Therefore, setting 𝜀 = 4 ULP for the value of 𝙙 should suffice:[^1]

```
ivec3 exp;
frexp(lambdaMax, exp);
vec3 epsilonMax = 4.0 * ldexp(vec3(FLT_EPSILON), max(exp - 1, ivec3(FLT_MIN_EXP)));
frexp(lambdaMin, exp);
vec3 epsilonMin = 4.0 * ldexp(vec3(FLT_EPSILON), max(exp - 1, ivec3(FLT_MIN_EXP)));
```

Now that we have 𝞴₊′ and 𝞴₋′, we find our target value:
𝛾 = min(𝞴₊′.x, min(𝞴₊′.y, min(𝞴₊′.z, min(𝞴₋′.x, min(𝞴₋′.y, 𝞴₋′.z)))));

To map 𝘋 to the texture3D data, let 𝙑′ = 𝙑 - 𝘤₋, 𝙧₁ = 𝙋 - 𝘤₋,
and 𝙧₂ = 𝙑′ + 𝛾𝙙. [^2]

To "march" down the ray, we interpolate between 𝙧₁ and 𝙧₂ at regular
intervals, taking a texture sample at each step. For this application,
2 samples per texel was chosen, so the step size will be 
1 / (||𝙧₂ - 𝙧₁|| * 2 * textureSize). Successive adding may be faster,
but we will need to interpolate between derivatives of 𝙧₁ and 𝙧₂ anyway.

[^1]: The exact justification for this is beyond the scope of this document.
A full explanation may be written at a later date.

[^2]: Since 𝘋 in this application was chosen to be of dimensions 1x1x1,
we do not need to scale. If this algorithm is used in a situation
where scaling is required, we leave that as an exercise for the
developers.