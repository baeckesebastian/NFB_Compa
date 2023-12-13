#ifndef BALKEN_H
#define BALKEN_H

#include <QGraphicsRectItem>



class Balken : public QObject, public QGraphicsRectItem
{
	Q_OBJECT //only Q_OBJECTs can be animated

    Q_PROPERTY(qreal height READ height WRITE resizeHeight DESIGNABLE false STORED false)

public:
    Balken(qreal x, qreal y, qreal width, qreal height, QObject *parent = 0);

	qreal height();
	void resizeHeight(qreal);
};
#endif // BALKEN_H