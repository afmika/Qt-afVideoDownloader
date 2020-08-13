#ifndef DONWLOADPROCESS_H
#define DONWLOADPROCESS_H

#include <QString>
#include <QProcess>
#include <QThread>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

namespace Downloader {
    const QString config_path = "./afdownloader.conf.json";
    
    QJsonDocument getDefaultConfig() {
        QJsonObject object;
        object["binary"] = "./bin/youtube-dl.exe";
        object["download_folder"] = "./downloads/%(title)s-%(id)s.%(ext)s";
        object["args"] = "[NONE]";
        QJsonDocument doc(object);
        return doc;
    }
    
    QJsonDocument getConfig() {
        QJsonDocument config = getDefaultConfig();
        QFile file(config_path);
        if ( file.open(QFile::ReadOnly) ) {
            // success
            QByteArray content = file.readAll();
            // qDebug() << content;
            config = QJsonDocument::fromJson(content);
        } else {
            if ( file.open(QFile::WriteOnly) ) {
                file.write(config.toJson());
                qDebug() << QString("%1 created").arg(config_path);                               
            } else {
                // fail
                qDebug() << QString("Couldn't create %1").arg(config_path);               
            }
        }
        qDebug() << "Binary " << config["binary"];
        qDebug() << "Download Folder" << config["download_folder"];
        qDebug() << "ARGS " << config["args"];
        
        return config;
    }
    
    QProcess* startDownload(QString url) {
        QJsonDocument config = getConfig();
        QString program = config["binary"].toString();
        QStringList arguments;
        arguments << "--newline";
        arguments << url;
        arguments << "-o";
        arguments << config["download_folder"].toString();
  
        QProcess *downloader = new QProcess;
        downloader->start(program, arguments);
        
        QThread *worker = new QThread;
        worker->start();
        
        return downloader;
    }
    
};
#endif // DONWLOADPROCESS_H
