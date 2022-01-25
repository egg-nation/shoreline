#ifndef GENERATOR_H
#define GENERATOR_H

#include <iostream>
#include <chrono>
#include <bitset>

#define uint64 unsigned long long
#define uint16 unsigned short

class Generator
{
private: 
	uint16 nodeId;
public:
	Generator(uint16 nodeId);
	uint64 generate();
};

#endif