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
    this->progress = 0;
    this->enter_password_dialog = new EnterPasswordDialog();

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

    if(this->data->chain_data["ethereum"].wallet_path.length() > 4)
    this->ui->webEngineView->page()->runJavaScript("ethWalletPath = '" + this->data->chain_data["ethereum"].wallet_path + "';"
                                                   "if(ethWalletPath.length > 0){"
                                                   "document.getElementById('selectWalletEth').innerHTML = '...' + ethWalletPath.substr(ethWalletPath.length - 20);"
                                                   "document.getElementById(\"keystore_e\").className += ' keystore_available';"
                                                   "}");

    if(this->data->chain_data["cardano"].wallet_path.length() > 4)
    this->ui->webEngineView->page()->runJavaScript("carWalletPath = '" + this->data->chain_data["cardano"].wallet_path + "';"
                                                   "if(carWalletPath.length > 0){"
                                                   "document.getElementById('selectWalletCar').innerHTML = '...' + carWalletPath.substr(carWalletPath.length - 20);"
                                                   "document.getElementById(\"keystore_c\").className += ' keystore_available';"
                                                   "}");

    if(this->data->chain_data["polkadot"].wallet_path.length() > 4)
    this->ui->webEngineView->page()->runJavaScript("polWalletPath = '" + this->data->chain_data["polkadot"].wallet_path + "';"
                                                   "if(polWalletPath.length > 0){"
                                                   "document.getElementById('selectWalletPol').innerHTML = '...' + polWalletPath.substr(polWalletPath.length - 20);"
                                                   "document.getElementById(\"keystore_p\").className += ' keystore_available';"
                                                   "}");
}

AccountManager::~AccountManager()
{
    delete ui;
    delete enter_password_dialog;
}

void AccountManager::increment_progress(){
    QMutexLocker ml(&mutex);

    this->progress+=1;
    this->data->changed = true;

    if(this->progress == 12){
        this->progress = 0;
        this->hide();
    }
}

void AccountManager::on_buttonBox_accepted()
{
    this->ui->webEngineView->page()->runJavaScript("ethPrivateKey", [this](const QVariant &v) { this->data->chain_data["ethereum"].private_key = v.toString();this->data->changed = true;});
    this->ui->webEngineView->page()->runJavaScript("ethAddress", [this](const QVariant &v) { this->data->chain_data["ethereum"].account = v.toString();this->data->changed = true;});
    this->ui->webEngineView->page()->runJavaScript("ethWalletPath", [this](const QVariant &v) { this->data->chain_data["ethereum"].wallet_path = v.toString();this->data->changed = true;});

    this->ui->webEngineView->page()->runJavaScript("binPrivateKey", [this](const QVariant &v) { this->data->chain_data["binance"].private_key = v.toString();this->data->changed = true;});
    this->ui->webEngineView->page()->runJavaScript("binAddress", [this](const QVariant &v) { this->data->chain_data["binance"].account = v.toString();this->data->changed = true;});
    this->ui->webEngineView->page()->runJavaScript("binWalletPath", [this](const QVariant &v) { this->data->chain_data["binance"].wallet_path = v.toString();this->data->changed = true;});

    this->ui->webEngineView->page()->runJavaScript("carPrivateKey", [this](const QVariant &v) { this->data->chain_data["cardano"].private_key = v.toString();this->data->changed = true;});
    this->ui->webEngineView->page()->runJavaScript("carAddress", [this](const QVariant &v) { this->data->chain_data["cardano"].account = v.toString();this->data->changed = true;});
    this->ui->webEngineView->page()->runJavaScript("carWalletPath", [this](const QVariant &v) { this->data->chain_data["cardano"].wallet_path = v.toString();this->data->changed = true;});

    this->ui->webEngineView->page()->runJavaScript("polPrivateKey", [this](const QVariant &v) { this->data->chain_data["polkadot"].private_key = v.toString();this->data->changed = true;});
    this->ui->webEngineView->page()->runJavaScript("polAddress", [this](const QVariant &v) { this->data->chain_data["polkadot"].account = v.toString();this->data->changed = true;});
    this->ui->webEngineView->page()->runJavaScript("polWalletPath", [this](const QVariant &v) { this->data->chain_data["polkadot"].wallet_path = v.toString();this->data->changed = true;});

    delay();
}

void AccountManager::on_pushButton_continue_clicked()
{
    this->ui->webEngineView->page()->runJavaScript("ethPrivateKey", [this](const QVariant &v) { this->data->chain_data["ethereum"].private_key = v.toString();increment_progress();});
    this->ui->webEngineView->page()->runJavaScript("ethAddress", [this](const QVariant &v) { this->data->chain_data["ethereum"].account = v.toString();increment_progress();});
    this->ui->webEngineView->page()->runJavaScript("ethWalletPath", [this](const QVariant &v) { this->data->chain_data["ethereum"].wallet_path = v.toString();increment_progress();});

    this->ui->webEngineView->page()->runJavaScript("binPrivateKey", [this](const QVariant &v) { this->data->chain_data["binance"].private_key = v.toString();increment_progress();});
    this->ui->webEngineView->page()->runJavaScript("binAddress", [this](const QVariant &v) { this->data->chain_data["binance"].account = v.toString();increment_progress();});
    this->ui->webEngineView->page()->runJavaScript("binWalletPath", [this](const QVariant &v) { this->data->chain_data["binance"].wallet_path = v.toString();increment_progress();});

    this->ui->webEngineView->page()->runJavaScript("carPrivateKey", [this](const QVariant &v) { this->data->chain_data["cardano"].private_key = v.toString();increment_progress();});
    this->ui->webEngineView->page()->runJavaScript("carAddress", [this](const QVariant &v) { this->data->chain_data["cardano"].account = v.toString();increment_progress();});
    this->ui->webEngineView->page()->runJavaScript("carWalletPath", [this](const QVariant &v) { this->data->chain_data["cardano"].wallet_path = v.toString();increment_progress();});

    this->ui->webEngineView->page()->runJavaScript("polPrivateKey", [this](const QVariant &v) { this->data->chain_data["polkadot"].private_key = v.toString();increment_progress();});
    this->ui->webEngineView->page()->runJavaScript("polAddress", [this](const QVariant &v) { this->data->chain_data["polkadot"].account = v.toString();increment_progress();});
    this->ui->webEngineView->page()->runJavaScript("polWalletPath", [this](const QVariant &v) { this->data->chain_data["polkadot"].wallet_path = v.toString();increment_progress();});
}

void AccountManager::on_pushButton_skip_clicked()
{
    this->hide();
}
