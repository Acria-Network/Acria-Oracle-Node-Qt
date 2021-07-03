#include "scripteditor.h"
#include "ui_scripteditor.h"
#include "util.h"

#include <QWebChannel>
#include <QFileInfo>
#include <fstream>


ScriptEditor::ScriptEditor(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ScriptEditor)
{
    ui->setupUi(this);

    //QWebChannel * channel = new QWebChannel(this->ui->webEngineView->page());
    //this->ui->webEngineView->page()->setWebChannel(channel);
    //this->ui->webEngineView->page()->webChannel()->registerObject(QStringLiteral("AccountManager"), this);

    this->ui->webEngineView->page()->setBackgroundColor(Qt::transparent);
    this->ui->webEngineView->setUrl(QUrl::fromLocalFile(QFileInfo("js/code_editor/lint.html").absoluteFilePath()));
    this->ui->webEngineView->setZoomFactor(0.5);
}

ScriptEditor::~ScriptEditor()
{
    delete ui;
}

void ScriptEditor::load_script(QString script_file){
    QString script;
    std::ifstream t(script_file.toStdString());
    if(!t){
        qDebug() << "error file";
    }
    else{
        std::string str;

        t.seekg(0, std::ios::end);
        str.reserve(t.tellg());
        t.seekg(0, std::ios::beg);

        str.assign((std::istreambuf_iterator<char>(t)),
                    std::istreambuf_iterator<char>());

        script = QString::fromStdString(str).replace("\n","\\n").replace("'","\\'");
        qDebug() << script;
    }

    this->ui->webEngineView->page()->runJavaScript("editor.setValue('" + script + "');");
    this->ui->lineEdit_script_file_name->setText(script_file);
}


void ScriptEditor::save_script(QString script_content){
    if(Util::fileExists(this->ui->lineEdit_script_file_name->text())){
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, tr("Overwrite"), tr("There exists a file with this name. Should it be overwritten?"), QMessageBox::Yes|QMessageBox::No);
          if (reply != QMessageBox::Yes) {
                return;
          }
    }

    std::ofstream script_file(this->ui->lineEdit_script_file_name->text().toStdString());

    if (script_file.is_open())
    {
      script_file << script_content.toStdString();
      script_file.close();
    }
    else{
        qDebug() << "Unable to save script";
        Util::show_msgBox(tr("Unable to save script"));
    }

    qDebug() << "Saved Script: " << this->ui->lineEdit_script_file_name->text();

    accept();
}

void ScriptEditor::on_pushButton_save_script_clicked()
{
    this->ui->webEngineView->page()->runJavaScript("editor.getValue()", [this](const QVariant &v) { this->save_script(v.toString());});
}

void ScriptEditor::on_pushButton_discard_script_clicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, tr("Discard Changes"), tr("Are you sure you want to discard all changes?"), QMessageBox::Yes|QMessageBox::No);
      if (reply == QMessageBox::Yes) {
    reject();
    }
}
