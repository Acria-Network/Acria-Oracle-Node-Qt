#include "configitem.h"
#include "ui_configitem.h"
#include "json.hpp"
#include "util.h"
#include "resource.h"


ConfigItem::ConfigItem(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfigItem)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::Tool);

    t1.push_back(this->ui->lineEdit_json1);
    t1.push_back(this->ui->lineEdit_json2);
    t1.push_back(this->ui->lineEdit_json3);
    t1.push_back(this->ui->lineEdit_json4);

    this->ui->verticalWidget_response_regex->hide();
    this->ui->verticalWidget_response_json->show();

    manager = new QNetworkAccessManager();

    QObject::connect(manager, SIGNAL(finished(QNetworkReply*)),
        this, SLOT(managerFinished(QNetworkReply*)));
}

ConfigItem::~ConfigItem()
{
    delete ui;
}

void ConfigItem::clear(){
    this->ui->lineEdit_2_api_url->setText("");
    this->ui->lineEdit_resource_name->setText("");

    for(uint i = 0;i<this->t1.size();i++){
        t1[i]->setText("");
    }
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
    if(arg1.length()>32){
        static_cast<QLineEdit*>(sender())->setText(arg1.mid(0, 32));
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
    }
    else if(index == 1){
        this->ui->verticalWidget_response_json->hide();
        this->ui->verticalWidget_response_regex->show();
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
            this->ui->label_response_parsed->setText("error");
        }
    }
    else if(this->ui->comboBox->currentIndex() == 1){
        QRegularExpression re(this->ui->lineEdit_regex_text->text());
        QRegularExpressionMatch match = re.match(answer);
        if (match.hasMatch()) {
            QString matched = match.captured(1);
            if(matched == ""){
                this->ui->label_response_parsed->setText("Nothing captured. Did you forget the brackets ()?");
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
            this->ui->label_response_parsed->setText("error");
        }
    }
}

void ConfigItem::on_pushButton_make_example_request_with_parameter_clicked()
{
    QString hex = QString::fromStdString(Util::tohex(this->ui->lineEdit_parameter_input->text().toStdString()));
    QString par = Resource::convert_parameter(this->ui->lineEdit_2_parameter_type->text(), hex);
    this->ui->label_data_type_conversion->setText("Parameter Conversion: " + this->ui->lineEdit_parameter_input->text() + " -> 0x"+ hex + " -> " + par);
    request.setUrl(this->ui->lineEdit_2_api_url_2->text().replace("%data%", par));
    request.setRawHeader("Content-Type", "application/json");
    manager->get(request);
}
