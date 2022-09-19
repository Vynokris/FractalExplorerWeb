#include "BhuddaRenderer.h"
#include <random>
#include <cmath>
#include <vector>
#include <iostream>
#include <mutex>

// Returns the sum of the two given complex numbers.
Vector2 complexSum(const Vector2 c1, const Vector2 c2) {
    return Vector2{ c1.x + c2.x, c1.y + c2.y };
}

// Returns the square of the given complex number.
Vector2 complexSquare(const Vector2 c) {
    return Vector2{ c.x * c.x - c.y * c.y, 2.f * c.x * c.y };
}

float remap(const float& value, const float& inputStart, const float& inputEnd, const float& outputStart, const float& outputEnd) {
    return (value - inputStart) / (inputEnd - inputStart) * (outputEnd - outputStart) + outputStart;
}

void BuddhaRenderer::Render(const int& threadId)
{
    srand(time(NULL) + threadId * 1000);
    while (!killThreads.load())
    {
        if (!rendering.load())
            continue;
        
        // The point at which a pixel becomes black.
        float escapeRadius = 2.0;
        float escapeRadSq  = escapeRadius * escapeRadius;

        // Get a random complex inside of the screen bounds.
        Vector2 c = { remap((rand() % ((int)screenSize.x * 4)) / 4.f, 0.f, screenSize.x, screenBounds[0].x, screenBounds[1].x),
                      remap((rand() % ((int)screenSize.y * 4)) / 4.f, 0.f, screenSize.y, screenBounds[0].y, screenBounds[1].y) };

        // Define z, z^2 and z's history.
        Vector2 z  = Vector2{ 0.f, 0.f };
        Vector2 z2 = z;
        std::vector<Vector2> history;

        // Repeatedly apply the fractal function to the point to check if it's in the set or not.
        int i, iMax = 500;
        history.reserve(iMax);
        for (i = 0; i < iMax && (z2.x + z2.y < escapeRadSq); i++) {
            z  = complexSum(z2, c);
            z2 = complexSquare(z);
            history.push_back(z);
        }

        // If the point is not in the set, iterate over its position history.
        if (i < iMax) 
        {
            for (int j = 0; j < i; j++) 
            {
                // Make sure that it is still in the screen bounds.
                if (screenBounds[0].x > history[j].x || history[j].x >= screenBounds[1].x ||
                    screenBounds[0].y > history[j].y || history[j].y >= screenBounds[1].y)
                {
                    continue;
                }

                // Convert the complex coordinates to screen coordinates.
                Vector2 cScreenCoords = { remap(history[j].x, screenBounds[0].x, screenBounds[1].x, 0, exportSize.x),
                                          remap(history[j].y, screenBounds[0].y, screenBounds[1].y, 0, exportSize.y) };

                // Make sure the complex's screen coordinates are still inside of the export texture.
                if (0 > cScreenCoords.x || cScreenCoords.x >= exportSize.x ||
                    0 > cScreenCoords.y || cScreenCoords.y >= exportSize.y)
                {
                    continue;
                }

                // Get the coordinates of the point and its neighboors in the heatmap.
                int arrayPos      = (int) cScreenCoords.y * (int)exportSize.x + (int)cScreenCoords.x;
                int upArrayPos    = (int)(cScreenCoords.y - 1 >= 0            ? cScreenCoords.y - 1 : cScreenCoords.y) * (int)exportSize.x + (int)cScreenCoords.x;
                int downArrayPos  = (int)(cScreenCoords.y + 1 <  exportSize.y ? cScreenCoords.y + 1 : cScreenCoords.y) * (int)exportSize.x + (int)cScreenCoords.x;
                int leftArrayPos  = (int) cScreenCoords.y * (int)exportSize.x + (int)(cScreenCoords.x - 1 >= 0            ? cScreenCoords.x - 1 : cScreenCoords.x);
                int rightArrayPos = (int) cScreenCoords.y * (int)exportSize.x + (int)(cScreenCoords.x + 1 <  exportSize.x ? cScreenCoords.x + 1 : cScreenCoords.x);

                // Increment the heatmap at the screen position of the complex.
                writingLock.lock();
                heatmap[arrayPos     ] += 2;
                heatmap[upArrayPos   ]++;
                heatmap[downArrayPos ]++;
                heatmap[leftArrayPos ]++;
                heatmap[rightArrayPos]++;
                if (++heatmap[arrayPos     ] > maxHeatmapVal) maxHeatmapVal = heatmap[arrayPos     ];
                if (++heatmap[upArrayPos   ] > maxHeatmapVal) maxHeatmapVal = heatmap[upArrayPos   ];
                if (++heatmap[downArrayPos ] > maxHeatmapVal) maxHeatmapVal = heatmap[downArrayPos ];
                if (++heatmap[leftArrayPos ] > maxHeatmapVal) maxHeatmapVal = heatmap[leftArrayPos ];
                if (++heatmap[rightArrayPos] > maxHeatmapVal) maxHeatmapVal = heatmap[rightArrayPos];
                writingLock.unlock();
            }
        }
    }
}

BuddhaRenderer::BuddhaRenderer(const Vector2& _screenSize, Vector2& _offset, float& _scale, float& _exportScale)
    : screenSize(_screenSize), offset(_offset), scale(_scale), exportScale(_exportScale)
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

        float powScale = pow(2.0, scale);
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
    image.width   = exportSize.x;
    image.height  = exportSize.y;
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
