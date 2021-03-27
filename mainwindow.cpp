#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "verifyethaddress.h"

#include <QFileInfo>
#include <QApplication>
#include <QMessageBox>
#include <QDebug>
#include <QtWidgets>
#include <QtGui>
#include <QIcon>
#include <QtConcurrent/QtConcurrent>
#include <QtCore>
#include <QLineEdit>


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

    data->load_settings();

    this->config = new Config();

    if(!fileExists("config.conf")){
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Config Missing", "No Configuration File Found. Create one now?", QMessageBox::Yes|QMessageBox::No);
          if (reply == QMessageBox::Yes) {
            acria_config->show();
          } else {

          }
    }
    else{
        if(!this->config->parseConfig()){
            qDebug() << "Error config";
        }
    }

    this->acria_config = new AcriaConfig(this, this->config);
    this->processing_window = new ProcessingWindow();

    this->eth_based_chain["ethereum"] = new EthBasedChain(this, "ethereum", this->data, this->processing_window);
    this->eth_based_chain["binance"] = new EthBasedChain(this, "binance", this->data, this->processing_window);

    ui->setupUi(this);

    QtConcurrent::run(this, &MainWindow::get_status_geth);
    QtConcurrent::run(this, &MainWindow::get_status_polkadot);
    QtConcurrent::run(this, &MainWindow::get_status_acria);
    QtConcurrent::run(this, &MainWindow::get_status_config);
    QtConcurrent::run(this, &MainWindow::get_status_binance);

    timer_update_requests = new QTimer(this);
    connect(timer_update_requests, SIGNAL(timeout()), this, SLOT(update_requests()));
    timer_update_requests->start(INTERVAL_RUN);

    timer_update_events = new QTimer(this);
    connect(timer_update_events, SIGNAL(timeout()), this, SLOT(update_events()));
    timer_update_events->start(INTERVAL_RUN);

    timer_update_balances = new QTimer(this);
    connect(timer_update_balances, SIGNAL(timeout()), this, SLOT(update_balances()));
    timer_update_balances->start(INTERVAL_RUN);

    timer_update_gas_price = new QTimer(this);
    connect(timer_update_gas_price, SIGNAL(timeout()), this, SLOT(update_gas_price()));
    timer_update_gas_price->start(INTERVAL_RUN);

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

void MainWindow::update_settings(){
    this->ui->lineEdit_geth_url->setText(this->data->geth_url);
    this->ui->lineEdit_eth_account->setText(this->data->eth_account);
    this->ui->lineEdit_eth_contract->setText(this->data->eth_contract);
    this->ui->checkBox_eth->setChecked(this->data->eth_enabled);

    this->ui->lineEdit_polkadot_url->setText(this->data->polkadot_url);
    this->ui->lineEdit_polkadot_account->setText(this->data->polkadot_account);
    this->ui->lineEdit_polkadot_contract->setText(this->data->polkadot_contract);
    this->ui->checkBox_polkadot->setChecked(this->data->polkadot_enabled);

    this->ui->lineEdit_binance_url->setText(this->data->binance_url);
    this->ui->lineEdit_binance_account->setText(this->data->binance_account);
    this->ui->lineEdit_binance_contract->setText(this->data->binance_contract);
    this->ui->checkBox_binance->setChecked(this->data->binance_enabled);
}

void MainWindow::get_status_geth(){
    QThread::msleep(INTERVAL_RUN_CONNECTION);

    if(this->eth_based_chain["ethereum"]->node->get_status_geth()){
        ui->status_geth->setText("Ok");
    }
}

void MainWindow::get_status_polkadot(){
    QThread::msleep(INTERVAL_RUN_CONNECTION);

    if(true){
        ui->status_polkadot->setText("Ok");
    }
}

void MainWindow::get_status_binance(){
    QThread::msleep(INTERVAL_RUN_CONNECTION);

    if(this->eth_based_chain["binance"]->node->get_status_geth()){
        ui->status_binance->setText("Ok");
    }
}

void MainWindow::get_status_acria(){
    QThread::msleep(INTERVAL_RUN_CONNECTION);

    if(true){
        ui->status_acria->setText("Ok");
    }
}

void MainWindow::get_status_config(){
    if(true){
        ui->status_config->setText("Ok");
    }
}

