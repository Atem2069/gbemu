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
	Display m_disp(160 * 5, 144 * 5);

	while (!m_disp.shouldClose() && m_disp.getInitialized())
	{
		m_disp.upload((void*)m_ppu->getDisplay());
		m_disp.draw();

		m_inputState = { m_disp.getKeyPressed(GLFW_KEY_UP),m_disp.getKeyPressed(GLFW_KEY_DOWN),
			m_disp.getKeyPressed(GLFW_KEY_LEFT),m_disp.getKeyPressed(GLFW_KEY_RIGHT),
			m_disp.getKeyPressed(GLFW_KEY_Z),m_disp.getKeyPressed(GLFW_KEY_X),
			m_disp.getKeyPressed(GLFW_KEY_ENTER),m_disp.getKeyPressed(GLFW_KEY_RIGHT_SHIFT) };

	}

	m_shouldRun = false;

	Logger::getInstance()->msg(LoggerSeverity::Info, "Display worker: ending. .");
}

void GameBoy::m_initialise()
{

	m_loadCartridge("Games\\mario.gb", m_mmu);

	//initialize MMU now
	m_interruptManager = std::make_shared<InterruptManager>(m_mmu);
	m_cpu = std::make_shared<CPU>(m_mmu, m_interruptManager);
	m_ppu = std::make_shared<PPU>(m_mmu, m_interruptManager);
	m_inputManager = std::make_shared<InputManager>(m_mmu,m_interruptManager);
	m_timer = std::make_shared<Timer>(m_mmu, m_interruptManager);

	Logger::getInstance()->msg(LoggerSeverity::Info, "Initialized new Game Boy instance!");
}

void GameBoy::m_destroy()
{
	Logger::getInstance()->msg(LoggerSeverity::Info, "De-initing current Game Boy instance.");
	//delete m_mmu;	
	//delete m_cpu;
	//delete m_ppu;
	//delete m_interruptManager;
	//delete m_inputManager;
	//delete m_timer;
}

bool GameBoy::m_loadCartridge(std::string name, std::shared_ptr<MMU>& mmu)
{
	std::vector<uint8_t> cartData;
	std::ifstream romReadHandle(name, std::ios::in | std::ios::binary);
	if (!romReadHandle)
	{
		Logger::getInstance()->msg(LoggerSeverity::Error, "Invalid file " + name);
		return false;
	}
	romReadHandle >> std::noskipws;
	while (!romReadHandle.eof())
	{
		unsigned char curByte;
		romReadHandle.read((char*)&curByte, sizeof(uint8_t));
		cartData.push_back((uint8_t)curByte);
	}

	std::string title = "";
	uint8_t i = 0;
	while(cartData[CART_TITLE + i])					//read until zero terminated
		title += (char)cartData[CART_TITLE + i++];
	Logger::getInstance()->msg(LoggerSeverity::Info, "ROM Title: " + title);

	std::array<uint8_t,256> m_bios;
	//m_bios.assign(BIOS::rawData, BIOS::rawData + 256);	//256 byte BIOS.
	std::copy(std::begin(BIOS::rawData), std::end(BIOS::rawData), m_bios.begin());
	uint8_t cartridgeType = cartData[CART_TYPE];
	Logger::getInstance()->msg(LoggerSeverity::Info, "Cartridge Type: " + std::to_string((int)cartridgeType));

	uint8_t cartridgeRomVal = cartData[CART_ROMSIZE];
	int cartridgeRomSize = 32768 * (int)pow(2, (double)cartridgeRomVal);
	int cartridgeBankNo = (int)pow(2, (double)cartridgeRomVal + 1);

	Logger::getInstance()->msg(LoggerSeverity::Info, "Cartridge Size: " + std::to_string(cartridgeRomSize) + " bytes (" + std::to_string(cartridgeBankNo) + " banks)");

	int ramLookup[] = { 0, 2, 8, 32, 128, 64 };	
	uint8_t ramSizeIdx = cartData[CART_RAMSIZE] % 6;
	if(ramSizeIdx)
		Logger::getInstance()->msg(LoggerSeverity::Info, "Cartridge contains an additional " + std::to_string(ramLookup[ramSizeIdx]) + " KB of RAM");

	if (!cartridgeType)
		mmu = std::make_shared<MMU>(m_bios, cartData);
	else if (cartridgeType >= 1 && cartridgeType <= 3)
		mmu = std::make_shared<MBC1>(m_bios, cartData);
	else
		Logger::getInstance()->msg(LoggerSeverity::Error, "Invalid cartridge specified. The Bank Switcher chip is not supported.");

	return true;
}