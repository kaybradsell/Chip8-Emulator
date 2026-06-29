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
#include <string>

//----------------------------------------------

class Chip
{
public:
	Chip(double cycles, bool cosmacVIP);

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

	/** Set Cycles Per Second
	* @brief - Changes the amount of instructions done per second
	* @param - double cycles
	**/
	void SetCyclesPerSecond(double cycles);

	/** Set COSMAC VIP mode
	* @brief - changes certain opcode behaviours depending on this mode.
	* @param - bool cosmacVIP
	* @note - it false, it will instead use SUPER-CHIP/CHIP-48 behaviours
	**/
	void SetCosmacVIPMode(bool cosmacVIP);

	/** Print Display
	* @brief - temporary display into CLI from display
	* @note - gonna nuke this and use raylib or something + imgui.
	**/
	void PrintDisplay();

	/** Load ROM
	* @brief - Loads a ROM into memory
	* @param -
	**/
	void LoadROM(const std::string& fileName);

	int GetHeight() const;
	int GetWidth() const;

	const uint8_t* GetDisplay() const;

private:
	static const int height = 32;
	static const int width = 64;
	std::array<uint8_t, 4096> memory{};			// the cool 4KB RAM
	std::array<uint8_t, width * height> display{};		// cool 64x32 display.
	std::array<uint8_t, 16> V{};				// V0 - VF general purpose variables register
	uint16_t I = 0;								// index register for pointing at mem
	uint16_t PC = 0x200;						// points at current instruction in mem
	double CPUAccumulator = 0.0;				// tracks cpu timer
	double cyclesPerSecond = 700.0;				// instructions per second
// TODO: some programs may cause stack overflow so add a limit to stack if encountered!
	std::stack<uint16_t> stack;					// the 16-bit address saving stack.

	uint8_t delayTimer = 0;						// 60fps delay counter
	uint8_t soundTimer = 0;						// 60fps audio counter
	std::chrono::high_resolution_clock::time_point lastTime; // tracks the time from last frame
	double timerAccumulator = 0.0;				// tracks timer timer

	uint16_t keys = 0;							// 16b tracking the 16 keys this frame
	uint16_t prevKeys = 0;						// 16b tracking the 16 keys from last frame

	bool newDraw = false;						// tracks if display should update
	bool cosmacVIPMode = false;					// toggles opcodes for COSMAC VIP interpreter instead.

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

	/** Cycle
	* @brief - Handles every 1/CyclesPerSecond instructions
	**/
	void Cycle();

	/** Fetch Instructions
	* @brief - From current PC, returns the uint16 from memory (high and low byte)
	* @return - uint16_t
	* @note - increases PC by 2.
	**/
	uint16_t FetchInstruction();

	/** Decode Instruction
	* @brief - Decodes and Executes instruction given the parameter.
	* @param - uint16_t
	* @note - Different versions of opcodes can be ran depending on mode
	**/
	void DecodeInstruction(const uint16_t& instruction);

	/** Execute Opcode 0x0
	* @brief - Executes the 0x0 family of opcodes depending on instruction
	* @param - uint16_t instruction
	**/
	void ExecuteOpcode0x0(const uint16_t& instruction);

	/** Execute Opcode 0x8
	* @brief - Executes the 0x8 family of opcodes depending on last nibble (n)
	* @param - uint8_t x
	* @param - uint8_t y
	* @param - uint8_t n
	**/
	void ExecuteOpcode0x8(const uint8_t& x, const uint8_t& y, const uint8_t& n);

	/** Execute Opcode 0xE
	* @brief - Executes the 0xE family of opcodes depending on last two nibbles
	* @param - uint8_t x
	* @param - uint8_t nn
	**/
	void ExecuteOpcode0xE(const uint8_t& x, const uint8_t& nn);

	/** Execute Opcode 0xF
	* @brief - Executes the 0xF family of opcodes depending on last two nibbles
	* @param - uint8_t x
	* @param - uint8_t nn
	**/
	void ExecuteOpcode0xF(const uint8_t& x, const uint8_t& nn);

	/** Clear Display
	* @brief - sets display to be all 0s, clearing the screen
	**/
	void ClearDisplay();
	
	/** Draw Sprite
	* @brief - Draws sprite given data in memory at I, with h height, at x/y.
	* @param - uint8_t x
	* @param - uint8_t y
	* @param - uint8_t h
	* @note - please have pointed I to the sprite you want to draw in memory
	**/
	void DrawSprite(uint8_t x, uint8_t y, uint8_t h);
};

//----------------------------------------------