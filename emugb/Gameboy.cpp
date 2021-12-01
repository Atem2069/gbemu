#include"Gameboy.h"

GameBoy::GameBoy()
{
	m_initialise();
}

GameBoy::~GameBoy()
{
	m_destroy();
}

void GameBoy::run()
{
	std::thread m_dispWorkerThread(&GameBoy::displayWorker, this);
	while (m_shouldRun)
	{
		auto lastTime = std::chrono::high_resolution_clock::now();
		unsigned long lastCycleCount = m_cpu->getCycleCount();
		//step CPU
		m_cpu->step();
		m_ppu->step(m_cpu->getCycleCount());
		m_inputManager->tick(m_inputState);
		m_timer->tick();

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

void GameBoy::m_initialise()
{
	std::vector<uint8_t> m_bios;
	m_bios.assign(BIOS::rawData, BIOS::rawData + 256);	//256 byte BIOS.

	std::vector<uint8_t> m_ROM;
	std::ifstream romReadHandle("Games\\tetris.gb", std::ios::in | std::ios::binary);
	if (!romReadHandle)
	{
		Logger::getInstance()->msg(LoggerSeverity::Error, "ROM file specified does not exist! Fatal error");
		return;
	}
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
	m_cpu = new CPU(m_mmu, m_interruptManager);
	m_ppu = new PPU(m_mmu, m_interruptManager);
	m_inputManager = new InputManager(m_mmu);
	m_timer = new Timer(m_mmu, m_interruptManager);

	Logger::getInstance()->msg(LoggerSeverity::Info, "Initialized new Game Boy instance!");
}

void GameBoy::m_destroy()
{
	Logger::getInstance()->msg(LoggerSeverity::Info, "De-initing current Game Boy instance.");
	delete m_mmu;	
	delete m_cpu;
	delete m_ppu;
	delete m_interruptManager;
	delete m_inputManager;
	delete m_timer;
}