#include<iostream>

#include"Logger.h"

int main()
{
	Logger::getInstance()->msg(LoggerSeverity::Info, "Hello world!");
	Logger::getInstance()->dumpToConsole();
	return 0;
}