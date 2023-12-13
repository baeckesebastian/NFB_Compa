#ifndef TRIGGERTHREAD_H
#define TRIGGERTHREAD_H

#include "Trigger.h"



class TriggerThread : public QThread
{
	Q_OBJECT

public:
	
	TriggerThread(uint Address);		
	~TriggerThread();				

protected:

	void run();

private:

	Trigger myTrigger;
	uint uiAddress;

signals:

	void TriggerReceived();

};
#endif
