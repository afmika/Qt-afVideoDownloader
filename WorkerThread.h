#ifndef WORKERTHREAD_H
#define WORKERTHREAD_H
#include <QThread>
#include <QProcess>
#include <QDebug>

struct DownloadInfos {
    QString infos;
    QString timeleft;
    QString resume_infos;
    float progress;
    DownloadInfos() {
        infos = "";
        progress = 0;
        timeleft = "";
        resume_infos = ""; 
    }
    void loadFromString(QString infos) {
        QStringList datas = infos.split(" ");
        QStringList values;
        for (auto part : datas) {
            if ( !part.trimmed().isEmpty() ) {
                values.push_back(part);
            } 
        }
        // [download]   0.4% of 85.79MiB at 14.62KiB/s ETA 01:39:42        
        if ( values.size() == 8 ) {
            // qDebug() << "Size  == 8 ";
            progress = values[1].split("%")[0].toFloat();
            timeleft += values[values.size() - 1];                

            // size   speed
            infos = QString("%1    %2").arg(values[3], values[5]);
            resume_infos = QString("%1     Time left %2").arg(infos, timeleft);
            qDebug() << resume_infos;            
        }
    }
};

class WorkerThread : public QThread {
    Q_OBJECT
public:
    WorkerThread(QObject* parent) : QThread(parent) {
        
    }
    
    QProcess *downloader;
    void defineDownloader(QProcess *downloader) {
        this->downloader = downloader;
    }
public slots:
    void onStateChange() {
        QString infos = downloader->readAllStandardOutput();
        emit stdOutSignal(infos);
    }
private:
    QString fetchValuableInfosFrom (QStringList list) {
        QString infos = "";
        for (auto v : list) {
            if ( !v.isEmpty() ) {
                infos = v;
            }
        }
        return infos;
    }
    
    void run() override {
        while ( true ) {
            QString errors = downloader->readAllStandardError();
            QString infos = downloader->readAllStandardOutput();
            
            if ( !errors.isEmpty() ) {
                QStringList temp = errors.split("\n");
                // qDebug() << "Error " << temp.size();                
                QString datas = fetchValuableInfosFrom(temp);
                emit stdErrorSignal(datas);                        
            }
            if ( !infos.isEmpty() ) {
                QStringList temp = infos.split("\n");
                // qDebug() << temp.size();
                QString datas = fetchValuableInfosFrom(temp);
                emit stdOutSignal(datas);                
            }
            
            msleep(500); // msec
        }
        
    }
signals:
    // custom signals, (yup there is no overriding here)
    void stdErrorSignal(const QString &s);
    void stdOutSignal(const QString &s);
};

#endif // WORKERTHREAD_H
