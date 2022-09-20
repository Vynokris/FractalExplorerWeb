#pragma once
#include <raylib.h>
#include <cmath>
#include <mutex>


// ----- Vector2 operators ----- //

Vector2 operator- (const Vector2& v);
Vector2 operator+=(      Vector2& v1, const Vector2& v2);
Vector2 operator+=(      Vector2& v1, const float  & v2);
Vector2 operator+ (const Vector2& v1, const Vector2& v2);
Vector2 operator+ (const Vector2& v1, const float  & v2);
Vector2 operator+ (const float  & v1, const Vector2& v2);
Vector2 operator-=(      Vector2& v1, const Vector2& v2);
Vector2 operator-=(      Vector2& v1, const float  & v2);
Vector2 operator- (const Vector2& v1, const Vector2& v2);
Vector2 operator- (const Vector2& v1, const float  & v2);
Vector2 operator- (const float  & v1, const Vector2& v2);
Vector2 operator*=(      Vector2& v1, const Vector2& v2);
Vector2 operator*=(      Vector2& v1, const float  & v2);
Vector2 operator* (const Vector2& v1, const Vector2& v2);
Vector2 operator* (const Vector2& v1, const float  & v2);
Vector2 operator* (const float  & v1, const Vector2& v2);
Vector2 operator/=(      Vector2& v1, const Vector2& v2);
Vector2 operator/=(      Vector2& v1, const float  & v2);
Vector2 operator/ (const Vector2& v1, const Vector2& v2);
Vector2 operator/ (const Vector2& v1, const float  & v2);
Vector2 operator/ (const float  & v1, const Vector2& v2);

float   remap       (const float  & value, const float  & inputStart, const float  & inputEnd, const float  & outputStart, const float  & outputEnd);
Vector2 remapVector2(const Vector2& value, const Vector2& inputStart, const Vector2& inputEnd, const Vector2& outputStart, const Vector2& outputEnd);


// ----- Complex maths ----- //

#define complexI Vector2{ 0, 1 }

// Returns the product of the two given complex numbers.
Vector2 complexProd(const Vector2 c1, const Vector2 c2);

// Returns the division of the first given complex number by the second.
Vector2 complexDiv(const Vector2 c1, const Vector2 c2);

// Returns the absolute value of the given complex number.
float complexAbs(const Vector2 c);

// Returns the argument (theta) of the given complex number.
float complexArg(const Vector2 c);

// Returns the conjugate of the given complex number.
Vector2 complexConj(const Vector2 c);

// Returns the square of the given complex number.
Vector2 complexSquare(const Vector2 c);

// Returns the nth power of the given complex.
Vector2 complexPow(const Vector2 c, const float n);

// Returns the square root of the given complex number.
Vector2 complexSqrt(const Vector2 c);

// Returns the exponential of the given complex number.
Vector2 complexExp(const Vector2 c);

// Returns the natural logarithm of the given complex number.
Vector2 complexLog(const Vector2 c);

// Returns the given complex number risen to the given complex power.
Vector2 complexPowC(const Vector2 c, const Vector2 n);

// Returns the cosine of the given complex number.
Vector2 complexCos(const Vector2 c);

// Returns the sine of the given complex number.
Vector2 complexSin(const Vector2 c);

// Returns the tangent of the given complex number.
Vector2 complexTan(const Vector2 c);


// ----- Fractal maths ----- //

// Apply the right fractal transformation to z and z2. Returns false if iterations should stop.
bool fractalFunc(const int& i, Vector2& z, Vector2& z2, Vector2 c, const float& escapeRadSq, const int& curFractal, const float& time);

// Returns the offset normally applied to c in the current fractal.
Vector2 getFractalOffset(const int& curFractal);

// Adds 1 to the heatmap at the position of the given complex.
void plotComplexOnHeatmap(unsigned int* heatmap, unsigned int& maxHeatmapVal, std::mutex& writingLock, const Vector2* screenBounds, const Vector2& exportSize, const int& curFractal, Vector2 z);
