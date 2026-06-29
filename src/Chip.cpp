//----------------------------------------------

#include <Chip.h>
#include <stdexcept>
#include <fstream>
#include <vector>
#include <random>
#include <ctime>
#include <bitset>

//----------------------------------------------

Chip::Chip(double cycles, bool cosmacVIP)
{
    SetCyclesPerSecond(cycles);
    SetCosmacVIPMode(cosmacVIP);
}

//----------------------------------------------

void Chip::Init()
{
    std::cout << "[CHIP-8] Starting Init...\n";

    lastTime = std::chrono::high_resolution_clock::now();
    LoadFont(); // load in the cool fonts :D

    std::cout << "[CHIP-8] Finished Init! Enjoy :D\n";
}

//----------------------------------------------

int Chip::GetHeight() const
{
    return height;
}

//----------------------------------------------

int Chip::GetWidth() const
{
    return width;
}

//----------------------------------------------

void Chip::Update()
{
    std::chrono::high_resolution_clock::time_point nowTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> deltaTime = nowTime - lastTime;
    lastTime = nowTime;

    CPUAccumulator += deltaTime.count();
    timerAccumulator += deltaTime.count();

    double cycleTime = 1.0 / cyclesPerSecond;

    while (CPUAccumulator >= cycleTime)
    {
        Cycle();
        CPUAccumulator -= cycleTime;
    }

    while (timerAccumulator >= (1.0 / 60.0))
    {
        Tick();
        timerAccumulator -= (1.0 / 60.0);
    }
}

//----------------------------------------------

void Chip::Cycle()
{
    DecodeInstruction(FetchInstruction());
}

//----------------------------------------------

uint16_t Chip::FetchInstruction()
{
    // check if PC goes out of bounds.
    if (PC + 1 >= memory.size())
        throw std::runtime_error("CHIP: PC out of bounds.");

    // read where PC is pointing, + 1, join, then PC++ ++
    uint8_t first = memory[PC];
    uint8_t second = memory[PC + 1];

    PC += 2;

    return (static_cast<uint16_t>(first) << 8) | second;
}

//----------------------------------------------

void Chip::DecodeInstruction(const uint16_t& instruction)
{
    // get the nibbles :)
    uint8_t f = (instruction >> 12) & 0xF;
    uint8_t x = (instruction >> 8) & 0xF;
    uint8_t y = (instruction >> 4) & 0xF;
    uint8_t n = instruction & 0xF;
    uint8_t nn = instruction & 0xFF;
    uint16_t nnn = instruction & 0x0FFF;

    // huge switch-case which will EXECUTE as well.
    switch (f)
    {
    case 0x0: // 0 stuff
        ExecuteOpcode0x0(instruction);
        break;

    case 0x1: // 1NNN - Jump to nnn
        PC = nnn;
        break;

    case 0x2: // 2NNN - Do NNN but don't jump
        stack.push(PC);
        PC = nnn;
        break;

    case 0x3: // 3XNN - skip 2 PC if V[x] == NN
        if (V[x] == nn)
            PC += 2;
        break;

    case 0x4: // 4XNN - skip 2 PC if V[x] != NN
        if (V[x] != nn)
            PC += 2;
        break;

    case 0x5: // 5XY0 - skip 2 if V[x] == V[y]
        if (V[x] == V[y])
            PC += 2;
        break;

    case 0x6: // 6XNN - Set register V[x] to nn
        V[x] = nn;
        break;

    case 0x7: // 7XNN - Add value nn to register V[x]
        V[x] += nn;
        break;

    case 0x8: // 8XYN - Arithmatic instructions
        ExecuteOpcode0x8(x, y, n);
        break;

    case 0x9: // 9XY0 - skip 2 if V[x] != V[y]
        if (V[x] != V[y])
            PC += 2;
        break;

    case 0xA: // ANNN - Set I to nnn
        I = nnn;
        break;

    case 0xB: // BNNN/BXNN - former for Cosmac, latter for S-CHIP
    {
        uint16_t location = (cosmacVIPMode) ? nnn + V[0] : nnn + V[x];
        location = location & 0x0FFF;
        PC = location;
        break;
    }

    case 0xC: // CXNN - random. V[x] = nn + random.
        V[x] = (std::rand() & 0xFF) & nn;
        break;

    case 0xD: // DXYN - draw sprite
        DrawSprite(x, y, n);
        break;

    case 0xE: // EXNN - key presses
        ExecuteOpcode0xE(x, nn);
        break;

    case 0xF: // FXNN - misc shit
        ExecuteOpcode0xF(x, nn);
        break;

    default:
        break;
    }

    prevKeys = keys;
}

