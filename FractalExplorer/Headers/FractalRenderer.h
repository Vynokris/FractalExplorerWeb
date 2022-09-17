#pragma once
#include <raylib.h>
#include <chrono>

#define FRACTAL_COUNT 4
enum class FractalTypes
{
    Mandelbrot,
    BurningShip,
    MoonSet,
    CrossSet,
};
FractalTypes operator++(FractalTypes& type);

class FractalNames
{
public:
    static const char* names[FRACTAL_COUNT];
};

enum class ModifiableValues
{
    Scale,
    Offset,
    Hue,
    Complex,
    CurFractal,
    ColorStyle,
};

class FractalRenderer
{
private:
    std::chrono::time_point<std::chrono::system_clock> startTime;
    float         exportScale;
    RenderTexture screenTexture, exportTexture;
    Shader        fractalShader;
    bool          valueModifiedThisFrame = true;
    bool          shouldExportImage      = false;

    void ExportToImage();

public:
    float         scale      =   0.f;
    Vector2       offset     = { 0.f, 0.f };
    Vector2       complexC   = { -1.35f, 0.05f };
    Vector2       customHue  = { 2.26893f, 3.14159f };
    Vector2       sineParams = { 1.f, 0.f };
    const Vector2 screenSize;
    FractalTypes  curFractal     = FractalTypes::Mandelbrot;
    bool          renderJuliaSet = false;
    bool          colorPxWithZ   = false;

    FractalRenderer(const Vector2& _screenSize);
    ~FractalRenderer();
    void  SendDataToShader();
    void  Draw();
    void  StartImageExport();
    float GetExportScale();
    void  SetExportScale(const float& _exportScale);
    void  ValueModifiedThisFrame(const ModifiableValues& modifiedValue);
};