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
	std::ifstream romReadHandle("Tests\\02-interrupts.gb", std::ios::in | std::ios::binary);
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
}

GameBoy::~GameBoy()
{
	delete m_mmu;	//Objects are heap allocated, so ideally should be specifically deleted (to prevent mem leak)
}

void GameBoy::run()
{
	//spawn render thread here (initializes Renderer object, then runs indefinitely)
	while (m_shouldRun)
	{
		//step CPU
		if (!m_cpu->step())
		{
			Logger::getInstance()->msg(LoggerSeverity::Error, "Critical error occurred while stepping CPU. dumping logs");
			return;
		}
		//step PPU

		//if necessary, update display
	}
	//join render thread, then exit
}