void MainWindow::update_requests(){
    if(this->data->transaction_fee_geth != 0 && this->data->eth_enabled != false && this->eth_based_chain["ethereum"]->nonce_manager->is_ready())
        this->eth_based_chain["ethereum"]->tasks->update_requests();
    if(this->data->transaction_fee_binance != 0 && this->data->binance_enabled != false && this->eth_based_chain["binance"]->nonce_manager->is_ready())
        this->eth_based_chain["binance"]->tasks->update_requests();

    std::vector<req> r;
    for (auto const& x : eth_based_chain)
    {
        std::vector<req> r2 = x.second->tasks->get_requests();
        r.insert(r.end(), r2.begin(), r2.end());
    }

    this->ui->tableWidget_req->clear();
    while (ui->tableWidget_req->rowCount() > 0)
    {
        ui->tableWidget_req->removeRow(0);
    }
    this->ui->tableWidget_req->setRowCount(r.size());

    this->ui->tableWidget_req->setColumnCount(7);
    this->ui->tableWidget_req->setColumnWidth(4, 100);
    this->ui->tableWidget_req->setColumnWidth(6, 50);
    this->ui->tableWidget_req->setColumnWidth(0, 20);

    this->ui->tableWidget_req->setHorizontalHeaderItem(0, new QTableWidgetItem("#"));
    this->ui->tableWidget_req->setHorizontalHeaderItem(1, new QTableWidgetItem("Item"));
    this->ui->tableWidget_req->setHorizontalHeaderItem(2, new QTableWidgetItem("Fee"));
    this->ui->tableWidget_req->setHorizontalHeaderItem(3, new QTableWidgetItem("Expiration"));
    this->ui->tableWidget_req->setHorizontalHeaderItem(4, new QTableWidgetItem("Chain"));
    this->ui->tableWidget_req->setHorizontalHeaderItem(5, new QTableWidgetItem("Id"));
    this->ui->tableWidget_req->setHorizontalHeaderItem(6, new QTableWidgetItem("Status"));

    for (uint d=0; d<r.size(); d++){
        this->ui->tableWidget_req->setItem( d, 0, new QTableWidgetItem(QString::number(d)));
        this->ui->tableWidget_req->setItem( d, 1, new QTableWidgetItem(r[d].requestID));
        this->ui->tableWidget_req->setItem( d, 2, new QTableWidgetItem(QString::number(double(r[d].fee / pow(10,18)))));
        this->ui->tableWidget_req->setItem( d, 3, new QTableWidgetItem(QString::number(r[d].expiration)));
        this->ui->tableWidget_req->setItem( d, 4, new QTableWidgetItem(r[d].chain));
        this->ui->tableWidget_req->setItem( d, 5, new QTableWidgetItem(QString::number(r[d].id)));
        uint state = this->eth_based_chain[r[d].chain]->state[r[d].id];
        this->ui->tableWidget_req->setItem( d, 6, new QTableWidgetItem(state==0?"new":state==1?"data updated":state==2?"sent":"completed"));
    }

    this->ui->tableWidget_req->horizontalHeader()->setStretchLastSection(true);

    for (uint d=0; d<r.size(); d++){
            if (!std::count(nt.begin(), nt.end(), r[d].chain + QString::number(r[d].id))){
                nlohmann::json conf1 = config->get_config();
                for(uint i = 0; i<conf1.size();i++){
                    if(QString::fromStdString(conf1[i]["rname"]) == r[d].requestID){
                        std::vector<QString> l_json;

                        for(uint f = 0; f<conf1[i]["json"].size();f++){
                            l_json.push_back(QString::fromStdString(conf1[i]["json"][f]));
                        }

                        this->eth_based_chain[r[d].chain]->state[r[d].id] = 0;
                        Resource* rr = new Resource(QString::fromStdString(conf1[i]["url"]), l_json, this->data->eth_contract, QString::fromStdString(conf1[i]["rname"]), this->data, r[d].chain, r[d].id, &this->eth_based_chain[r[d].chain]->state[r[d].id], r[d].max_gas, r[d].fee, this->eth_based_chain[r[d].chain]->nonce_manager);
                        rr->update_resource();

                        this->tm_resources.push_back(rr);
                        nt.push_back(r[d].chain + QString::number(r[d].id));

                        QThread::msleep(100);
                    }

            }
        }
    }

    if(r.size() != 0){
        this->ui->tableWidget_req->horizontalHeader()->show();
        this->ui->horizontalWidget_6->hide();
        this->ui->label_progress->hide();
    }
    else{
        this->ui->tableWidget_req->horizontalHeader()->hide();
        this->ui->horizontalWidget_6->show();
        this->ui->label_progress->show();
    }

    for(uint i=0; i<this->tm_resources.size();i++){
        if(this->tm_resources[i]->get_state() == 3){
            delete this->tm_resources[i];
            this->tm_resources.erase(this->tm_resources.begin() + i);
        }
    }
}

