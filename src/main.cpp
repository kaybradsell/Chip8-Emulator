// Chip8-Emulator.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <Chip.h>
#include <Input.h>

int main()
{
    Chip chip = Chip(700, false);
    chip.Init();

    Input input = Input(&chip);

    chip.LoadROM("tests/test_opcode.ch8");

    while (true)
    {
        input.Poll();
        chip.Update();
        chip.PrintDisplay();
    }
}
