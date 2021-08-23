#include "logwindow.h"
#include "ui_logwindow.h"

#include <fstream>

std::string replace_once(std::string str, std::string what, std::string to){
    int nInx = str.find(what);
    if (nInx >= 0)
    {    str.erase(nInx, 1);
         str.insert(nInx, to);
    }
    return str;
}

std::string get_n_lines(std::string fileName, int n)
{
    std::string out;
    try{
        const int k = n;
        std::ifstream file(fileName);
        std::string l[k];
        int size = 0 ;

        while(file.good()){
            getline(file, l[size%k]);
            size++;
        }

        int start = size > k ? (size%k) : 0 ;
        int count = std::min(k, size);

        for(int i = 0; i< count ; i++){
            std::string str = l[(start+i)%k];

            str = replace_once(str, "[", "<font color=\"DeepPink\">[");
            str = replace_once(str, "]", "]</font>");
            str = replace_once(str, "{", "<font color=\"Green\">{");
            str = replace_once(str, "}", "}</font>");
            out += str + "<br />";
        }
    }
    catch(...){
        return "";
    }
    return out;
}

LogWindow::LogWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LogWindow)
{
    ui->setupUi(this);

    /*
    std::ifstream t("./logs/LogFile.log");
    if(!t){
    }
    std::string str;

    t.seekg(0, std::ios::end);
    str.reserve(t.tellg());
    t.seekg(0, std::ios::beg);

    str.assign((std::istreambuf_iterator<char>(t)),
                std::istreambuf_iterator<char>());
    */

    this->ui->textEdit_log->setText(QString::fromStdString(get_n_lines("./logs/LogFile.log", 100)));
}

LogWindow::~LogWindow()
{
    delete ui;
}
