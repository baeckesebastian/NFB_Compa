#ifndef FEEDBACKPRESENTER_H
#define FEEDBACKPRESENTER_H

#include <QtWidgets>
#include "balken.h"



class FeedbackPresenter : public QWidget
{
	Q_OBJECT

public:

	FeedbackPresenter(float &fMax);
	~FeedbackPresenter();

	int updateSignal(float fCurrPerc);

	int						iCurrentTargetLevel;
	
private:

	void					mouseDoubleClickEvent(QMouseEvent *e);

	QGraphicsScene			*QGSceneEmpty, *QGSceneBaseline, *QGSceneThermometer, *QGSceneFace, *QGSceneNoFeedback, *QGSceneResults;
	QLabel					*myTaskLabel, *myPixLabel;
	QGraphicsView			*myView;
	QGraphicsRectItem		*bRect;
	QPropertyAnimation		*myAnim;
	QGraphicsSimpleTextItem	*PosResults, *NegResults;
	QImage					Neutral, P2, P5, P8;
	float					fCurrPercChange, fNewPercChange, *fMaxPerc;
	bool					bFace, bThermometer, bNoFeedback;
	QVector<float>			qvResults;
	
signals:

	void LogBaseline();
	void LogThermometer(int);
	void LogFace(int);
	void LogNoFB(int);
	void LogShowResults(int);

private slots:

	void StartBaseline();
	void StartThermometer(int level);
	void StartFaces(int level);
	void StartNoFeedback(int level);
	void ShowResults();
	void StopPresentation();
	void incMaximum();
	void decMaximum();
};

#endif