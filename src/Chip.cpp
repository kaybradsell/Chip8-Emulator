//----------------------------------------------

#include <Chip.h>
#include <stdexcept>

//----------------------------------------------

void Chip::Init()
{
    std::cout << "[CHIP-8] Starting Init...\n";

    lastTime = std::chrono::high_resolution_clock::now();
    LoadFont(); // load in the cool fonts :D

    std::cout << "[CHIP-8] Finished Init! Enjoy :D\n";
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
    case 0x0: // 00E0 - Clear Screen
        if (instruction == 0x00E0)
            ClearDisplay();
        break;

    case 0x1: // 1NNN - Jump to nnn
        PC = nnn;
        break;

    case 0x6: // 6XNN - Set register V[x] to nn
        V[x] = nn;
        break;

    case 0x7: // 7XNN - Add value nn to register V[x]
        V[x] += nn;
        break;

    case 0xA: // ANNN - Set I to nnn
        I = nnn;
        break;

    case 0xD:
        DrawDisplay(x, y, n);
        break;

    default:
        break;
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
    keys[key] = pressed ? 1 : 0;
}

//----------------------------------------------

void Chip::SetCyclesPerSecond(double cycles)
{
    if (cycles <= 0)
        throw std::invalid_argument("CHIP: Cycles per Second cannot be <= 0.");

    cyclesPerSecond = cycles;
}

//----------------------------------------------

void Chip::ClearDisplay()
{
    newDraw = true;
    display.fill(0);
}

//----------------------------------------------

void Chip::DrawDisplay(uint8_t x, uint8_t y, uint8_t h)
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
            std::cout << (display[y * width + x] ? '#' : '.');
        }

        std::cout << "\n";
    }

    newDraw = false;
}

//----------------------------------------------