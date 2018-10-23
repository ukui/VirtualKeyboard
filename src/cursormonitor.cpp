#include "cursormonitor.h"
#include <X11/Xlib.h>
#include <QPoint>

CursorMonitor::CursorMonitor(QObject *parent) : QThread(parent)
{

}

void CursorMonitor::run()
{
    Display *display;
    XEvent xevent;
    Window window;

    display = XOpenDisplay(NULL);


    window = DefaultRootWindow(display);
    XAllowEvents(display, AsyncBoth, CurrentTime);

    XGrabPointer(display,
                 window,
                 1,
                 PointerMotionMask | ButtonPressMask | ButtonReleaseMask ,
                 GrabModeAsync,
                 GrabModeAsync,
                 None,
                 None,
                 CurrentTime);

    while(1) {
        XNextEvent(display, &xevent);

        switch (xevent.type) {
            case MotionNotify:
            //printf("Mouse move      : [%d, %d]\n", xevent.xmotion.x_root, xevent.xmotion.y_root);
                Q_EMIT cursorPosChanged(QPoint(xevent.xmotion.x_root, xevent.xmotion.y_root));
            break;
            case ButtonPress:
//              printf("Button pressed  : %s\n", key_name[xevent.xbutton.button - 1]);
            break;
            case ButtonRelease:
//              printf("Button released : %s\n", key_name[xevent.xbutton.button - 1]);
            break;
        }
    }
}
