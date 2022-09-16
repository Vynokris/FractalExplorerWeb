#version 130
#define complexI vec2(0, 1)
precision highp float;

// Input vertex attributes (from vertex shader).
varying vec2 fragTexCoord;
varying vec4 fragColor;

// Input uniform values.
uniform vec2  screenSize;
uniform int   curFractal;
uniform int   juliaSet;
uniform int   colorWithZ;
uniform vec2  offset;
uniform float scale;
uniform vec2  customHue;
uniform vec2  complexC;

// Returns the sum of the two given complex numbers.
vec2 complexSum(const vec2 c1, const vec2 c2) {
    return vec2(c1.x + c2.x, c1.y + c2.y);
}

// Returns the substraction of the first given complex number by the second.
vec2 complexSub(const vec2 c1, const vec2 c2) {
    return vec2(c1.x + c2.x, c1.y + c2.y);
}

// Returns the product of the two given complex numbers.
vec2 complexProd(const vec2 c1, const vec2 c2) {
    return vec2(c1.x*c2.x - c1.y*c2.y, c1.x*c2.y + c1.y*c2.x);
}

// Returns the division of the first given complex number by the second.
vec2 complexDiv(const vec2 c1, const vec2 c2) {
    float c2x2 = c2.x * c2.x, c2y2 = c2.y * c2.y;
    return vec2((c1.x*c2.x + c1.y*c2.y) / (c2x2 + c2y2), (c1.y*c2.x - c1.x*c2.y) / (c2x2 + c2y2));
}

// Returns the absolute value of the given complex number.
float complexAbs(const vec2 c) {
    return sqrt(c.x * c.x + c.y * c.y);
}

// Returns the argument (theta) of the given complex number.
float complexArg(const vec2 c) {
    return atan(c.y / c.x);
}

// Returns the square of the given complex number.
vec2 complexSquare(const vec2 c) {
    return vec2(c.x * c.x - c.y * c.y, 2.0 * c.x * c.y);
}

// Returns the nth power of the given complex.
vec2 complexPow(const vec2 c, const float n) {
    float rn = pow(complexAbs(c), n);
    float on = complexArg(c) * n;
    return vec2(cos(on), sin(on)) * rn;
}

// Returns the square root of the given complex number.
vec2 complexSqrt(const vec2 c) {
    float tmp = sqrt((complexAbs(c) + c.x) / 2.0);
    return vec2(tmp, (c.y / abs(c.y)) * tmp);
}

// Returns the exponential of the given complex number.
vec2 complexExp(const vec2 c) {
    return exp(c.x) * vec2(cos(c.y), sin(c.y));
}

// Returns the natural logarithm of the given complex number.
vec2 complexLn(const vec2 c) {
    return vec2(log(complexAbs(c)), complexArg(c));
}

// Returns the cosine of the given complex number.
vec2 complexCos(const vec2 c) {
    return vec2(cos(c.x) + sinh(c.y), sin(c.x) + cosh(c.y));
}

// Returns the sine of the given complex number.
vec2 complexSin(const vec2 c) {
    return vec2(sin(c.x) + cosh(c.y), cos(c.x) + sinh(c.y));
}

// Returns the tangent of the given complex number.
vec2 complexTan(const vec2 c) {
    float tanX  = tan (c.x),   tanY   = tan (c.y);
    float tanhX = tanh(c.x),   tanhY  = tanh(c.y);
    float tanX2 = tanX * tanX, tanhY2 = tanhY * tanhY;
    return vec2((tanX-tanX*tanhY2) / (1.0+tanX2*tanhY2), (tanhY+tanX2*tanhY) / (1.0+tanX2*tanhY2));
}

// Returns the given float in scientific notation (x: val, y: exponent).
vec2 scFloatCreate(float val, int exponent) {
    for (int i = 0; i < 1000 && abs(val) > 100.0; i++) {
        val /= 10.0;
        exponent++;
    }
    while (abs(val) < 1.0 && val != 0.0) {
        val *= 10.0;
        exponent--;
    }

    return vec2(val, float(exponent));
}

