// Chip8-Emulator.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "Chip.h"

int main()
{
    Chip chip = Chip();
    chip.Init();

    while (true)
    {
        chip.Update();
    }
}
