#include "Window.h"

Window::Window(int w, int h, Chip* chip)
{
	InitWindow(w, h, "Chip 8 Emulator");
	SetTargetFPS(60);
	rlImGuiSetup(true);

	this->chip = chip;
	screenTexture = LoadRenderTexture(chip->GetWidth(), chip->GetHeight());
	pixels.resize(chip->GetWidth() * chip->GetHeight());
}

bool Window::Running()
{
	return !WindowShouldClose();
}

void Window::Display()
{
	MakeTexture();

	BeginDrawing();
	ClearBackground(BLACK);

	// imgui shit
	rlImGuiBegin();

	ImGui::Begin("Display");
	ImVec2 space = ImGui::GetContentRegionAvail();

	ImGui::Image((ImTextureID)(intptr_t)screenTexture.texture.id, space);
	ImGui::End();

	rlImGuiEnd();

	EndDrawing();
}

void Window::MakeTexture()
{
	const auto& display = chip->GetDisplay();
	int size = chip->GetWidth() * chip->GetHeight();

	for (int i = 0; i < size; i++)
	{
		pixels[i] = display[i] ? WHITE : BLACK; // replace
	}

	UpdateTexture(screenTexture.texture, pixels.data());
}

void Window::Keys()
{
	for (const auto& key : keyMap)
	{
		chip->SetKey(key.chipKey, IsKeyDown(key.raylibKey));
	}
}