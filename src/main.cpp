// Chip8-Emulator.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <Chip.h>

int main()
{
    srand(time(0));

    Chip chip = Chip(700, false);
    chip.Init();

    chip.LoadROM("tests/5-quirks.ch8");

    while (true)
    {
        chip.Update();
        chip.PrintDisplay();
    }
}
