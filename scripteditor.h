#ifndef SCRIPTEDITOR_H
#define SCRIPTEDITOR_H

#include <QDialog>

namespace Ui {
class ScriptEditor;
}

class ScriptEditor : public QDialog
{
    Q_OBJECT

public:
    explicit ScriptEditor(QWidget *parent = nullptr);
    ~ScriptEditor();

    void load_script(QString script_file);

private slots:
    void on_pushButton_save_script_clicked();

    void on_pushButton_discard_script_clicked();

private:
    Ui::ScriptEditor *ui;
    void save_script(QString script_content);
};

#endif // SCRIPTEDITOR_H
