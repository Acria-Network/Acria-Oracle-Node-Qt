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
    void completed(QString str, bool error = false);

private slots:
    void on_pushButton_clicked();

    void on_ProcessingWindow_rejected();

private:
    QProgressIndicator* pi;

    void reset();
};

#endif // PROCESSINGWINDOW_H
