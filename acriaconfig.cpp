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

    this->update_table();
}

AcriaConfig::~AcriaConfig()
{
    delete ui;
}

void AcriaConfig::update_table()
{
    this->ui->tableWidget_config->setRowCount(cjson.size());

    this->ui->tableWidget_config->setColumnCount(2);

    this->ui->tableWidget_config->setHorizontalHeaderItem(0, new QTableWidgetItem("Name"));
    this->ui->tableWidget_config->setHorizontalHeaderItem(1, new QTableWidgetItem("Url"));

    try{
        for(unsigned i = 0; i<this->cjson.size(); i++){
            this->ui->tableWidget_config->setItem( i, 0, new QTableWidgetItem(QString::fromStdString(cjson[i]["rname"])));
            this->ui->tableWidget_config->setItem( i, 1, new QTableWidgetItem(QString::fromStdString(cjson[i]["url"])));
        }
    }
    catch(...){
        qDebug() << "Error config table";
    }

    this->ui->tableWidget_config->horizontalHeader()->setStretchLastSection(true);
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
        this->update_table();
    }

    qDebug() << "Added Config Item";
}

void AcriaConfig::on_tableWidget_config_cellDoubleClicked(int row, int column)
{
    Q_UNUSED(column)
    ci->fill(this->cjson[row]);

    if(ci->exec() == QDialog::Accepted){
        //this->cjson.push_back(this->ci->ijson);
        this->cjson[row] = this->ci->ijson;

        this->ui->plainTextEdit->setPlainText(QString::fromStdString(this->cjson.dump()));
        this->update_table();
    }

    qDebug() << "Added Config Item";
}
