#include "settingswindow.h"
#include "ui_settingswindow.h"
#include <QFileDialog>

SettingsWindow::SettingsWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SettingsWindow)
{
    ui->setupUi(this);
    connect(ui->push_robogen_browse, SIGNAL(pressed()), this, SLOT(onPushBrowse()));
}

SettingsWindow::~SettingsWindow()
{
    delete ui;
}

QString SettingsWindow::get_robogen_directory()
{
    return ui->line_robogen->text();
}

void SettingsWindow::set_robogen_directory(const QString &dir)
{
    ui->line_robogen->setText(dir);
}

void SettingsWindow::onPushBrowse()
{
    QString directory = QFileDialog::getExistingDirectory(this, "Choose the robogen directory", "");
    ui->line_robogen->setText(directory);
}
