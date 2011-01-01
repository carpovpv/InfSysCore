#ifndef STYLE03_H
#define STYLE03_H

#include <QPlastiqueStyle>

class Style03 : public QPlastiqueStyle
{
public:
    Style03();
    ~Style03();

    void drawPrimitive(PrimitiveElement element, const QStyleOption *option,
                            QPainter *painter, const QWidget *widget) const;
};

#endif // STYLE03_H
