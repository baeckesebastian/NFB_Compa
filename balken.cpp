#include "balken.h"



Balken::Balken(qreal x, qreal y, qreal width, qreal height, QObject *parent) : QObject(parent), QGraphicsRectItem(x,y,width,height)
{
}

qreal Balken::height()
{
	return QGraphicsRectItem::rect().height();
}

void Balken::resizeHeight(qreal height)
{
	QGraphicsRectItem::setRect(Balken::rect().x(),Balken::rect().y(),Balken::rect().width(),height);
}