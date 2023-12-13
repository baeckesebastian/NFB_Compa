#include "NFB_Comparatio.h"

const float PlugInVersion = 0.7;



NFB_Comparatio::NFB_Comparatio(QObject *parent)
{	
}

NFB_Comparatio::~NFB_Comparatio()
{
	//myTriggerThread->terminate();
	myTimerThread->terminate();
	myFeedbackPresenter->close();
	myRTLogTable->close();
}

bool NFB_Comparatio::initPlugin() 
{
	return InitTBVAccess();
}

bool NFB_Comparatio::executePreRun()
{
	uiTriggerCounter = 0;

	/* RTLogTable */
	//int iTR = tGetIntegerInput("TR in ms: ", 2000, 1, 5000);
	myRTLogTable = new RTLogTable(qetTimer, 2000, uiTriggerCounter);
    myRTLogTable->show();

	/* size for sliding window */
	iMoveBaseWinSize = tGetIntegerInput("Window size for moving window?", 10, 1, 100);
	sprintf_s(cLogString, "<b>MovingWindowBaseline (Size:%i)</b>", iMoveBaseWinSize);
	tLogText(cLogString);
	
	/* maximum percentage signal increase */
	fMaxPercChange = tGetFloatInput("Maximum percentage signal increase?", 2.5, 0.5, 10);
	sprintf_s(cLogString, "<b>fMaxPercChange:</b> %2.2f%%", fMaxPercChange);
	tLogText(cLogString);

	/* TimerThread */
	tGetPluginsPath(cPluginsPath);
	tGetProjectName(cProjectName);
	myTimerThread = new TimerThread(qetTimer, uiTriggerCounter, QString(cPluginsPath), QString(cProjectName));
	myTimerThread->start();

	/* FeedbackPresenter */
	myFeedbackPresenter = new FeedbackPresenter(fMaxPercChange);
	myFeedbackPresenter->show();

	/* TriggerThread */
	//char cTriggerPort[601];
	//if(tGetTextInput("Trigger Memory Address: ", "C011", cTriggerPort))		// 7T: EC00????
	/*char cTriggerPort[] = "C011";
	{
		QString qsTriggerPort = cTriggerPort;
		myTriggerThread = new TriggerThread( qsTriggerPort.toUInt(0, 16) );
		myTriggerThread->start();
		if( myTriggerThread->isRunning() )
			tLogText("TriggerThread is running.");
	}*/
	QShortcut* qTriggerShortcut = new QShortcut(QKeySequence(Qt::Key_T), myFeedbackPresenter);

	/* TriggerSimulation */
	QShortcut *qHiddenShortcut1 = new QShortcut(QKeySequence(Qt::CTRL+Qt::Key_T), myFeedbackPresenter);
	tLogText("  Strg+T: \tTrigger-Simulation Switch");
	
	/* Increase/Decrease maximum percentage signal change */
	QShortcut *qHiddenShortcut2 = new QShortcut(QKeySequence(Qt::CTRL+Qt::Key_Plus), myFeedbackPresenter);
	tLogText("  Strg++: \tIncrease maximum percentage signal change (+0.1%)");
	QShortcut *qHiddenShortcut3 = new QShortcut(QKeySequence(Qt::CTRL+Qt::Key_Minus), myFeedbackPresenter);
	tLogText("  Strg+-: \tDecrease maximum percentage signal change (-0.1%)");

	/* CONNECTIONS */
	//connect( myTriggerThread,		SIGNAL( TriggerReceived() ),		myTimerThread,			SLOT( TriggerReceived() ));
	connect( qTriggerShortcut,		SIGNAL( activated() ),				myTimerThread,			SLOT( TriggerReceived() ));
	//connect( myTriggerThread,		SIGNAL( TriggerReceived() ),		myRTLogTable,			SLOT( TriggerReceivedLog() ));
	connect( qTriggerShortcut,		SIGNAL( activated() ),				myRTLogTable,			SLOT( TriggerReceivedLog() ));
	connect( this,					SIGNAL( ImageReceived() ),			myTimerThread,			SLOT( LogImgReceived() ));
	connect( this,					SIGNAL( ImageProcessed() ),			myTimerThread,			SLOT( LogImgProcessed() ));
	connect( this,					SIGNAL( ImageReceived() ),			myRTLogTable,			SLOT( ImageReceivedLog() ));
	connect( this,					SIGNAL( ImageProcessed() ),			myRTLogTable,			SLOT( ImageProcessedLog() ));
	connect( this,					SIGNAL( StopPresentation() ),		myFeedbackPresenter,	SLOT( StopPresentation() ));
	connect( qHiddenShortcut1,		SIGNAL( activated() ),				myTimerThread,			SLOT( TriggerSimSwitch() ));
	connect( qHiddenShortcut2,		SIGNAL( activated() ),				myFeedbackPresenter,	SLOT( incMaximum() ));
	connect( qHiddenShortcut3,		SIGNAL( activated() ),				myFeedbackPresenter,	SLOT( decMaximum() ));
	connect( myTimerThread,			SIGNAL( StartBaseline() ),			myFeedbackPresenter,	SLOT( StartBaseline() ));
	connect( myTimerThread,			SIGNAL( StartThermometer(int) ),	myFeedbackPresenter,	SLOT( StartThermometer(int) ));
	connect( myTimerThread,			SIGNAL( StartFaces(int) ),			myFeedbackPresenter,	SLOT( StartFaces(int) ));
	connect( myTimerThread,			SIGNAL( StartNoFeedback(int) ),		myFeedbackPresenter,	SLOT( StartNoFeedback(int) ));
	connect( myTimerThread,			SIGNAL( ShowResults() ),			myFeedbackPresenter,	SLOT( ShowResults() ));
	connect( myTimerThread,			SIGNAL( SimulatedTrigger() ),		myRTLogTable,			SLOT( TriggerReceivedLog() ));
	connect( myFeedbackPresenter,	SIGNAL( LogBaseline() ),			myTimerThread,			SLOT( LogBaseline() ));
	connect( myFeedbackPresenter,	SIGNAL( LogThermometer(int) ),		myTimerThread,			SLOT( LogThermometer(int) ));
	connect( myFeedbackPresenter,	SIGNAL( LogFace(int) ),				myTimerThread,			SLOT( LogFace(int) ));
	connect( myFeedbackPresenter,	SIGNAL( LogNoFB(int) ),				myTimerThread,			SLOT( LogNoFB(int) ));
	connect( myFeedbackPresenter,	SIGNAL( LogShowResults(int) ),		myTimerThread,			SLOT( LogShowResults(int) ));

	tLogText("  DoubleClick: \tFull-Screen Switch");
	tLogText("<br>");
	return true;
}

