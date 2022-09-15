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
    : screenSize(_screenSize), exportScale(4)
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

void FractalRenderer::ProcessInputs()
{
    {
        // Update fractal scale.
        bool  scaleChanged = false;
        float scaleSpeed = 0.01f;
        float prevScale  = scale;
        if (IsKeyDown(KEY_E)) { scale += scaleSpeed; scaleChanged = true; }
        if (IsKeyDown(KEY_Q)) { scale -= scaleSpeed; scaleChanged = true; }
        if (float mouseWheelMove = GetMouseWheelMove()) {
            scale += mouseWheelMove / 6;
            scaleChanged = true;
        }

        if (scaleChanged)
        {
            float scaleOffset = (float)(pow(2.0, scale) / pow(2.0, prevScale));
            offset = { offset.x * scaleOffset, offset.y * scaleOffset };
            ValueModifiedThisFrame(ModifiableValues::Scale);
        }
    }

    {
        // Update fractal offset.
        bool  offsetChanged = false;
        float moveSpeed = 0.01f;
        if (IsKeyDown(KEY_D)) { offset.x += moveSpeed; offsetChanged = true; }
        if (IsKeyDown(KEY_A)) { offset.x -= moveSpeed; offsetChanged = true; }
        if (IsKeyDown(KEY_S)) { offset.y += moveSpeed; offsetChanged = true; }
        if (IsKeyDown(KEY_W)) { offset.y -= moveSpeed; offsetChanged = true; }
        if (IsMouseButtonDown(0)) {
            Vector2 mouseDelta = GetMouseDelta();
            if (mouseDelta.x != 0 && mouseDelta.y != 0) {
                offset = { offset.x - mouseDelta.x / 500, offset.y - mouseDelta.y / 500 };
                offsetChanged = true;
            }
        }
        
        if (offsetChanged)
            ValueModifiedThisFrame(ModifiableValues::Offset);
    }

    {
        // Update the sine automation parameters.
        float wlModifSpeed = 0.1f;
        float aModifSpeed  = 0.0001f;
        if (IsKeyDown(KEY_KP_3)) sineParams.x += wlModifSpeed;
        if (IsKeyDown(KEY_KP_1)) sineParams.x -= wlModifSpeed;
        if (IsKeyDown(KEY_KP_5)) sineParams.y += aModifSpeed;
        if (IsKeyDown(KEY_KP_2)) sineParams.y -= aModifSpeed;
        if (sineParams.x < 0.01f)
            sineParams.x = 0.01f;
        if (sineParams.y < 0.f)
            sineParams.y = 0.f;
    }

    if (curFractal == FractalTypes::JuliaSet)
    {
        // Update julia set complex c.
        bool  complexChanged = false;
        float cModifSpeed = 0.001f;
        if (IsKeyDown(KEY_LEFT_SHIFT )) cModifSpeed *= 0.1f;
        if (IsKeyDown(KEY_RIGHT_SHIFT)) cModifSpeed *= 0.1f;
        if (IsKeyDown(KEY_RIGHT)) { complexC.x += cModifSpeed; complexChanged = true; }
        if (IsKeyDown(KEY_LEFT )) { complexC.x -= cModifSpeed; complexChanged = true; }
        if (IsKeyDown(KEY_UP   )) { complexC.y += cModifSpeed; complexChanged = true; }
        if (IsKeyDown(KEY_DOWN )) { complexC.y -= cModifSpeed; complexChanged = true; }

        if (sineParams.x >= 0.1 && sineParams.y > 0) 
            complexChanged = true;
        if (complexChanged)
            ValueModifiedThisFrame(ModifiableValues::Complex);
    }

    {
        // Update the fractal hues.
        bool  hueChanged = false;
        float hueModifSpeed = 0.01f;
        if (IsKeyDown(KEY_KP_9)) { customHue.x += hueModifSpeed; hueChanged = true; }
        if (IsKeyDown(KEY_KP_7)) { customHue.x -= hueModifSpeed; hueChanged = true; }
        if (IsKeyDown(KEY_KP_6)) { customHue.y += hueModifSpeed; hueChanged = true; }
        if (IsKeyDown(KEY_KP_4)) { customHue.y -= hueModifSpeed; hueChanged = true; }

        if (hueChanged)
            ValueModifiedThisFrame(ModifiableValues::Hue);
    }

    // Change the current fractal.
    static bool enterDownLastFrame = false;
    if (IsKeyDown(KEY_ENTER) || IsKeyDown(KEY_KP_ENTER))  {
        if (!enterDownLastFrame) {
            ++curFractal;
            ValueModifiedThisFrame(ModifiableValues::CurFractal);
        }
        enterDownLastFrame = true;
    }
    else {
        enterDownLastFrame = false;
    }
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
