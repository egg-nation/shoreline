#include <iostream>
#include <chrono>
#include <bitset>

#define uint64 unsigned long long

// get time since epoch in microseconds(0.000001 or 10 ^ (-6)s)
uint64 get()
{
    return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

void printBinary(uint64 x)
{
    std::bitset<64> xb(x);
    std::cout << xb << '\n';
}

double timer(void(*compute)())
{
    auto start = std::chrono::high_resolution_clock::now();
    compute();
    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / 1000000.0;
}

class Generator
{
    uint64 nodeId;

public:
    Generator(uint64 nodeId)
    {
        if (nodeId >= 0 && nodeId <= 1023)
        {
            this->nodeId = nodeId;
        }
        else
        {
            exit(0);
        }
    }

    uint64 generate()
    {
        /* 1024 is 2 ^ 10 so we reserve the last 10 bits of the UUID for the nodeId number by shifting the value of the time since epoch by 10 bits
        and then add the value of the nodeId at the end of the returned number*/
        uint64 id = get() << 10;
        id = id + nodeId;

        return id;
    }
};

void doGenerate()
{

    std::cout << timer([]
    {
        Generator g(5);
        for (int i = 0; i < 100000; ++i)
        {
            g.generate();
        }
    });
}

int main()
{
    doGenerate();
    return 0;
}