#include "TimerThread.h"



TimerThread::TimerThread(QElapsedTimer &qetTimer, uint &uiTriggerCounter, QString qsPluginsPath, QString qsProjectName)
{
	qsPath				= qsPluginsPath;
	qsProject			= qsProjectName;
	bStarted			= false;
	bBaseline			= false;
	iTriggerCounter		= &uiTriggerCounter;
	qTimer				= &qetTimer;
	qtTriggerSimTimer	= new QTimer(this);
	connect(qtTriggerSimTimer, SIGNAL( timeout() ), this, SLOT( TriggerReceived() ));

	cond << 1 << 5 << 8 << 2 << 1 << 0 << 5 << 8 << 0 << 7 << 6 << 4 << 7 << 1 << 7 << 2 << 3 << 2 << 8 << 0 << 6 << 4 << 3 << 6 << 4 << 5 << 3;
	//cond << 1 << 2 << 2 << 2 << 1 << 0 << 2 << 2 << 0 << 1 << 0 << 1 << 1 << 1 << 1 << 2 << 0 << 2 << 2 << 0 << 0 << 1 << 0 << 0 << 1 << 2 << 0;
}

TimerThread::~TimerThread()
{
}

void TimerThread::run()
{
}

void TimerThread::TriggerSimSwitch()
{
	if( !bStarted )
	{
		qtTriggerSimTimer->start( 2000-3 );
		TriggerReceived();
	}
	else
	{
		if( qtTriggerSimTimer->isActive() )
		{
			qtTriggerSimTimer->stop();
		}
	}
}

void TimerThread::TriggerReceived()
{
	*iTriggerCounter = *iTriggerCounter + 1;
	if( !bStarted )
	{
		qTimer->start();
		bStarted = true;
	}
	updatePresentation();
	if( qtTriggerSimTimer->isActive() )
	{
		emit SimulatedTrigger();
	}
}

void TimerThread::updatePresentation()
{
	//int blocklength = 22;
	int blocklength = 30;
	
	switch( *iTriggerCounter%blocklength )	// Modulo-Division
	{
		case 1:
			emit StartBaseline();
			bBaseline = true;
			break;
		case 9: 
			switch( cond[*iTriggerCounter/blocklength]/3 )
			{
			case 0:
				emit StartThermometer((cond[*iTriggerCounter/blocklength]%3)+1);
				break;
			case 1:
				emit StartFaces((cond[*iTriggerCounter/blocklength]%3)+1);
				break;
			case 2:
				emit StartNoFeedback((cond[*iTriggerCounter/blocklength]%3)+1);
				break;
			}
			bBaseline = false;
			break;
		//case 21:		
		case 29:
			emit ShowResults();
			bBaseline = true;
			break;
	}

	if( *iTriggerCounter == ((blocklength*27)+8) && qtTriggerSimTimer->isActive() )
		qtTriggerSimTimer->stop();
}


void TimerThread::LogImgReceived()
{
	qlImgReceived.append( qTimer->elapsed() );
}

void TimerThread::LogImgProcessed()
{
	qlImgProcessed.append( qTimer->elapsed() );
}

void TimerThread::LogBaseline()
{
	qlBase.append( qTimer->elapsed() );
}

void TimerThread::LogThermometer(int level)
{
	switch( level )
	{
		case 1:		qlThermo1.append( qTimer->elapsed() ); break;
		case 2:		qlThermo2.append( qTimer->elapsed() ); break;
		case 3:		qlThermo3.append( qTimer->elapsed() ); break;
	}
}

void TimerThread::LogFace(int level)
{
	switch( level )
	{
		case 1:		qlFace1.append( qTimer->elapsed() ); break;
		case 2:		qlFace2.append( qTimer->elapsed() ); break;
		case 3:		qlFace3.append( qTimer->elapsed() ); break;
	}
}

void TimerThread::LogNoFB(int level)
{
	switch( level )
	{
		case 1:		qlNoFB1.append( qTimer->elapsed() ); break;
		case 2:		qlNoFB2.append( qTimer->elapsed() ); break;
		case 3:		qlNoFB3.append( qTimer->elapsed() ); break;
	}
}

