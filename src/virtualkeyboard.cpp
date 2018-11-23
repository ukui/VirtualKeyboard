#include "virtualkeyboard.h"
#include <QFile>
#include <QDesktopWidget>
#include <QHBoxLayout>
#include <QApplication>

VirtualKeyboard::VirtualKeyboard(QWidget *parent)
    : QWidget(parent)
{
    Q_INIT_RESOURCE(keyboard);

    setAutoFillBackground(true);
    QPalette plt;
    plt.setBrush(QPalette::Background, Qt::black);
    setPalette(plt);

    setWindowFlags(Qt::FramelessWindowHint |
                   Qt::WindowStaysOnTopHint |
                   Qt::WindowDoesNotAcceptFocus);


    keyboardWidget = new KeyboardWidget(this);
    QHBoxLayout *hl_keyboard = new QHBoxLayout(this);
    QSpacerItem *spacer = new QSpacerItem(40, 20);
    hl_keyboard->addSpacerItem(spacer);
    hl_keyboard->addWidget(keyboardWidget);
    hl_keyboard->addSpacerItem(spacer);

    QFile qssFile(":/qss/keyboard.qss");
    qssFile.open(QIODevice::ReadOnly);
    setStyleSheet(qssFile.readAll());
    qssFile.close();

    QDesktopWidget *desktop = QApplication::desktop();
    cursorMonitor = new CursorMonitor(this);

    //在多显示器情况下，监视鼠标指针的位置和主显示器变化信号
    connect(cursorMonitor, &CursorMonitor::cursorPosChanged,
            this, [&](const QPoint &pos){
        adjustGeometry(desktop->screenNumber(pos));
    });

    connect(desktop, &QDesktopWidget::primaryScreenChanged,
            this, [&]{
        adjustGeometry(desktop->primaryScreen());
    });

    connect(keyboardWidget, &KeyboardWidget::aboutToClose,
            this, &VirtualKeyboard::aboutToClose);

    adjustGeometry(desktop->primaryScreen());
}

void VirtualKeyboard::adjustGeometry(int screen)
{
    QDesktopWidget *desktop = QApplication::desktop();
    QWidget *activateScreen = desktop->screen(screen);
    setGeometry(0, activateScreen->height() - activateScreen->height() / 3,
                activateScreen->width(), activateScreen->height() / 3);
}
