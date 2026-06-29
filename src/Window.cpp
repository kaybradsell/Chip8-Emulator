#include "Window.h"

Window::Window(int w, int h, Chip* chip)
{
	InitWindow(w, h, "Chip 8 Emulator");
	SetTargetFPS(60);
	rlImGuiSetup(true);

	this->chip = chip;
}

bool Window::Running()
{
	return !WindowShouldClose();
}

void Window::Display()
{

}

void Window::Keys()
{
	for (const auto& key : keyMap)
	{
		chip->SetKey(key.chipKey, IsKeyDown(key.raylibKey));
	}
}