#include "FeedbackPresenter.h"



FeedbackPresenter::FeedbackPresenter(float &fMax)
{
	fMaxPerc		= &fMax;	
	fCurrPercChange = 0;	
	fNewPercChange	= 0;

	bFace			= false;
	bThermometer	= false;
	bNoFeedback		= false;

	iCurrentTargetLevel	= 0;

	qvResults.clear();

	setWindowTitle(tr("FeedbackPresenter"));
	setWindowFlags( windowFlags() ^ Qt::WindowStaysOnTopHint );
	setFixedSize( QSize(1024,768) );
	setStyleSheet("color: white; background-color: black");

	QGSceneEmpty		= new QGraphicsScene();
	QGSceneBaseline		= new QGraphicsScene();
	QGSceneThermometer	= new QGraphicsScene();
	QGSceneFace			= new QGraphicsScene();
	QGSceneNoFeedback	= new QGraphicsScene();
	QGSceneResults		= new QGraphicsScene();

	myView = new QGraphicsView();
	myView->setAlignment(Qt::AlignCenter);
	myView->setScene(QGSceneEmpty);
	myView->setMinimumSize(QSize(0, 610));

	myPixLabel = new QLabel;
	myPixLabel->setAlignment(Qt::AlignCenter);   
 	myPixLabel->setFont( QFont("Helvetica [Cronyx]", 70) );

	myTaskLabel = new QLabel;
	myTaskLabel->setAlignment(Qt::AlignCenter);
	myTaskLabel->setFont(QFont("Helvetica [Cronyx]", 40));

	QVBoxLayout *myLayout = new QVBoxLayout();
	myLayout->addStretch();
	myLayout->addWidget(myView);
	myLayout->addWidget(myTaskLabel);
	myLayout->addStretch();

	// wegen besserer Sichtbarkeit am 3T wurde alles etwas nach oben verschoben
	QGraphicsView *myGhost = new QGraphicsView();	
	myGhost->setMinimumSize(QSize(0, 40));
	myLayout->addWidget(myGhost);

    setLayout(myLayout);


	// QGSceneBaseline
	QGraphicsSimpleTextItem *FixationCross1 = QGSceneBaseline->addSimpleText(QString("+"));
	FixationCross1->setBrush(QBrush(Qt::white));
	FixationCross1->setFont(QFont("Helvetica [Cronyx]", 50));


	// QGSceneNoFeedback
	QGraphicsSimpleTextItem *FixationCross2 = QGSceneNoFeedback->addSimpleText(QString("+"));
	FixationCross2->setBrush(QBrush(Qt::white));
	FixationCross2->setFont(QFont("Helvetica [Cronyx]", 50));


	// QGSceneThermometer
	bRect = new QGraphicsRectItem(0,0,150,550);				// Blue rect
	bRect->setBrush(QBrush(Qt::blue));
	QGSceneThermometer->addItem(bRect);

	Balken *myBalken = new Balken(0,0,150,550,this);		// Emptiness (will be animated and paint over the background-filling)
	myBalken->setBrush(QBrush(Qt::black));
	QGSceneThermometer->addItem(myBalken);

	myAnim = new QPropertyAnimation(myBalken, "height");	// Animation (adjusting lowness of the emptiness according to incoming signal)
	myAnim->setDuration(1000);
	myAnim->setEasingCurve(QEasingCurve::OutCubic);

	QPen *myQPen1 = new QPen;
	myQPen1->setStyle( Qt::SolidLine );
	myQPen1->setWidth( 5 );
	myQPen1->setBrush( Qt::white );
	myQPen1->setCapStyle( Qt::RoundCap );

	QGraphicsLineItem *line01 = new QGraphicsLineItem(-3,-3,-3,553);
	line01->setPen( *myQPen1 );
	QGSceneThermometer->addItem( line01 );
	QGraphicsLineItem *line02 = new QGraphicsLineItem(-3,-3,153,-3);
	line02->setPen( *myQPen1 );
	QGSceneThermometer->addItem( line02 );
	QGraphicsLineItem *line03 = new QGraphicsLineItem(-3,553,153,553);
	line03->setPen( *myQPen1 );
	QGSceneThermometer->addItem( line03 );
	QGraphicsLineItem *line04 = new QGraphicsLineItem(153,-3,153,553);
	line04->setPen( *myQPen1 );
	QGSceneThermometer->addItem( line04 );

	QPen *myQPen2 = new QPen;
	myQPen2->setStyle( Qt::DotLine );	// WiP: Qt::SolidLine/Qt::DotLine
	myQPen2->setWidth( 3 );
	myQPen2->setBrush( Qt::lightGray );

	QGraphicsLineItem *line05 = new QGraphicsLineItem(1,164,149,164);
	line05->setPen( *myQPen2 );
	QGSceneThermometer->addItem( line05 );
	QGraphicsLineItem *line06 = new QGraphicsLineItem(1,330,149,330);
	line06->setPen( *myQPen2 );
	QGSceneThermometer->addItem( line06 );
	QGraphicsLineItem *line07 = new QGraphicsLineItem(1,496,149,496);
	line07->setPen( *myQPen2 );
	QGSceneThermometer->addItem( line07 );


	// QGSceneFace
	//Neutral.load(":/Neutral");
	//P2.load(":/P2");
	//P5.load(":/P5");
	//P8.load(":/P8");
	Neutral.load(":/SM0");
	P2.load(":/SM1");
	P5.load(":/SM2");
	P8.load(":/SM3");
	QGSceneFace->addPixmap(QPixmap::fromImage(Neutral));


	// QGSceneResults
	PosResults = new QGraphicsSimpleTextItem;
	PosResults->setBrush( QBrush(Qt::green) );
	PosResults->setFont( QFont("Helvetica [Cronyx]", 50) );
	PosResults->setText( QString("+") );

	NegResults = new QGraphicsSimpleTextItem;
	NegResults->setBrush( QBrush(Qt::red) );
	NegResults->setFont( QFont("Helvetica [Cronyx]", 50) );
	NegResults->setText( QString("+") );
}

