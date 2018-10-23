#ifndef CURSORMONITOR_H
#define CURSORMONITOR_H

#include <QObject>
#include <QThread>

class CursorMonitor : public QThread
{
    Q_OBJECT
public:
    explicit CursorMonitor(QObject *parent = nullptr);
    void run();

Q_SIGNALS:
    void cursorPosChanged(const QPoint& pos);
};

#endif // CURSORMONITOR_H
