#include"Display.h"

Display::Display(int width, int height)
{
	Logger::getInstance()->msg(LoggerSeverity::Info, "Init display width=" + std::to_string(width) + " height=" + std::to_string(height));

	if (!glfwInit())
	{
		Logger::getInstance()->msg(LoggerSeverity::Error, "Fatal error attempting to initialize GLFW!");
		MessageBoxA(NULL, "Could not initialize GLFW..", "Error", MB_OK | MB_ICONERROR);
		return;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	m_window = glfwCreateWindow(width, height, "GBEmu", nullptr, nullptr);
	if (!m_window)
	{
		Logger::getInstance()->msg(LoggerSeverity::Error, "Fatal error creating OpenGL Context! Requested version was 4.6");
		MessageBoxA(NULL, "Could not create an OpenGL Context. Your GPU is either unsupported, or your drivers are out of date", "Error", MB_OK | MB_ICONERROR);
		return;
	}

	glfwMakeContextCurrent(m_window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		Logger::getInstance()->msg(LoggerSeverity::Error, "Fatal error loading OpenGL function pointers!");
		MessageBoxA(NULL, "Could not load necessary OpenGL function pointers. This is likely a GPU driver issue.", "Error", MB_OK | MB_ICONERROR);
		return;
	}
	m_initialized = true;
	//initialize vbo,vao,shaders,textures etc.
}

Display::~Display()
{
	Logger::getInstance()->msg(LoggerSeverity::Info, "Display: Shutting down");
	glfwTerminate();
}

bool Display::shouldClose()
{
	return glfwWindowShouldClose(m_window);
}

void Display::draw()
{
	glfwPollEvents();

	glClear(GL_COLOR_BUFFER_BIT);


	glfwSwapBuffers(m_window);
}

void Display::upload(void* data)
{
	//todo
}

bool Display::getInitialized() { return m_initialized; }