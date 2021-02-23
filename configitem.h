#ifndef CONFIGITEM_H
#define CONFIGITEM_H

#include <QDialog>

#include "json.hpp"


namespace Ui {
class ConfigItem;
}

class ConfigItem : public QDialog
{
    Q_OBJECT

public:
    explicit ConfigItem(QWidget *parent = nullptr);
    ~ConfigItem();

    nlohmann::json ijson;

private slots:
    void on_buttonBox_1_accepted();

    void on_buttonBox_1_rejected();

private:
    Ui::ConfigItem *ui;
};

#endif // CONFIGITEM_H
