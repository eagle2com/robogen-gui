#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeWidget>
#include <QProcess>
#include <QHash>
#include <QList>
#include <QFileSystemWatcher>
#include <QJsonObject>
#include "dirwatcher.h"

#include "robotconfigform.h"
#include "simulationconfigform.h"
#include "evolutionconfigform.h"
#include "configoverviewform.h"
#include "settingswindow.h"

#include "configurations.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QString project_directory = "";
    QString getRobogenPath();

public slots:
    void onProjectTreeSelect(QTreeWidgetItem*,int);
    void onProjectTreeAdd();
    void onProjectTreeRemove();

    void loadSimulation();
    void loadEvolution();

    void onPushSimulate();
    void onPushAnalyze();
    void onPushStop();
    void onFileChanged(QString filename);
    void onNewFileList(QStringList files);

    void evolveReadReady();
    void onEvolveFinished(int);

    void onFileEvent(const QString &);

    void onNewProject();
    void onOpenProject();
    bool onSaveProject();

    void onEvolve();

private:
    Ui::MainWindow *ui;

    QFileSystemWatcher *fs_watcher = nullptr;

    int n_generation;

    bool first_dir_change = true;
    bool run_dir_added = false;

    QProcess *process_server;
    QProcess *process_evolve;
    QProcess *process_simulate;

    QList<QProcess*> process_list;
    DirWatcher dir_watcher;

    SimulationConfigForm *simulation_config_form = nullptr;
    EvolutionConfigForm* evolution_config_form = nullptr;
    RobotConfigForm* robot_config_form = nullptr;
    ConfigOverviewForm* overview_form = nullptr;
    SettingsWindow* settings_window = nullptr;

    ProjectConfiguration* current_config = nullptr;
    ProjectConfiguration* current_running_config = nullptr;
    QString current_run_path = "";

    void closeEvent(QCloseEvent* ev);
};

#endif // MAINWINDOW_H
