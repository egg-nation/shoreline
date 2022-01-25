#include "server.h"

int main()
{
    Server svr;
    svr.WaitForClient();

    system("pause");
    return 0;
}