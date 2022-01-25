#ifndef LOADQUEUE_H
#define LOADQUEUE_H

#include <iostream>
#include <chrono>
#include <queue>

using std::queue;

#define uint16 unsigned short

class LoadQueue
{
private:
	queue<uint16> freeNodes;
public:
	LoadQueue();
	~LoadQueue();
	uint16 getFreeNode();
	void addFreeNode(uint16);
	void showLoad();
};

#endif