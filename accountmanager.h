#ifndef ACCOUNTMANAGER_H
#define ACCOUNTMANAGER_H

#include "data.h"

#include <QDialog>
#include <QDebug>
#include <fstream>
#include <QDateTime>
#include <QDir>
#include <QFileDialog>
#include <QMainWindow>


namespace Ui {
class AccountManager;
}

class AccountManager : public QDialog
{
    Q_OBJECT

public:
    explicit AccountManager(QWidget *parent = nullptr, Data* _data = nullptr);
    ~AccountManager();

    Q_INVOKABLE QString save_wallet(QString wallet, QString type, QString address){
        QString date = QDateTime::currentDateTime().toString();

        if(!QDir("./keystore").exists())
            QDir().mkdir("./keystore");
        if(!QDir("./keystore/"+type).exists())
            QDir().mkdir("./keystore/"+type);

        std::string location = "./keystore/" + type.toStdString() + "/" + date.toStdString() + "--" + address.toStdString();
        std::ofstream wallet_file(location);

        if (wallet_file.is_open())
        {
          wallet_file << wallet.toStdString();
          wallet_file.close();
        }
        else{
            qDebug() << "Unable to save wallet";
        }

        qDebug() << "Saved Wallet: " << type << ": " << wallet;

        return QString::fromStdString(location);
    }
    //Q_PROPERTY(QString stringValue READ save_wallet)
    Q_INVOKABLE QString read_keystore(QString path){
        qDebug() << "Read Keystore: " << path;
        std::ifstream t(path.toStdString());
        if(!t){
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

    Q_INVOKABLE QString select_file(){
        QString dir = QFileDialog::getOpenFileName(this, tr("Open Keystore File"),
                                                     "./keystore");

        return dir;
    }

private:
    Ui::AccountManager *ui;

    Data* data;

public slots:


private slots:
    void on_buttonBox_accepted();
};

#endif // ACCOUNTMANAGER_H
