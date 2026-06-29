#pragma once

#include <raylib.h>
#include <rlImGui.h>
#include <imGui.h>
#include <Chip.h>
#include <Vector2.h>
#include <vector>

class Window
{
public:
	Window(int w, int h, Chip* chip);
	void Display();
	void Keys();
    bool Running();

private:
	struct KeyMapping
	{
		KeyboardKey raylibKey;
		uint8_t chipKey;
	};

    static constexpr KeyMapping keyMap[] = {
        { KEY_X, 0x0 },
        { KEY_ONE, 0x1 },
        { KEY_TWO, 0x2 },
        { KEY_THREE, 0x3 },
        { KEY_Q, 0x4 },
        { KEY_W, 0x5 },
        { KEY_E, 0x6 },
        { KEY_A, 0x7 },
        { KEY_S, 0x8 },
        { KEY_D, 0x9 },
        { KEY_Z, 0xA },
        { KEY_C, 0xB },
        { KEY_FOUR, 0xC },
        { KEY_R, 0xD },
        { KEY_F, 0xE },
        { KEY_V, 0xF }
    };

    Chip* chip = nullptr;
    RenderTexture2D screenTexture;
    std::vector<Color> pixels;

    void MakeTexture();
};