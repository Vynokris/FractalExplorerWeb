#include "FractalRenderer.h"
#include "Ui.h"
#if defined PLATFORM_WEB
    #include <emscripten/emscripten.h>
#endif

void UpdateAndDrawFrame()
{
    // Setup fractal renderer and imgui.
    static FractalRenderer renderer({ 1728, 972 }, 60);
    static Ui              ui      (renderer);

    // Main loop.
    BeginDrawing();
    {
        renderer.Draw();
        ui.Draw();
    }
    EndDrawing();
    ui.ProcessInputs();
}

int main(void)
{
    #if defined PLATFORM_WEB
        emscripten_set_main_loop(UpdateAndDrawFrame, 60, 1);
    #else
        // Setup fractal renderer and imgui.
        FractalRenderer renderer({ 1728, 972 }, 60);
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
            ui.ProcessInputs();
        }
    #endif
    return 0;
}

