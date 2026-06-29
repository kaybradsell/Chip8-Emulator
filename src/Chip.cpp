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
    // Fetch current instruction
    uint16_t instruction = FetchInstruction();

    // Decode the instruction
    // Execute the instruction
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

void Chip::Tick()
{
    // reduce timers
    if (delayTimer > 0)
        delayTimer--;

    if (soundTimer > 0)
        soundTimer--;
}

//----------------------------------------------

// the font set used for Chip-8, which is read into 050-09F (from 000 rn)
static std::array<uint8_t, 80> fontset{
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

void Chip::LoadFont()
{
    std::cout << "[CHIP-8] Loading in fontset...\n";

    for (int i = 0; i < fontset.size(); i++)
    {
        memory[i] = fontset[i];
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