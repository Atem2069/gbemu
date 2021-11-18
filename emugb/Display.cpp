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

	Vertex vertices[6] =
	{
		{{1.0f,-1.0f,0.0f},{1.0f,1.0f} },
		{{1.0f,1.0f,0.0f}, {1.0f,0.0f}},
		{{-1.0f,1.0f,0.0f}, {0.0f,0.0f}},
		{{-1.0f,1.0f,0.0f}, {0.0f,0.0f}},
		{{-1.0f,-1.0f,0.0f}, {0.0f,1.0f} },
		{{ 1.0f,-1.0f,0.0f}, {1.0f,1.0f} }
	};

	glGenBuffers(1, &m_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	GLuint m_vs = 0, m_fs = 0;

	std::string m_vsSource =
#include"Shaders/vertex.h"
		;

	std::string m_fsSource =
#include"Shaders/fragment.h"
		;

	const char* m_vsSourcePtr = m_vsSource.c_str();	//account for opengl 2-level pointer bs
	const char* m_fsSourcePtr = m_fsSource.c_str();

	m_vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(m_vs, 1, &m_vsSourcePtr, 0);
	glCompileShader(m_vs);

	m_fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(m_fs, 1, &m_fsSourcePtr, 0);
	glCompileShader(m_fs);

	m_program = glCreateProgram();
	glAttachShader(m_program, m_vs);
	glAttachShader(m_program, m_fs);
	glLinkProgram(m_program);
	glUseProgram(m_program);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &m_texHandle);
	glBindTexture(GL_TEXTURE_2D, m_texHandle);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 160, 144, 0, GL_RGB, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	m_initialized = true;
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

	glUseProgram(m_program);
	glBindVertexArray(m_VAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_texHandle);	//redundant?

	glDrawArrays(GL_TRIANGLES, 0, 6);


	glfwSwapBuffers(m_window);
}

void Display::upload(void* data)
{
	glBindTexture(GL_TEXTURE_2D, m_texHandle);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 160, 144, GL_RGB, GL_FLOAT, data);
}

bool Display::getInitialized() { return m_initialized; }