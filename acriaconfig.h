#ifndef ACRIACONFIG_H
#define ACRIACONFIG_H

#include <QDialog>

#include "node.h"


namespace Ui {
class AcriaConfig;
}

class AcriaConfig : public QDialog
{
    Q_OBJECT

public:
    explicit AcriaConfig(QWidget *parent = nullptr, Node* _node = nullptr);
    ~AcriaConfig();

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::AcriaConfig *ui;
    Node* node;
};

#endif // ACRIACONFIG_H