// Function used only in HSVtoRGB to convert from hsv to rgb.
float ColorConversion(const vec4 hsv, float k)
{
    float t = 4.0 - k;
    k = (t < k)? t : k;
    k = (k < 1.0)? k : 1.0;
    k = (k > 0.0)? k : 0.0;
    return hsv.z - hsv.z * hsv.y * k;
}

// Convert hsva color to rgba.
vec4 HSVtoRGB(const vec4 hsv)
{
    vec4 color = vec4(0, 0, 0, hsv.a);
    color.r = ColorConversion(hsv, mod((5.0 + hsv.x), 6.0));
    color.g = ColorConversion(hsv, mod((3.0 + hsv.r), 6.0));
    color.b = ColorConversion(hsv, mod((1.0 + hsv.r), 6.0));

    return color;
}


// Apply the right fractal transformation to z and z2.
void fractalFunc(inout vec2 z, inout vec2 z2, const vec2 c) 
{
    // Mandelbrot set.
    if (curFractal == 0)
    {
        z  = complexSum(z2, c);
    }
    // Burning ship.
    else if (curFractal == 1)
    {
        z = complexSum(complexSquare(vec2(abs(z.x), abs(z.y))), c);
    }
    // Moon set.
    else if (curFractal == 2)
    {
        z = complexDiv(complexSub(z, vec2(1, 0)), complexSum(complexExp(z), c));
    }
    z2 = complexSquare(z);
}


void main()
{
    // The point at which a pixel becomes black.
    float escapeRadius = 2.0;
    float escapeRadSq  = escapeRadius * escapeRadius;

    vec2 c, z, z2;
    if (juliaSet == 0)
    {
        c = (fragTexCoord * screenSize - screenSize / 2.0) / (0.5 * scale * screenSize.y) + offset.xy / scale + vec2(-0.125, 0.0);
        z  = vec2(0.0, 0.0);
        z2 = z;
    }
    else
    {
        c  = complexC;
        z  = (fragTexCoord * screenSize - screenSize / 2.0) / (0.5 * scale * screenSize.y) + offset / scale;
        z2 = complexSquare(z);
    }

    // Compute the mandelbrot equation.
    int i = 0; const int iMax = 500;
    for (int counter = 0; counter < iMax; counter++)
    {
        if (z2.x + z2.y < escapeRadSq) 
        {
            i = counter + 1;
            fractalFunc(z, z2, c);
        }
    };
    

    // Color the pixel in function of the number of iterations.
    if (colorWithZ == 0)
    {
        vec4 startHSV  = vec4(0.0,         0.0, 1.0, 1.0); // Black.
        vec4 middleHSV = vec4(customHue.x, 1.0, 1.0, 1.0); // Custom foreground hue.
        vec4 endHSV    = vec4(customHue.y, 1.0, 0.0, 1.0); // Custom background hue.
        vec4 centerRGB = vec4(0.0,         0.0, 0.0, 1.0); // Black.

        if (i >= iMax)
        {
            gl_FragColor = centerRGB;
        }
        else
        {
            float lerpVal = 1.0 - float(i) / float(iMax);
            if (lerpVal < 0.5) {
                gl_FragColor = HSVtoRGB(mix(startHSV, middleHSV, lerpVal * 2.0));
            }
            else {
                gl_FragColor = HSVtoRGB(mix(middleHSV, endHSV, lerpVal * 2.0 - 1.0));
            }
        }
    }

    // Color the pixel in function of z's value and exponent.
    else 
    {
        vec2  scZx = scFloatCreate(z.x, 0);
        vec2  scZy = scFloatCreate(z.y, 0);
        float mixX = mix(float(abs(scZx.y)) / 2.0, 1.0 / scZx.x, 0.5);
        float mixY = mix(float(abs(scZy.y)) / 2.0, 1.0 / scZy.x, 0.5);
        gl_FragColor = vec4(mixX, mix(mixX, mixY, 0.5), mixY, 1.0);
    }
}