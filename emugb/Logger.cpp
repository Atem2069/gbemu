#include"Logger.h"

Logger::Logger()
{
	//todo
}

Logger* Logger::getInstance()
{
	if (!instance)
		instance = new Logger();


	return instance;
}

void Logger::msg(LoggerSeverity severity, std::string msg)
{
	std::string prefix = "";
	switch (severity)
	{
	case LoggerSeverity::Error: prefix = "[ERROR]"; break;
	case LoggerSeverity::Warn: prefix = "[WARN]"; break;
	case LoggerSeverity::Info:prefix = "[INFO]"; break;
	}

	prefix += msg;

	m_msgLog.push(prefix);
	if (m_msgLog.size() > 255)
		m_msgLog.pop();
	//std::cout << prefix << std::endl;	//todo add logs to queue which can be output to log file, or in GUI
}

void Logger::dumpToConsole()
{
	while (!m_msgLog.empty())
	{
		std::cout << m_msgLog.front() << std::endl;
		m_msgLog.pop();
	}
}

Logger* Logger::instance = nullptr;	//initialize static instance to null explicitly inside a translation unit