#include"Config.h"

Config* Config::getInstance()
{
	if (!instance)
		instance = new Config();

	return instance;
}

Config* Config::instance = nullptr;