void MainWindow::update_gas_price(){
    for (auto const& x : eth_based_chain)
    {
        x.second->gas_price->update_gas_price();
    }
}

void MainWindow::update_events(){
    for (auto const& x : eth_based_chain)
    {
        x.second->cinfo->update_events();
    }

    std::vector<comp> r;
    for (auto const& x : eth_based_chain)
    {
        std::vector<comp> r2 = x.second->cinfo->get_completed();
        r.insert(r.end(), r2.begin(), r2.end());
    }

    this->ui->tableWidget_comp->clear();
    while (ui->tableWidget_comp->rowCount() > 0)
    {
        ui->tableWidget_comp->removeRow(0);
    }

    this->ui->tableWidget_comp->setRowCount(r.size());
    this->ui->tableWidget_comp->setColumnCount(6);
    this->ui->tableWidget_comp->setColumnWidth(0, 30);
    this->ui->tableWidget_comp->setColumnWidth(4, 50);

    this->ui->tableWidget_comp->setHorizontalHeaderItem(0, new QTableWidgetItem("#"));
    this->ui->tableWidget_comp->setHorizontalHeaderItem(1, new QTableWidgetItem("Item"));
    this->ui->tableWidget_comp->setHorizontalHeaderItem(2, new QTableWidgetItem("Chain"));
    this->ui->tableWidget_comp->setHorizontalHeaderItem(3, new QTableWidgetItem("Block"));
    this->ui->tableWidget_comp->setHorizontalHeaderItem(4, new QTableWidgetItem("Fee"));
    this->ui->tableWidget_comp->setHorizontalHeaderItem(5, new QTableWidgetItem("Hash"));

    if(r.size() != 0){
        this->ui->tableWidget_comp->horizontalHeader()->show();
        this->ui->horizontalWidget_3->hide();
        this->ui->label_progress_2->hide();

        for (int d=r.size()-1; d>=0; d--){
            uint dd = r.size()-1-d;
            this->ui->tableWidget_comp->setItem( dd, 0, new QTableWidgetItem(QString::number(d)));
            this->ui->tableWidget_comp->setItem( dd, 1, new QTableWidgetItem(r[d].requestID.trimmed().replace("\0","")));
            this->ui->tableWidget_comp->setItem( dd, 2, new QTableWidgetItem(r[d].chain));
            this->ui->tableWidget_comp->setItem( dd, 3, new QTableWidgetItem(QString::number(r[d].block)));
            this->ui->tableWidget_comp->setItem( dd, 5, new QTableWidgetItem(r[d].hash));
            this->ui->tableWidget_comp->setItem( dd, 4, new QTableWidgetItem(QString::number(double(r[d].fee / pow(10,18)))));
        }
    }
    else{
        this->ui->tableWidget_comp->horizontalHeader()->hide();
        this->ui->horizontalWidget_3->show();
        this->ui->label_progress_2->show();
    }
    this->ui->tableWidget_comp->horizontalHeader()->setStretchLastSection(true);
}

