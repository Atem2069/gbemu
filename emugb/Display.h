#pragma once

#include<iostream>
#include<Windows.h>
#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include"Logger.h"
#include"common/vec3.h"



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

	GLuint m_VBO=0, m_VAO=0, m_program=0;

	bool m_initialized = false;
};