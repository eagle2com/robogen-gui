#include "configoverviewform.h"
#include "ui_configoverviewform.h"
#include <QMessageBox>
#include "mainwindow.h"
#include <QProcess>
#include <QDebug>

ConfigOverviewForm::ConfigOverviewForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConfigOverviewForm)
{
    ui->setupUi(this);
    connect(ui->line_name, SIGNAL(returnPressed()), this, SLOT(onNameChange()));
    connect(ui->tree_runs, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)), this, SLOT(onRunItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)));
}

ConfigOverviewForm::~ConfigOverviewForm()
{
    delete ui;
}

void ConfigOverviewForm::loadAll()
{
    ui->line_name->setText(current_config->text(0));
    refresh_run_list();
}

void ConfigOverviewForm::saveAll()
{

}

void ConfigOverviewForm::refresh_run_list()
{
    ui->tree_runs->clear();
    if(current_config && current_config->root_directory != "") {
        QStringList run_list = QDir(current_config->root_directory).entryList({"run_*"});
        for(QString& run: run_list) {
            QTreeWidgetItem* item = new QTreeWidgetItem();
            item->setText(0, run);
            ui->tree_runs->addTopLevelItem(item);
        }
    }
}

void ConfigOverviewForm::refresh_generation_list(const QString& run_name)
{
    if(current_config && current_config->root_directory != "") {
        QStringList generation_list = QDir(current_config->root_directory + "/" + run_name).entryList({"*.json"});
        for(QString& gen: generation_list) {
            QTreeWidgetItem* item = new QTreeWidgetItem();
            item->setText(0, gen);
            ui->tree_generations->addTopLevelItem(item);
        }
    }
}

void ConfigOverviewForm::onNameChange()
{
    if(current_config) {
        current_config->setText(0, ui->line_name->text());
    }
}

void ConfigOverviewForm::onRunItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    ui->tree_generations->clear();
    if(current)
        refresh_generation_list(current->text(0));
}