void MainWindow::update_balances(){
    for (auto const& x : eth_based_chain)
    {
        x.second->balances->update_withdrawable();
    }

    this->ui->tableWidget_balances->clear();
    while (ui->tableWidget_balances->rowCount() > 0)
    {
        ui->tableWidget_balances->removeRow(0);
    }

    this->ui->tableWidget_balances->setRowCount(4);
    this->ui->tableWidget_balances->setColumnCount(4);

    this->ui->tableWidget_balances->setColumnWidth(1, 128);
    this->ui->tableWidget_balances->setColumnWidth(2, 128);
    this->ui->tableWidget_balances->setColumnWidth(3, 128);

    this->ui->tableWidget_balances->setHorizontalHeaderItem(0, new QTableWidgetItem("Chain"));
    this->ui->tableWidget_balances->setHorizontalHeaderItem(1, new QTableWidgetItem("Fees Earned Total"));
    this->ui->tableWidget_balances->setHorizontalHeaderItem(2, new QTableWidgetItem("Withdrawable Fees"));
    this->ui->tableWidget_balances->setHorizontalHeaderItem(3, new QTableWidgetItem("Completed Requests"));
    this->ui->tableWidget_balances->horizontalHeader()->show();

    this->ui->tableWidget_balances->setItem( 0, 0, new QTableWidgetItem("Ethereum"));
    this->ui->tableWidget_balances->setItem( 1, 0, new QTableWidgetItem("Polkadot"));
    this->ui->tableWidget_balances->setItem( 2, 0, new QTableWidgetItem("Binance"));
    this->ui->tableWidget_balances->setItem( 3, 0, new QTableWidgetItem("Total"));

    uint eth_completed = this->eth_based_chain["ethereum"]->cinfo->get_completed().size();
    uint binance_completed = this->eth_based_chain["binance"]->cinfo->get_completed().size();
    uint polkadot_completed = 0;

    double binance_total_fees = this->eth_based_chain["binance"]->cinfo->get_total_fees();
    double eth_total_fees = this->eth_based_chain["ethereum"]->cinfo->get_total_fees();
    double polkadot_total_fees = 0;

    double binance_withdrawable = this->eth_based_chain["binance"]->balances->get_withdrawable();
    double eth_withdrawable = this->eth_based_chain["ethereum"]->balances->get_withdrawable();
    double polkadot_withdrawable = 0;

    this->ui->tableWidget_balances->setItem( 0, 3, new QTableWidgetItem(QString::number(eth_completed)));
    this->ui->tableWidget_balances->setItem( 1, 3, new QTableWidgetItem(QString::number(polkadot_completed)));
    this->ui->tableWidget_balances->setItem( 2, 3, new QTableWidgetItem(QString::number(binance_completed)));
    this->ui->tableWidget_balances->setItem( 3, 3, new QTableWidgetItem(QString::number(binance_completed + polkadot_completed + eth_completed)));

    this->ui->tableWidget_balances->setItem( 2, 1, new QTableWidgetItem(QString::number(binance_total_fees)));
    this->ui->tableWidget_balances->setItem( 1, 1, new QTableWidgetItem(QString::number(polkadot_total_fees)));
    this->ui->tableWidget_balances->setItem( 0, 1, new QTableWidgetItem(QString::number(eth_total_fees)));

    this->ui->tableWidget_balances->setItem( 2, 2, new QTableWidgetItem(QString::number(binance_withdrawable)));
    this->ui->tableWidget_balances->setItem( 1, 2, new QTableWidgetItem(QString::number(polkadot_withdrawable)));
    this->ui->tableWidget_balances->setItem( 0, 2, new QTableWidgetItem(QString::number(eth_withdrawable)));

    this->ui->tableWidget_balances->horizontalHeader()->setStretchLastSection(true);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete acria_config;
    delete processing_window;

    delete pi;
    delete pi2;
    delete data;

    delete timer_update_requests;
    delete timer_update_events;
    delete timer_update_balances;
    delete timer_update_gas_price;

    for(uint i=0; i<this->tm_resources.size();i++){
        delete this->tm_resources[i];
    }

    for (auto const& x : eth_based_chain)
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


    if(dir != ""){
        std::vector<comp> r;
        for (auto const& x : eth_based_chain)
        {
            std::vector<comp> r2 = x.second->cinfo->get_completed();
            r.insert(r.end(), r2.begin(), r2.end());
        }

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

       show_msgBox("Successfully exported as Json!");
    }
}