//----------------------------------------------

void Chip::ExecuteOpcode0x0(const uint16_t& instruction)
{
    switch (instruction)
    {
    case 0x00E0: // clear screen
        ClearDisplay();
        break;

    case 0x00EE: // return to PC from stack
        if (stack.empty())
            throw std::runtime_error("CHIP: Can't return when stack is empty.");

        PC = stack.top();
        stack.pop();
        break;
    }
}

//----------------------------------------------

void Chip::ExecuteOpcode0x8(const uint8_t& x, const uint8_t& y, const uint8_t& n)
{
    switch (n) // N (last nibble) determines instruction
    {
    case 0x0: // 8XY0 - VX = VY
        V[x] = V[y];
        break;

    case 0x1: // 8XY1 - VX = VX OR VY
        V[x] = V[x] | V[y];
        break;

    case 0x2: // 8XY2 - VX = VX AND VY
        V[x] = V[x] & V[y];
        break;

    case 0x3: // 8XY3 - VX = VX XOR VY
        V[x] = V[x] ^ V[y];
        break;

    case 0x4: // 8XY4 - VX = VX + VY, sets VF to 1 if overflow.
    {
        uint16_t result = V[x] + V[y];
        V[x] = result; // only grabs lst 8 bits from result.
        V[0xF] = (result > 0xFF) ? 1 : 0;
        break;
    }

    case 0x5: // 8XY5 - VX = VX - VY. if X >= Y, VF = 1.
    {
        uint8_t vx = V[x];
        uint8_t vy = V[y];

        V[x] = vx - vy;
        V[0xF] = (vx >= vy) ? 1 : 0;
        break;
    }
        

    case 0x6: // 8XY6 - depends on COSMAC VIP. Shift V[x] one bit to right
    {
        uint8_t val = cosmacVIPMode ? V[y] : V[x];
        V[x] = val >> 1;
        V[0xF] = val & 0x1;
        break;
    }

    case 0x7: // 8XY7 - VX = VY - VX. if Y >= X, VF = 1.
    {
        uint8_t vx = V[x];
        uint8_t vy = V[y];

        V[x] = vy - vx;
        V[0xF] = (vy >= vx) ? 1 : 0;
        break;
    }

    case 0xE: // 8XYE - depends on COSMAC VIP. Shift V[x] one bit to the left
    {
        uint8_t val = cosmacVIPMode ? V[y] : V[x];
        V[x] = val << 1;
        V[0xF] = (val & 0x80) >> 7;
        break;
    }
    }
}

//----------------------------------------------

void Chip::ExecuteOpcode0xE(const uint8_t& x, const uint8_t& nn)
{
    switch (nn)
    {
    case 0x9E: // EX9E - PC + 2 if VX is pressed
        if (keys & (1 << V[x]))
            PC += 2;
        break;
    
    case 0xA1: // EXA1 - PC + 2 if VX NOT pressed
        if (!(keys & (1 << V[x])))
            PC += 2;
        break;
    }
}

//----------------------------------------------

void Chip::ExecuteOpcode0xF(const uint8_t& x, const uint8_t& nn)
{
    switch (nn)
    {
    case 0x07: // FX07 - sets V[x] to delay timer
        V[x] = delayTimer;
        break;

    case 0x15: // FX15 - sets delay timer to V[x]
        delayTimer = V[x];
        break;

    case 0x18: // FX18 - sets sound timer to V[x]
        soundTimer = V[x];
        break;

    case 0x1E: // FX1E - adds V[x] to I. If COSMAC, do not affect V[F], else do.
    {
        uint16_t result = V[x] + I;
        if (!cosmacVIPMode)
            V[0xF] = (result > 0x0FFF) ? 1 : 0;
        I = result & 0x0FFF;
        break;
    }

    case 0x0A: // FX0A - wait until key press.
    {
        PC -= 2;

        if (cosmacVIPMode)
        {
            // TODO: implement COSMAC VIP mode where it waits for press and release
        }

        uint16_t pressed = keys & ~prevKeys;

        if (pressed)
        {
            for (int i = 0; i < 16; i++)
            {
                if (pressed & (1 << i))
                {
                    V[x] = i;
                    PC += 2;
                    break;
                }
            }
        }

        break;
    }

    case 0x29: // FX29 - Point I to font specified in V[x]
        I = 0x50 + (V[x] * 5);
        break;

    case 0x33: // FX33 - Binary-coded decimal conversion. V[x] -> 3 decimal digits. Store those at I.
    {
        uint8_t value = V[x];

        memory[I] = value / 100;
        memory[I + 1] = (value / 10) % 10;
        memory[I + 2] = value % 10;

        break;
    }
    
    case 0x55: // FX55 - store V0 - VX (inc) into I, I++, I++, etc. If cosmac, I = new, else I = old I
    {
        size_t amt = x;
        for (int i = 0; i <= amt; i++)
            memory[I + i] = V[i];

        if (cosmacVIPMode) I += amt + 1;
        break;
    }

    case 0x65: // FX65 - load V0 - VX (inc) from I, I++, I++, etc. If cosmac, I = new, else I = old I
    {
        size_t amt = x;
        for (int i = 0; i <= amt; i++)
            V[i] = memory[I + i];

        if (cosmacVIPMode) I += amt + 1;
        break;
    }
    }
}

