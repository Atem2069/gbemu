#pragma once

#include<iostream>
#include<format>
#include<imgui.h>
#include<imgui_impl_glfw.h>
#include<imgui_impl_opengl3.h>

#include"Config.h"

class GuiRenderer
{
public:
	static void init(GLFWwindow* window);
	static void prepareFrame();
	static void render();
private:
	static bool m_showAboutDialog;
	static bool m_showCPUDialog;
};