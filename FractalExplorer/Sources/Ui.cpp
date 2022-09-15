#include "Ui.h"
#include "FractalRenderer.h"
#include <rlImGui/rlImGui.h>
#include <algorithm>

Ui::Ui(FractalRenderer& fractalRendererRef)
    : fractalRenderer(fractalRendererRef)
{
    SetupRLImGui(true);
    ImGui::GetStyle().WindowRounding = 5.0;
}

Ui::~Ui()
{
    ShutdownRLImGui();
}

void Ui::Draw()
{
    interactingWithUi = false;
    BeginRLImGui();
    {
        if (ImGui::Begin("World Coords", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize))
        {
            // Current fractal.
            ImGui::AlignTextToFramePadding();
            ImGui::Text("Fractal:          ");
            ImGui::SameLine();
            ImGui::Text("%s", FractalNames::names[(int)fractalRenderer.curFractal]);
            ImGui::SameLine();
            if (ImGui::Button(">##changeFractal")) {
                ++fractalRenderer.curFractal;
                fractalRenderer.ValueModifiedThisFrame(ModifiableValues::CurFractal);
                interactingWithUi = true;
            }

            // Position.
            ImGui::AlignTextToFramePadding();
            ImGui::Text("Position:         ");
            ImGui::SameLine();
            if (ImGui::DragFloat2("##offsetSlider", &fractalRenderer.offset.x, 0.01f)) {
                fractalRenderer.ValueModifiedThisFrame(ModifiableValues::Offset);
                interactingWithUi = true;
            }
            if (ImGui::IsItemActive()) {
                interactingWithUi = true;
            }

            // Scale.
            ImGui::AlignTextToFramePadding();
            ImGui::Text("Zoom:             ");
            ImGui::SameLine();
            float prevScale = fractalRenderer.scale;
            if (ImGui::DragFloat("##scaleSlider", &fractalRenderer.scale, 0.01f)) {
                float scaleOffset = (float)(pow(2.0, fractalRenderer.scale) / pow(2.0, prevScale));
                fractalRenderer.offset = { fractalRenderer.offset.x * scaleOffset, fractalRenderer.offset.y * scaleOffset };
                fractalRenderer.ValueModifiedThisFrame(ModifiableValues::Scale);
                interactingWithUi = true;
            }
            if (ImGui::IsItemActive()) {
                interactingWithUi = true;
            }

            // Background color.
            ImGui::AlignTextToFramePadding();
            ImGui::Text("Background color: ");
            Vector3 rgbBgCol; Vector2 conversionOutput;
            ImGui::ColorConvertHSVtoRGB(fractalRenderer.customHue.y / (2*PI), 1.f, 1.f, rgbBgCol.x, rgbBgCol.y, rgbBgCol.z);
            ImGui::SameLine();
            if (ImGui::ColorEdit3("##bgColEdit", &rgbBgCol.x)) {
                ImGui::ColorConvertRGBtoHSV(rgbBgCol.x, rgbBgCol.y, rgbBgCol.z, fractalRenderer.customHue.y, conversionOutput.x, conversionOutput.y);
                fractalRenderer.customHue.y *= 2*PI;
                fractalRenderer.ValueModifiedThisFrame(ModifiableValues::Hue);
                interactingWithUi = true;
            }
            if (ImGui::IsItemActive()) {
                interactingWithUi = true;
            }

            // Foreground color.
            ImGui::AlignTextToFramePadding();
            ImGui::Text("Fractal color:    ");
            Vector3 rgbFgCol; 
            ImGui::ColorConvertHSVtoRGB(fractalRenderer.customHue.x / (2*PI), 1.f, 1.f, rgbFgCol.x, rgbFgCol.y, rgbFgCol.z);
            ImGui::SameLine();
            if (ImGui::ColorEdit3("##fgColEdit", &rgbFgCol.x)) {
                ImGui::ColorConvertRGBtoHSV(rgbFgCol.x, rgbFgCol.y, rgbFgCol.z, fractalRenderer.customHue.x, conversionOutput.x, conversionOutput.y);
                fractalRenderer.customHue.x *= 2*PI;
                fractalRenderer.ValueModifiedThisFrame(ModifiableValues::Hue);
                interactingWithUi = true;
            }
            if (ImGui::IsItemActive()) {
                interactingWithUi = true;
            }

            if (fractalRenderer.curFractal == FractalTypes::JuliaSet) 
            {
                // Complex C.
                ImGui::AlignTextToFramePadding();
                ImGui::Text("Complex number:   ");
                ImGui::SameLine();
                if (ImGui::DragFloat2("##complexSlider", &fractalRenderer.complexC.x, 0.0001f)) {
                    fractalRenderer.ValueModifiedThisFrame(ModifiableValues::Complex);
                    interactingWithUi = true;
                }
                if (ImGui::IsItemActive()) {
                    interactingWithUi = true;
                }

                // SineParams.
                ImGui::AlignTextToFramePadding();
                ImGui::Text("Sine automation:  ");
                ImGui::SameLine();
                if (ImGui::DragFloat2("##sineSlider", &fractalRenderer.sineParams.x, 0.0001f)) {
                    interactingWithUi = true;
                }
                if (ImGui::IsItemActive()) {
                    interactingWithUi = true;
                }
            }
        }
        ImGui::End();

        if (ImGui::Begin("Image Saving", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize))
        {
            // Export scale.
            float exportScale = fractalRenderer.GetExportScale();
            ImGui::AlignTextToFramePadding();
            ImGui::Text("Image scale: ");
            ImGui::SameLine();
            ImGui::PushItemWidth(43);
            if (ImGui::DragFloat("##imageScaleInput", &exportScale, 0.01f, 0.5f, 5.689f, "%.3f", ImGuiSliderFlags_AlwaysClamp)) {
                fractalRenderer.SetExportScale(exportScale);
                interactingWithUi = true;
            }
            if (ImGui::IsItemActive()) {
                interactingWithUi = true;
            }

            // Export button.
            ImGui::SameLine();
            ImGui::Text("(%dx%d)", (int)(1920 * exportScale), (int)(1080 * exportScale));
            ImGui::PopItemWidth();
            if (ImGui::Button("Export image")) {
                ImGui::AlignTextToFramePadding();
                ImGui::SameLine();
                ImGui::Text("Exporting...");
                fractalRenderer.StartImageExport();
                interactingWithUi = true;
            }
        }
        ImGui::End();

        if (ImGui::Begin("Keyboard Controls", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize))
        {
            ImGui::Text("[Enter]   to change fractal.");
            ImGui::Text("[W-A-S-D] to move.");
            ImGui::Text("[Q-E]     to zoom.");
            ImGui::Text("[4-6]     to change the background color.");
            ImGui::Text("[7-9]     to change the fractal's  color.");
            ImGui::Text("[Arrows]  to change the complex.");
            ImGui::Text("[Shift]   to make the complex change slower.");
            ImGui::Text("[1-3]     to change the sine automation's duration.");
            ImGui::Text("[2-5]     to change the sine automation's amplitude.");
            ImGui::NewLine();
            ImGui::Text("You can click and drag on the UI input boxes to edit\nthem gradually.");
        }
        ImGui::End();

        if (popupOpen) {
            if (ImGui::Begin("Notes", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse))
            {
                // Above 13.5 zoom, images look pixelated.
                ImGui::Text("Due to technical limitations, images above\nzoom level 12 can look pixelated.");
                ImGui::NewLine();

                // Somewhat low export resolution.
                ImGui::Text("This being a web app means exported images\nare limited to 10922x6144 resolution, and\nmouse controls are disabled for some reason.");
                ImGui::NewLine();

                // What I'm working on.
                ImGui::Text("What I'm working on:\n");
                ImGui::BulletText("Fixing mouse controls.");
                ImGui::BulletText("Finding a way to zoom further.");
                ImGui::BulletText("Exporting higher resolution images.");
                ImGui::BulletText("Adding more fractals.");
                ImGui::BulletText("Computing buddha sets.");

                // Please download exe.
                // ImGui::Text("To get higher resolution and less artifacts,\nyou can download the software and run it on\nyour windows pc.");
                // ImGui::NewLine();

                // Download software button.
                // if (ImGui::Button("Download software")) {
                // 
                // }

                // Close button.
                // ImGui::SameLine();
                ImGui::Indent(265);
                if (ImGui::Button("Close")) {
                    popupOpen = false;
                }
                ImGui::Unindent(265);
            }
            ImGui::End();
        }
    }
    EndRLImGui();
}
