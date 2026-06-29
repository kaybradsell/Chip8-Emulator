//----------------------------------------------

#include <Input.h>
#include <stdexcept>
#include <conio.h>

//----------------------------------------------

Input::Input(Chip* c)
{
	if (!c)
	{
		std::cout << "[INPUT]: ERROR: Chip has not been initialised yet.";
		throw std::invalid_argument("INPUT: Chip not initialised, cannot set.");
	}

	chip = c;
	MapKeys();
}

//----------------------------------------------

void Input::Bind(uint8_t key, int hostKey)
{
	keymap[key] = hostKey;
}

//----------------------------------------------

void Input::MapKeys()
{	
	Bind(0, '1');
	Bind(1, '2');
	Bind(2, '3');
	Bind(3, '4');

	Bind(4, 'q');
	Bind(5, 'w');
	Bind(6, 'e');
	Bind(7, 'r');

	Bind(8, 'a');
	Bind(9, 's');
	Bind(10, 'd');
	Bind(11, 'f');

	Bind(12, 'z');
	Bind(13, 'x');
	Bind(14, 'c');
	Bind(15, 'v');
}

//----------------------------------------------

void Input::KeyDown(int hostKey)
{
	for (int i = 0; i < 16; i++)
	{
		if (keymap[i] == hostKey)
			chip->SetKey(i, true);
	}
}

//----------------------------------------------

void Input::KeyUp(int hostKey)
{
	for (int i = 0; i < 16; i++)
	{
		if (keymap[i] == hostKey)
			chip->SetKey(i, false);
	}
}

//----------------------------------------------

void Input::Poll()
{
	// uses conio.h cuz this is on console
	if (_kbhit())
	{
		int key = _getch();

		KeyDown(key);
		KeyUp(key);
	}
}

//----------------------------------------------