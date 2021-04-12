#include "acriaconfig.h"
#include "ui_acriaconfig.h"
#include "configitem.h"

#include <QDebug>


AcriaConfig::AcriaConfig(QWidget *parent, Config* _config) :
    QDialog(parent),
    ui(new Ui::AcriaConfig)
{
    config = _config;

    ci = new ConfigItem(this);

    this->cjson = config->get_config();

    ui->setupUi(this);

    this->ui->plainTextEdit->setPlainText(QString::fromStdString(this->cjson.dump()));
}

AcriaConfig::~AcriaConfig()
{
    delete ui;
}

void AcriaConfig::on_buttonBox_accepted()
{
    qDebug() << "saving config";
    if(!config->createConfig(this->ui->plainTextEdit->toPlainText())){
        qDebug() << "Error config";
    }

    this->hide();
}

void AcriaConfig::on_buttonBox_rejected()
{
    this->hide();
}

void AcriaConfig::on_pushButton_add_item_clicked()
{
    if(ci->exec() == QDialog::Accepted){
        this->cjson.push_back(this->ci->ijson);

        this->ui->plainTextEdit->setPlainText(QString::fromStdString(this->cjson.dump()));
    }

    qDebug() << "Added Config Item";
}
