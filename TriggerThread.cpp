#include "TriggerThread.h"



TriggerThread::TriggerThread(uint address)
{
	uiAddress = address;
}

TriggerThread::~TriggerThread()
{
}

void TriggerThread::run()
{
	while( true )
	{
		if (myTrigger.WaitForTrigger(uiAddress))
		{
			emit TriggerReceived();
			Sleep(20);
		}
	}
}
