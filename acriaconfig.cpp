#include "acriaconfig.h"
#include "ui_acriaconfig.h"

#include <QDebug>


AcriaConfig::AcriaConfig(QWidget *parent, Node* _node) :
    QDialog(parent),
    ui(new Ui::AcriaConfig)
{
    node = _node;

    ui->setupUi(this);
}

AcriaConfig::~AcriaConfig()
{
    delete ui;
}

void AcriaConfig::on_buttonBox_accepted()
{
    if(!node->createConfig(ui->plainTextEdit->toPlainText())){
        qDebug() << "Error config";
    }

    this->hide();
}

void AcriaConfig::on_buttonBox_rejected()
{
    this->hide();
}
