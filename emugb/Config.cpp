#include"Config.h"
#include"Logger.h"

Config* Config::getInstance()
{
	if (!instance)
		instance = new Config();

	return instance;
}

Config::Config()
{
	Logger::getInstance()->msg(LoggerSeverity::Info, "Config: instance created.");
}

Config* Config::instance = nullptr;