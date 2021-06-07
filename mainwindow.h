#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <map>
#include <QTableWidgetItem>

#include "acriaconfig.h"
#include "qprogressindicator.h"
#include "data.h"
#include "config.h"
#include "processingwindow.h"
#include "ethbasedchain.h"
#include "accountmanager.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

const uint INTERVAL_RUN = 5000;
const uint INTERVAL_RUN_CONNECTION = 4000;
const uint INTERVAL_UPDATE_STATUS = 10000;
const uint INTERVAL_UPDATE_ACTIVE = 20000;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_export_json_clicked();

    void on_pushButton_export_csv_clicked();

    void on_pushButton_export_json_3_clicked();

    void on_pushButton_refresh_clicked();

    void update_requests();

    void update_events();

    void update_balances();

    void update_gas_price();

    void on_pushButton_eth_settings_clicked();

    void on_pushButton_polkadot_settings_clicked();

    void get_status_geth();

    void get_status_polkadot();

    void get_status_acria();

    void get_status_binance();

    void get_status_config();

    void get_status_cardano();

    void update_status();

    void update_active();

    void on_pushButton_setting_save_clicked();

    void on_pushButton_setting_discard_clicked();

    void on_pushButton_eth_info_clicked();

    void on_pushButton_binance_settings_clicked();

    void on_pushButton_withdraw_eth_clicked();

    void on_pushButton_withdraw_polkadot_clicked();

    void on_pushButton_withdraw_binance_clicked();

    void on_pushButton_binance_info_clicked();

    void on_pushButton_deploy_contract_binance_clicked();

    void on_pushButton_deploy_contract_eth_clicked();

    void on_pushButton_accounts_binance_clicked();

    void on_pushButton_accounts_eth_clicked();

    void on_lineEdit_eth_contract_textChanged(const QString &arg1);

    void on_lineEdit_binance_account_textChanged(const QString &arg1);

    void on_lineEdit_eth_account_textChanged(const QString &arg1);

    void on_lineEdit_binance_contract_textChanged(const QString &arg1);

    void on_tableWidget_comp_cellDoubleClicked(int row, int column);

    void on_pushButton_accounts_cardano_clicked();

    void on_pushButton_deploy_contract_cardano_clicked();

    void on_pushButton_withdraw_cardano_clicked();

    void on_pushButton_cardano_info_clicked();

    void on_lineEdit_cardano_account_textChanged(const QString &arg1);

    void on_lineEdit_cardano_contract_textChanged(const QString &arg1);

private:
    Ui::MainWindow *ui;
    AcriaConfig* acria_config;
    ProcessingWindow* processing_window;

    std::map<QString, EthBasedChain*> eth_based_chain;

    Config* config;

    std::vector<Resource*> tm_resources;
    std::vector<QString> nt;

    QProgressIndicator* pi;
    QProgressIndicator* pi2;

    AccountManager* account_manager;

    Data* data;

    QTimer *timer_update_requests;
    QTimer *timer_update_events;
    QTimer *timer_update_balances;
    QTimer *timer_update_gas_price;
    QTimer *timer_update_status;
    QTimer *timer_update_active;

    void update_settings();
    void line_edit_check_eth_address(QString address, QObject *senderObj);

    QString get_state_string(uint state);

    bool node_ready(QString type);
};
#endif // MAINWINDOW_H
