#include "keyboardwidget.h"
#include <QApplication>


int main(int argc, char *argv[])
{
    qputenv("QT_IM_MODULE", QByteArray("qtvirtualkeyboard"));
    QApplication a(argc, argv);
    KeyboardWidget w;
    w.show();

    return a.exec();
}
