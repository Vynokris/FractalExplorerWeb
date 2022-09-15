#pragma once
#include <raylib.h>
#include <chrono>

enum class FractalTypes
{
    Mandelbrot,
    JuliaSet,
};
FractalTypes operator++(FractalTypes& type);

class FractalNames
{
public:
    static const char* names[2];
};

enum class ModifiableValues
{
    Scale,
    Offset,
    Hue,
    Complex,
    CurFractal,
};

class FractalRenderer
{
private:
    std::chrono::time_point<std::chrono::system_clock> startTime;
    const Vector2 screenSize;
    float         exportScale;
    RenderTexture screenTexture, exportTexture;
    Shader        fractalShaders[2];
    bool          valueModifiedThisFrame = true;
    bool          shouldExportImage      = false;

    void ExportToImage();

public:
    float        scale      =   0.f;
    Vector2      offset     = { 0.f, 0.f };
    Vector2      complexC   = { -1.35f, 0.05f };
    Vector2      customHue  = { 2.26893f, 3.14159f };
    Vector2      sineParams = { 1.f, 0.f };
    FractalTypes curFractal = FractalTypes::Mandelbrot;

    FractalRenderer(const Vector2& _screenSize);
    ~FractalRenderer();
    void  SendDataToShader();
    void  Draw();
    void  StartImageExport();
    float GetExportScale();
    void  SetExportScale(const float& _exportScale);
    void  ValueModifiedThisFrame(const ModifiableValues& modifiedValue);
};