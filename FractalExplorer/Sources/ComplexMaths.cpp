#include "ComplexMaths.h"
#include <vector>


// ----- Vector2 operators ----- //

Vector2 operator-(const Vector2& v) {
    return Vector2{ -v.x, -v.y };
}

Vector2 operator+=(Vector2& v1, const Vector2& v2) {
    v1 = v1 + v2;
    return v1;
}

Vector2 operator+=(Vector2& v1, const float& v2) {
    v1 = v1 + v2;
    return v1;
}

Vector2 operator+(const Vector2& v1, const Vector2& v2) {
    return Vector2{ v1.x + v2.x, v1.y + v2.y };
}

Vector2 operator+(const Vector2& v1, const float& v2) {
    return Vector2{ v1.x + v2, v1.y + v2 };
}

Vector2 operator+(const float& v1, const Vector2& v2) {
    return Vector2{ v1 + v2.x, v1 + v2.y };
}

Vector2 operator-=(Vector2& v1, const Vector2& v2) {
    v1 = v1 - v2;
    return v1;
}

Vector2 operator-=(Vector2& v1, const float& v2) {
    v1 = v1 - v2;
    return v1;
}

Vector2 operator-(const Vector2& v1, const Vector2& v2) {
    return Vector2{ v1.x - v2.x, v1.y - v2.y };
}

Vector2 operator-(const Vector2& v1, const float& v2) {
    return Vector2{ v1.x - v2, v1.y - v2 };
}

Vector2 operator-(const float& v1, const Vector2& v2) {
    return Vector2{ v1 - v2.x, v1 - v2.y };
}

Vector2 operator*=(Vector2& v1, const Vector2& v2) {
    v1 = v1 * v2;
    return v1;
}

Vector2 operator*=(Vector2& v1, const float& v2) {
    v1 = v1 * v2;
    return v1;
}

Vector2 operator*(const Vector2& v1, const Vector2& v2) {
    return Vector2{ v1.x * v2.x, v1.y * v2.y };
}

Vector2 operator*(const Vector2& v1, const float& v2) {
    return Vector2{ v1.x * v2, v1.y * v2 };
}

Vector2 operator*(const float& v1, const Vector2& v2) {
    return Vector2{ v1 * v2.x, v1 * v2.y };
}

Vector2 operator/=(Vector2& v1, const Vector2& v2) {
    v1 = v1 / v2;
    return v1;
}

Vector2 operator/=(Vector2& v1, const float& v2) {
    v1 = v1 / v2;
    return v1;
}

Vector2 operator/(const Vector2& v1, const Vector2& v2) {
    return Vector2{ v1.x / v2.x, v1.y / v2.y };
}

Vector2 operator/(const Vector2& v1, const float& v2) {
    return Vector2{ v1.x / v2, v1.y / v2 };
}

Vector2 operator/(const float& v1, const Vector2& v2) {
    return Vector2{ v1 / v2.x, v1 / v2.y };
}

float remap(const float& value, const float& inputStart, const float& inputEnd, const float& outputStart, const float& outputEnd) {
    return (value - inputStart) / (inputEnd - inputStart) * (outputEnd - outputStart) + outputStart;
}

Vector2 remapVector2(const Vector2& value, const Vector2& inputStart, const Vector2& inputEnd, const Vector2& outputStart, const Vector2& outputEnd) {
    return (value - inputStart) / (inputEnd - inputStart) * (outputEnd - outputStart) + outputStart;
}


// ----- Complex maths ----- //

// Returns the product of the two given complex numbers.
Vector2 complexProd(const Vector2 c1, const Vector2 c2) {
    return Vector2{ c1.x*c2.x - c1.y*c2.y, c1.x*c2.y + c1.y*c2.x };
}

// Returns the division of the first given complex number by the second.
Vector2 complexDiv(const Vector2 c1, const Vector2 c2) 
{
    float c2x2 = c2.x * c2.x, c2y2 = c2.y * c2.y;
    float c2sqSum = c2x2 + c2y2;
    if (c2sqSum == 0.f)
        return Vector2{ 0, 0 };

    return Vector2{ (c1.x*c2.x + c1.y*c2.y) / c2sqSum, (c1.y*c2.x - c1.x*c2.y) / c2sqSum };
}

// Returns the absolute value of the given complex number.
float complexAbs(const Vector2 c) {
    return sqrt(c.x * c.x + c.y * c.y);
}

