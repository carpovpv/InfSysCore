#include "iisiconlabel.h"
#include "iistaskpanelscheme.h"

iisIconLabel::iisIconLabel(QAction *new_action, QWidget *parent)
        : QWidget(parent),
        myAction(new_action),
        mySchemePointer(0),
        m_over(false),
        m_pressed(false),
        m_changeCursorOver(true),
        m_underlineOver(true)
{
    if (!myAction)
        myAction = new QAction(this);

    init();
}

iisIconLabel::iisIconLabel(const QString &title, QWidget *parent)
        : QWidget(parent),
        mySchemePointer(0),
        m_over(false),
        m_pressed(false),
        m_changeCursorOver(true),
        m_underlineOver(true)
{
    myAction = new QAction(title, this);

    init();
}

iisIconLabel::iisIconLabel(const QIcon &icon, const QString &title, QWidget *parent)
        : QWidget(parent),
        mySchemePointer(0),
        m_over(false),
        m_pressed(false),
        m_changeCursorOver(true),
        m_underlineOver(true)
{
    myAction = new QAction(icon, title, this);

    init();
}

void iisIconLabel::init()
{
    setFocusPolicy(Qt::StrongFocus);

    myFont.setWeight(0);
    myFont.setBold(true);
    //myFont.setItalic(true);

    myPen.setStyle(Qt::NoPen);

    myColor = myColorOver = myColorDisabled = QColor();

    addAction(myAction);

    actionChanged();
}

iisIconLabel::~iisIconLabel()
{

}

void iisIconLabel::actionChanged()
{
    setEnabled(myAction->isEnabled());
    setVisible(myAction->isVisible());

    setToolTip(myAction->toolTip() == myAction->text() ? "" : myAction->toolTip());

    update();
}

void iisIconLabel::setSchemePointer(iisIconLabelScheme **pointer)
{
  mySchemePointer = pointer;
  update();
}

void iisIconLabel::setColors(const QColor &color, const QColor &colorOver, const QColor &colorOff)
{
  myColor = color;
  myColorOver = colorOver;
  myColorDisabled = colorOff;
  update();
}

void iisIconLabel::setFont(const QFont &font)
{
  myFont = font;
  update();
}

void iisIconLabel::setFocusPen(const QPen &pen)
{
  myPen = pen;
  update();
}

QSize iisIconLabel::sizeHint() const
{
  return minimumSize();
}

QSize iisIconLabel::minimumSizeHint() const
{
  int s = (mySchemePointer && *mySchemePointer) ? (*mySchemePointer)->iconSize : 16;
        QPixmap px = myAction->icon().pixmap(s, isEnabled() ? QIcon::Normal : QIcon::Disabled);

  int h = 4+px.height();
  int w = 8 + px.width();
        if (!myAction->text().isEmpty()) {
    QFontMetrics fm(myFont);
                w += fm.width(myAction->text());
    h = qMax(h, 4+fm.height());
  }

  return QSize(w+2,h+2);
}

void iisIconLabel::paintEvent ( QPaintEvent * event )
{
  QPainter p(this);

  QRect textRect(rect().adjusted(0,0,-1,0));

  int x = 2;

  int s = (mySchemePointer && *mySchemePointer) ? (*mySchemePointer)->iconSize : 16;
  QPixmap px = myAction->icon().pixmap(s, isEnabled() ? QIcon::Normal : QIcon::Disabled);
  if (!px.isNull()) {
    p.drawPixmap(x,0,px);
    x += px.width() + 4;
  }

  if (!myAction->text().isEmpty()) {
    QColor text = myColor, textOver = myColorOver, textOff = myColorDisabled;
    QFont fnt = myFont;
    QPen focusPen = myPen;
    bool underline = m_underlineOver, cursover = m_changeCursorOver;
    if (mySchemePointer && *mySchemePointer) {
      if (!text.isValid()) text = (*mySchemePointer)->text;
      if (!textOver.isValid()) textOver = (*mySchemePointer)->textOver;
      if (!textOff.isValid()) textOff = (*mySchemePointer)->textOff;
      if (!fnt.weight()) fnt = (*mySchemePointer)->font;
      if (focusPen.style() == Qt::NoPen) focusPen = (*mySchemePointer)->focusPen;
      underline = (*mySchemePointer)->underlineOver;
      cursover = (*mySchemePointer)->cursorOver;
    }

    p.setPen(isEnabled() ? m_over ? textOver : text : textOff);

    if (isEnabled() && underline && m_over)
      fnt.setUnderline(true);
    p.setFont(fnt);

    textRect.setLeft(x);
    QRect boundingRect;

    QFontMetrics fm(fnt);
                QString txt(fm.elidedText(myAction->text(), Qt::ElideRight, textRect.width()));

    p.drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, txt, &boundingRect);

    if (hasFocus()) {
      p.setPen(focusPen);
      p.drawRect(boundingRect.adjusted(-2,-1,0,0));
    }
  }
}


void iisIconLabel::enterEvent ( QEvent * /*event*/ )
{
  m_over = true;

  if (m_changeCursorOver)
    QApplication::setOverrideCursor(Qt::PointingHandCursor);

        myAction->hover();

  update();
}

void iisIconLabel::leaveEvent ( QEvent * /*event*/ )
{
  m_over = false;
  update();

  if (m_changeCursorOver)
    QApplication::restoreOverrideCursor();
}

void iisIconLabel::mousePressEvent ( QMouseEvent * event )
{
  if (event->button() == Qt::LeftButton) {
    m_pressed = true;
    emit pressed();
  } else
    if (event->button() == Qt::RightButton)
      emit contextMenu();

  update();
}

void iisIconLabel::mouseReleaseEvent ( QMouseEvent * event )
{
  if (event->button() == Qt::LeftButton) {
    m_pressed = false;
    emit released();

    if (rect().contains( event->pos() )) {
      emit clicked();
      emit activated();
                        myAction->activate(QAction::Trigger);
    }
  }

  update();
}

void iisIconLabel::keyPressEvent ( QKeyEvent * event )
{
  switch (event->key()) {
    case Qt::Key_Space:
    case Qt::Key_Return:
      emit activated();
                        myAction->activate(QAction::Trigger);
      break;

    default:;
  }

  QWidget::keyPressEvent(event);
}

void iisIconLabel::actionEvent ( QActionEvent * event )
{
    if (event->type() == QEvent::ActionChanged)
        actionChanged();

    QWidget::actionEvent(event);
}

