#include "processingwindow.h"
#include "ui_processingwindow.h"

#include "qprogressindicator.h"

ProcessingWindow::ProcessingWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ProcessingWindow)
{
    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);

    ui->setupUi(this);

    this->pi = new QProgressIndicator();
    this->ui->horizontalLayout_2->layout()->addWidget(pi);

    pi->startAnimation();
    pi->setColor(QColor::fromRgb(255,255,255));

    this->setFixedSize(QSize(this->width(), this->height()));

    this->reset();
}

ProcessingWindow::~ProcessingWindow()
{
    delete ui;
    delete pi;
}

void ProcessingWindow::completed(QString str, bool error){
    this->ui->widget->show();
    this->ui->plainTextEdit->insertPlainText(str);

    if(error){
        this->ui->label_3->setPixmap(QPixmap("./resources/error.svg"));
    }
}

void ProcessingWindow::reset(){
    this->ui->widget->hide();
    this->hide();
    this->ui->label_3->setPixmap(QPixmap("./resources/success.svg"));
    this->ui->plainTextEdit->clear();
}

void ProcessingWindow::on_pushButton_clicked()
{
    this->reset();
}

void ProcessingWindow::on_ProcessingWindow_rejected()
{
    this->reset();
}
