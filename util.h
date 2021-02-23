#ifndef UTIL_H
#define UTIL_H

#include <QString>
#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>


static QJsonObject ObjectFromString(const QString& in)
{
    QJsonObject obj;

    QJsonDocument doc = QJsonDocument::fromJson(in.toUtf8());

    // check validity of the document
    if(!doc.isNull())
    {
        if(doc.isObject())
        {
            obj = doc.object();
        }
        else
        {
            qDebug() << "Document is not an object" << Qt::endl;
        }
    }
    else
    {
        qDebug() << "Invalid JSON...\n" << in << Qt::endl;
    }

    return obj;
}

static QString hex2str(QString r){
    QString tmp = "";

    for(int f = 1; f< r.length(); f+=2){
        tmp += QString(static_cast<char>((QString(r.at(f-1)) + QString(r.at(f))).toUInt(NULL,16)));
    }

    return tmp;
}

#endif // UTIL_H