// Returns the argument (theta) of the given complex number.
float complexArg(const Vector2 c) {
    return atan(c.y / c.x);
}

// Returns the conjugate of the given complex number.
Vector2 complexConj(const Vector2 c) {
    return Vector2{ c.x, -c.y };
}

// Returns the square of the given complex number.
Vector2 complexSquare(const Vector2 c) {
    return Vector2{ c.x * c.x - c.y * c.y, 2.f * c.x * c.y };
}

// Returns the nth power of the given complex.
Vector2 complexPow(const Vector2 c, const float n) {
    float rn = pow(complexAbs(c), n);
    float on = complexArg(c) * n;
    return Vector2{ cos(on), sin(on) } * rn;
}

// Returns the square root of the given complex number.
Vector2 complexSqrt(const Vector2 c) {
    float tmp = sqrtf((complexAbs(c) + c.x) / 2.f);
    return Vector2{ tmp, (c.y / abs(c.y)) * tmp };
}

// Returns the exponential of the given complex number.
Vector2 complexExp(const Vector2 c) {
    return Vector2{ cos(c.y), sin(c.y) } * exp(c.x);
}

// Returns the natural logarithm of the given complex number.
Vector2 complexLog(const Vector2 c) {
    return Vector2{ log(complexAbs(c)), complexArg(c) };
}

// Returns the given complex number risen to the given complex power.
Vector2 complexPowC(const Vector2 c, const Vector2 n) {
    return complexExp(complexProd(complexLog(c), n));
}

// Returns the cosine of the given complex number.
Vector2 complexCos(const Vector2 c) {
    return Vector2{ cos(c.x) + sinh(c.y), sin(c.x) + cosh(c.y) };
}

// Returns the sine of the given complex number.
Vector2 complexSin(const Vector2 c) {
    return Vector2{ sin(c.x) + cosh(c.y), cos(c.x) + sinh(c.y) };
}

// Returns the tangent of the given complex number.
Vector2 complexTan(const Vector2 c) {
    float tanX  = tan (c.x),   tanY   = tan (c.y);
    float tanhX = tanh(c.x),   tanhY  = tanh(c.y);
    float tanX2 = tanX * tanX, tanhY2 = tanhY * tanhY;
    return Vector2{ (tanX-tanX*tanhY2) / (1.f+tanX2*tanhY2), (tanhY+tanX2*tanhY) / (1.f+tanX2*tanhY2) };
}


// ----- Fractal maths ----- //

// Apply the right fractal transformation to z and z2. Returns false if iterations should stop.
bool fractalFunc(const int& i, Vector2& z, Vector2& z2, Vector2 c, const float& escapeRadSq, const int& curFractal, const float& time)
{
    if (z2.x + z2.y < escapeRadSq)
    {
        switch (curFractal)
        {
            // Mandelbrot Set.
            case 0:
            {
                // c -= Vector2{ 0.25f, 0.0 };
                z = z2 + c;
                break;
            }
            // Burning Ship.
            case 1:
            {
                c -= Vector2{ 0.25f, 0.0 };
                z = complexSquare(Vector2{ abs(z.x), abs(z.y) }) + c;
                break;
            }
            // Crescent Moon.
            case 2:
            {
                z = complexDiv(z + Vector2{ 1, 0 }, complexExp(z) + c / 0.47f);
                break;
            }
            // North Star.
            case 3:
            {
                Vector2 zPlusC = z + complexProd((c + Vector2{ 0, -0.1f }) / 0.65f, -complexI);
                z = complexDiv(Vector2{ 1, 0 }, complexPow(zPlusC, 4));
                break;
            }
            // Black Hole.
            case 4:
            {
                Vector2 warp = Vector2{ cosf(time / 5), sinf(time / 5) } / 10;
                z = complexDiv(z, c) + complexI * complexPow(c * warp, 8) + complexDiv(z, warp) / 4.7f;
                break;
            }
            // The Orb.
            case 5:
            {
                if (i >= 250) return false;
                        c    = Vector2{ abs(-c.x), c.y };
                Vector2 warp = Vector2{ cosf(time + c.x), sinf(time + c.y) } / 10;
                z = complexDiv(z, c) + complexI * complexPowC(c, complexPow(c * warp, 4));
                break;
            }
            // Lovers' Fractal.
            case 6:
            {
                c = Vector2{ abs(c.x), c.y } * 0.75f + Vector2{ 0.125f, 0.155f };
                z = complexDiv(z2, -complexI + complexPowC(c, z)) + c;
                break;
            }
            default:
                return false;
        }

        z2 = complexSquare(z);
        return true;
    }
    return false;
}

