#version 100
precision highp float;

// Input vertex attributes (from vertex shader).
varying vec2 fragTexCoord;
varying vec4 fragColor;

// Input uniform values.
uniform vec2  screenSize;
uniform vec2  offset;
uniform float scale;
uniform vec2  customHue;
uniform vec2  complexC;


// Returns the sum of the two given complex numbers.
vec2 complexSum(const vec2 c1, const vec2 c2) {
    return vec2(c1.x + c2.x, c1.y + c2.y);
}

// Returns the square of the given complex number.
vec2 complexSquare(const vec2 c) {
    return vec2(c.x * c.x - c.y * c.y, 2.0 * c.x * c.y);
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


void main()
{
    // The escape radius for points in the set.
    float escapeRadius = 2.0;
    float escapeRadSq  = escapeRadius * escapeRadius;
    
    // Define z and z^2.
    vec2 z  = (fragTexCoord * screenSize - screenSize / 2.0) / (0.5 * scale * screenSize.y) + offset / scale;
    vec2 z2 = complexSquare(z);

    // Compute the julia set.
    int i = 0; const int iMax = 1000;
    for (int counter = 1; counter < iMax; counter++)
    {
        if (z2.x + z2.y < escapeRadSq)
            i = counter + 1;

        z  = complexSum(z2, complexC);
        z2 = complexSquare(z);
    };

    // Define gradient colors.
    vec4 startHSV  = vec4(0.0,         0.0, 1.0, 1.0); // Black.
    vec4 middleHSV = vec4(customHue.x, 1.0, 1.0, 1.0); // Custom color.
    vec4 endHSV    = vec4(customHue.y, 1.0, 0.0, 1.0); // Custom background hue.
    vec4 centerRGB = vec4(0.0,         0.0, 0.0, 0.0); // Black.

    // Color the pixel in function of the number of iterations.
    if (i >= iMax)
    {
        gl_FragColor = centerRGB;
    }
    else
    {
        float lerpVal = 1.0 - float(i) / float(iMax);
        if (lerpVal < 0.5)
            gl_FragColor = HSVtoRGB(mix(startHSV, middleHSV, lerpVal * 2.0));
        else
            gl_FragColor = HSVtoRGB(mix(middleHSV, endHSV, lerpVal * 2.0 - 1.0));
    }
}