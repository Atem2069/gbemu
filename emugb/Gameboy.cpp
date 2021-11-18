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
	m_cpu = new CPU(m_mmu);
	m_ppu = new PPU(m_mmu);
}

GameBoy::~GameBoy()
{
	delete m_mmu;	//Objects are heap allocated, so ideally should be specifically deleted (to prevent mem leak)
	delete m_cpu;
	delete m_ppu;
}

void GameBoy::run()
{
	std::thread m_dispWorkerThread(&GameBoy::displayWorker, this);
	while (m_shouldRun)
	{
		//step CPU
		if (!m_cpu->step())
		{
			Logger::getInstance()->msg(LoggerSeverity::Error, "CPU attempted to execute invalid address. dumping logs");
			return;
		}
		m_ppu->step(m_cpu->getCycleCount());

		//if necessary, update display
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
	}

	m_shouldRun = false;

	Logger::getInstance()->msg(LoggerSeverity::Info, "Display worker: ending. .");
}