//----------------------------------------------

void Chip::Tick()
{
    // reduce timers
    if (delayTimer > 0)
        delayTimer--;

    if (soundTimer > 0)
        soundTimer--;
}

//----------------------------------------------

// the font set used for Chip-8, which is read into 0x50-0x9F
static const std::array<uint8_t, 80> FONTSET{
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

static const uint16_t FONT_START = 0x50;

void Chip::LoadFont()
{
    std::cout << "[CHIP-8] Loading in fontset...\n";

    for (int i = 0; i < FONTSET.size(); i++)
    {
        memory[FONT_START + i] = FONTSET[i];
    }

    std::cout << "[CHIP-8] Finished loading in fontset.\n";
}

//----------------------------------------------

void Chip::SetKey(uint8_t key, bool pressed)
{
    if (pressed)
        keys |= (1 << key);
    else
        keys &= ~(1 << key);
}

//----------------------------------------------

void Chip::SetCyclesPerSecond(double cycles)
{
    if (cycles <= 0)
        throw std::invalid_argument("CHIP: Cycles per Second cannot be <= 0.");

    cyclesPerSecond = cycles;
}

//----------------------------------------------

void Chip::SetCosmacVIPMode(bool cosmacVIP)
{
    cosmacVIPMode = cosmacVIP;
}

//----------------------------------------------

void Chip::ClearDisplay()
{
    newDraw = true;
    display.fill(0);
}

//----------------------------------------------

void Chip::DrawSprite(uint8_t x, uint8_t y, uint8_t h)
{
    newDraw = true;
    V[0xF] = 0; // set register VF to 0 (collision flag)

    for (int row = 0; row < h; row++)
    {
        uint8_t sprite = memory[I + row];

        for (int col = 0; col < 8; col++)
        {
            if (sprite & (0x80 >> col))
            {
                int index = (V[x] + col) % width + ((V[y] + row) % height) * width;
                display[index] ^= 1;

                if (display[index] == 0)
                    V[0xF] = 1;
            }
        }
    }
}

//----------------------------------------------

void Chip::PrintDisplay()
{
    if (!newDraw) return;

    system("cls"); // clear screen

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            std::cout << (display[y * width + x] ? '#' : ' ');
        }

        std::cout << "\n";
    }

    newDraw = false;
}

//----------------------------------------------

void Chip::LoadROM(const std::string& fileName)
{
    std::cout << "[CHIP-8] Reading in ROM " << fileName << ".\n";

    PC = 0x200; // reset PC to '0'

    std::ifstream file(fileName, std::ios::binary);

    if (!file)
        throw std::runtime_error("CHIP: Cannot read " + fileName + ".");

    // read size!
    file.seekg(0, std::ios::end);
    std::size_t size = file.tellg();
    file.seekg(0, std::ios::beg);

    // read the actual data in wahoo!
    std::vector<uint8_t> rom(size);
    file.read(reinterpret_cast<char*>(rom.data()), size);

    // checks
    if (rom.size() > memory.size() - 0x200)
        throw std::runtime_error("CHIP: ROM " + fileName + " is too large to read!");

    for (size_t i = 0; i < rom.size(); i++)
    {
        memory[0x200 + i] = rom[i];
    }

    std::cout << "[CHIP-8] Loaded in ROM " << fileName << " (" << rom.size() << " bytes)\n";
}

//----------------------------------------------

const uint8_t* Chip::GetDisplay() const
{
    return display.data();
}

//----------------------------------------------