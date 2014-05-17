#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>

class AVConv;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    void updateSelections();
    ~MainWindow();

protected slots:
    void readListOutput();
    void refreshDevices();

private slots:
    void on_addStream_clicked();
    void on_startRecording_clicked();

protected:
    void updateSelections(QStringList disabledDevices);
    bool event(QEvent *ev);

private:
    QMetaObject::Connection processConnection;
    QHash<QString, QString> devices;
    QList<AVConv*> activeStreams;
    QList<AVConv*> streams;
    Ui::MainWindow *ui;
    QProcess process;
};

#endif // MAINWINDOW_H
