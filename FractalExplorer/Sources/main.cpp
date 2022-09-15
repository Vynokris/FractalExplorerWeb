#include "FractalRenderer.h"
#include "Ui.h"

int main(void)
{
    // Setup fractal renderer and imgui.
    FractalRenderer renderer({ 1728, 972 });
    Ui              ui      (renderer);

    // Main loop.
    while (!WindowShouldClose())
    {
        BeginDrawing();
        {
            renderer.Draw();
            ui.Draw();
        }
        EndDrawing();
        if (!ui.IsInteractedWith())
        {
            renderer.ProcessInputs();
        }
    }

    CloseWindow();
    return 0;
}

