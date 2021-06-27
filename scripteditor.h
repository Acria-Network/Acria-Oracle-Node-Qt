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

private:
    Ui::ScriptEditor *ui;
};

#endif // SCRIPTEDITOR_H
