#pragma once
#include <raylib.h>
#include <chrono>

#define FRACTAL_COUNT 5
enum class FractalTypes
{
    MandelbrotSet,
    BurningShip,
    CrescentMoon,
    NorthStar,
    LoversFractal,
};
FractalTypes operator++(FractalTypes& type);
FractalTypes operator--(FractalTypes& type);

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
    SineParams,
    CurFractal,
    ColorStyle,
};

class FractalRenderer
{
private:
    std::chrono::time_point<std::chrono::system_clock> startTime;
    Vector2       screenSize;
    float         exportScale;
    RenderTexture screenTexture, exportTexture;
    Shader        fractalShader;
    bool          valueModifiedThisFrame = true;
    bool          shouldExportImage      = false;

    void ExportToImage();
    void UpdateShaderTime();

public:
    float         scale      =   0.f;
    Vector2       offset     = { 0.f, 0.f };
    Vector2       complexC   = { -1.35f, 0.05f };
    Vector2       customHue  = { 2.26893f, 3.14159f };
    Vector2       sineParams = { 1.f, 0.f };
    FractalTypes  curFractal     = FractalTypes::MandelbrotSet;
    bool          renderJuliaSet = false;
    bool          colorPxWithZ   = false;

    FractalRenderer(const Vector2& _screenSize, const int& targetFPS);
    ~FractalRenderer();

    void  SendDataToShader();
    void  Draw();
    void  StartImageExport();
    void  SetExportScale(const float& _exportScale);
    void  ValueModifiedThisFrame(const ModifiableValues& modifiedValue);

    Vector2 GetScreenSize () { return screenSize;  }
    float   GetExportScale() { return exportScale; }
};
