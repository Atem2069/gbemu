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
		std::string interruptsEnabled = (Config::getInstance()->getValue<bool>("IE")) ? "Yes" : "No ";
		int PC = Config::getInstance()->getValue<int>("PC");
		int AF = Config::getInstance()->getValue<int>("AF");
		int BC = Config::getInstance()->getValue<int>("BC");
		int DE = Config::getInstance()->getValue<int>("DE");
		int HL = Config::getInstance()->getValue<int>("HL");
		int SP = Config::getInstance()->getValue<int>("SP");
		std::string cpuDebug = std::format("SHARP LR35902\nPC={:#x}\nStack pointer={:#x}\nAF={:#x} BC={:#x}\nDE={:#x} HL={:#x}\nInterrupts are enabled? {}", PC, AF, BC, DE, HL, SP, interruptsEnabled);
		ImGui::Text(cpuDebug.c_str());
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