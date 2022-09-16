#include "FractalRenderer.h"
#include <cmath>
#include <string>
#include <rlgl.h>
#include <external/stb_image.h>
#include <external/stb_image_write.h>
#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

const char* FractalNames::names[FRACTAL_COUNT] = { "Mandelbrot Set", "Burning Ship", "Moon Set" };

FractalTypes operator++(FractalTypes& type)
{
    type = static_cast<FractalTypes>(((int)type + 1) % FRACTAL_COUNT);
    return type;
}


FractalRenderer::FractalRenderer(const Vector2& _screenSize)
    :  exportScale(4), screenSize(_screenSize)
{
    startTime = std::chrono::system_clock::now();

    // Initialize raylib.
    InitWindow((int)screenSize.x, (int)screenSize.y, "Fractal Explorer");
    SetTargetFPS(60);

    // Load rendertextures and shaders.
    screenTexture = LoadRenderTexture((int)screenSize.x, (int)screenSize.y);
    exportTexture = LoadRenderTexture((int)(1920 * exportScale), (int)(1080 * exportScale));
    fractalShader = LoadShader(NULL, "Shaders/Fractal.frag");
    SetShaderValue(fractalShader, GetShaderLocation(fractalShader, "screenSize"), &screenSize, SHADER_UNIFORM_VEC2);
    SendDataToShader();
}

FractalRenderer::~FractalRenderer()
{
    CloseWindow();
}

void FractalRenderer::SendDataToShader()
{
    float scaleSquare   = (float)pow(2.0, scale);
    int   curFractalInt = (int)curFractal;
    int   juliaSetInt   = (int)renderJuliaSet;
    int   colorWithZInt = (int)colorPxWithZ;
    SetShaderValue(fractalShader, GetShaderLocation(fractalShader, "curFractal"), &curFractalInt, SHADER_UNIFORM_INT);
    SetShaderValue(fractalShader, GetShaderLocation(fractalShader, "juliaSet"  ), &juliaSetInt,   SHADER_UNIFORM_INT);
    SetShaderValue(fractalShader, GetShaderLocation(fractalShader, "colorWithZ"), &colorWithZInt, SHADER_UNIFORM_INT);
    SetShaderValue(fractalShader, GetShaderLocation(fractalShader, "offset"    ), &offset,        SHADER_UNIFORM_VEC2);
    SetShaderValue(fractalShader, GetShaderLocation(fractalShader, "scale"     ), &scaleSquare,   SHADER_UNIFORM_FLOAT);
    SetShaderValue(fractalShader, GetShaderLocation(fractalShader, "customHue" ), &customHue,     SHADER_UNIFORM_VEC2);
    SetShaderValue(fractalShader, GetShaderLocation(fractalShader, "complexC"  ), &complexC,      SHADER_UNIFORM_VEC2);
}

void FractalRenderer::Draw()
{
    if (valueModifiedThisFrame)
    {
        // Draw the current fractal onto the screen rendertexture.
        BeginTextureMode(screenTexture);
        {
            ClearBackground(BLACK);
            BeginShaderMode(fractalShader);
            {
                DrawTextureRec(screenTexture.texture, { 0, 0, screenSize.x, -screenSize.y }, { 0, 0 }, WHITE);
            }
            EndShaderMode();
        }
        EndTextureMode();
    }
    valueModifiedThisFrame = false;

    // Draw the fractal rendertexture on the screen.
    ClearBackground(BLACK);
    DrawTextureRec(screenTexture.texture, { 0, 0, screenSize.x, screenSize.y }, { 0, 0 }, WHITE);

    // Export the fractal to an image if specified.
    if (shouldExportImage)
        ExportToImage();
}

void FractalRenderer::StartImageExport()
{
    shouldExportImage = true;
}

void FractalRenderer::ExportToImage()
{
    // Draw the current fractal on the export rendertexture.
    BeginTextureMode(exportTexture);
    {
        ClearBackground(BLACK);
        BeginShaderMode(fractalShader);
        {
            DrawTextureRec(exportTexture.texture, { 0, 0, 1920 * exportScale, 1080 * exportScale }, { 0, 0 }, WHITE);
        }
        EndShaderMode();
    }
    EndTextureMode();

    // Save the export rendertexture to an image file.
    Image image = LoadImageFromTexture(exportTexture.texture);
    // ImageFlipVertical(&image);
    const char* filename = "fractal.png";

    #if defined(PLATFORM_WEB)
        int dataSize = 0;
        unsigned char* fileData = stbi_write_png_to_mem((const unsigned char*)image.data, image.width*4, image.width, image.height, 4, &dataSize);
        EM_ASM_({ window.download($0, $1, $2) }, filename, fileData, dataSize);
        RL_FREE(fileData);
    #else
        ExportImage(image, filename);
    #endif

    UnloadImage(image);
    shouldExportImage = false;
}

float FractalRenderer::GetExportScale()
{
    return exportScale;
}

void FractalRenderer::SetExportScale(const float& _exportScale)
{
    UnloadRenderTexture(exportTexture);
    exportScale   = _exportScale;
    exportTexture = LoadRenderTexture((int)(1920 * exportScale), (int)(1080 * exportScale));
}

void FractalRenderer::ValueModifiedThisFrame(const ModifiableValues& modifiedValue)
{
    switch (modifiedValue)
    {
        case ModifiableValues::Scale:
        {
            float scaleSquare = (float)pow(2.0, scale);
            SetShaderValue(fractalShader, GetShaderLocation(fractalShader, "scale"), &scaleSquare, SHADER_UNIFORM_FLOAT);
        }
        [[fallthrough]];
        case ModifiableValues::Offset:
        {
            SetShaderValue(fractalShader, GetShaderLocation(fractalShader, "offset"), &offset, SHADER_UNIFORM_VEC2);
            break;
        }
        case ModifiableValues::Hue:
        {
            SetShaderValue(fractalShader, GetShaderLocation(fractalShader, "customHue"), &customHue, SHADER_UNIFORM_VEC2);
            break;
        }
        case ModifiableValues::Complex:
        {
            if (renderJuliaSet)
            {
                Vector2 animatedC = complexC;
                if (sineParams.x >= 0.1 && sineParams.y > 0) {
                    float timeSinceStart = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - startTime).count() / 1000.f;
                    float sinOffset = sin(timeSinceStart / sineParams.x) * sineParams.y;
                    animatedC = Vector2({ complexC.x + sinOffset, complexC.y + sinOffset });
                }
                SetShaderValue(fractalShader, GetShaderLocation(fractalShader, "complexC"), &animatedC, SHADER_UNIFORM_VEC2);
            }
            break;
        }
        case ModifiableValues::CurFractal:
        {
            int curFractalInt = (int)curFractal;
            int juliaSetInt   = (int)renderJuliaSet;
            SetShaderValue(fractalShader, GetShaderLocation(fractalShader, "curFractal"), &curFractalInt, SHADER_UNIFORM_INT);
            SetShaderValue(fractalShader, GetShaderLocation(fractalShader, "juliaSet"  ), &juliaSetInt,   SHADER_UNIFORM_INT);
            break;
        }
        case ModifiableValues::ColorStyle:
        {
            int colorWithZInt = (int)colorPxWithZ;
            SetShaderValue(fractalShader, GetShaderLocation(fractalShader, "colorWithZ"), &colorWithZInt, SHADER_UNIFORM_INT);
            break;
        }
        default:
        {
            break;
        }
    }
    valueModifiedThisFrame = true;
}
