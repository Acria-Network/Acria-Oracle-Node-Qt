#ifndef CONFIGITEM_H
#define CONFIGITEM_H

#include <QDialog>
#include <QLineEdit>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkAccessManager>

#include "json.hpp"
#include "scripteditor.h"


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

    bool save_as_copy;
    bool delete_item;

    void fill(nlohmann::json _json);

    void show_save_as_copy_button(bool visible);

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

    void on_pushButton_save_and_continue_clicked();

    void on_pushButton_delete_item_clicked();

    void on_pushButton_select_script_file_clicked();

    void on_pushButton_create_script_file_clicked();

    void on_lineEdit_script_file_textChanged(const QString &arg1);

private:
    Ui::ConfigItem *ui;
    std::vector<QLineEdit*> t1;

    QNetworkAccessManager *manager;
    QNetworkRequest request;

    ScriptEditor* script_editor;

    void clear();
    bool check_input_valid();
};

#endif // CONFIGITEM_H
