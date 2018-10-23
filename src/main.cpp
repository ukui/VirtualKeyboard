#include "virtualkeyboard.h"
#include <QApplication>

int main(int argc, char *argv[])
{
//    qputenv("QT_IM_MODULE", QByteArray("qtvirtualkeyboard"));
    QApplication a(argc, argv);
    VirtualKeyboard *keyboard = new VirtualKeyboard;
    QObject::connect(keyboard, &VirtualKeyboard::aboutToClose, &a, &QApplication::quit);
    keyboard->show();

    return a.exec();
}
