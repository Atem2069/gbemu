#include"Gameboy.h"

GameBoy::GameBoy()
{
	//for now, we hardcode a game and BIOS in. MUST BE FIXED LATER
	std::vector<uint8_t> m_bios;
	std::ifstream biosReadHandle("BIOS\\gb_bios.bin", std::ios::in | std::ios::binary);
	biosReadHandle >> std::noskipws;
	while (!biosReadHandle.eof())
	{
		char curchar;
		biosReadHandle.read((char*)&curchar, sizeof(char));
		m_bios.push_back(curchar);
	}
	biosReadHandle.close();
	std::vector<uint8_t> m_ROM;
	std::ifstream romReadHandle("Games\\tetris.gb", std::ios::in | std::ios::binary);
	romReadHandle >> std::noskipws;
	while (!romReadHandle.eof())
	{
		unsigned char curchar;
		romReadHandle.read((char*)&curchar, sizeof(uint8_t));
		m_ROM.push_back((uint8_t)curchar);
	}

	//initialize MMU now
	m_mmu = new MMU(m_bios, m_ROM);
	m_interruptManager = new InterruptManager(m_mmu);
	m_cpu = new CPU(m_mmu,m_interruptManager);
	m_ppu = new PPU(m_mmu, m_interruptManager);
	m_inputManager = new InputManager(m_mmu);
}

GameBoy::~GameBoy()
{
	delete m_mmu;	//Objects are heap allocated, so ideally should be specifically deleted (to prevent mem leak)
	delete m_cpu;
	delete m_ppu;
	delete m_interruptManager;
	delete m_inputManager;
}

void GameBoy::run()
{
	std::thread m_dispWorkerThread(&GameBoy::displayWorker, this);
	while (m_shouldRun)
	{
		auto lastTime = std::chrono::high_resolution_clock::now();
		unsigned long lastCycleCount = m_cpu->getCycleCount();
		//step CPU
		if (!m_cpu->step())
		{
			Logger::getInstance()->msg(LoggerSeverity::Error, "CPU attempted to execute invalid address. dumping logs");
			return;
		}
		m_ppu->step(m_cpu->getCycleCount());
		m_inputManager->tick(m_inputState);

		auto curTime = std::chrono::high_resolution_clock::now();
		unsigned long cycleCountDiff = m_cpu->getCycleCount() - lastCycleCount;
		double timePeriod = 0.000952 * (double)cycleCountDiff;	//cpu clocks are measured in machine cycles (1.05MHz). 1 m-cycle is 0.000952 milliseconds
		auto timeDiff = std::chrono::duration<double, std::milli>(curTime - lastTime).count();
		while (timeDiff < timePeriod)
			timeDiff = std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now() - lastTime).count();

	}
	m_dispWorkerThread.join();
}

void GameBoy::displayWorker()
{

	Logger::getInstance()->msg(LoggerSeverity::Info, "Display worker: starting. .");
	Display m_disp(160 * 3, 144 * 3);

	while (!m_disp.shouldClose() && m_disp.getInitialized())
	{
		m_disp.upload((void*)m_ppu->getDisplay());
		m_disp.draw();

		m_inputState = { m_disp.getKeyPressed(GLFW_KEY_UP),m_disp.getKeyPressed(GLFW_KEY_DOWN),
			m_disp.getKeyPressed(GLFW_KEY_LEFT),m_disp.getKeyPressed(GLFW_KEY_RIGHT),
			m_disp.getKeyPressed(GLFW_KEY_Z),m_disp.getKeyPressed(GLFW_KEY_X),
			m_disp.getKeyPressed(GLFW_KEY_ENTER),false };

	}

	m_shouldRun = false;

	Logger::getInstance()->msg(LoggerSeverity::Info, "Display worker: ending. .");
}