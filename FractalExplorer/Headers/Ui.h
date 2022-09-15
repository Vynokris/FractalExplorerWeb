#pragma once
#include <raylib.h>

class FractalRenderer;

class Ui
{
private:
    FractalRenderer& fractalRenderer;
    bool    interactingWithUi = false;
    bool    popupOpen         = true;

public:
    Vector2 mouseDelta        = { 0, 0 };
    float   mouseWheelMove    = 0;

    Ui(FractalRenderer& fractalRendererRef);
    ~Ui();
    void Draw();
    void ProcessInputs();
    bool IsInteractedWith() { return interactingWithUi; }
};