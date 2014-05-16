#ifndef AVCONV_H
#define AVCONV_H

#include <QProcess>
#include <QTemporaryFile>

class AVConv : public QProcess
{
    Q_OBJECT
public:
    explicit AVConv(QString alsaDevice, QObject *parent = 0);

    inline QString outputPath() {
        return tempFile.fileName();
    }

signals:
    void finished(AVConv*, int);
    void started(AVConv*);

private slots:
    void handleFinished(int);
    void handleStarted();

private:
    QTemporaryFile tempFile;
};

#endif // AVCONV_H
