#include "avconv.h"

// avconv -f pulse -i default -acodec flac -y test.flac
AVConv::AVConv(QString alsaDevice, QObject *parent) :
    QProcess(parent),
    tempFile(alsaDevice)
{
    tempFile.setAutoRemove(true);
    connect(this, SIGNAL(finished(int)), this, SLOT(handleFinished(int)));
    connect(this, SIGNAL(started()), this, SLOT(handleStarted()));

    setProgram("avconv");
#ifdef Q_OS_LINUX
    setArguments(QStringList() << "-f" << "pulse" << "-i" << alsaDevice << "-acodec" << "flac" << "-y" << "test.flac");
#else
#error Only supported on Linux with PulseAudio at this time.
#endif
}

void AVConv::handleStarted() {
    emit started(this);
}

void AVConv::handleFinished(int code) {
    emit finished(this, code);
}
