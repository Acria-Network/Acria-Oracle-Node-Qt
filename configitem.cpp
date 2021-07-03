#include "configitem.h"
#include "ui_configitem.h"
#include "json.hpp"
#include "util.h"
#include "resource.h"

#include <QFileDialog>
#include <QJSEngine>
#include <QJSValue>
#include <QJSValueList>
#include <fstream>


ConfigItem::ConfigItem(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfigItem)
{
    ui->setupUi(this);
    this->script_editor = new ScriptEditor();
    this->setWindowFlags(Qt::Tool);

    t1.push_back(this->ui->lineEdit_json1);
    t1.push_back(this->ui->lineEdit_json2);
    t1.push_back(this->ui->lineEdit_json3);
    t1.push_back(this->ui->lineEdit_json4);

    this->ui->verticalWidget_response_regex->hide();
    this->ui->verticalWidget_response_json->show();
    this->ui->verticalWidget_script->hide();

    manager = new QNetworkAccessManager();

    QObject::connect(manager, SIGNAL(finished(QNetworkReply*)),
        this, SLOT(managerFinished(QNetworkReply*)));

    save_as_copy = false;
    this->show_save_as_copy_button(false);
    this->delete_item = false;
}

ConfigItem::~ConfigItem()
{
    delete ui;
    delete script_editor;
}

void ConfigItem::fill(nlohmann::json _json){
    this->clear();

    if(_json.contains("url"))
        this->ui->lineEdit_2_api_url->setText(QString::fromStdString(_json["url"]));
    if(_json.contains("rname"))
        this->ui->lineEdit_resource_name->setText(QString::fromStdString(_json["rname"]));
    if(_json.contains("url_data"))
        this->ui->lineEdit_2_api_url_2->setText(QString::fromStdString(_json["url_data"]));
    if(_json.contains("regex"))
        this->ui->lineEdit_regex_text->setText(QString::fromStdString(_json["regex"]));
    if(_json.contains("parameter_type"))
        this->ui->lineEdit_2_parameter_type->setText(QString::fromStdString(_json["parameter_type"]));
    if(_json.contains("description"))
        this->ui->plainTextEdit_description->setPlainText(QString::fromStdString(_json["description"]));
    if(_json.contains("example_value"))
        this->ui->label_response_parsed->setText(QString::fromStdString(_json["example_value"]));

    if(_json.contains("regex"))
        this->ui->comboBox->setCurrentIndex(1);
    else
        this->ui->comboBox->setCurrentIndex(0);

    if(_json.contains("json"))
        for(uint i = 0;i<_json["json"].size();i++){
            t1[i]->setText(QString::fromStdString(_json["json"][i]));
        }
}

void ConfigItem::clear(){
    this->ui->lineEdit_2_api_url->setText("");
    this->ui->lineEdit_resource_name->setText("");
    this->ui->lineEdit_2_api_url_2->setText("");
    this->ui->lineEdit_regex_text->setText("");
    this->ui->lineEdit_parameter_input->setText("");
    this->ui->lineEdit_2_parameter_type->setText("");
    this->ui->plainTextEdit_description->setPlainText("");
    this->ui->label_response_parsed->setText("");
    this->ui->label_example_request->setText("");
    this->ui->label_data_type_conversion->setText("");

    for(uint i = 0;i<this->t1.size();i++){
        t1[i]->setText("");
    }

    save_as_copy = false;
    this->delete_item = false;
}

void ConfigItem::managerFinished(QNetworkReply *reply) {
    if (reply->error()) {
        qDebug() << reply->errorString();
        return;
    }

    QString answer = reply->readAll();

    this->ui->label_example_request->setText(answer);
}

bool ConfigItem::check_input_valid(){
    if(this->ui->lineEdit_2_api_url->text() != "" && this->ui->lineEdit_resource_name->text() != "")
        if((this->t1[0]->text() != "" && this->ui->comboBox->currentIndex() == 0) || (this->ui->lineEdit_regex_text->text() != "" && this->ui->comboBox->currentIndex() == 1))
            if((this->ui->lineEdit_2_api_url_2->text() != "" && this->ui->lineEdit_2_parameter_type->text() != "") || (this->ui->lineEdit_2_api_url_2->text() == "" && this->ui->lineEdit_2_parameter_type->text() == ""))
                return true;
    return false;
}

