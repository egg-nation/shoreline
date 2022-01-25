#include "generator.h"
#include <iostream>

uint64 get()
{
	return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

void printBinary(uint64 x)
{
	std::bitset<64> xb(x);
	std::cout << xb << '\n';
}

Generator::Generator(uint16 nodeId)
{
	this->nodeId = nodeId;
}

uint64 Generator::generate()
{
	uint64 id = get() << 10;
	id = id + nodeId;

	return id;
}