void TimerThread::LogShowResults(int result)
{
	switch( result )
	{
		case 0:		qlShowResultLose.append( qTimer->elapsed() ); break;
		case 1:		qlShowResultWin.append( qTimer->elapsed() ); break;
	}
}

QString TimerThread::WriteLogFile(QList<QVector<QVariant>> dataValues)
{
	QString qsFilename = qsPath + "/" + QDateTime::currentDateTime().toString("yyyyMMddhhmm") + "_" + qsProject + ".txt";
	QFile qfOut(qsFilename);
	qfOut.open(QIODevice::WriteOnly);
	if( qfOut.isOpen() )
	{
		QTextStream qtStream(&qfOut);

		qtStream << "ImgReceived: " << "\t";
		for(int i=0; i<qlImgReceived.size(); i++)	qtStream << qlImgReceived[i] << "\t";
		qtStream << "\n";
		qtStream << "ImgProcessed: ";
		for(int i=0; i<qlImgProcessed.size(); i++)	qtStream << qlImgProcessed[i] << "\t";
		qtStream << "\n";
		qtStream << "\n";
		qtStream << "Baseline: " << "\t\t";
		for(int i=0; i<qlBase.size(); i++)	qtStream << qlBase[i] << "\t";
		qtStream << "\n";
		qtStream << "Face1: " << "\t";
		for(int i=0; i<qlFace1.size(); i++)	qtStream << qlFace1[i] << "\t";
		qtStream << "\n";
		qtStream << "Face2: " << "\t";
		for(int i=0; i<qlFace2.size(); i++)	qtStream << qlFace2[i] << "\t";
		qtStream << "\n";
		qtStream << "Face3: " << "\t";
		for(int i=0; i<qlFace3.size(); i++)	qtStream << qlFace3[i] << "\t";
		qtStream << "\n";
		qtStream << "Thermo1: " << "\t";
		for(int i=0; i<qlThermo1.size(); i++)	qtStream << qlThermo1[i] << "\t";
		qtStream << "\n";
		qtStream << "Thermo2: " << "\t";
		for(int i=0; i<qlThermo2.size(); i++)	qtStream << qlThermo2[i] << "\t";
		qtStream << "\n";
		qtStream << "Thermo3: " << "\t";
		for(int i=0; i<qlThermo3.size(); i++)	qtStream << qlThermo3[i] << "\t";
		qtStream << "\n";
		qtStream << "NoFB1: " << "\t";
		for(int i=0; i<qlNoFB1.size(); i++)	qtStream << qlNoFB1[i] << "\t";
		qtStream << "\n";
		qtStream << "NoFB2: " << "\t";
		for(int i=0; i<qlNoFB2.size(); i++)	qtStream << qlNoFB2[i] << "\t";
		qtStream << "\n";
		qtStream << "NoFB3: " << "\t";
		for(int i=0; i<qlNoFB3.size(); i++)	qtStream << qlNoFB3[i] << "\t";
		qtStream << "\n";
		qtStream << "ShowResultsWin: " << "\t";
		for(int i=0; i<qlShowResultWin.size(); i++)	qtStream << qlShowResultWin[i] << "\t";
		qtStream << "\n";
		qtStream << "ShowResultsLose: " << "\t";
		for(int i=0; i<qlShowResultLose.size(); i++)	qtStream << qlShowResultLose[i] << "\t";
		qtStream << "\n";
		qtStream << "\n";
		for(int i=0; i<dataValues.size(); i++)
		{
			qtStream << i+1 << "\t";
			for(int j=0; j<dataValues[i].size(); j++)
			{
				qtStream << dataValues[i][j].toString() << "\t";
			}
			qtStream << "\n";
		}
 		qfOut.close();
	}
	return qsFilename;
}

bool TimerThread::BaselinePeriod()
{
	return bBaseline;
}

bool TimerThread::isStarted()
{
	return bStarted;
}


