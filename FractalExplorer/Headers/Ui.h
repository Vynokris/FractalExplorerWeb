#pragma once

class FractalRenderer;

class Ui
{
private:
    FractalRenderer& fractalRenderer;
    bool interactingWithUi = false;
    bool popupOpen         = true;

public:
    Ui(FractalRenderer& fractalRendererRef);
    ~Ui();
    void Draw();
    bool IsInteractedWith() { return interactingWithUi; }
};