#include "infogeth.h"
#include "ui_infogeth.h"

InfoGeth::InfoGeth(QWidget *parent, Node* _node, Data *_data) :
    QDialog(parent),
    ui(new Ui::InfoGeth)
{
    ui->setupUi(this);

    this->node = _node;
    this->data = _data;

    this->update_info();
}

InfoGeth::~InfoGeth()
{
    delete ui;
}

void InfoGeth::update_info(){
    this->ui->label_version->setText(node->get_geth_version());
    this->ui->label_url->setText(this->data->geth_url);

    QString tmp;

    for(uint i = 0; i<data->items.size();i++){
        tmp+=data->items[i];
        tmp+="\n";
    }

    this->ui->label_items->setText(tmp);
}
