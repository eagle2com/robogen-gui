#include "custombrowser.h"
#include "ui_custombrowser.h"

CustomBrowser::CustomBrowser(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CustomBrowser)
{
    ui->setupUi(this);


}

CustomBrowser::~CustomBrowser()
{
    delete ui;
}