void MainWindow::on_pushButton_export_csv_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                 "/home",
                                                 QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);

    if(dir != ""){
        std::vector<comp> r;
        for (auto const& x : eth_based_chain)
        {
            std::vector<comp> r2 = x.second->cinfo->get_completed();
            r.insert(r.end(), r2.begin(), r2.end());
        }

        QString tmp = "";
        QString delim = ";";
        tmp += "requestID"+delim+"callback"+delim+"hash"+delim+"block"+delim+"address\n";

        for(uint i = 0; i<r.size(); i++){
            tmp += QString::fromStdString(r[i].requestID.toStdString());
            tmp += delim;
            tmp += QString::fromStdString(r[i].callback.toStdString());
            tmp += delim;
            tmp += QString::fromStdString(r[i].hash.toStdString());
            tmp += delim;
            tmp += QString::number(r[i].block);
            tmp += delim;
            tmp += QString::fromStdString(r[i].address.toStdString());
            tmp += "\n";
        }

        QString filename=dir + "/transactions.csv";
        QFile file(filename);

        if(!file.exists()){
            qDebug() << "File exists: "<<filename;
        }else{
            qDebug() << filename<<" does not exist";
        }

       if (file.open(QIODevice::WriteOnly | QIODevice::Text)){
            QTextStream out(&file);
            out << tmp;
            file.close();
       }

       show_msgBox("Successfully exported as CSV!");
    }
}

void MainWindow::on_pushButton_export_json_3_clicked()
{
    if(this->acria_config->exec() == QDialog::Accepted){

    }
}

void MainWindow::on_pushButton_refresh_clicked()
{
    ui->status_geth->setText("...");
    ui->status_polkadot->setText("...");
    ui->status_acria->setText("...");
    ui->status_config->setText("...");

    for (auto const& x : eth_based_chain)
    {
        x.second->node->update_geth_status();
    }

    QtConcurrent::run(this, &MainWindow::get_status_geth);
    QtConcurrent::run(this, &MainWindow::get_status_polkadot);
    QtConcurrent::run(this, &MainWindow::get_status_acria);
    QtConcurrent::run(this, &MainWindow::get_status_config);
}

void MainWindow::on_pushButton_eth_settings_clicked()
{
    this->ui->tabWidget->setCurrentIndex(4);
    this->ui->tabWidget_2->setCurrentIndex(0);
}

void MainWindow::on_pushButton_polkadot_settings_clicked()
{
    this->ui->tabWidget->setCurrentIndex(4);
    this->ui->tabWidget_2->setCurrentIndex(1);
}

void MainWindow::on_pushButton_acria_settings_clicked()
{
    this->ui->tabWidget->setCurrentIndex(4);
}

void MainWindow::on_pushButton_config_settings_clicked()
{
    this->acria_config->show();
}

void MainWindow::show_msgBox(QString text){
    QMessageBox msgBox;
    msgBox.setStyleSheet("QWidget{background-color:#232323;color:white;}QPushButton{background-color:white;color:black;border: none;width:80px;padding-top:3px;padding-bottom:3px;background-color:orange;}");
    msgBox.setWindowIcon(QPixmap("resources/acria_logo5_colored.svg"));
    msgBox.setText(text);
    msgBox.exec();
}

void MainWindow::on_pushButton_setting_save_clicked()
{
    bool reset_b = false;
    bool reset_e = false;

    if(this->data->binance_url != this->ui->lineEdit_binance_url->text() || this->data->binance_account != this->ui->lineEdit_binance_account->text())
        reset_b = true;
    if(this->data->geth_url != this->ui->lineEdit_geth_url->text() || this->data->eth_account != this->ui->lineEdit_eth_account->text())
        reset_e = true;

    this->data->binance_url = this->ui->lineEdit_binance_url->text();
    this->data->binance_account = this->ui->lineEdit_binance_account->text();
    this->data->binance_contract = this->ui->lineEdit_binance_contract->text();
    this->data->binance_enabled = this->ui->checkBox_binance->isChecked();

    this->data->geth_url = this->ui->lineEdit_geth_url->text();
    this->data->eth_account = this->ui->lineEdit_eth_account->text();
    this->data->eth_contract = this->ui->lineEdit_eth_contract->text();
    this->data->eth_enabled = this->ui->checkBox_eth->isChecked();

    this->data->polkadot_url = this->ui->lineEdit_polkadot_url->text();
    this->data->polkadot_account = this->ui->lineEdit_polkadot_account->text();
    this->data->polkadot_contract = this->ui->lineEdit_polkadot_contract->text();
    this->data->polkadot_enabled = this->ui->checkBox_polkadot->isChecked();

    this->data->save_settings();

    for (auto const& x : eth_based_chain)
    {
        x.second->cinfo->create_filter_events();
    }

    if(reset_e)
        this->eth_based_chain["ethereum"]->nonce_manager->reset();
    if(reset_b)
        this->eth_based_chain["binance"]->nonce_manager->reset();

    show_msgBox("Successfully saved the settings!");
}

