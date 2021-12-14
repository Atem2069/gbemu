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
			ImGui::MenuItem("Open...", nullptr, nullptr);
			ImGui::MenuItem("Exit", nullptr, nullptr);
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Debug"))
		{
			ImGui::MenuItem("CPU State",nullptr,&m_showCPUDialog);
			ImGui::MenuItem("Pause emulation",nullptr,nullptr);
			ImGui::MenuItem("Reset", nullptr, nullptr);
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("View"))
		{
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

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

bool GuiRenderer::m_showAboutDialog = false;
bool GuiRenderer::m_showCPUDialog = false;