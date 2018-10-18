#ifndef KEYBOARDWIDGET_H
#define KEYBOARDWIDGET_H

#include <QWidget>
#include <QMap>
#include <QTime>
#include "x11keyboard.h"

namespace Ui {
class KeyboardWidget;
}

class QPushButton;

class KeyboardWidget : public QWidget
{
    Q_OBJECT

public:
    explicit KeyboardWidget(QWidget *parent = 0);
    ~KeyboardWidget();

protected:
    void resizeEvent(QResizeEvent *event);

private:
    void bindSingal();
    void toggleCase();
    void switchPage();

private slots:
    void onButtonClicked();

signals:
    void keyPressed(QChar c);
    void keyPressed(FuncKey key);

private:
    Ui::KeyboardWidget      *ui;
    bool                    capsLock;           //是否大写锁定
    bool                    isShift;
    QTime                   shiftLastClicked;   //shift键上次被点击的时间
    int                     page;               //当前是第几页的键盘
    X11Keyboard             *vKeyboard;
};

#endif // KEYBOARDWIDGET_H
