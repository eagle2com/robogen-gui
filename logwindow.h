#ifndef LOGWINDOW_H
#define LOGWINDOW_H

#include <QDialog>

namespace Ui {
class LogWindow;
}

class LogWindow : public QDialog
{
    Q_OBJECT

public:
    explicit LogWindow(QWidget *parent = 0);
    ~LogWindow();

    void onNewEvolution();

public slots:
    void onServerLog();
    void onServerError();

    void onEvolverLog();
    void onEvolverError();

private:
    Ui::LogWindow *ui;
};

#endif // LOGWINDOW_H
