#include"guiRenderer.h"

void GuiRenderer::init(GLFWwindow* window)
{
	ImGui::CreateContext();
	ImGui::StyleColorsClassic();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();
}

void GuiRenderer::prepareFrame()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void GuiRenderer::render()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			ImGui::MenuItem("Open...", nullptr, &m_openFileDialog);
			ImGui::MenuItem("Exit", nullptr, nullptr);
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Debug"))
		{
			ImGui::MenuItem("CPU State",nullptr,&m_showCPUDialog);
			ImGui::MenuItem("IO Register Viewer", nullptr, &m_showIODialog);
			ImGui::MenuItem("Pause emulation",nullptr,nullptr);
			bool reset = false;
			ImGui::MenuItem("Reset", nullptr, &reset);
			if (reset)
				Config::getInstance()->setValue<bool>("reset", true);
			bool serial = Config::getInstance()->getValue<bool>("serialDebug");
			ImGui::MenuItem("Serial Debug Output", nullptr, &serial);
			Config::getInstance()->setValue<bool>("serialDebug", serial);

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("View"))
		{
			ImGui::MenuItem("PPU Settings", nullptr, &m_showPPUDialog);
			ImGui::MenuItem("Hide Menubar", nullptr, nullptr);
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Help"))
		{
			ImGui::MenuItem("About gbemu", nullptr, &m_showAboutDialog);
			ImGui::EndMenu();
		}
	}
	ImGui::EndMainMenuBar();

	if (m_showAboutDialog)
	{
		ImGui::Begin("About", &m_showAboutDialog, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize);
		ImGui::SetCursorPosX((ImGui::GetWindowSize().x - 5) * 0.5f);	//center text hack
		ImGui::Text("GBEmu");
		ImGui::Text("NEA Project. By Henry Southall");
		ImGui::End();
	}

	if (m_showCPUDialog)
	{
		ImGui::Begin("CPU", &m_showCPUDialog, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize);
		CPUState curState = Config::getInstance()->getValue<CPUState>("CPUState");
		std::string interruptsEnabled = (curState.IME) ? "Yes" : "No ";
		std::string cpuDebug = std::format("SHARP LR35902\nPC={:#x}\nStack pointer={:#x}\nAF={:#x}\nBC={:#x}\nDE={:#x}\nHL={:#x}\nInterrupts are enabled? {}\n", (int)curState.PC, (int)curState.AF, (int)curState.BC, (int)curState.DE, (int)curState.HL, (int)curState.SP, interruptsEnabled);
		cpuDebug += std::format("Carry: {}  Half Carry: {}\nZero: {}  Subtract: {}", curState.carry, curState.halfCarry, curState.zero, curState.subtract);
		ImGui::Text(cpuDebug.c_str());
		ImGui::End();
	}

	if (m_showIODialog)
	{
		ImGui::Begin("IO Registers", &m_showIODialog, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize);
		IOState curState = Config::getInstance()->getValue<IOState>("IOState");
		std::string ioDebug = std::format("{:#x} JOYPAD: {:#x}\n{:#x} DIV: {:#x}\n{:#x} TIMA: {:#x}\n{:#x} TMA: {:#x}\n{:#x} TAC: {:#x}\n{:#x} IF: {:#x}\n{:#x} STAT: {:#x}\n{:#x} SCX: {:#x}\n", REG_JOYPAD, curState.JOYPAD, REG_DIV, curState.DIV, REG_TIMA, curState.TIMA, REG_TMA, curState.TMA, REG_TAC, curState.TAC, REG_IFLAGS, curState.IFLAGS, REG_STAT, curState.STAT, REG_SCX, curState.SCX);
		ioDebug += std::format("{:#x} SCY: {:#x}\n{:#x} LY: {:#x}\n{:#x} LYC: {:#x}\n{:#x} DMA: {:#x}\n{:#x} WX: {:#x}\n{:#x} WY: {:#x}\n{:#x} IE: {:#x}", REG_SCY, curState.SCY, REG_LY, curState.LY, REG_LYC, curState.LYC, REG_DMA, curState.DMA, REG_WX, curState.WX, REG_WY, curState.WY, REG_IE, curState.IE);
		ImGui::Text(ioDebug.c_str());
		ImGui::End();
	}

	if (m_showPPUDialog)
	{
		ImGui::Begin("PPU", &m_showPPUDialog, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize);

		ImGui::Text("Display palette");
		int paletteIdx = Config::getInstance()->getValue<int>("paletteIdx");
		ImGui::RadioButton("Classic", &paletteIdx, 0);
		ImGui::RadioButton("Pocket", &paletteIdx, 1);
		Config::getInstance()->setValue<int>("paletteIdx", paletteIdx);

		ImGui::Separator();
		ImGui::TextColored(ImVec4(1, 0, 0, 1), "Debug settings");
		bool ppuOverride = Config::getInstance()->getValue<bool>("ppuDebugOverride");
		ImGui::Checkbox("Override default behaviour", &ppuOverride);
		Config::getInstance()->setValue<bool>("ppuDebugOverride", ppuOverride);

		if (ppuOverride)
		{
			bool showSprites = Config::getInstance()->getValue<bool>("sprites");
			bool showBackground = Config::getInstance()->getValue<bool>("background");
			bool showWindow = Config::getInstance()->getValue<bool>("window");

			ImGui::Checkbox("Draw background layer", &showBackground);
			ImGui::Checkbox("Draw window layer", &showWindow);
			ImGui::Checkbox("Show sprites", &showSprites);

			Config::getInstance()->setValue<bool>("sprites", showSprites);
			Config::getInstance()->setValue<bool>("background", showBackground);
			Config::getInstance()->setValue<bool>("window", showWindow);
		}
		ImGui::End();
	}

	if (m_openFileDialog)
	{
		OPENFILENAMEA ofn = {};
		CHAR szFile[255] = { 0 };

		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = NULL;
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = "Game Boy ROM Files\0*.gb\0";
		ofn.nFilterIndex = 1;
		ofn.lpstrFileTitle = NULL;
		ofn.nMaxFileTitle = 0;
		ofn.lpstrInitialDir = NULL;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

		if (GetOpenFileNameA(&ofn) == TRUE)
		{
			std::string filename = szFile;
			Config::getInstance()->setValue<std::string>("RomName", filename);
			Config::getInstance()->setValue<bool>("reset", true);
		}

		m_openFileDialog = false;
	}

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

bool GuiRenderer::m_showAboutDialog = false;
bool GuiRenderer::m_showCPUDialog = false;
bool GuiRenderer::m_openFileDialog = false;
bool GuiRenderer::m_showPPUDialog = false;
bool GuiRenderer::m_showIODialog = false;