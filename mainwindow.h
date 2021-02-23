#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <map>

#include "acriaconfig.h"
#include "node.h"
#include "tasks.h"
#include "compinfo.h"
#include "qprogressindicator.h"
#include "data.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

const uint INTERVAL_RUN = 5000;
const uint INTERVAL_RUN_CONNECTION = 4000;

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

    void on_pushButton_eth_settings_clicked();

    void on_pushButton_polkadot_settings_clicked();

    void on_pushButton_acria_settings_clicked();

    void on_pushButton_config_settings_clicked();

    void get_status_geth();

    void get_status_polkadot();

    void get_status_acria();

    void get_status_config();

    void on_pushButton_setting_save_clicked();

    void on_pushButton_setting_discard_clicked();

private:
    Ui::MainWindow *ui;
    AcriaConfig* acria_config;
    Node* node;
    Tasks* tasks;
    compinfo* cinfo;
    std::map<QString, Resource*> resources;
    QProgressIndicator* pi;
    QProgressIndicator* pi2;

    Data* data;

    QTimer *timer_update_requests;
    QTimer *timer_update_events;

    void update_settings();
};
#endif // MAINWINDOW_H
