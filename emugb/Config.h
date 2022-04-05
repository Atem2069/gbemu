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
	bool exit;
	bool DmgMode;
};

struct DisplayConfig
{
	int displayScale;
	bool resize;
	bool colorCorrect;
	bool frameBlend;
};

struct GBConfig
{
	CPUState cpuState;
	PPUConfig PPU;
	SystemConfig System;
	DisplayConfig Display;
};

class Config
{
public:
	static GBConfig GB;
};