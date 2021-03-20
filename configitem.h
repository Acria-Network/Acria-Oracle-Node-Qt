#ifndef CONFIGITEM_H
#define CONFIGITEM_H

#include <QDialog>
#include <QLineEdit>

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

    void on_lineEdit_resource_name_textChanged(const QString &arg1);

private:
    Ui::ConfigItem *ui;
    std::vector<QLineEdit*> t1;

    void clear();
};

#endif // CONFIGITEM_H
