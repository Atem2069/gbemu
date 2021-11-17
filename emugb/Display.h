#pragma once

#include<iostream>
#include<Windows.h>
#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include"Logger.h"



class Display
{
public:
	Display(int width, int height);
	~Display();

	bool shouldClose();
	void draw();

	void upload(void* newData);

	bool getInitialized();

private:
	GLFWwindow* m_window;
	bool m_initialized = false;
};