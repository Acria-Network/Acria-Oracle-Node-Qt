#ifndef ETHBASEDCHAIN_H
#define ETHBASEDCHAIN_H

#include <map>

#include "node.h"
#include "tasks.h"
#include "compinfo.h"
#include "qprogressindicator.h"
#include "data.h"
#include "infogeth.h"
#include "balances.h"
#include "withdraw.h"
#include "processingwindow.h"
#include "deploywindow.h"
#include "gasprice.h"
#include "availableaccounts.h"
#include "noncemanager.h"
#include "abouttransactionwindow.h"


class EthBasedChain{
private:
    Data* data;
    QString type;
    QWidget* main_window;
    ProcessingWindow* processing_window;
public:
    EthBasedChain(QWidget *parent, QString _type, Data* _data, ProcessingWindow* _processing_window);
    ~EthBasedChain();

    Node* node;
    Tasks* tasks;
    compinfo* cinfo;
    InfoGeth* igeth;
    Balances* balances;
    Withdraw* withdraw;
    DeployWindow* deploy_window;
    GasPrice* gas_price;
    AvailableAccounts* available_accounts;
    std::map<uint, uint> state;
    NonceManager* nonce_manager;
    AboutTransactionWindow* about_transaction_window;
};

#endif // ETHBASEDCHAIN_H
