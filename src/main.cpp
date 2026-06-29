// Chip8-Emulator.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <Chip.h>
#include <Input.h>

int main()
{
    srand(time(0));

    Chip chip = Chip(700, false);
    chip.Init();

    Input input = Input(&chip);

    chip.LoadROM("tests/5-quirks.ch8");

    while (true)
    {
        input.Poll();
        chip.Update();
        chip.PrintDisplay();
    }
}
