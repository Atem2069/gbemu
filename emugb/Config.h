#pragma once

#include<iostream>

#include"debugStates.h"

struct PPUConfig
{
	bool debugOverride;
	bool background;
	bool window;
	bool sprites;
};

struct SystemConfig
{
	std::string RomName;
	bool useBootRom;
	bool reset;
	bool pause;
	bool DmgMode;
};

struct GBConfig
{
	CPUState cpuState;
	PPUConfig PPU;
	SystemConfig System;
};

class Config
{
public:
	static GBConfig GB;
};