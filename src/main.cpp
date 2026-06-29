// Chip8-Emulator.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <Chip.h>
#include <Window.h>

int main()
{
    srand(time(0));
    
    Chip chip = Chip(700, false);
    chip.Init();
    Window window = Window(1280, 720, &chip);

    chip.LoadROM("tests/octojam2title.ch8");

    while (window.Running())
    {
        window.Keys();
        chip.Update();
        window.Display();
    }
}
