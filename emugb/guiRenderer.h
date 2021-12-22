#pragma once

#include<iostream>
#include<Windows.h>
#include<format>
#include<imgui.h>
#include<imgui_impl_glfw.h>
#include<imgui_impl_opengl3.h>

#include"Config.h"
#include"dmgRegisters.h"
#include"debugStates.h"

class GuiRenderer
{
public:
	static void init(GLFWwindow* window);
	static void prepareFrame();
	static void render();
private:
	static bool m_showAboutDialog;
	static bool m_showCPUDialog;
	static bool m_openFileDialog;
	static bool m_showPPUDialog;
	static bool m_showIODialog;
};