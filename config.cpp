#include "config.h"

#include <QString>
#include <QFile>
#include <QDebug>


Config::Config()
{
    config_location = "./config.conf";
}

bool Config::createConfig(QString conf){
    QString filename=this->config_location;
    QFile file(filename);

    if(file.exists()){
        qDebug() << "Config File exists: "<<filename;
    }else{
        qDebug() << filename<<" does not exist";
    }

   if (file.open(QIODevice::WriteOnly | QIODevice::Text)){
        QTextStream out(&file);
        out << conf;
        file.close();
   }

   return this->parseConfig();
}

QString Config::loadConfig(){
    QString filename=this->config_location;
    QFile file(filename);

    if(file.exists()){
        qDebug() << "Config File exists: "<<filename;
    }else{
        qDebug() << filename<<" does not exist";
    }

    QString line;

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)){
        QTextStream stream(&file);
        while (!stream.atEnd()){
            line += stream.readLine();
            qDebug() << "line: "<<line;
        }
    }
    file.close();

    return line;
}

bool Config::parseConfig(){
    this->config = nlohmann::json::parse(this->loadConfig().toStdString());

    return true;
}
