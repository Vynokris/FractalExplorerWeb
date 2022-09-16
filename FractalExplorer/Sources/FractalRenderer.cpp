#include "FractalRenderer.h"
#include <cmath>
#include <string>
#include <rlgl.h>
#include <external/stb_image.h>
#include <external/stb_image_write.h>
#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

FractalTypes operator++(FractalTypes& type)
{
    type = static_cast<FractalTypes>(((int)type + 1) % 2);
    return type;
}

const char* FractalNames::names[2] = { "Mandelbrot Set", "Julia Set" };


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
    fractalShaders[0] = LoadShader(NULL, "Shaders/Mandelbrot.frag");
    fractalShaders[1] = LoadShader(NULL, "Shaders/JuliaSet.frag");
    SetShaderValue(fractalShaders[0], GetShaderLocation(fractalShaders[0], "screenSize"), &screenSize, SHADER_UNIFORM_VEC2);
    SetShaderValue(fractalShaders[1], GetShaderLocation(fractalShaders[1], "screenSize"), &screenSize, SHADER_UNIFORM_VEC2);
    SendDataToShader();
}

FractalRenderer::~FractalRenderer()
{
    CloseWindow();
}

void FractalRenderer::SendDataToShader()
{
    float scaleSquare = (float)pow(2.0, scale);
    SetShaderValue(fractalShaders[(int)curFractal], GetShaderLocation(fractalShaders[(int)curFractal], "offset"   ), &offset,      SHADER_UNIFORM_VEC2);
    SetShaderValue(fractalShaders[(int)curFractal], GetShaderLocation(fractalShaders[(int)curFractal], "scale"    ), &scaleSquare, SHADER_UNIFORM_FLOAT);
    SetShaderValue(fractalShaders[(int)curFractal], GetShaderLocation(fractalShaders[(int)curFractal], "customHue"), &customHue,   SHADER_UNIFORM_VEC2);
    if (curFractal == FractalTypes::JuliaSet) {
        SetShaderValue(fractalShaders[(int)curFractal], GetShaderLocation(fractalShaders[(int)curFractal], "complexC"), &complexC, SHADER_UNIFORM_VEC2);
    }
}

void FractalRenderer::Draw()
{
    if (valueModifiedThisFrame)
    {
        // Draw the current fractal onto the screen rendertexture.
        BeginTextureMode(screenTexture);
        {
            ClearBackground(BLACK);
            BeginShaderMode(fractalShaders[(int)curFractal]);
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
        BeginShaderMode(fractalShaders[(int)curFractal]);
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
            SetShaderValue(fractalShaders[(int)curFractal], GetShaderLocation(fractalShaders[(int)curFractal], "scale"), &scaleSquare, SHADER_UNIFORM_FLOAT);
        }
        [[fallthrough]];
        case ModifiableValues::Offset:
        {
            SetShaderValue(fractalShaders[(int)curFractal], GetShaderLocation(fractalShaders[(int)curFractal], "offset"), &offset, SHADER_UNIFORM_VEC2);
            break;
        }
        case ModifiableValues::Hue:
        {
            SetShaderValue(fractalShaders[(int)curFractal], GetShaderLocation(fractalShaders[(int)curFractal], "customHue"), &customHue, SHADER_UNIFORM_VEC2);
            break;
        }
        case ModifiableValues::Complex:
        {
            if (curFractal == FractalTypes::JuliaSet)
            {
                Vector2 animatedC = complexC;
                if (sineParams.x >= 0.1 && sineParams.y > 0) {
                    float timeSinceStart = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - startTime).count() / 1000.f;
                    float sinOffset = sin(timeSinceStart / sineParams.x) * sineParams.y;
                    animatedC = Vector2({ complexC.x + sinOffset, complexC.y + sinOffset });
                }
                SetShaderValue(fractalShaders[(int)curFractal], GetShaderLocation(fractalShaders[(int)curFractal], "complexC"), &animatedC, SHADER_UNIFORM_VEC2);
            }
            break;
        }
        case ModifiableValues::CurFractal:
        {
            SendDataToShader();
            break;
        }
        default:
        {
            break;
        }
    }
    valueModifiedThisFrame = true;
}
