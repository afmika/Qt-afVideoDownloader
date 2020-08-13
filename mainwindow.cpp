#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include "WorkerThread.h"
#include "DonwloadProcess.h"
#include <QTime>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    
    initAll();
    
    connect(ui->download_btn, SIGNAL(clicked()), SLOT(onDownload()));
    connect(ui->stop_btn, SIGNAL(clicked()), SLOT(onStopDownload()));
    connect(ui->clear_logs_btn, SIGNAL(clicked()), SLOT(onClearLogs()));
    
    setFixedSize(size()); // fixed size
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::logs(QString text) {
    if ( !text.trimmed().isEmpty() ) {
        QTime current_time = QTime::currentTime();
        QString before = ui->event_logs->toPlainText();
        text = QString("%1 : %2").arg(current_time.toString(), text);
        ui->event_logs->setPlainText(text + "\n" + before);   
    }
}

void MainWindow::resetDownloaderProcess() {
    if ( worker != nullptr) {
        worker->downloader->kill();
        worker->terminate();
    }
}
void MainWindow::initAll() {
    ui->progress_bar->setRange(0, 100);
    ui->progress_bar->setValue(0);
    ui->status_label->clear();
    resetDownloaderProcess();
}

void MainWindow::onDownload() {
    
    initAll();
    
    worker = new WorkerThread(this);
    QString url = ui->url_input->text();
    qDebug() << "Fetching " << url;
    logs("Fetching " + url);
    QProcess* downloader = Downloader::startDownload(url);
    worker->defineDownloader(downloader);
    
    worker->start();
    
    connect(worker, SIGNAL(stdOutSignal(const QString&)), SLOT(onInfos(const QString&)));
    connect(worker, SIGNAL(stdErrorSignal(const QString&)), SLOT(onInfos(const QString&)));
    connect(downloader, SIGNAL(finished(int, QProcess::ExitStatus)), SLOT(onDownloadExit()));
    
}

void MainWindow::onStopDownload() {
    initAll();
    logs("Download stopped");    
}


void MainWindow::onInfos(const QString& s) {
    // qDebug() << s;
    logs(s);
    DownloadInfos downinfos;
    downinfos.loadFromString(s);
    int progress = static_cast<int>(downinfos.progress);
    ui->progress_bar->setValue(progress);
    ui->status_label->setText(downinfos.resume_infos);
}

void MainWindow::onDownloadExit() {
    logs("Process exited");
    qDebug() << "Process exited";    
    initAll();
}

void MainWindow::onClearLogs () {
    ui->event_logs->clear();    
}


void MainWindow::closeEvent(QCloseEvent* event) {
    // stops all threads and close all subprocess
    qDebug() << event;
    resetDownloaderProcess();
}
