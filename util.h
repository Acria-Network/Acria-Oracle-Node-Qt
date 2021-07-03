#ifndef UTIL_H
#define UTIL_H

#include <QString>
#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QByteArray>
#include <QMessageBox>
#include <QPixmap>
#include <QIcon>
#include <QFile>
#include <fstream>


const std::string digits = "0123456789abcdef";

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

    static std::string tohex(std::string number) {                        // Decimal to Hexadecimal function
        long length = number.length();
        std::string result = "";
        std::vector<long> nibbles;
        for ( long i = 0; i < length; i++ ) {
            nibbles.push_back(digits.find(number[i]));
        }
        long newlen = 0;
        do {
            long value = 0;
            newlen = 0;
            for ( long i = 0; i < length; i++ ) {
                value = (value * 10) + nibbles[i];
                if (value >= 16) {
                    nibbles[newlen++] = value / 16;
                    value %= 16;
                } else if (newlen > 0) {
                    nibbles[newlen++] = 0;
                };
            };
            length = newlen;
            result = digits[value] + result;
        } while (newlen != 0);
        return result;
    }

    static __uint128_t toUint128(QString s){
        s.remove(0,2);
        unsigned ctr = 0;
        __uint128_t out = 0;
        for(int i = s.length()-1; i>=0; i--){
            __uint128_t add = QString(s.at(i)).toUInt(NULL, 16);
            for(unsigned i=0;i<ctr;i++){
                add*=16;
            }
            out+=add;
            ctr++;
        }

        return out;
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

    static QString str2bytes(QString d2){
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
        obj3.push_back("latest");

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

    static void show_msgBox(QString text){
        QMessageBox msgBox;
        msgBox.setStyleSheet("QWidget{background-color:#232323;color:white;}QPushButton{background-color:white;color:black;border: none;width:80px;padding-top:3px;padding-bottom:3px;background-color:orange;}");
        msgBox.setWindowIcon(QPixmap("resources/acria_logo5_colored.svg"));
        msgBox.setText(text);
        msgBox.exec();
    }

    static bool fileExists(QString path) {
        // check if file exists and if yes: Is it really a file and no directory?
        if (QFile::exists(path)) {
            return true;
        } else {
            return false;
        }
    }

    static QString read_file(std::string _path){
        std::ifstream t(_path);
        if(!t){
            qDebug() << "error file";
            return "";
        }
        std::string str;

        t.seekg(0, std::ios::end);
        str.reserve(t.tellg());
        t.seekg(0, std::ios::beg);

        str.assign((std::istreambuf_iterator<char>(t)),
                    std::istreambuf_iterator<char>());

        return QString::fromStdString(str);
    }

    static QString double2uint256(double d){
        QString tmp2 = QString::number(d, 'f', 10);
        int point = tmp2.indexOf('.');

        if(point != -1){
            tmp2 = tmp2.replace(".", "");
            for(uint i=tmp2.length()-point;i<18;i++){
                tmp2 += "0";
            }
        }
        else{
            for(uint i=0;i<18;i++){
                tmp2 += "0";
            }
        }
        return tmp2;
    }
};

#endif // UTIL_H
