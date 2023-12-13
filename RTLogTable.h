#ifndef RTLOGTABLE_H
#define RTLOGTABLE_H

#include <QtWidgets>



class RTLogTable : public QMainWindow {

	Q_OBJECT

	public:
	
		RTLogTable(QElapsedTimer &qetTimer, int iTR, uint &uiTriggerCounter);
		QTableWidget *tableWidget;

	public slots:

		void TriggerReceivedLog();
		void ImageReceivedLog();
		void ImageProcessedLog();
		void finalize();

	private:

		uint				*iTriggerCounter;
		uint				uiImgCounter;
		uint				uiProcCounter;
		int					iTR;
		QElapsedTimer		*qTimer;
		QTimer				*qtTriggerEmulationTimer;
		QStringList			verticHeader;
		QList<int>			qListTransferTime;
		QList<int>			qListProcessingTime;
};

#endif