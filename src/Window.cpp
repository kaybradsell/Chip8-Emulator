#include "Window.h"
#include <ImGuiFileDialog.h>

Window::Window(int w, int h, Chip* chip)
{
	SetTraceLogLevel(LOG_ERROR);
	InitWindow(w, h, "Chip-8 Emulator");
	SetTargetFPS(60);
	rlImGuiSetup(true);

	ImGui::StyleColorsDarkPink();

	this->chip = chip;
	screenTexture = LoadRenderTexture(chip->GetWidth(), chip->GetHeight());
	pixels.resize(chip->GetWidth() * chip->GetHeight());

	try {
		LoadConfig();
	}
	catch (std::runtime_error e) {
		std::cout << e.what();
	}
	
}

void Window::LoadConfig()
{
	std::ifstream file("data/config.dat", std::ios::binary);
	if (!file.is_open())
		throw std::runtime_error("[WINDOW] Unable to load config.dat. Loading defaults...");

	file.read(reinterpret_cast<char*>(&on), sizeof(Color));
	file.read(reinterpret_cast<char*>(&off), sizeof(Color));

	if (!file)
		throw std::runtime_error("[WINDOW] Config file is corrupted.");

	std::cout << "[WINDOW] Successfully loaded config.dat.\n";
}

void Window::SaveConfig()
{
	std::ofstream file("data/config.dat", std::ios::binary);
	if (!file.is_open())
		throw std::runtime_error("[WINDOW]: Unable to save to config.dat.");

	file.write(reinterpret_cast<const char*>(&on), sizeof(Color));
	file.write(reinterpret_cast<const char*>(&off), sizeof(Color));

	std::cout << "[WINDOW]: Successfully saved to config.dat.\n";
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
	DrawConfig();
	DrawColourPicker();

	rlImGuiEnd();

	EndDrawing();
}

void Window::MakeTexture()
{
	const auto& display = chip->GetDisplay();
	int size = chip->GetWidth() * chip->GetHeight();

	for (int i = 0; i < size; i++)
	{
		pixels[i] = display[i] ? on : off;
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

			if (base + i + 1 == chip->GetPC() || base + i + 2 == chip->GetPC())
			{
				ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(209, 16, 100, 255));
				ImGui::Text("%02X", mem[base + i]);
				ImGui::PopStyleColor();
			}
			else
			{
				ImGui::Text("%02X", mem[base + i]);
			}
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
	if (ImGui::Button("+5"))
	{
		for (int i = 0; i < 5; i++)
			chip->Step();
	}

	ImGui::SameLine();
	if (ImGui::Button("+10"))
	{
		for (int i = 0; i < 10; i++)
			chip->Step();
	}

	ImGui::SameLine();
	if (ImGui::Button("+50"))
	{
		for (int i = 0; i < 50; i++)
			chip->Step();
	}

	static int speed = 700;
	ImGui::DragInt("CPU Speed (Hz)", &speed, 1.0f, 1, 2000);
	chip->SetCyclesPerSecond(speed);

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

	ImGui::SameLine();
	if (ImGui::Button("Reset"))
	{
		try {
			chip->ResetROM();
		}
		catch (std::runtime_error e) {
			std::cout << e.what();
		}
	}

	if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey"))
	{
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
			std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();

			std::string fileName = ImGuiFileDialog::Instance()->GetCurrentFileName();
			SetWindowTitle(("Chip-8 Emulator: " + fileName).c_str());

			try {
				chip->LoadROM(filePathName);
			}
			catch (std::runtime_error e) {
				std::cout << e.what();
			}
		}

		ImGuiFileDialog::Instance()->Close();
	}

	ImGui::End();
}

void Window::DrawConfig()
{
	ImGui::Begin("Config");

	static bool cosmacMode = false;
	ImGui::Checkbox("Toggle COSMAC-VIP Mode", &cosmacMode);
	chip->SetCosmacVIPMode(cosmacMode);

	ImGui::End();
}

void Window::DrawColourPicker()
{
	ImGui::Begin("Colours");

	if (ImGui::Button("Save"))
	{
		try { SaveConfig(); }
		catch (std::runtime_error e) { std::cout << e.what(); }
	}

	ImGui::SameLine();
	if (ImGui::Button("Load"))
	{
		try { LoadConfig(); }
		catch (std::runtime_error e) { std::cout << e.what(); }
	}

	ImGui::SameLine();
	ImColor onCol = to_imcolor(on);
	ImColor offCol = to_imcolor(off);
	ImGui::Text("On: ");
	ImGui::SameLine();
	ImGui::ColorEdit3("OnColour", (float*)&onCol, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
	ImGui::SameLine();
	ImGui::Text("Off: ");
	ImGui::SameLine();
	ImGui::ColorEdit3("OffColour", (float*)&offCol, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);

	on = to_rlcolor(onCol);
	off = to_rlcolor(offCol);

	ImGui::End();
}

ImColor Window::to_imcolor(Color col)
{
	return ImColor((col.r / 255.0f), (col.g / 255.0f), (col.b / 255.0f));
}

Color Window::to_rlcolor(ImColor col)
{
	return Color{ (unsigned char)(col.Value.x * 255.0f), (unsigned char)(col.Value.y * 255.0f), (unsigned char)(col.Value.z * 255.0f), (unsigned char)(255) };
}