#ifndef TRIGGER_H
#define TRIGGER_H

#include <windows.h>	
#include <stdio.h>
#include <QtGui>	

class Trigger
{
public:

	Trigger();	// Constructor
	~Trigger(); // Destructor

	static bool WaitForTrigger(uint address);	// wait for trigger signal - the relevant bits are from status register bits 6 and 7
	
private:

};

#endif
