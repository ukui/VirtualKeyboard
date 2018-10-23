#ifndef VIRTUALKEYBOARD_H
#define VIRTUALKEYBOARD_H

#include <QWidget>
#include <QPushButton>

#include "keyboardwidget.h"
#include "cursormonitor.h"

class VirtualKeyboard : public QWidget
{
    Q_OBJECT
public:
    explicit VirtualKeyboard(QWidget *parent = 0);

private:
    void adjustGeometry(int screen);

Q_SIGNALS:
    void aboutToClose();

private:
    KeyboardWidget      *keyboardWidget;
    CursorMonitor       *cursorMonitor;
    bool                isApplication;
};


#endif // VIRTUALKEYBOARD_H
