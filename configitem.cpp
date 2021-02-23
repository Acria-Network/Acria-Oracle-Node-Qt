#include "configitem.h"
#include "ui_configitem.h"
#include "json.hpp"

ConfigItem::ConfigItem(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfigItem)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::Tool);
}

ConfigItem::~ConfigItem()
{
    delete ui;
}

void ConfigItem::on_buttonBox_1_accepted()
{
    nlohmann::json tmp;
    tmp["url"] = this->ui->lineEdit_2_api_url->text().toStdString();
    tmp["rname"] = this->ui->lineEdit_resource_name->text().toStdString();

    std::vector<QLineEdit*> t1;
    t1.push_back(this->ui->lineEdit_json1);
    t1.push_back(this->ui->lineEdit_json2);
    t1.push_back(this->ui->lineEdit_json3);
    t1.push_back(this->ui->lineEdit_json4);

    for(uint i = 0;i<t1.size();i++){
        if(t1[i]->text().trimmed() != "")
        tmp["json"][i] = t1[i]->text().toStdString();
    }

    ijson = tmp;
}

void ConfigItem::on_buttonBox_1_rejected()
{

}