bool NFB_Comparatio::executePreStep()
{
	emit ImageReceived();
	dataValuesTMP.clear();
	return true;
}

bool NFB_Comparatio::executePostStep()
{
	emit ImageProcessed();

	int iCurrentTR = tGetCurrentTimePoint(); // time point just finished processing: 1-based (for access of 0-based data, subtract 1)
	sprintf_s(cLogString, "<b>%04i</b>", iCurrentTR);

	float fValueOfDesignMatrix = tGetValueOfDesignMatrix(0, iCurrentTR-1);

	if( fValueOfDesignMatrix < 0.1)
		qviBaseline.append( iCurrentTR-1 );

	if(tGetNrOfROIs() > 0)
	{
		dataValuesTMP.append( tGetNrOfVoxelsOfROI(0) );			//[0]
		dataValuesTMP.append( tGetMeanOfROI(0) );				//[1]
		dataValuesTMP.append( calcBaseline() );					//[2]
			
		sprintf_s(cLogString, "%s [Voxel:%i Mean:%4.3f]", cLogString, dataValuesTMP[0].toInt(), dataValuesTMP[1].toFloat());
		
		if(fValueOfDesignMatrix < 0.1)
		{
			sprintf_s(cLogString, "%s <b>%4.3f</b>", cLogString, dataValuesTMP[2].toFloat());
		}
		else
		{
			float fPercSignChange = ((dataValuesTMP[1].toFloat() * 100) / dataValuesTMP[2].toFloat()) - 100;

			if( !myTimerThread->BaselinePeriod() )
			{
				dataValuesTMP.append( fPercSignChange );			//[3]

				if( fPercSignChange < 0 )	
					fPercSignChange = 0;
				if( fPercSignChange > fMaxPercChange )	
					fPercSignChange = fMaxPercChange;
						
				dataValuesTMP.append( fPercSignChange );										//[4]
				dataValuesTMP.append( fMaxPercChange );											//[5]
				dataValuesTMP.append( myFeedbackPresenter->updateSignal( fPercSignChange ) );	//[6]
				dataValuesTMP.append( myFeedbackPresenter->iCurrentTargetLevel );				//[7]
				
				sprintf_s(cLogString, "%s <b><font color='red'>%2.2f%%</font></b> (%2.1f%%) -> %i/%i", cLogString, fPercSignChange, fMaxPercChange, dataValuesTMP[6].toInt(), dataValuesTMP[7].toInt());


			}
		}
	}

	tLogText(cLogString);
	dataValues.append(dataValuesTMP);

	return true;
}

