#include "configoverviewform.h"
#include "ui_configoverviewform.h"

ConfigOverviewForm::ConfigOverviewForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConfigOverviewForm)
{
    ui->setupUi(this);
    connect(ui->line_name, SIGNAL(returnPressed()), this, SLOT(onNameChange()));
}

ConfigOverviewForm::~ConfigOverviewForm()
{
    delete ui;
}

void ConfigOverviewForm::loadAll()
{
    ui->line_name->setText(current_config->text(0));
}

void ConfigOverviewForm::saveAll()
{

}

void ConfigOverviewForm::onNameChange()
{
    if(current_config) {
        current_config->setText(0, ui->line_name->text());
    }
}
