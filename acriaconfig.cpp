#include "acriaconfig.h"
#include "ui_acriaconfig.h"
#include "configitem.h"

#include <QDebug>


AcriaConfig::AcriaConfig(QWidget *parent, Node* _node) :
    QDialog(parent),
    ui(new Ui::AcriaConfig)
{
    node = _node;

    ci = new ConfigItem(this);

    ui->setupUi(this);
}

AcriaConfig::~AcriaConfig()
{
    delete ui;
}

void AcriaConfig::on_buttonBox_accepted()
{
    qDebug() << "saving config";
    if(!node->createConfig(this->ui->plainTextEdit->toPlainText())){
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
        // update form from settings
    }

    this->cjson.push_back(this->ci->ijson);

    this->ui->plainTextEdit->setPlainText(QString::fromStdString(this->cjson.dump()));

    qDebug() << "added item";
}
