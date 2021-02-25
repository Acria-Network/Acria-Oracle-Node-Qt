#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileInfo>
#include <QApplication>
#include <QMessageBox>
#include <QDebug>
#include <QtWidgets>
#include <QtGui>
#include <QIcon>
#include <QtConcurrent/QtConcurrent>
#include <QtCore>


bool fileExists(QString path) {
    QFileInfo check_file(path);

    // check if file exists and if yes: Is it really a file and no directory?
    if (check_file.exists() && check_file.isFile()) {
        return true;
    } else {
        return false;
    }
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    qDebug() << "started";

    this->data = new Data();
    //this->data->geth_url = "http://127.0.0.1:9545/";
    //this->data->eth_contract = "0xD4cA7302185a7346557709eFf49d05536B766d4A";
    //this->data->eth_account = "0x56B832aB0E5615CD72256204406177Ed5887145c";

    data->load_settings();

    node = new Node(this->data);
    acria_config = new AcriaConfig(this, node);
    tasks = new Tasks(this->data);
    cinfo = new compinfo(this->data);

    if(!fileExists("config.conf")){
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Config Missing", "No Configuration File Found. Create one now?", QMessageBox::Yes|QMessageBox::No);
          if (reply == QMessageBox::Yes) {
            acria_config->show();
          } else {

          }
    }

    if(!this->node->parseConfig()){
        qDebug() << "Error config";
    }

    ui->setupUi(this);

    QtConcurrent::run(this, &MainWindow::get_status_geth);
    QtConcurrent::run(this, &MainWindow::get_status_polkadot);
    QtConcurrent::run(this, &MainWindow::get_status_acria);
    QtConcurrent::run(this, &MainWindow::get_status_config);

    tasks->update_tasks();

    timer_update_requests = new QTimer(this);
    connect(timer_update_requests, SIGNAL(timeout()), this, SLOT(update_requests()));
    timer_update_requests->start(INTERVAL_RUN);

    timer_update_events = new QTimer(this);
    connect(timer_update_events, SIGNAL(timeout()), this, SLOT(update_events()));
    timer_update_events->start(INTERVAL_RUN);

    this->load_resources();

    this->pi = new QProgressIndicator();
    this->ui->horizontalLayout->layout()->addWidget(pi);

    pi->startAnimation();
    pi->setColor(QColor::fromRgb(255,255,255));

    this->pi2 = new QProgressIndicator();
    this->ui->horizontalLayout_2->layout()->addWidget(pi2);

    pi2->startAnimation();
    pi2->setColor(QColor::fromRgb(255,255,255));

    this->update_settings();
}

void MainWindow::load_resources(){
    for(uint i = 0; i<node->get_config().size();i++){
        std::vector<QString> l_json;

        for(uint d = 0; d<node->get_config()[i]["json"].size();d++){
            l_json.push_back(QString::fromStdString(node->get_config()[i]["json"][d]));
        }

        Resource* rr = new Resource(QString::fromStdString(node->get_config()[i]["url"]), l_json, this->data->eth_contract, QString::fromStdString(node->get_config()[i]["rname"]), this->data);
        resources[QString::fromStdString(node->get_config()[i]["rname"])] = rr;
    }
}

void MainWindow::update_settings(){
    this->ui->lineEdit_geth_url->setText(this->data->geth_url);
    this->ui->lineEdit_eth_account->setText(this->data->eth_account);
    this->ui->lineEdit_eth_contract->setText(this->data->eth_contract);

    this->ui->lineEdit_polkadot_url->setText(this->data->polkadot_url);
    this->ui->lineEdit_polkadot_account->setText(this->data->polkadot_account);
    this->ui->lineEdit_polkadot_contract->setText(this->data->polkadot_contract);
}

void MainWindow::get_status_geth(){
    QThread::msleep(INTERVAL_RUN_CONNECTION);

    if(node->get_status_geth()){
        ui->status_geth->setText("Ok");
    }
}

void MainWindow::get_status_polkadot(){
    QThread::msleep(INTERVAL_RUN_CONNECTION);

    if(node->get_status_polkadot()){
        ui->status_polkadot->setText("Ok");
    }
}

void MainWindow::get_status_acria(){
    QThread::msleep(INTERVAL_RUN_CONNECTION);

    if(node->get_status_acria()){
        ui->status_acria->setText("Ok");
    }
}

void MainWindow::get_status_config(){
    if(node->get_status_config()){
        ui->status_config->setText("Ok");
    }
}

void MainWindow::update_requests(){
    tasks->update_requests();

    std::vector<req> r = tasks->get_requests();

    this->ui->tableWidget_req->clear();
    while (ui->tableWidget_req->rowCount() > 0)
    {
        ui->tableWidget_req->removeRow(0);
    }
    this->ui->tableWidget_req->setRowCount(r.size());
    qDebug() << "r " <<r.size();
    this->ui->tableWidget_req->setColumnCount(5);
    this->ui->tableWidget_req->setColumnWidth(4, 240);
    this->ui->tableWidget_req->setColumnWidth(0, 20);
    for (uint d=0; d<r.size(); d++){
        this->ui->tableWidget_req->setItem( d, 0, new QTableWidgetItem(QString::number(d)));
        this->ui->tableWidget_req->setItem( d, 1, new QTableWidgetItem(r[d].requestID));
        this->ui->tableWidget_req->setItem( d, 2, new QTableWidgetItem(r[d].cancelled==true?"Cancelled":"Active"));
        this->ui->tableWidget_req->setItem( d, 3, new QTableWidgetItem(QString::number(r[d].expiration)));
        this->ui->tableWidget_req->setItem( d, 4, new QTableWidgetItem(r[d].callback));
    }

    std::vector<QString> nt;

    for (uint d=0; d<r.size(); d++){
        if ( resources.find(r[d].requestID) == resources.end() ) {
          qDebug() << "key not available";
        } else {
            if (!std::count(nt.begin(), nt.end(), r[d].requestID)){
                qDebug() <<"dddd";
                resources[r[d].requestID]->update_resource();
                nt.push_back(r[d].requestID);
            }
        }
    }

    if(r.size() != 0){
        this->ui->horizontalLayoutWidget->hide();
        this->ui->label_progress->hide();
    }
    else{
        this->ui->horizontalLayoutWidget->show();
        this->ui->label_progress->show();
    }
}

