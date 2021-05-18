#ifndef ACRIACONFIG_H
#define ACRIACONFIG_H

#include <QDialog>

#include "config.h"
#include "configitem.h"
#include "json.hpp"
#include "signconfigwindow.h"


namespace Ui {
class AcriaConfig;
}

class AcriaConfig : public QDialog
{
    Q_OBJECT

public:
    explicit AcriaConfig(QWidget *parent = nullptr, Config* _config = nullptr, Data* data = nullptr);
    ~AcriaConfig();

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

    void on_pushButton_add_item_clicked();

    void on_tableWidget_config_cellDoubleClicked(int row, int column);

    void on_pushButton_sign_config_clicked();

private:
    Ui::AcriaConfig *ui;
    Config* config;

    ConfigItem* ci;
    nlohmann::json cjson;

    SignConfigWindow* sign_config_window;

    void update_table();
};

#endif // ACRIACONFIG_H
