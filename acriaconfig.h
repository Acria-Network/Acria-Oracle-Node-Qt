#ifndef ACRIACONFIG_H
#define ACRIACONFIG_H

#include <QDialog>

#include "config.h"
#include "configitem.h"
#include "json.hpp"


namespace Ui {
class AcriaConfig;
}

class AcriaConfig : public QDialog
{
    Q_OBJECT

public:
    explicit AcriaConfig(QWidget *parent = nullptr, Config* _config = nullptr);
    ~AcriaConfig();

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

    void on_pushButton_add_item_clicked();

private:
    Ui::AcriaConfig *ui;
    Config* config;

    ConfigItem* ci;
    nlohmann::json cjson;
};

#endif // ACRIACONFIG_H
