#pragma once

#include <array>
#include <cstdint>
#include <iostream>
#include <stack>
#include <chrono>

class Chip
{
public:
	void Init();
	void Update();

private:
	std::array<uint8_t, 4096> memory{};			// the cool 4KB RAM
	std::array<uint8_t, 64 * 32> display{};		// cool 64x32 display.
	std::array<uint8_t, 16> V{};				// V0 - VF general purpose variables register
	uint16_t I = 0;								// index register for pointing at mem
	uint16_t pc = 0x200;						// points at current instruction in mem
// TODO: some programs may cause stack overflow so add a limit to stack if encountered!
	std::stack<uint16_t> stack;					// the 16-bit address saving stack.

	// timers
	uint8_t delayTimer = 0;
	uint8_t soundTimer = 0;
	std::chrono::high_resolution_clock::time_point lastTime;
	double accumulator = 0.0;

	void LoadFont();
	void Tick(); // handles every 1/60th second updates
};