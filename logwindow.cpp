#include "logwindow.h"
#include "ui_logwindow.h"
#include <QListWidgetItem>
#include <QCheckBox>
#include <QProcess>
#include <QPushButton>

#ifdef Q_OS_WIN
    #include <windows.h>
#endif

LogWindow::LogWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LogWindow)
{
    ui->setupUi(this);

    connect(ui->push_evolver_clear, SIGNAL(clicked(bool)), ui->list_error_evolver, SLOT(clear()));
    connect(ui->push_server_clear, SIGNAL(clicked(bool)), ui->list_error_server, SLOT(clear()));
    this->show();

    SetWindowPos((HWND)this->winId(), HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
}

LogWindow::~LogWindow()
{
    delete ui;
}

bool keepLine(char line[256]) {
    return line[0] != '\n' && line[0] != '.';
}

void LogWindow::onNewEvolution()
{
    if(ui->check_clear_evolver->isChecked()) {
        ui->list_error_evolver->clear();
    }

    if(ui->check_clear_server->isChecked()) {
        ui->list_error_server->clear();
    }
}

void LogWindow::onServerLog()
{
    if(!ui->check_log_server->isChecked())
        return;

    QProcess* process = dynamic_cast<QProcess*>(QObject::sender());
    process->setReadChannel(QProcess::StandardOutput);

    char line[256];

    while(process->readLine(line, 256)) {
        if(!keepLine(line))
            continue;
        QListWidgetItem* item = new QListWidgetItem();
        item->setText(line);
        ui->list_error_server->addItem(item);
    }
}

void LogWindow::onServerError()
{
    if(!ui->check_error_server->isChecked())
        return;

    QProcess* process = dynamic_cast<QProcess*>(QObject::sender());
    process->setReadChannel(QProcess::StandardError);

    char line[256];

    while(process->readLine(line, 256)) {
        if(!keepLine(line))
            continue;
        QListWidgetItem* item = new QListWidgetItem();
        item->setText(line);
        item->setBackgroundColor(QColor(255,0,0,50));
        ui->list_error_server->addItem(item);
    }
}

void LogWindow::onEvolverLog()
{
    if(!ui->check_log_evolver->isChecked())
        return;

    QProcess* process = dynamic_cast<QProcess*>(QObject::sender());
    process->setReadChannel(QProcess::StandardOutput);

    char line[256];

    while(process->readLine(line, 256)) {
        if(!keepLine(line))
            continue;
        QListWidgetItem* item = new QListWidgetItem();
        item->setText(line);
        ui->list_error_evolver->addItem(item);
    }
}

void LogWindow::onEvolverError()
{
    if(!ui->check_error_evolver->isChecked())
        return;

    QProcess* process = dynamic_cast<QProcess*>(QObject::sender());
    process->setReadChannel(QProcess::StandardError);

    char line[256];

    while(process->readLine(line, 256)) {
        if(!keepLine(line))
            continue;
        QListWidgetItem* item = new QListWidgetItem();
        item->setText(line);
        item->setBackgroundColor(QColor(255,0,0,50));
        ui->list_error_evolver->addItem(item);
    }
}
