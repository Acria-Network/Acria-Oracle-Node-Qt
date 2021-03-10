#ifndef INFOGETH_H
#define INFOGETH_H

#include <QDialog>

#include "node.h"


namespace Ui {
class InfoGeth;
}

class InfoGeth : public QDialog
{
    Q_OBJECT

public:
    explicit InfoGeth(QWidget *parent = nullptr, Node* _node = nullptr, Data *_data = nullptr, QString _type = "");
    ~InfoGeth();

    void update_info();

private:
    Ui::InfoGeth *ui;
    Node* node;
    Data *data;
    QString type;
};

#endif // INFOGETH_H
