#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QLabel>
#include <QStandardItemModel>
#include <QListWidgetItem>
#include <QComboBox>
#include <QVariant>
#include <QRegExp>
#include <QDebug>
#include <QHash>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Fixed);
    ui->tableWidget->setColumnWidth(1, 80);

    process.start("pactl list sources");
    connect(&process, SIGNAL(finished(int)), this, SLOT(readListOutput()));
}

#ifdef Q_OS_LINUX
void MainWindow::readListOutput() {
    static QRegExp nameRegExp("\\sName: (alsa.+)$");
    static QRegExp deviceDescRegExp("^\\s+device\\.description = \"(.+)\"\\s*$");
    if(!deviceDescRegExp.isValid())
        qWarning() << deviceDescRegExp.errorString();

    QString alsaDeviceName;
    //QDebug debug(QtDebugMsg);
    while(process.canReadLine()) {
        QByteArray line = process.readLine();
        if(nameRegExp.exactMatch(line)) {
            alsaDeviceName = nameRegExp.cap(1).trimmed();
            qDebug() << "device found" << alsaDeviceName;
        } else if(!alsaDeviceName.isEmpty() &&
                  deviceDescRegExp.exactMatch(line)) {
            QString deviceName = deviceDescRegExp.cap(1).trimmed();
            qDebug() << "device name found" << deviceName;

            devices.insert(alsaDeviceName, deviceName);
            alsaDeviceName.clear();
        }/* else
            debug << line;*/
    }
    ui->addStream->setEnabled(true);
    ui->tableWidget->setEnabled(true);
    ui->startRecording->setEnabled(true);
    ui->status->setText("Ready");
    ui->progressBar->hide();
    on_addStream_clicked();
}
#else
#error Only supported on Linux with PulseAudio at this time.
#endif

void MainWindow::updateSelections() {
    QStringList selectedDevices;
    for(int i=0; i<ui->tableWidget->rowCount(); i++) {
        QComboBox* rowCombo = (QComboBox*)ui->tableWidget->cellWidget(i, 0);
        selectedDevices << rowCombo->currentData().toString();
    }
    qDebug() << "Updating Selections" << selectedDevices;
    for(int i=0; i<ui->tableWidget->rowCount(); i++) {
        QComboBox* rowCombo = (QComboBox*)ui->tableWidget->cellWidget(i, 0);
        const QStandardItemModel* model = qobject_cast<const QStandardItemModel*>(rowCombo->model());
        for(int r=0; r<rowCombo->count(); r++) {
            if(r == rowCombo->currentIndex())
                continue;

            QVariant value;
            QStandardItem* item = model->item(r);
            if(selectedDevices.contains(item->data(Qt::UserRole).toString()))
                value = 0;
            else {
                qDebug() << "Enabling" << item->text();
                value = 1 | 32;
            }
            item->setData(value, Qt::UserRole - 1);
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_addStream_clicked()
{
    int rowID = ui->tableWidget->rowCount();
    ui->tableWidget->insertRow(rowID);

    QComboBox* sinkSelection = new QComboBox();
    QPushButton* removeStream = new QPushButton("Remove");
    if(rowID == 0)
        removeStream->setEnabled(false);
    else if(rowID == 1)
        ui->tableWidget->cellWidget(0, 1)->setEnabled(true);
    connect(removeStream, &QPushButton::clicked, [=] () {
        for(int i=0; i<ui->tableWidget->rowCount(); i++) {
            if(ui->tableWidget->cellWidget(i, 0) == sinkSelection) {
                ui->tableWidget->removeRow(i);
                break;
            }
        }
        ui->addStream->setEnabled(true);
        if(ui->tableWidget->rowCount() == 1) {
            ui->tableWidget->cellWidget(0, 1)->setEnabled(false);
            QComboBox* rowCombo = (QComboBox*)ui->tableWidget->cellWidget(0, 0);
            const QStandardItemModel* model = qobject_cast<const QStandardItemModel*>(rowCombo->model());
            for(int r=0; r<rowCombo->count(); r++) {
                if(r == rowCombo->currentIndex())
                    continue;

                QStandardItem* item = model->item(r);
                item->setData(1 | 32, Qt::UserRole - 1);
            }
        } else
            updateSelections();
    });
    int defaultItem = 0;
    bool lastWasDisabled = true;
    QHashIterator<QString, QString> it(devices);
    while(it.hasNext()) {
        it.next();
        bool disabled = false;
        sinkSelection->addItem(it.value(), it.key());
        for(int i=0; i<rowID; i++) {
            QComboBox* rowCombo = (QComboBox*)ui->tableWidget->cellWidget(i, 0);
            if(rowCombo->currentData() == it.key()) {
                disabled = true;
                break;
            }
        }
        if(lastWasDisabled) {
            if(disabled)
                defaultItem ++;
            else
                lastWasDisabled = false;
        }
    }
    if(defaultItem > 0)
        sinkSelection->setCurrentIndex(defaultItem);
    if(rowID == devices.size()-1)
        ui->addStream->setEnabled(false);
    ui->tableWidget->setCellWidget(rowID, 0, sinkSelection);
    ui->tableWidget->setCellWidget(rowID, 1, removeStream);
    updateSelections();

    connect(sinkSelection, &QComboBox::currentTextChanged, [=] (QString) {
        updateSelections();
    });
}

void MainWindow::on_startRecording_clicked() {
    ui->addStream->setEnabled(false);
    ui->tableWidget->setEnabled(false);

    ui->status->setText("Opening Streams...");
    ui->startRecording->setText("Cancel");
    ui->progressBar->setMaximum(0);
    ui->progressBar->show();
}