void ConfigItem::on_buttonBox_1_accepted()
{
    nlohmann::json tmp;
    tmp["url"] = this->ui->lineEdit_2_api_url->text().toStdString();
    tmp["url_data"] = this->ui->lineEdit_2_api_url_2->text().toStdString();
    tmp["parameter_type"] = this->ui->lineEdit_2_parameter_type->text().toStdString();
    tmp["rname"] = this->ui->lineEdit_resource_name->text().toStdString();
    tmp["description"] = this->ui->plainTextEdit_description->toPlainText().toStdString();
    tmp["example_value"] = this->ui->label_response_parsed->text().toStdString();

    if(this->ui->comboBox->currentIndex() == 0){
        for(uint i = 0;i<this->t1.size();i++){
            if(this->t1[i]->text().trimmed() != "")
            tmp["json"][i] = this->t1[i]->text().toStdString();
        }
    }
    else if(this->ui->comboBox->currentIndex() == 1){
        tmp["regex"] = this->ui->lineEdit_regex_text->text().toStdString();
    }


    ijson = tmp;

    this->clear();
}

void ConfigItem::on_buttonBox_1_rejected()
{

}

void ConfigItem::on_lineEdit_resource_name_textChanged(const QString &arg1)
{
    if(arg1.length()>8){
        static_cast<QLineEdit*>(sender())->setText(arg1.mid(0, 8));
    }
}

void ConfigItem::on_comboBox_currentTextChanged(const QString &arg1)
{

}

void ConfigItem::on_comboBox_currentIndexChanged(int index)
{
    if(index == 0){
        this->ui->verticalWidget_response_regex->hide();
        this->ui->verticalWidget_response_json->show();
        this->ui->verticalWidget_script->hide();
    }
    else if(index == 1){
        this->ui->verticalWidget_response_json->hide();
        this->ui->verticalWidget_response_regex->show();
        this->ui->verticalWidget_script->hide();
    }
    else if(index == 2){
        this->ui->verticalWidget_response_json->hide();
        this->ui->verticalWidget_response_regex->hide();
        this->ui->verticalWidget_script->show();
    }
}

void ConfigItem::on_pushButton_make_example_request_clicked()
{
    request.setUrl(this->ui->lineEdit_2_api_url->text());
    request.setRawHeader("Content-Type", "application/json");
    manager->get(request);
}

void ConfigItem::on_pushButton_response_parse_clicked()
{
    QString answer = this->ui->label_example_request->text();
    if(answer == ""){
        this->ui->label_response_parsed->setText(tr("Please make an example request first."));
        return;
    }
    if(this->ui->comboBox->currentIndex() == 0){
        try{
            nlohmann::json tmp1 = nlohmann::json::parse(answer.toStdString());
            std::vector<QString> l_json;

            for(uint i = 0;i<this->t1.size();i++){
                if(this->t1[i]->text().trimmed() != "")
                    l_json.push_back(this->t1[i]->text());
            }

            int i = 0;
            for(;i<static_cast<int>(l_json.size())-1; i++){
                if(l_json[i] != ""){
                    if(tmp1.contains(l_json[i].toStdString())){
                        tmp1 = tmp1[l_json[i].toStdString()];
                    }
                    else{
                        throw;
                    }
                }
                else{
                    throw;
                }
            }

            double ans = static_cast<double>(tmp1[l_json[i].toStdString()]);

            QString tmp2 = QString::number(ans);
            int point = tmp2.indexOf('.');
            tmp2 = tmp2.replace(".", "");

            for(uint i=tmp2.length()-point;i<18;i++){
                tmp2 += "0";
            }

            this->ui->label_response_parsed->setText("Value: " + QString::number(ans)+"\nuint256: "+tmp2);
        }
        catch(...){
            this->ui->label_response_parsed->setText(tr("error"));
        }
    }
    else if(this->ui->comboBox->currentIndex() == 1){
        QRegularExpression re(this->ui->lineEdit_regex_text->text());
        QRegularExpressionMatch match = re.match(answer);
        if (match.hasMatch()) {
            QString matched = match.captured(1);
            if(matched == ""){
                this->ui->label_response_parsed->setText(tr("Nothing captured. Did you forget the brackets ()?"));
                return;
            }
            double ans = matched.toDouble();

            QString tmp2 = QString::number(ans);
            int point = tmp2.indexOf('.');
            tmp2 = tmp2.replace(".", "");

            for(uint i=tmp2.length()-point;i<18;i++){
                tmp2 += "0";
            }

            this->ui->label_response_parsed->setText("Value: " + matched+"\nuint256: "+tmp2);
        }
        else{
            this->ui->label_response_parsed->setText(tr("error"));
        }
    }
    else if(this->ui->comboBox->currentIndex() == 2){
            QString script = Util::read_file(this->ui->lineEdit_script_file->text().toStdString());
            QString bn = Util::read_file("scripts/bignumber.min.js");
            QJSEngine engine;
            QJSValue function_js = engine.evaluate(bn+"(function(api_response, arg1) {" + script + "})");
            QJSValueList args;
            args << answer << this->ui->lineEdit_script_parameter->text();
            QJSValue result = function_js.call(args);
            qDebug() << "parse Script ";

            if (result.isError()){
                qDebug() << "error Script";
                this->ui->label_response_parsed->setText(tr("Uncaught exception at line ") + result.property("lineNumber").toString() + ":" + result.toString());
                return;
            }
            else{
                QString tmp2;
                if(result.isNumber()){
                    tmp2 = Util::double2uint256(result.toNumber());
                    this->ui->label_response_parsed->setText("Value: " + result.toString()+"\nuint256: "+tmp2);
                }
                else if(result.isString()){
                    if (QRegExp("^0x[A-Fa-f0-9]+$").exactMatch(result.toString()))
                       tmp2 = result.toString();
                    else
                       tmp2 = Util::str2bytes32(result.toString());

                    this->ui->label_response_parsed->setText("Value: " + result.toString()+"\nuint256: "+tmp2);
                }
                else if(result.isUndefined()){
                    this->ui->label_response_parsed->setText("Undefined Value");
                }
            }
    }
}

