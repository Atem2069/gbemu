#pragma once
#include<iostream>

const uint16_t REG_HDMA1 = 0xFF51;	//dma src (high)
const uint16_t REG_HDMA2 = 0xFF52;	//dma src (low)
const uint16_t REG_HDMA3 = 0xFF53;	//dma dest (high)
const uint16_t REG_HDMA4 = 0xFF54;	//dma dest (low)
const uint16_t REG_HDMA5 = 0xFF55;	//dma length/mode/start

const uint16_t REG_VBK = 0xFF4F;	//vram bank (bit 0)
const uint16_t REG_SVBK = 0xFF70;	//warm bank (bits 0-2)

const uint16_t REG_OPRI = 0xFF6C;	//object priority 