bool NFB_Comparatio::executePostRun()
{
	emit StopPresentation();

	tLogText( "<br><b>LOGFILE:</b>" );
	QString qsFilename = myTimerThread->WriteLogFile(dataValues);
	tLogText( qsFilename.toStdString().c_str() );
	tLogText("<br>Plugin completed processing.");

	myRTLogTable->finalize();
	myTimerThread->terminate();
	//myTriggerThread->terminate();

	return true;
}

float NFB_Comparatio::calcBaseline()
{
	int iTimepoints		= iMoveBaseWinSize;
	int iNrVoxels		= tGetNrOfVoxelsOfROI(0);
	if( iTimepoints > qviBaseline.size() )
		iTimepoints		= qviBaseline.size();
	float fMeanSignal	= 0.0;

	for(int vx=0; vx<iNrVoxels; vx++)
	{
		int vx_x, vx_y, vx_z;
		tGetCoordsOfVoxelOfROI(0, vx, vx_x, vx_y, vx_z);
		
		for(int t=qviBaseline.size()-1; t>=qviBaseline.size()-iTimepoints; t--)
		{
			fMeanSignal += tGetValueOfVoxelAtTime(vx_x, vx_y, vx_z, qviBaseline[t]);
		}
	}
	return fMeanSignal / (iTimepoints * iNrVoxels);
}



PLUGIN_ACCESS TBVPluginInterface *createPlugin() 
{
	return new NFB_Comparatio();
}

PLUGIN_ACCESS const char *getPluginName() 
{
	char *PluginName = new char[100];
	sprintf_s(PluginName, 100, "NFB [Thermometer 27x(8+20+2)+8=818Sc] v%1.1f", PlugInVersion);
	//sprintf_s(PluginName, 100, "NFB [Smiley 27x(8+12+2)+8=602Sc] v%1.1f", PlugInVersion);
	//sprintf_s(PluginName, 100, "NFB [Smiley 27x(8+20+2)+8=818Sc] v%1.1f", PlugInVersion);
	return PluginName;
}

static const char *PluginDescription = "This simple plugin shows ...";
PLUGIN_ACCESS const char *getPluginDescription() 
{
	return PluginDescription;
}

PLUGIN_ACCESS const char *getAuthor()
{
	return "<i>Sebastian Baecke</i>, Otto-von-Guericke-University Magdeburg";
}

PLUGIN_ACCESS const char *getHelpFile()
{
	return ""; // "NFB_ComparatioHelp/index.html";
}

PLUGIN_ACCESS const char *getPluginVersion()
{
	char *PluginVersionString = new char[6];
	sprintf_s(PluginVersionString, 6, "%1.1f", PlugInVersion);
	return PluginVersionString;
}
