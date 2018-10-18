#include "x11keyboard.h"
#include <QDebug>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/extensions/XTest.h>



struct CharMap {
    QChar    name;
    KeySym   code;
};

struct CharMap XSpecialSymbolMap[] {
    {' ',   XK_space},
    {',',   XK_comma},
    {'.',   XK_period},
    {'\'',  XK_quoteright},
    {'@',   XK_at},
    {'#',   XK_numbersign},
    {'$', XK_dollar},
    {'%',   XK_percent},
    {'&',   XK_ampersand},
    {'*',   XK_asterisk},
    {'(',   XK_parenleft},
    {')',   XK_parenright},
    {'-',   XK_minus},
    {'+',   XK_plus},
    {'!',   XK_exclam},
    {'"',   XK_quotedbl},
    {'<',   XK_less},
    {'>',   XK_greater},
    {':',   XK_colon},
    {';',   XK_semicolon},
    {'/',   XK_slash},
    {'?',   XK_question},
    {'=',   XK_equal},
    {'.',   XK_kana_middledot},
    {'~',   XK_asciitilde},
    {'`',   XK_grave},
    {'|',   XK_bar},
    {'^',   XK_asciicircum},
    {'{',   XK_braceleft},
    {'}',   XK_braceright},
    {'[',   XK_bracketleft},
    {']',   XK_bracketright},
    {'_',   XK_underscore},
    {'\\',  XK_backslash},
};

QMap<FuncKey, KeySym> funckeyMap = {
    {SPACE,     XK_space},
    {BACKSPACE, XK_BackSpace},
    {ENTER,     XK_Return},
    {HOME,      XK_Home},
    {END,       XK_End},
    {PGUP,      XK_Page_Up},
    {PGDN,      XK_Page_Down},
    {INSERT,    XK_Insert},
    {DELETE,    XK_Delete},
    {UP,        XK_Up},
    {DOWN,      XK_Down},
    {LEFT,      XK_Left},
    {RIGHT,     XK_Right}
};

QMap<Modifier, KeySym> modifierMap = {
    {CTRL,  XK_Control_L},
    {ALT,   XK_Alt_L},
    {SUPER, XK_Super_L},
    {SHIFT, XK_Shift_L}
};

QVector<QChar> shiftKeyVec = {'~', '!', '@', '#', '$', '%', '^', '&', '*',
                              '(', ')', '_', '+', '{', '}', '|', ':', '"',
                              '>', '?'};

static Display *display = XOpenDisplay(0);
bool isShift = false;
unsigned int keyCodeOfChar(QChar c)
{
    QString text(c);

    KeySym keysym = XStringToKeysym(text.toLocal8Bit().data());

    if(keysym == NoSymbol) {
        int symbolCount = sizeof(XSpecialSymbolMap) / sizeof(struct CharMap);
        for(int i = 0; i < symbolCount; i++) {
            if(XSpecialSymbolMap[i].name == c) {
                keysym = XSpecialSymbolMap[i].code;
                break;
            }
        }
    }
    qDebug() << "keysym: " << keysym;

    if(shiftKeyVec.contains(c) || (c >= 'A' && c <= 'Z'))
        isShift = true;
    else
        isShift = false;

    KeyCode code = XKeysymToKeycode(display, keysym);

    return code;
}

X11Keyboard::X11Keyboard(QObject *parent)
    : QObject(parent)
{

}

X11Keyboard::~X11Keyboard()
{
    XCloseDisplay(display);
}

void X11Keyboard::addModifier(Modifier mod)
{
    modList.push_back(mod);
}

void X11Keyboard::removeModifier(Modifier mod)
{
    modList.removeOne(mod);
}

void X11Keyboard::clearModifier()
{
    modList.clear();
}


void X11Keyboard::onKeyPressed(QChar c)
{
    unsigned int keyCode = keyCodeOfChar(c);
    sendKey(keyCode);
}

void X11Keyboard::onKeyPressed(FuncKey key)
{
    KeyCode keyCode;
    KeySym keysym = funckeyMap[key];

    if(keysym != NoSymbol)
        keyCode = XKeysymToKeycode(display, keysym);

    sendKey(keyCode);
}

void X11Keyboard::sendKey(unsigned int keyCode)
{
    Window focusWindow;
    int revert;
    XGetInputFocus(display, &focusWindow, &revert);

    for(auto mod : modList){
        KeyCode keyCode = XKeysymToKeycode(display, modifierMap[mod]);
        XTestFakeKeyEvent(display, keyCode, True, 2);
    }

    if(isShift)
        XTestFakeKeyEvent(display, XKeysymToKeycode(display, XK_Shift_L), True, 2);

    XTestFakeKeyEvent(display, keyCode, True, CurrentTime);
    XTestFakeKeyEvent(display, keyCode, False, CurrentTime);

    if(isShift)
        XTestFakeKeyEvent(display, XKeysymToKeycode(display, XK_Shift_L), False, 2);
    for(auto mod : modList){
        KeyCode keyCode = XKeysymToKeycode(display, modifierMap[mod]);
        XTestFakeKeyEvent(display, keyCode, False, 2);
    }

    XFlush(display);
}
