#ifndef UTIL_H
#define UTIL_H

#include <QString>
#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QByteArray>

class Util{
public:
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
            uint char0 = (QString(r.at(f-1)) + QString(r.at(f))).toUInt(NULL,16);
            if(char0 != 0)
                tmp += QString(static_cast<char>(char0));
        }

        return tmp;
    }

    template <typename I> static std::string n2hexstr(I w, size_t hex_len = sizeof(I)<<1) {
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

    static QString str2bytes8(QString d2){
        /*for(uint i = d2.size(); i<8;i++){
            d2 += "\0";
        }*/

        QString s = "";
        for(int i = 0; i<d2.size(); i++){
            s += QString::fromStdString(n2hexstr(static_cast<uint>(QString(d2.at(i)).toStdString()[0]), 2));
        }

        for(uint i = s.size(); i<16;i++){
            s += "0";
        }

        return s;
    }

    static QByteArray generate_rpc_call(QString _method, QString _from, QString _to, QString _data, unsigned long long _transaction_fee, unsigned long long _gas, QJsonValue _id, int nonce){
        QJsonObject obj1;

        if(_from != "")
            obj1["from"] = _from;
        if(_to != "")
            obj1["to"] = _to;
        if(_data != "")
            obj1["data"] = _data;
        if(_transaction_fee != 0)
            obj1["gasPrice"] = QString::fromStdString("0x" + n2hexstr(_transaction_fee));
        if(_gas != 0)
            obj1["gas"] = QString::fromStdString("0x" + n2hexstr(_gas));

        Q_UNUSED(nonce);
        /**
         -- disabled nonce --
        if(nonce != -1)
            obj1["nonce"] = QString::fromStdString("0x" + n2hexstr(nonce));
        **/

        QJsonArray obj3;
        obj3.push_back(obj1);

        QJsonObject obj;
        obj["jsonrpc"] = "2.0";
        obj["method"] = _method;
        if(!(_from == "" && _to == "" && _data == "" && _transaction_fee == 0))
            obj["params"] = obj3;
        obj["id"] = _id;
        QJsonDocument doc(obj);
        QByteArray data = doc.toJson();

        return data;
    }
};

#endif // UTIL_H
