#include "scripteditor.h"
#include "ui_scripteditor.h"

#include <QWebChannel>
#include <QFileInfo>


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
