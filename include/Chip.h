/*********************************************
 * @file -  Chip.h
 * @brief - The core of the emulator. It handles most things
 *			related to the program.
 *
 * @version 0.1
 * @date 29/06/2026 Kay Bradsell
 *				0.1 First Iteration
*********************************************/

#pragma once

//----------------------------------------------

#include <array>
#include <cstdint>
#include <iostream>
#include <stack>
#include <chrono>

//----------------------------------------------

class Chip
{
public:
	/** Init
	* @brief - Initialises all components of this emulator 
	**/
	void Init();

	/** Update
	* @brief - Updates the emulator.
	* @note - Not frame dependent
	**/
	void Update();

	/** Set Key
	* @brief - Updates key to new state
	* @param - uint8_t key
	* @param - bool pressed
	**/
	void SetKey(uint8_t key, bool pressed);

private:
	std::array<uint8_t, 4096> memory{};			// the cool 4KB RAM
	std::array<uint8_t, 64 * 32> display{};		// cool 64x32 display.
	std::array<uint8_t, 16> V{};				// V0 - VF general purpose variables register
	uint16_t I = 0;								// index register for pointing at mem
	uint16_t pc = 0x200;						// points at current instruction in mem
// TODO: some programs may cause stack overflow so add a limit to stack if encountered!
	std::stack<uint16_t> stack;					// the 16-bit address saving stack.
	uint8_t delayTimer = 0;						// 60fps delay counter
	uint8_t soundTimer = 0;						// 60fps audio counter
	std::chrono::high_resolution_clock::time_point lastTime; // tracks the time from last frame
	double accumulator = 0.0;					// tracks elapsed time
	std::array<uint8_t, 16> keys{};				// key state tracker

	/** Load Font
	* @brief - One-time load for font into memory
	* @note - Loads in 80 bytes from 0x000 to 0x050
	**/
	void LoadFont();

	/** Tick
	* @brief - Handles every 1/60th second frame.
	* @note - Decrements timers as well.
	**/
	void Tick();
};

//----------------------------------------------