FeedbackPresenter::~FeedbackPresenter()
{
}

int FeedbackPresenter::updateSignal(float fCurrPerc)
{
	float fLevel	= fCurrPerc/(*fMaxPerc);
	int iLevel		= 0;
	if( fLevel < 0.1 ) 
	{
		iLevel = 0;
	}
	else
	{
		if( fLevel < 0.4 )
		{
			iLevel = 1;
		}
		else
		{
			if( fLevel < 0.7 )
			{
				iLevel = 2;
			}
			else
			{
				iLevel = 3;
			}
		}
	}
	qvResults.append( iLevel==iCurrentTargetLevel );

	if( bFace )
	{
		switch( iLevel )
		{
			case 0:
				QGSceneFace->addPixmap(QPixmap::fromImage(Neutral));
				break;
			case 1:
				QGSceneFace->addPixmap(QPixmap::fromImage(P2));
				break;
			case 2:
				QGSceneFace->addPixmap(QPixmap::fromImage(P5));
				break;
			case 3:
				QGSceneFace->addPixmap(QPixmap::fromImage(P8));
				break;
		}
	}

	if( bThermometer )
	{
		fNewPercChange = fCurrPerc*(100/(*fMaxPerc))*5.5;	//maxPercent equals 550
		myAnim->setStartValue(550-fCurrPercChange);			//from
		myAnim->setEndValue(550-fNewPercChange);			//to
		myAnim->start();
		bRect->setBrush(QBrush(Qt::blue));
		fCurrPercChange = fNewPercChange; 
	}

	return iLevel;
}

void FeedbackPresenter::StartBaseline()
{
	myTaskLabel->setText( " " );

	myAnim->setStartValue(550-fCurrPercChange); //from
	myAnim->setEndValue(550); //to
	myAnim->start();
	fCurrPercChange = 0;	
	fNewPercChange	= 0;	
	bRect->setBrush(QBrush(Qt::black));

	myView->setScene(QGSceneBaseline);

	emit LogBaseline();

	bFace			= false;
	bThermometer	= false;
	bNoFeedback		= false;

	iCurrentTargetLevel	= 0;
}

void FeedbackPresenter::StartThermometer(int level)
{
	myTaskLabel->setText( QString::number(level) );
	myView->setScene(QGSceneThermometer);

	emit LogThermometer( level );

	bFace			= false;
	bThermometer	= true;
	bNoFeedback		= false;

	iCurrentTargetLevel	= level;
}
	
void FeedbackPresenter::StartFaces(int level)
{
	QGSceneFace->addPixmap(QPixmap::fromImage(Neutral));	// SB: Fehlerkorrektur1
	myView->setScene( QGSceneFace );

	myTaskLabel->setText( QString::number(level) );
	
	emit LogFace( level );

	bFace			= true;
	bThermometer	= false;
	bNoFeedback		= false;

	iCurrentTargetLevel	= level;
}
	
void FeedbackPresenter::StartNoFeedback(int level)
{
	myTaskLabel->setText( QString::number(level) );
	myView->setScene(QGSceneNoFeedback);

	emit LogNoFB( level );

	bFace			= false;
	bThermometer	= false;
	bNoFeedback		= true;

	iCurrentTargetLevel	= level;
}

void FeedbackPresenter::ShowResults()
{
	myTaskLabel->setText( " " );
	
	float fMeanResult = 0;
	for(int i=0; i<=qvResults.size(); i++)
	{
		fMeanResult += qvResults[i];
	}
	fMeanResult /= qvResults.size();
	int iMeanResult = qRound( fMeanResult );

	QGSceneResults->removeItem( NegResults );
	QGSceneResults->removeItem( PosResults );

	QGSceneResults->addItem( NegResults );

	if( iMeanResult )
		QGSceneResults->addItem( PosResults );

	myView->setScene(QGSceneResults);

	emit LogShowResults( iMeanResult );

	bFace			= false;
	bThermometer	= false;
	bNoFeedback		= false;

	qvResults.clear();
}

void FeedbackPresenter::StopPresentation()
{
	myTaskLabel->setText( " " );
	myView->setScene(QGSceneEmpty);

	bFace			= false;
	bThermometer	= false;
	bNoFeedback		= false;
}

void FeedbackPresenter::mouseDoubleClickEvent(QMouseEvent *e)
{
	if( isFullScreen() )
		setWindowState( Qt::WindowMaximized );
	else
		setWindowState( Qt::WindowFullScreen );
}

void FeedbackPresenter::incMaximum()
{
	*fMaxPerc += 0.1;
}

void FeedbackPresenter::decMaximum()
{
	*fMaxPerc -= 0.1;
}