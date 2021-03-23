#include "ethbasedchain.h"


EthBasedChain::EthBasedChain(QWidget *parent, QString _type, Data* _data, ProcessingWindow* _processing_window){
    this->type = _type;
    this->data = _data;
    this->main_window = parent;
    this->processing_window = _processing_window;

    this->node = new Node(this->data, this->type);
    this->gas_price = new GasPrice(this->data, this->type);
    this->tasks = new Tasks(this->data, this->type);
    this->cinfo = new compinfo(this->data, this->type);
    this->igeth = new InfoGeth(this->main_window, this->node, this->data, this->type);
    this->nonce_manager = new NonceManager(this->data, this->type);
    this->balances = new Balances(this->data, this->type, this->nonce_manager);
    this->withdraw = new Withdraw(this->data, this->type, this->nonce_manager);
    this->deploy_window = new DeployWindow(this->main_window, this->data, this->type, this->processing_window, "");
    this->available_accounts = new AvailableAccounts(this->main_window, this->data, this->type);
    this->about_transaction_window = new AboutTransactionWindow(this->main_window, this->data, this->type);
};

EthBasedChain::~EthBasedChain(){
    delete node;
    delete tasks;
    delete cinfo;
    delete igeth;
    delete balances;
    delete withdraw;
    delete deploy_window;
    delete gas_price;
    delete available_accounts;
    delete nonce_manager;
    delete about_transaction_window;
};
