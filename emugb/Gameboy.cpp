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
	while (!m_initialized)
	{
		Sleep(500);	//sleep periodically until user specifies ROM
		m_initialise();
		if (!m_shouldRun)
		{
			m_dispWorkerThread.join();
			return;
		}
	}

	m_lastTime = std::chrono::high_resolution_clock::now();

	while (m_shouldRun)
	{

		if (Config::GB.System.reset)
		{
			m_bufAccessLock.lock();
			m_destroy();
			m_initialise();
			m_bufAccessLock.unlock();
		}

		while (Config::GB.System.pause)
		{
			//do nothing until unpaused..
			Sleep(1);
			if (!m_shouldRun)
				break;
		}
		auto lastTime = std::chrono::high_resolution_clock::now();
		uint64_t lastCycleCount = m_cpu->getCycleCount();
		//step CPU
		m_cpu->step();
		uint64_t steps = m_cpu->getCycleCount();
		m_ppu->step(steps);
		m_apu->step(steps, m_bus->getInDoubleSpeedMode());
		m_inputManager->tick(m_inputState);
		m_timer->tick(m_cpu->getCycleCount() - lastCycleCount);


		m_cyclesSinceLastVblank += (m_cpu->getCycleCount() - lastCycleCount);
		if ((!m_cpu->getInDoubleSpeedMode() && m_cyclesSinceLastVblank >= 17556) || (m_cpu->getInDoubleSpeedMode() && m_cyclesSinceLastVblank >= 35112))
		{
			//m_apu->playSamples();	//apu waits for samples to go down to 0 before replaying
			m_cyclesSinceLastVblank = 0;

		}

	}
	m_dispWorkerThread.join();
}

void GameBoy::displayWorker()
{

	Logger::getInstance()->msg(LoggerSeverity::Info, "Display worker: starting. .");

	if (Config::GB.Display.displayScale == 0)
		Config::GB.Display.displayScale = 5;

	int dispScale = Config::GB.Display.displayScale;

	Display m_disp(160 * dispScale, 144 * dispScale);

	while (!m_disp.shouldClose() && m_disp.getInitialized())
	{
		m_bufAccessLock.lock();
		if (m_ppu.get())
		{
			bool newData = false;
			void* dataPtr = m_ppu->getDisplay(newData);
			m_disp.upload(dataPtr, newData);
			//m_disp.upload((void*)m_ppu->getDisplay());
		}
		m_bufAccessLock.unlock();
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
	std::string cart = Config::GB.System.RomName;
	if (cart.empty())
		return;

	m_apu = std::make_shared<APU>();

	if (!m_loadCartridge(cart, m_bus))
	{
		Config::GB.System.RomName = "";
		return;
	}


	//initialize bus now
	m_interruptManager = std::make_shared<InterruptManager>(m_bus);
	m_cpu = std::make_shared<CPU>(m_bus, m_interruptManager);
	m_ppu = std::make_shared<PPU>(m_bus, m_interruptManager);
	m_inputManager = std::make_shared<InputManager>(m_bus,m_interruptManager);
	m_timer = std::make_shared<Timer>(m_bus, m_interruptManager);

	Logger::getInstance()->msg(LoggerSeverity::Info, "Initialized new Game Boy instance!");
	Config::GB.System.reset = false;
	m_initialized = true;

}

void GameBoy::m_destroy()
{
	Logger::getInstance()->msg(LoggerSeverity::Info, "De-initing current Game Boy instance.");

	m_timer.reset();
	m_inputManager.reset();
	m_ppu.reset();
	m_cpu.reset();
	m_interruptManager.reset();
	m_bus.reset();
	m_apu.reset();
}

bool GameBoy::m_loadCartridge(std::string name, std::shared_ptr<Bus>& bus)
{
	std::vector<uint8_t> cartData;
	std::ifstream romReadHandle(name, std::ios::in | std::ios::binary);
	if (!romReadHandle)
	{
		Logger::getInstance()->msg(LoggerSeverity::Error, "Invalid file " + name);
		return false;
	}
	romReadHandle >> std::noskipws;
	while (!romReadHandle.eof() && cartData.size() <= (8*1024*1024))
	{
		unsigned char curByte;
		romReadHandle.read((char*)&curByte, sizeof(uint8_t));
		cartData.push_back((uint8_t)curByte);
	}

	std::string title = "";

	//check nintendo logo
	bool cartValid = true;
	for (int i = 0; i < 47; i++)
	{
		if (cartData[0x104 + i] != m_headerChecksumData[i])
			cartValid = false;
	}

	if (!cartValid)
	{
		Logger::getInstance()->msg(LoggerSeverity::Warn, "Checksum failed on input file!!");
		MessageBoxA(NULL, "Header checksum failed on file (Nintendo Logo bytes are incorrect).\nThis file would not run on a real Game Boy.", "Warning", MB_OK | MB_ICONWARNING);
	}

	uint8_t i = 0;
	while(cartData[CART_TITLE + i] && i<16)					//read until zero terminated
		title += (char)cartData[CART_TITLE + i++];
	Logger::getInstance()->msg(LoggerSeverity::Info, "ROM Title: " + title);

	std::vector<uint8_t> m_bios;
	m_bios.assign(BIOS::cgbDump, BIOS::cgbDump + 2304);	//256 byte BIOS.
	//std::copy(std::begin(BIOS::cgbDump), std::end(BIOS::cgbDump), m_bios.begin());
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

	bus = std::make_shared<Bus>(m_bios, cartData,m_apu);


	return true;
}