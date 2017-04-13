#ifndef CUSTOMBROWSER_H
#define CUSTOMBROWSER_H

#include <QDialog>

namespace Ui {
class CustomBrowser;
}

class CustomBrowser : public QDialog
{
    Q_OBJECT

public:
    explicit CustomBrowser(QWidget *parent = 0);
    ~CustomBrowser();

private:
    Ui::CustomBrowser *ui;
};

#endif // CUSTOMBROWSER_H
