#ifndef ACCOUNTMANAGER_H
#define ACCOUNTMANAGER_H

#include "data.h"
#include "enterpassworddialog.h"

#include <QDialog>
#include <QDebug>
#include <fstream>
#include <QDateTime>
#include <QDir>
#include <QFileDialog>
#include <QMainWindow>
#include <QMutex>


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

    Q_INVOKABLE QString get_password(){
        QString password;
        if(this->enter_password_dialog->exec() == QDialog::Accepted){
            password = this->enter_password_dialog->password;
        }

        return password;
    }

private:
    Ui::AccountManager *ui;

    Data* data;
    EnterPasswordDialog* enter_password_dialog;
    void increment_progress();
    QMutex mutex;
    unsigned progress;

public slots:


private slots:
    void on_buttonBox_accepted();
    void on_pushButton_continue_clicked();
    void on_pushButton_skip_clicked();
};

#endif // ACCOUNTMANAGER_H
