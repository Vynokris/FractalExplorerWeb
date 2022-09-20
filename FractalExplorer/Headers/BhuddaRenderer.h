#pragma once
#include <raylib.h>
#include <thread>
#include <atomic>
#include <mutex>
#include <vector>
#include <chrono>

enum class FractalTypes;

class BuddhaRenderer
{
private:
    const Vector2 screenSize;
    Vector2       screenBounds[2];

    FractalTypes& curFractal;
    Vector2&      offset;
    float&        scale;
    float&        exportScale;
    Vector2       exportSize;

    std::vector<std::thread> threads;
    const float              threadCount = 10;
    std::atomic_bool         rendering   = false;
    std::atomic_bool         killThreads = false;
    std::mutex               writingLock;

    unsigned int*  heatmap         = nullptr;
    unsigned int   maxHeatmapVal = 0;
    unsigned char* renderData      = nullptr;
    Texture2D*     renderedTexture = nullptr;
    std::chrono::time_point<std::chrono::system_clock> prevTextureUpdateTime;

    void Render(const int& threadId);

public:
    Vector3 renderColor = { 1.0f, 0.2f, 1.0f };

    BuddhaRenderer(const Vector2& _screenSize, FractalTypes& _curFractal, Vector2& _offset, float& _scale, float& _exportScale);
    ~BuddhaRenderer();

    void UpdateScreenBounds();
    void StartRendering();
    void StopRendering();
    bool IsRendering() { return rendering.load(); }
    Texture2D GetRenderedTexture();
};