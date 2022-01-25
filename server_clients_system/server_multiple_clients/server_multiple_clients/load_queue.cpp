#include "load_queue.h"
#include <iostream>

LoadQueue::LoadQueue()
{
	for (uint16 i = 0; i < 1024; ++i)
	{
		freeNodes.push(i);
	}
}

LoadQueue::~LoadQueue()
{
	freeNodes = queue<uint16>();
}

uint16 LoadQueue::getFreeNode()
{
	uint16 nodeId = freeNodes.front();
	freeNodes.pop();

	return nodeId;
}

void LoadQueue::addFreeNode(uint16 releasedNode)
{
	freeNodes.push(releasedNode);
}

void LoadQueue::showLoad()
{
	std::cout << "Available nodes: ";

	queue<uint16> freeNodesCopy = freeNodes;
	while (!freeNodesCopy.empty())
	{
		std::cout << '\t' << freeNodesCopy.front();
		freeNodesCopy.pop();
	}
	std::cout << '\n';
}