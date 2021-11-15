#pragma once

#include<iostream>
#include<string>
#include<queue>
#include<fstream>
enum class LoggerSeverity
{
	Error,
	Warn,
	Info
};

class Logger
{
private:
	Logger();
	static Logger* instance;

	std::queue<std::string> m_msgLog;

public:
	static Logger* getInstance();

	void msg(LoggerSeverity severity, std::string msg);

	void dumpToConsole();

	void dumpToFile(std::string filename);

};