void ConfigItem::on_pushButton_make_example_request_with_parameter_clicked()
{
    QString hex;
    QString input = this->ui->lineEdit_parameter_input->text();
    QRegExp re("\\d*");
    QRegExp re_hex("^0x[A-Fa-f0-9]+$");

    if (re.exactMatch(input))
       hex = QString::fromStdString(Util::tohex(input.toStdString()));
    else if (re_hex.exactMatch(input)){
       hex = input.remove(0,2);
       for(uint i = hex.size(); i<64;i++){
           hex = "0" + hex;
       }
    }
    else
       hex = Util::str2bytes32(input);

    QString par = Resource::convert_parameter(this->ui->lineEdit_2_parameter_type->text(), hex);
    this->ui->label_data_type_conversion->setText("Parameter Conversion: " + this->ui->lineEdit_parameter_input->text() + " -> 0x"+ hex + " -> " + par);
    request.setUrl(this->ui->lineEdit_2_api_url_2->text().replace("%data%", par));
    request.setRawHeader("Content-Type", "application/json");
    manager->get(request);
}

void ConfigItem::on_pushButton_save_and_continue_clicked()
{
    save_as_copy = true;

    nlohmann::json tmp;
    tmp["url"] = this->ui->lineEdit_2_api_url->text().toStdString();
    tmp["url_data"] = this->ui->lineEdit_2_api_url_2->text().toStdString();
    tmp["parameter_type"] = this->ui->lineEdit_2_parameter_type->text().toStdString();
    tmp["rname"] = this->ui->lineEdit_resource_name->text().toStdString();
    tmp["description"] = this->ui->plainTextEdit_description->toPlainText().toStdString();
    tmp["example_value"] = this->ui->label_response_parsed->text().toStdString();

    if(this->ui->comboBox->currentIndex() == 0){
        for(uint i = 0;i<this->t1.size();i++){
            if(this->t1[i]->text().trimmed() != "")
            tmp["json"][i] = this->t1[i]->text().toStdString();
        }
    }
    else if(this->ui->comboBox->currentIndex() == 1){
        tmp["regex"] = this->ui->lineEdit_regex_text->text().toStdString();
    }


    ijson = tmp;

    accept();
}

void ConfigItem::show_save_as_copy_button(bool visible){
    if(visible){
        this->ui->pushButton_save_and_continue->show();
        this->ui->pushButton_delete_item->show();
    }
    else{
        this->ui->pushButton_save_and_continue->hide();
        this->ui->pushButton_delete_item->hide();
    }
}

void ConfigItem::on_pushButton_delete_item_clicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, tr("Delete Item"), tr("Are you sure you want to delete this resource item?"), QMessageBox::Yes|QMessageBox::No);
      if (reply == QMessageBox::Yes) {
          this->delete_item = true;
            accept();
      }
}

void ConfigItem::on_pushButton_select_script_file_clicked()
{
    QString dir = QFileDialog::getOpenFileName(this, tr("Open Script File"),
                                                 "./scripts");

    this->ui->lineEdit_script_file->setText(dir);
}

void ConfigItem::on_pushButton_create_script_file_clicked()
{
    this->script_editor->load_script(this->ui->lineEdit_script_file->text());
    script_editor->exec();
}

void ConfigItem::on_lineEdit_script_file_textChanged(const QString &arg1)
{
    if(arg1 != "")
        this->ui->pushButton_create_script_file->setText(tr("Edit Script"));
    else
        this->ui->pushButton_create_script_file->setText(tr("Create Script"));
}
