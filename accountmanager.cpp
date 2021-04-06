#include "accountmanager.h"
#include "ui_accountmanager.h"

#include <QFileInfo>
#include <QWebChannel>

void delay()
{
    QTime dieTime= QTime::currentTime().addSecs(1);
    while (QTime::currentTime() < dieTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 2000);
}

AccountManager::AccountManager(QWidget *parent, Data* _data) :
    QDialog(parent),
    ui(new Ui::AccountManager)
{
    ui->setupUi(this);

    this->data = _data;

    QWebChannel * channel = new QWebChannel(this->ui->webEngineView->page());
    this->ui->webEngineView->page()->setWebChannel(channel);
    this->ui->webEngineView->page()->webChannel()->registerObject(QStringLiteral("AccountManager"), this);

    this->ui->webEngineView->page()->setBackgroundColor(Qt::transparent);
    this->ui->webEngineView->setUrl(QUrl::fromLocalFile(QFileInfo("js/index.html").absoluteFilePath()));
    this->ui->webEngineView->setZoomFactor(0.4);

    delay();
    if(this->data->chain_data["binance"].wallet_path.length() > 4)
    this->ui->webEngineView->page()->runJavaScript("binWalletPath = '" + this->data->chain_data["binance"].wallet_path + "';"
                                                   "if(binWalletPath.length > 0){"
                                                   "document.getElementById(\"selectWalletBin\").innerHTML = \"...\" + binWalletPath.substr(binWalletPath.length - 20);"
                                                   "document.getElementById(\"keystore_b\").className += ' keystore_available';"
                                                   "}");
    qDebug() << this->data->chain_data["ethereum"].wallet_path;
    if(this->data->chain_data["ethereum"].wallet_path.length() > 4)
    this->ui->webEngineView->page()->runJavaScript("ethWalletPath = '" + this->data->chain_data["ethereum"].wallet_path + "';"
                                                   "if(ethWalletPath.length > 0){"
                                                   "document.getElementById('selectWalletEth').innerHTML = '...' + ethWalletPath.substr(ethWalletPath.length - 20);"
                                                   "document.getElementById(\"keystore_e\").className += ' keystore_available';"
                                                   "}");


    /*
    QFile apiFile(":/qtwebchannel/qwebchannel.js"); //load the API from the resources
    if(!apiFile.open(QIODevice::ReadOnly))
        qDebug()<<"Couldn't load Qt's QWebChannel API!";
    QString apiScript = QString::fromLatin1(apiFile.readAll());
    qDebug() << apiScript;
    apiFile.close();
    this->ui->webEngineView->page()->runJavaScript(apiScript);

    std::ofstream webchannel ("./js/qwebchannel.js");

    if (webchannel.is_open())
    {
      webchannel << apiScript.toStdString();
      webchannel.close();
    }
    */


}

AccountManager::~AccountManager()
{
    delete ui;
}

void AccountManager::on_buttonBox_accepted()
{
    this->ui->webEngineView->page()->runJavaScript("ethPrivateKey", [this](const QVariant &v) { qDebug() << v.toString(); this->data->chain_data["ethereum"].private_key = v.toString();this->data->changed = true;});
    this->ui->webEngineView->page()->runJavaScript("ethAddress", [this](const QVariant &v) { qDebug() << v.toString(); this->data->chain_data["ethereum"].account = v.toString();this->data->changed = true;});
    this->ui->webEngineView->page()->runJavaScript("ethWalletPath", [this](const QVariant &v) { qDebug() << v.toString(); this->data->chain_data["ethereum"].wallet_path = v.toString();this->data->changed = true;});

    this->ui->webEngineView->page()->runJavaScript("binPrivateKey", [this](const QVariant &v) { qDebug() << v.toString(); this->data->chain_data["binance"].private_key = v.toString();this->data->changed = true;});
    this->ui->webEngineView->page()->runJavaScript("binAddress", [this](const QVariant &v) { qDebug() << v.toString(); this->data->chain_data["binance"].account = v.toString();this->data->changed = true;});
    this->ui->webEngineView->page()->runJavaScript("binWalletPath", [this](const QVariant &v) { qDebug() << v.toString(); this->data->chain_data["binance"].wallet_path = v.toString();this->data->changed = true;});

    delay();
}