// Returns the offset normally applied to c in the current fractal.
Vector2 getFractalOffset(const int& curFractal)
{
    switch (curFractal)
    {
        // Mandelbrot Set.
        case 0:
            return Vector2{ 0.25f, 0.0 };
        // Burning Ship.
        case 1:
            return Vector2{ 0.25f, 0.0 };
        // North Star.
        case 3:
            return Vector2{ 0, -0.1f };
        default:
            return Vector2{ 0, 0 };
    }
}

// Adds 1 to the heatmap at the position of the given complex.
void plotComplexOnHeatmap(unsigned int* heatmap, unsigned int& maxHeatmapVal, std::mutex& writingLock, const Vector2* screenBounds, const Vector2& exportSize, const int& curFractal, Vector2 z)
{
    // Make sure that it is still in the screen bounds.
    if (!(screenBounds[0].x <= z.x && z.x < screenBounds[1].x &&
          screenBounds[0].y <= z.y && z.y < screenBounds[1].y))
    {
        return;
    }

    bool verticalSymmetry   = false;
    bool horizontalSymmetry = false;
    if (curFractal == 0)
    {
        verticalSymmetry = true;
    }
    if (curFractal == 6)
    {
        z = Vector2{ abs(z.x - 0.125f), -z.y } / 0.75f + Vector2{ 0, 0.155f };
        horizontalSymmetry = true;
    }

    // Convert the complex coordinates to screen coordinates.
    Vector2 fractalOffset = getFractalOffset((int)curFractal);
    Vector2 zScreenCoords = remapVector2(z, screenBounds[0], screenBounds[1], fractalOffset, exportSize + fractalOffset);

    // Make sure the complex's screen coordinates are still inside of the export texture.
    if (!(0 <= zScreenCoords.x && zScreenCoords.x < exportSize.x &&
          0 <= zScreenCoords.y && zScreenCoords.y < exportSize.y))
    {
        return;
    }

    std::vector<size_t> arrayPositions; arrayPositions.reserve(5);
    static Vector2 neighboors[5] = { { 0, 0 }, { -1, 0 }, { 1, 0 }, { 0, -1 }, { 0, 1 } };
    for (int i = 0; i < 5; i++) {
        size_t pos = (size_t)((size_t)(zScreenCoords.y + neighboors[i].y) * (size_t)exportSize.x + (size_t)(zScreenCoords.x + neighboors[i].x));
        if (0 <= pos && pos < (size_t)(exportSize.x * exportSize.y))
            arrayPositions.push_back(pos);
    }
    if (verticalSymmetry) {
        Vector2 zVerticalSymmetry = remapVector2({ z.x, -z.y }, screenBounds[0], screenBounds[1], fractalOffset, exportSize + fractalOffset);
        for (int i = 0; i < 5; i++) {
            size_t pos = (size_t)((size_t)(zVerticalSymmetry.y + neighboors[i].y) * (size_t)exportSize.x + (size_t)(zVerticalSymmetry.x + neighboors[i].x));
            if (0 <= pos && pos < (size_t)(exportSize.x * exportSize.y))
                arrayPositions.push_back(pos);
        }
    }
    if (horizontalSymmetry) {
        Vector2 zHorizontalSymmetry = remapVector2({ -z.x, z.y }, screenBounds[0], screenBounds[1], fractalOffset, exportSize + fractalOffset);
        for (int i = 0; i < 5; i++) {
            size_t pos = (size_t)((size_t)(zHorizontalSymmetry.y + neighboors[i].y) * (size_t)exportSize.x + (size_t)(zHorizontalSymmetry.x + neighboors[i].x));
            if (0 <= pos && pos < (size_t)(exportSize.x * exportSize.y))
                arrayPositions.push_back(pos);
        }
    }

    // Increment the heatmap at the screen position of the complex.
    writingLock.lock();
    for (int i = 0; i < arrayPositions.size(); i++) {
        unsigned int newHeatmapVal = (i == 0 ? heatmap[arrayPositions[i]]+=2 : heatmap[arrayPositions[i]]++);
        if (newHeatmapVal > maxHeatmapVal) {
            maxHeatmapVal = newHeatmapVal;
        }
    }
    writingLock.unlock();
}
