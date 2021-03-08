#ifndef PROCESSINGWINDOW_H
#define PROCESSINGWINDOW_H

#include <QDialog>

#include "qprogressindicator.h"


namespace Ui {
class ProcessingWindow;
}

class ProcessingWindow : public QDialog
{
    Q_OBJECT

public:
    explicit ProcessingWindow(QWidget *parent = nullptr);
    ~ProcessingWindow();

    Ui::ProcessingWindow *ui;
    void completed(QString str);

private slots:
    void on_pushButton_clicked();

private:
    QProgressIndicator* pi;
};

#endif // PROCESSINGWINDOW_H
