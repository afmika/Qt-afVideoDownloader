#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "WorkerThread.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    WorkerThread *worker = nullptr;
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    
    void initAll();
    void logs(QString text);
    void resetDownloaderProcess();
    void closeEvent(QCloseEvent *event);
public slots:
    void onInfos(const QString &s);    
    void onDownload();
    void onStopDownload();
    void onDownloadExit();
    void onClearLogs();
    
private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
