#ifndef CONFIGITEM_H
#define CONFIGITEM_H

#include <QDialog>
#include <QLineEdit>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkAccessManager>

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

    void on_comboBox_currentTextChanged(const QString &arg1);

    void on_comboBox_currentIndexChanged(int index);

    void managerFinished(QNetworkReply *reply);

    void on_pushButton_make_example_request_clicked();

    void on_pushButton_response_parse_clicked();

    void on_pushButton_make_example_request_with_parameter_clicked();

private:
    Ui::ConfigItem *ui;
    std::vector<QLineEdit*> t1;

    QNetworkAccessManager *manager;
    QNetworkRequest request;

    void clear();
};

#endif // CONFIGITEM_H
