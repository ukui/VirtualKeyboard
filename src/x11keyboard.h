#ifndef X11KEYBOARD_H
#define X11KEYBOARD_H

#include <QObject>

enum Modifier {
    CTRL,
    SHIFT,
    ALT,
    SUPER,
    NONE
};

enum FuncKey{
    SPACE = 0,
    BACKSPACE,
    ENTER,
    HOME,
    END,
    PGUP,
    PGDN,
    INSERT,
    DELETE,
    UP,
    DOWN,
    LEFT,
    RIGHT
};

class X11Keyboard : public QObject
{
    Q_OBJECT
public:
    explicit X11Keyboard(QObject *parent = nullptr);
    ~X11Keyboard();
    void addModifier(Modifier mod);
    void removeModifier(Modifier mod);
    void clearModifier();

signals:

public slots:
    void onKeyPressed(QChar c);
    void onKeyPressed(FuncKey key);

private:
    void sendKey(unsigned int keyCode);

private:
    QList<Modifier> modList;
};

#endif // X11KEYBOARD_H