void MainWindow::on_pushButton_setting_discard_clicked()
{
    this->data->load_settings();
    this->update_settings();
}

void MainWindow::on_pushButton_eth_info_clicked()
{
    this->eth_based_chain["ethereum"]->igeth->update_info();

    this->eth_based_chain["ethereum"]->igeth->exec();
}

void MainWindow::on_pushButton_binance_settings_clicked()
{
    this->ui->tabWidget->setCurrentIndex(4);
    this->ui->tabWidget_2->setCurrentIndex(2);
}

void MainWindow::on_pushButton_withdraw_eth_clicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Withdraw", "Withdraw everything from the Binance contract?", QMessageBox::Yes|QMessageBox::No);
      if (reply == QMessageBox::Yes) {
          this->processing_window->show();
        this->eth_based_chain["ethereum"]->withdraw->withdraw(processing_window);
      } else {

      }
}

void MainWindow::on_pushButton_withdraw_polkadot_clicked()
{

}

void MainWindow::on_pushButton_withdraw_binance_clicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Withdraw", "Withdraw everything from the Binance contract?", QMessageBox::Yes|QMessageBox::No);
      if (reply == QMessageBox::Yes) {
          this->processing_window->show();
        this->eth_based_chain["binance"]->withdraw->withdraw(processing_window);
      } else {

      }
}

void MainWindow::on_pushButton_binance_info_clicked()
{
    this->eth_based_chain["binance"]->igeth->update_info();

    this->eth_based_chain["binance"]->igeth->exec();
}

void MainWindow::on_pushButton_deploy_contract_binance_clicked()
{
     this->eth_based_chain["binance"]->deploy_window->exec();
}

void MainWindow::on_pushButton_deploy_contract_eth_clicked()
{
    this->eth_based_chain["ethereum"]->deploy_window->exec();
}

void MainWindow::on_pushButton_accounts_binance_clicked()
{
    this->eth_based_chain["binance"]->available_accounts->update_accounts();

    if(this->eth_based_chain["binance"]->available_accounts->exec() == QDialog::Accepted){
        this->ui->lineEdit_binance_account->setText(this->eth_based_chain["binance"]->available_accounts->selected_account);
    }
}

void MainWindow::on_pushButton_accounts_eth_clicked()
{
    this->eth_based_chain["ethereum"]->available_accounts->update_accounts();

    if(this->eth_based_chain["ethereum"]->available_accounts->exec() == QDialog::Accepted){
        this->ui->lineEdit_eth_account->setText(this->eth_based_chain["ethereum"]->available_accounts->selected_account);
    }
}

void MainWindow::line_edit_check_eth_address(QString address, QObject *senderObj){
    if(VerifyEthAddress::is_valid_eth_address(address)){
        static_cast<QLineEdit*>(senderObj)->setStyleSheet("");
    }
    else{
        static_cast<QLineEdit*>(senderObj)->setStyleSheet("background-color:#432627;");
    }
}

void MainWindow::on_lineEdit_eth_contract_textChanged(const QString &arg1)
{
    this->line_edit_check_eth_address(arg1, sender());
}

void MainWindow::on_lineEdit_binance_account_textChanged(const QString &arg1)
{
    this->line_edit_check_eth_address(arg1, sender());
}

void MainWindow::on_lineEdit_eth_account_textChanged(const QString &arg1)
{
    this->line_edit_check_eth_address(arg1, sender());
}

void MainWindow::on_lineEdit_binance_contract_textChanged(const QString &arg1)
{
    this->line_edit_check_eth_address(arg1, sender());
}

void MainWindow::on_tableWidget_comp_cellDoubleClicked(int row, int column)
{
    Q_UNUSED(column)

    QString chain = this->ui->tableWidget_comp->model()->data(this->ui->tableWidget_comp->model()->index(row,2), Qt::DisplayRole).toString();
    QString hash = this->ui->tableWidget_comp->model()->data(this->ui->tableWidget_comp->model()->index(row,5), Qt::DisplayRole).toString();
    this->eth_based_chain[chain]->about_transaction_window->init(hash);
    this->eth_based_chain[chain]->about_transaction_window->exec();
}
