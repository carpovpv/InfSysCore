#include "style03.h"
#include <QPainter>
#include <QStyleOption>
#include <QComboBox>

Style03::Style03() : QPlastiqueStyle()
{

}

Style03::~Style03()
{

}

void Style03::drawPrimitive(PrimitiveElement element, const QStyleOption *option,
                        QPainter *painter, const QWidget *widget) const
{
    if (element == QStyle::PE_FrameFocusRect)
    {
        QWidget * wdg = const_cast<QWidget *> (widget);
        QComboBox *box = qobject_cast<QComboBox *>(wdg);
        if (box)
        {
            QPen pen(Qt::blue, 2, Qt::DotLine, Qt::RoundCap, Qt::RoundJoin);
            painter->setPen(pen);
            painter->drawRect(option->rect);
            return;
        }
    }
    return QPlastiqueStyle::drawPrimitive(element, option, painter, widget);
}
