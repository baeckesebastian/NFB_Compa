#ifndef TIMERTHREAD_H
#define TIMERTHREAD_H

#include <QtCore>



class TimerThread : public QThread
{
	Q_OBJECT

public:
	
	TimerThread(QElapsedTimer &qetTimer, uint &uiTriggerCounter, QString qsPluginsPath, QString qsProjectName);	
	~TimerThread();	
	QString WriteLogFile(QList<QVector<QVariant>> dataValues);
	bool BaselinePeriod();
	bool isStarted();

protected:
  
	void run();

private:

	bool				bStarted, bBaseline;
	QString				qsPath, qsProject;
	QElapsedTimer		*qTimer;
	QTimer				*qtTriggerSimTimer;
	uint				*iTriggerCounter;

	QVector<int>		cond;
	
	QList<int>			qlImgReceived, qlImgProcessed, qlBase, qlFace1, qlFace2, qlFace3, qlThermo1, qlThermo2, qlThermo3, qlNoFB1, qlNoFB2, qlNoFB3, qlShowResultWin, qlShowResultLose;

private slots:

	void TriggerSimSwitch();
	void TriggerReceived();
	void updatePresentation();
	void LogImgReceived();
	void LogImgProcessed();
	void LogBaseline();
	void LogThermometer(int);
	void LogFace(int);
	void LogNoFB(int);
	void LogShowResults(int);

signals:

	void StartBaseline();
	void StartThermometer(int);
	void StartFaces(int);
	void StartNoFeedback(int);
	void ShowResults();
	void SimulatedTrigger();

};
#endif