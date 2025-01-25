#ifndef QCONSOLEIODEVICE_H
#define QCONSOLEIODEVICE_H

#include <QIODevice>
#include <QByteArray>

#if defined(QCONSOLE_LIBRARY)
#  define QCONSOLE_EXPORT Q_DECL_EXPORT
#else
#  define QCONSOLE_EXPORT Q_DECL_IMPORT
#endif

class QConsoleWidget;

class QCONSOLE_EXPORT QConsoleIODevice : public QIODevice
{

    Q_OBJECT

public:

     explicit QConsoleIODevice(QConsoleWidget* w, QObject *parent = nullptr);
    ~QConsoleIODevice();
    qint64 bytesAvailable() const override;
    bool waitForReadyRead(int msecs) override;
    QConsoleWidget* widget() const { return widget_; }

protected:

    qint64 readData(char *data, qint64 maxlen) override;
    qint64 writeData(const char *data, qint64 len) override;

private:

    friend class QConsoleWidget;
    QConsoleWidget* widget_;
    QByteArray readbuff_;
    int readpos_;
    qint64 writtenSinceLastEmit_, readSinceLastEmit_;
    bool readyReadEmmited_;
    void consoleWidgetInput(const QString& in);
};

#endif

