#include "BhuddaRenderer.h"
#include "ComplexMaths.h"
#include <random>
#include <cmath>
#include <vector>
#include <iostream>
#include <mutex>
#include <algorithm>


void BuddhaRenderer::Render(const int& threadId)
{
    srand((unsigned int)time(NULL) + threadId * 1000);
    while (!killThreads.load())
    {
        if (!rendering.load())
            continue;
        
        // The point at which a pixel becomes black.
        float escapeRadius = 2.0;
        float escapeRadSq  = escapeRadius * escapeRadius;

        // Get a random complex inside of the screen bounds.
        Vector2 c = remapVector2({ (rand() % ((int)screenSize.x * 4)) / 4.f, (rand() % ((int)screenSize.y * 4)) / 4.f }, { 0, 0 }, screenSize, screenBounds[0], screenBounds[1]);

        // Define z, z^2 and z's history.
        Vector2 z  = Vector2{ 0.f, 0.f };
        Vector2 z2 = z;
        std::vector<Vector2> history;

        // Repeatedly apply the fractal function to the point to check if it's in the set or not.
        int i, iMax = 500;
        bool continueLoop = true;
        history.reserve(iMax);
        for (i = 0; i < iMax && continueLoop; i++) {
            continueLoop = fractalFunc(i, z, z2, c, escapeRadSq, (int)curFractal, 0);
            history.push_back(z);
        }

        // If the point is not in the set, iterate over its position history.
        if (i < iMax) {
            for (int j = 0; j < i; j++) {
                plotComplexOnHeatmap(heatmap, maxHeatmapVal, writingLock, screenBounds, exportSize, (int)curFractal, history[j]);
            }
        }
    }
}

BuddhaRenderer::BuddhaRenderer(const Vector2& _screenSize, FractalTypes& _curFractal, Vector2& _offset, float& _scale, float& _exportScale)
    : screenSize(_screenSize), curFractal(_curFractal), offset(_offset), scale(_scale), exportScale(_exportScale)
{
    for (int i = 0; i < threadCount; i++) {
        threads.push_back(std::move(std::thread(&BuddhaRenderer::Render, std::ref(*this), std::ref(i))));
    }
    UpdateScreenBounds();
}

BuddhaRenderer::~BuddhaRenderer()
{
    if (threads.size() > 0)
    {
        killThreads.store(true);
        for (int i = 0; i < threadCount; i++) {
            threads[i].join();
        }
        delete[] heatmap;
        delete[] renderData;
        delete   renderedTexture;
    }
}

void BuddhaRenderer::UpdateScreenBounds()
{
    if (!rendering.load())
    {
        exportSize = { 1920 * exportScale, 1080 * exportScale };

        float powScale = powf(2.0, scale);
        screenBounds[0] = Vector2{ (             - screenSize.x * 0.5f) / (powScale * screenSize.y * 0.5f) + offset.x / powScale,
                                   (             - screenSize.y * 0.5f) / (powScale * screenSize.y * 0.5f) + offset.y / powScale };
        screenBounds[1] = Vector2{ (screenSize.x - screenSize.x * 0.5f) / (powScale * screenSize.y * 0.5f) + offset.x / powScale,
                                   (screenSize.y - screenSize.y * 0.5f) / (powScale * screenSize.y * 0.5f) + offset.y / powScale };
    }
}

void BuddhaRenderer::StartRendering() 
{ 
    if (rendering.load())
        return;

    // Delete any previously allocated buffers.
    if (heatmap != nullptr && renderData != nullptr) {
        delete[] heatmap;
        delete[] renderData;
    }

    // Update the screen bounds and export size.
    UpdateScreenBounds();

    // Create new buffers and set all the hitcounts to 0.
    heatmap = new unsigned int [(int)(exportSize.x * exportSize.y)];
    renderData     = new unsigned char[(int)(exportSize.x * exportSize.y * sizeof(Color))];
    for (int i = 0; i < (int)(exportSize.x * exportSize.y); i++) {
        heatmap[i] = 0;
    }

    // Tell the threads to start rendering.
    rendering.store(true);
}

void BuddhaRenderer::StopRendering()  
{ 
    rendering.store(false);
}

Texture2D BuddhaRenderer::GetRenderedTexture()
{
    float timeSinceTextureUpdate = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - prevTextureUpdateTime).count() / 1000.f;
    if (timeSinceTextureUpdate < 2.f)
        return *renderedTexture;
    prevTextureUpdateTime = std::chrono::system_clock::now();

    for (int y = 0; y < (int)exportSize.y; y++) 
    {
        for (int x = 0; x < (int)exportSize.x; x++) 
        {
            int arrayPos = y * (int)exportSize.x + x;
            unsigned int hitcount = heatmap[arrayPos];

            renderData[arrayPos * sizeof(Color)    ] = (unsigned char)((hitcount / (float)maxHeatmapVal) * 255.f * renderColor.x);
            renderData[arrayPos * sizeof(Color) + 1] = (unsigned char)((hitcount / (float)maxHeatmapVal) * 255.f * renderColor.y);
            renderData[arrayPos * sizeof(Color) + 2] = (unsigned char)((hitcount / (float)maxHeatmapVal) * 255.f * renderColor.z);
            renderData[arrayPos * sizeof(Color) + 3] = 255;// (heatmap[arrayPos] > 0 ? 255 : 0);
        }
    }

    Image image;
    image.data    = renderData;
    image.width   = (int)exportSize.x;
    image.height  = (int)exportSize.y;
    image.format  = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
    image.mipmaps = 1;

    if (renderedTexture != nullptr) {
        UnloadTexture(*renderedTexture);
    }
    else {
        renderedTexture = new Texture2D();
    }
    *renderedTexture = LoadTextureFromImage(image);
    return *renderedTexture;
}
