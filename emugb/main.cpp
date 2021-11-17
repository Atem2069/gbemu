#include<iostream>

#include"Logger.h"
#include"Gameboy.h"

int main()
{
	Logger::getInstance()->msg(LoggerSeverity::Info, "Hello world!");

	GameBoy m_gameBoy;

	m_gameBoy.run();

	Logger::getInstance()->msg(LoggerSeverity::Info, "Emulator instance has ended. Dumping logs.. ");
	Logger::getInstance()->dumpToFile("cpu.log");
	return 0;
}