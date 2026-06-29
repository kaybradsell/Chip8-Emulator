/*********************************************
 * @file -  Input.h
 * @brief - Handles press of keys. Immediately releases them
 *			Uses conio.h since this is CLI currently.
 *			Also allows for custom keybinds, but it's hardcoded
 *
 * @version 0.1
 * @date 29/06/2026 Kay Bradsell
 *				0.1 First Iteration
*********************************************/

#pragma once

//----------------------------------------------

#include <Chip.h>
#include <array>
#include <cstdint>

//----------------------------------------------

class Input
{
public:
	/** Input Constructor
	* @brief - Constructs Input. Also hard codes keybinds.
	* @param - Chip* c
	**/
	Input(Chip* c);

	/** Poll
	* @brief - Checks with conio if a key press has occured.
	**/
	void Poll();

	/** Key Down
	* @brief - Checks hostkey with mappings and tells Chip that key is pressed.
	* @param - int hostKey
	**/
	void KeyDown(int hostKey);

	/** Key Up
	* @brief - Checks hostkey with mappings and tells Chip that key is released
	* @param - int hostKey
	**/
	void KeyUp(int hostKey);

private:
	Chip* chip;									// pointer to Chip
	std::array<int, 16> keymap{};				// maps keys to hostKey

	/** Map Keys
	* @brief - Hardcodes keybinds.
	**/
	void MapKeys();

	/** Bind
	* @brief - Binds an int hostkey to a uint8_t key (for Chip)
	* @param - uint8_t key
	* @param - int hostkey
	**/
	void Bind(uint8_t key, int hostKey);
};

//----------------------------------------------