void MainWindow::update_events(){
    cinfo->update_events();

    std::vector<comp> r = cinfo->get_completed();

    this->ui->tableWidget_comp->clear();
    while (ui->tableWidget_comp->rowCount() > 0)
    {
        ui->tableWidget_comp->removeRow(0);
    }

    this->ui->tableWidget_comp->setRowCount(r.size());
    this->ui->tableWidget_comp->setColumnCount(5);
    this->ui->tableWidget_comp->setColumnWidth(4, 240);
    this->ui->tableWidget_comp->setColumnWidth(0, 20);
    for (uint d=0; d<r.size(); d++){
        this->ui->tableWidget_comp->setItem( d, 0, new QTableWidgetItem(QString::number(d)));
        this->ui->tableWidget_comp->setItem( d, 1, new QTableWidgetItem(r[d].requestID));
        this->ui->tableWidget_comp->setItem( d, 2, new QTableWidgetItem(r[d].callback));
        this->ui->tableWidget_comp->setItem( d, 3, new QTableWidgetItem(QString::number(r[d].block)));
        this->ui->tableWidget_comp->setItem( d, 4, new QTableWidgetItem(r[d].hash));
    }

    if(r.size() != 0){
        this->ui->horizontalLayoutWidget_2->hide();
        this->ui->label_progress_2->hide();
    }
    else{
        this->ui->horizontalLayoutWidget_2->show();
        this->ui->label_progress_2->show();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
    delete acria_config;
    delete node;
    delete tasks;
    delete cinfo;
    delete pi;
    delete pi2;
    delete data;

    delete timer_update_requests;
    delete timer_update_events;

    for (auto const& x : resources)
    {
        delete x.second;
    }
}


void MainWindow::on_pushButton_export_json_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                 "/home",
                                                 QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);


    std::vector<comp> r = cinfo->get_completed();

    nlohmann::json tmp;

    for(uint i = 0; i<r.size(); i++){
        tmp[i]["requestID"] = r[i].requestID.toStdString();
        tmp[i]["callback"] = r[i].callback.toStdString();
        tmp[i]["hash"] = r[i].hash.toStdString();
        tmp[i]["block"] = r[i].block;
        tmp[i]["address"] = r[i].address.toStdString();
    }

    QString filename=dir + "/transactions.json";
    QFile file(filename);

    qDebug() << filename;

    if(!file.exists()){
        qDebug() << "File exists: "<<filename;
    }else{
        qDebug() << filename<<" does not exist";
    }

   if (file.open(QIODevice::WriteOnly | QIODevice::Text)){
        QTextStream out(&file);
        out << QString::fromStdString(tmp.dump());
        file.close();
   }

   QMessageBox msgBox;
   msgBox.setText("Successfully exported Json!");
   msgBox.exec();
}

void MainWindow::on_pushButton_export_csv_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                 "/home",
                                                 QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);

    QString filename=dir + "/transactions.csv";
    QFile file(filename);

    if(!file.exists()){
        qDebug() << "File exists: "<<filename;
    }else{
        qDebug() << filename<<" does not exist";
    }

   if (file.open(QIODevice::WriteOnly | QIODevice::Text)){
        QTextStream out(&file);
        out << "";
        file.close();
   }
}

void MainWindow::on_pushButton_export_json_3_clicked()
{
    this->acria_config->show();
}

void MainWindow::on_pushButton_refresh_clicked()
{
    ui->status_geth->setText("...");
    ui->status_polkadot->setText("...");
    ui->status_acria->setText("...");
    ui->status_config->setText("...");

    QtConcurrent::run(this, &MainWindow::get_status_geth);
    QtConcurrent::run(this, &MainWindow::get_status_polkadot);
    QtConcurrent::run(this, &MainWindow::get_status_acria);
    QtConcurrent::run(this, &MainWindow::get_status_config);
}

void MainWindow::on_pushButton_eth_settings_clicked()
{
    this->ui->tabWidget->setCurrentIndex(3);
}

void MainWindow::on_pushButton_polkadot_settings_clicked()
{
    this->ui->tabWidget->setCurrentIndex(3);
}

void MainWindow::on_pushButton_acria_settings_clicked()
{
    this->ui->tabWidget->setCurrentIndex(3);
}

void MainWindow::on_pushButton_config_settings_clicked()
{
    this->acria_config->show();
}

void MainWindow::on_pushButton_setting_save_clicked()
{
    this->data->save_settings();

    QMessageBox msgBox;
    msgBox.setText("Successfully saved the settings!");
    msgBox.exec();
}

void MainWindow::on_pushButton_setting_discard_clicked()
{
    this->data->load_settings();
    this->update_settings();
}
