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

template <typename I> std::string n2hexstr(I w, size_t hex_len = sizeof(I)<<1) {
    static const char* digits = "0123456789ABCDEF";
    std::string rc(hex_len,'0');
    for (size_t i=0, j=(hex_len-1)*4 ; i<hex_len; ++i,j-=4)
        rc[i] = digits[(w>>j) & 0x0f];
    return rc;
}

static QString str2bytes32(QString d2){
    for(uint i = d2.size(); i<32;i++){
        d2 += " ";
    }

    QString s = "";
    for(int i = 0; i<d2.size(); i++){
        s += QString::fromStdString(n2hexstr(static_cast<uint>(QString(d2.at(i)).toStdString()[0]), 2));
    }

    return s;
}

#endif // UTIL_H
