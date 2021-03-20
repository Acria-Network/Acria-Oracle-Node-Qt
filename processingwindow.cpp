#include "processingwindow.h"
#include "ui_processingwindow.h"

#include "qprogressindicator.h"

ProcessingWindow::ProcessingWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ProcessingWindow)
{
    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);

    ui->setupUi(this);

    this->ui->widget->hide();

    this->pi = new QProgressIndicator();
    this->ui->horizontalLayout_2->layout()->addWidget(pi);

    pi->startAnimation();
    pi->setColor(QColor::fromRgb(255,255,255));

    this->setFixedSize(QSize(this->width(), this->height()));
}

ProcessingWindow::~ProcessingWindow()
{
    delete ui;
    delete pi;
}

void ProcessingWindow::completed(QString str){
    this->ui->widget->show();
    this->ui->plainTextEdit->insertPlainText(str);
}

void ProcessingWindow::on_pushButton_clicked()
{
    this->ui->widget->hide();
    this->hide();
}
