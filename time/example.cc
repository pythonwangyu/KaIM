#include <iostream>
#include "TimerQueue.h"


void print(void* arg)
{
    std::cout << "he" << std::endl;
}



int main()
{
    KaNet::LoopEventList loopEvent;
    loopEvent.addLoopEvent(print,NULL);


    while(1)
    {

    }

    return 0;
}

