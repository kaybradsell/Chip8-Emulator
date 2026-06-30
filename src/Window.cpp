#include "Window.h"
#include <ImGuiFileDialog.h>

Window::Window(int w, int h, Chip* chip)
{
	InitWindow(w, h, "Chip 8 Emulator");
	SetTargetFPS(60);
	rlImGuiSetup(true);

	this->chip = chip;
	screenTexture = LoadRenderTexture(chip->GetWidth(), chip->GetHeight());
	pixels.resize(chip->GetWidth() * chip->GetHeight());

	pngTexture = LoadTexture("assets/KaramelBanner2.png");
}

bool Window::Running()
{
	return !WindowShouldClose();
}

void Window::Display()
{
	MakeTexture();

	BeginDrawing();
	ClearBackground(BLACK);

	// imgui shit
	rlImGuiBegin();

	ImGui::Begin("Display");
	ImVec2 space = ImGui::GetContentRegionAvail();
	ImGui::Image((ImTextureID)(intptr_t)screenTexture.texture.id, space);
	ImGui::End();

	DrawRegisters();
	DrawMemory();
	DrawHistory();
	DrawControls();
	DrawLoader();
	DrawBanner();

	rlImGuiEnd();

	EndDrawing();
}

void Window::MakeTexture()
{
	const auto& display = chip->GetDisplay();
	int size = chip->GetWidth() * chip->GetHeight();

	for (int i = 0; i < size; i++)
	{
		pixels[i] = display[i] ? WHITE : BLACK; // replace
	}

	UpdateTexture(screenTexture.texture, pixels.data());
}

void Window::Keys()
{
	for (const auto& key : keyMap)
	{
		chip->SetKey(key.chipKey, IsKeyDown(key.raylibKey));
	}
}

void Window::DrawRegisters()
{
	ImGui::Begin("Registers");
	for (int i = 0; i < 16; i++)
	{
		ImGui::Text("V%X: 0x%02X (%d)", i, chip->GetRegisters()[i], chip->GetRegisters()[i]);
	}

	ImGui::Separator();

	ImGui::Text("PC: 0x%04X", chip->GetPC());
	ImGui::Text("I : 0x%04X", chip->GetIndex());
	ImGui::End();
}

void Window::DrawMemory()
{
	ImGui::Begin("Memory");

	ImGui::BeginChild("MemoryScrollRegion", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar);
	const uint8_t* mem = chip->GetMemory();

	for (int row = 5; row < 256; row++)
	{
		int base = row * 16;
		ImGui::Text("%04X:", base);
		ImGui::SameLine();

		for (int i = 0; i < 16; i++)
		{
			ImGui::SameLine();
			ImGui::Text("%02X", mem[base + i]);
		}

		ImGui::SameLine();

		ImGui::SameLine();
		char ascii[17];

		for (int i = 0; i < 16; i++)
		{
			uint8_t v = mem[base + i];
			ascii[i] = (v >= 32 && v <= 126) ? (char)v : '.';
		}

		ascii[16] = '\0';

		ImGui::Text("%s", ascii);
	}

	ImGui::EndChild();
	ImGui::End();
}

static constexpr int HISTORY_SIZE = 1024;
void Window::DrawHistory()
{
    ImGui::Begin("History");

    auto history = chip->GetHistory();
    int idx = chip->GetHistoryIndex();

    for (int i = 0; i < HISTORY_SIZE; i++)
    {
        idx = (idx - 1 + HISTORY_SIZE) % HISTORY_SIZE; // walk backwards

        const auto& ins = history[idx];

        if (ins.opcode == 0)
            continue;

        ImGui::Text("%04X %04X | %s", ins.pc, ins.opcode, chip->DecodeOpcode(ins.opcode).c_str());
    }

    ImGui::End();
}

void Window::DrawControls()
{
	ImGui::Begin("Controls");

	static bool paused = false;
	ImGui::Checkbox("Pause", &paused);
	chip->Pause(paused);

	if (ImGui::Button("Step"))
		chip->Step();

	ImGui::SameLine();
	if (ImGui::Button("x5"))
	{
		for (int i = 0; i < 5; i++)
			chip->Step();
	}

	ImGui::SameLine();
	if (ImGui::Button("x10"))
	{
		for (int i = 0; i < 10; i++)
			chip->Step();
	}

	ImGui::SameLine();
	if (ImGui::Button("x50"))
	{
		for (int i = 0; i < 50; i++)
			chip->Step();
	}

	static int speed = 700;
	ImGui::SliderInt("CPU Speed (Hz)", &speed, 1, 5000);
	chip->SetCyclesPerSecond(speed);

	ImGui::End();
}

void Window::DrawBanner()
{
	ImGui::Begin("Banner", nullptr, ImGuiWindowFlags_NoTitleBar);
	ImVec2 space = ImGui::GetContentRegionAvail();
	ImGui::Image((ImTextureID)(intptr_t)pngTexture.id, space);
	ImGui::End();
}

void Window::DrawLoader()
{
	ImGui::Begin("ROM Loader");
	
	ImGui::SameLine();
	if (ImGui::Button("Open"))
	{
		IGFD::FileDialogConfig config;
		config.path = "\ROMS";
		ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", ".ch8", config);
	}

	if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey"))
	{
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
			std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();

			chip->LoadROM(filePathName);
		}

		ImGuiFileDialog::Instance()->Close();
	}

	ImGui::End();
}