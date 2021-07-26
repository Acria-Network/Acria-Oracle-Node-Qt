#include "infogeth.h"
#include "ui_infogeth.h"

InfoGeth::InfoGeth(QWidget *parent, Node* _node, Data *_data, QString _type) :
    QDialog(parent),
    ui(new Ui::InfoGeth)
{
    ui->setupUi(this);

    this->node = _node;
    this->data = _data;

    this->update_info();

    this->type = _type;
}

InfoGeth::~InfoGeth()
{
    delete ui;
}

void InfoGeth::update_info(){
    this->ui->label_version->setText(node->get_geth_version());
    this->ui->label_url->setText(this->data->chain_data[this->type].url);
    this->ui->label_chain_id->setText(QString::number(this->data->chain_data[this->type].chain_id));
}
