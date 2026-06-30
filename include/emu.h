#pragma once

#include <array>
#include <cstdint>
#include <iostream>
#include <stack>
#include <chrono>
#include <string>
#include <vector>

class Chip
{
public:
	struct Instruction
	{
		uint16_t pc;
		uint16_t opcode;
	};

	struct Quirks
	{
		bool shiftUsesVy;
		bool loadStoreIncrementsIndex;
		bool jumpUsesV0;
		bool VFOnOverflow;
		bool waitForRelease;
		bool spriteWrap;

		static Quirks CosmacVIP()
		{
			return {
				true,	// shift uses VY
				true,	// loads store increments Index
				true,	// jump uses V0
				false,	// VF no overflow
				true,	// wait for release
				false	// sprites do not wrap
			};
		}

		static Quirks SCHIP()
		{
			return {
				false,	// shift does not use VY
				false,	// loads do not increment Index
				false,	// jump does not use V0
				true,	// VF does overflow
				false,	// does not wait for release
				false	// sprites do not wrap
			};
		}

		static Quirks XOCHIP()
		{
			return {
				false,	// shift does not use VY
				false,	// loads do not increment Index
				false,	// jump does not use V0
				true,	// VF does overflow
				false,	// does not wait for release
				false	// sprites do not wrap
			};
		}
	};

	enum ChipType
	{
		COSMAC,
		SCHIP,
		XOCHIP
	};

	Chip(double cycles, ChipType type);
	void Init();
	void Update();
	void TogglePause();
	void Step();
	void Cycle();
	void SetKey(uint8_t key, bool pressed);
	void SetCyclesPerSecond(double cycles);
	void SetChipType(ChipType type);
	void PrintDisplay();
	void LoadROM(const std::string& fileName);
	void ResetROM();
	int GetHeight() const;
	int GetWidth() const;


private:
	std::array<uint8_t, 65536> memory{};
	std::array<uint8_t, 16> V{};
	std::stack<uint16_t> stack;
	uint16_t index = 0;
	uint16_t pc = 0x200;
	uint16_t keys = 0;
	uint16_t prevKeys = 0;
	uint8_t dt = 0;
	uint8_t st = 0;
	double cpuAccu = 0.0;
	double cps = 700.0;
	double tAccu = 0.0f;
	std::chrono::high_resolution_clock::time_point lastTime;
	bool newDraw = false;
	bool paused = false;
	bool loaded = false;
	std::string currentROM = "";

	static constexpr int HISTORY_SIZE = 1024;
	Instruction history[HISTORY_SIZE];
	int historyIndex = 0;
	Quirks quirks;
	ChipType type;
};